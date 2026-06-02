#include "tree.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

// Node Constructor
Tree::Node::Node(std::string id_, std::string title_, std::string isbn_, int year_,
                 std::string lang_, std::string desc_, double rating_, int pages_, Node* p) {
    id = id_;
    title = title_;
    isbn = isbn_;
    publicationYear = year_;
    languageCode = lang_;
    description = desc_;
    averageRating = rating_;
    numPages = pages_;
    parent = p;
}

// Tree Constructor corregido
Tree::Tree(int k) {
    this->k = k; // almacena valor maximo de hijos
    rootNode = new Node("ROOT", "Raiz Artificial", "", 0, "", "", 0.0, 0, nullptr);
    treeSize = 0;
}

Tree::~Tree() {
    deleteSubtree(rootNode);
}

bool Tree::isEmpty() {
    return treeSize == 0;
}

int Tree::size() {
    return treeSize;
}

void Tree::deleteSubtree(Node* node) {
    if (!node) return;
    for (auto child : node->children) {
        deleteSubtree(child);
    }
    delete node;
}

// Inserción de nodos
bool Tree::insertBook(std::string id, std::string title, std::string isbn, int year,
                      std::string lang, std::string desc, double rating, int pages,
                      const std::vector<SimilarBook>& similares) {
    if (!rootNode) return false;

    // Comprueba que la raiz no tengo mas que k nodos
    if (static_cast<int>(rootNode->children.size()) >= k) {
        std::cerr << "[WARN] No se pudo insertar el libro " << id << ". Se alcanzó el límite k = " << k << " en la raíz." << std::endl;
        return false;
    }

    Node* newNode = new Node(id, title, isbn, year, lang, desc, rating, pages, rootNode);
    newNode->similarBooks = similares;
    
    rootNode->children.push_back(newNode);
    treeSize++;
    return true;
}

void Tree::cargarDesdeArchivo(const std::string& rutaArchivo) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir el archivo: " << rutaArchivo << std::endl;
        return;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;

        std::stringstream ss(linea);
        std::string id, titulo, isbn, s_anio, idioma, desc, s_rating, s_paginas, s_similares;

        std::getline(ss, id, '|');
        std::getline(ss, titulo, '|');
        std::getline(ss, isbn, '|');
        std::getline(ss, s_anio, '|');
        std::getline(ss, idioma, '|');
        std::getline(ss, desc, '|');
        std::getline(ss, s_rating, '|');
        std::getline(ss, s_paginas, '|');
        std::getline(ss, s_similares);

        int anio = (s_anio == "N/A" || s_anio.empty()) ? 0 : std::stoi(s_anio);
        double rating = s_rating.empty() ? 0.0 : std::stod(s_rating);
        int paginas = (s_paginas == "N/A" || s_paginas.empty()) ? 0 : std::stoi(s_paginas);

        std::vector<SimilarBook> listaSimilares;
        if (s_similares != "0" && !s_similares.empty()) {
            std::stringstream ss_sim(s_similares);
            std::string item_sim;
            
            while (std::getline(ss_sim, item_sim, '#')) {
                std::stringstream ss_data(item_sim);
                std::string t_sim, i_sim, a_sim;
                
                std::getline(ss_data, t_sim, ';');
                std::getline(ss_data, i_sim, ';');
                std::getline(ss_data, a_sim);

                int anio_sim = (a_sim == "N/A" || a_sim.empty() || a_sim == "0") ? 0 : std::stoi(a_sim);
                listaSimilares.push_back({t_sim, i_sim, anio_sim});
            }
        }

        
        if (!insertBook(id, titulo, isbn, anio, idioma, desc, rating, paginas, listaSimilares)) {
     
            break;
        }
    }
    archivo.close();
    std::cout << "[INFO] Proceso de carga finalizado. Nodos totales en el árbol: " << treeSize << std::endl;
}


/**
 * Método privado y recursivo que realiza el recorrido PreOrder sobre el árbol.
 * @param node Puntero al nodo actual en la recursión.
 * @param result Vector donde se acumulan los IDs de los libros en el orden correcto.
 */
void Tree::preOrder(Node* node, std::vector<std::string>& result) {
    
    if (!node) return;
    
    // registramos el id de cualquier nodo que no sea la raiz
    if (node != rootNode) {
        result.push_back(node->id); 
    }
    
    // itera recursivamente a través de cada hijo que tenga el nodo.
    for (auto child : node->children) {
        preOrder(child, result);
    }
}

/**
 * Método público que lista los IDs de los libros siguiendo un recorrido preorder.
 * @return Vector con todos los IDs de los libros ordenados en preorden.
 */
std::vector<std::string> Tree::listar() {
    std::vector<std::string> result;
    preOrder(rootNode, result);
    return result;
}



/**
 * Método público para eliminar del árbol todos los libros con rating promedio menor o igual al parámetro r..
 * @param r Calificación de corte.
 */
void Tree::borrar_ratings(double r) {
    // Invoca al método recursivo pasándole el nodo raíz.
    borrar_ratings(rootNode, r);
}

/**
 * Método privado recursivo que evalúa los hijos de cada nodo para eliminarlos si aplica.
 * @param node Puntero al nodo padre.
 * @param r Calificación de corte.
 */
void Tree::borrar_ratings(Node* node, double r) {
    if (!node) return;

    for (int i = static_cast<int>(node->children.size()) - 1; i >= 0; --i) {
        Node* child = node->children[i]; // Apunta al hijo actual en la posición i
        
        // Si la calificación promedio es menor o igual al parámetro dado
        if (child->averageRating <= r) {
            // libera de memoria toda la descendencia de este hijo.
            deleteSubtree(child);
            
            // 2. Elimina el puntero de la lista de hijos del padre
            node->children.erase(node->children.begin() + i);
            
            treeSize--;
        } else {
            // Si el hijo tiene un rating promedio mayor que r , se evalua a los hijos de este
            borrar_ratings(child, r);
        }
    }
}


/**
 * Método público que expone la funcionalidad para descubrir libros precursores.
 * @return Vector con los IDs de los libros que cumplen con la condición temporal de originalidad.
 */
std::vector<std::string> Tree::precursores() {
    std::vector<std::string> result;
    // Inicia el escaneo recursivo en preorden desde el nodo raíz.
    precursores(rootNode, result);
    return result;
}

/**
 * Método privado y recursivo que evalúa la colección de libros similares de cada nodo.
 * @param node Puntero al libro actual bajo análisis.
 * @param result Vector donde se acumularán los IDs que cumplan la regla.
 */
void Tree::precursores(Node* node, std::vector<std::string>& result) {
    if (!node) return;

    if (node != rootNode && !node->similarBooks.empty()) {
        bool todosPosteriores = true; 
        
        // Evaluamos uno a uno los libros similares
        for (const auto& sim : node->similarBooks) {
            // Si encontramos un libro similar que se publicó antes o el mismo año, no es un precursor
            
            if (sim.publicationYear <= node->publicationYear || sim.publicationYear == 0) {
                todosPosteriores = false; // Cambia el estado a inválido
                break;                   
            }
        }
        
        // Si el libro superó con éxito la evaluación de todos sus libros similares:
        if (todosPosteriores) {
            result.push_back(node->id); // Registramos su ID en la lista 
        }
    }

    // Continúa el recorrido en preorden visitando recursivamente el resto del arbol
    for (auto child : node->children) {
        precursores(child, result);
    }
}