#include "tree.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

// ── Node Constructor
// ──────────────────────────────────────────────────────────
Tree::Node::Node(std::string id_, std::string title_, std::string isbn_,
                 int year_, std::string lang_, std::string desc_,
                 double rating_, int pages_, Node *p) {
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

// ── Tree Constructor / Destructor
// ─────────────────────────────────────────────
Tree::Tree(int k) {
    this->k = k;
    rootNode =
        new Node("ROOT", "Raiz Artificial", "", 0, "", "", 0.0, 0, nullptr);
    treeSize = 0;
}

Tree::~Tree() { deleteSubtree(rootNode); }

// ── Métodos de control
// ────────────────────────────────────────────────────────
bool Tree::isEmpty() const { return treeSize == 0; }

int Tree::size() const { return treeSize; }

/**
 * Libera recursivamente toda la memoria dinámica de un subárbol.
 * @param node Raíz del subárbol a eliminar.
 */
void Tree::deleteSubtree(Node *node) {
    if (!node)
        return;
    for (auto child : node->children)
        deleteSubtree(child);
    delete node;
}

// ── Inserción
// ─────────────────────────────────────────────────────────────────

/**
 * Inserta un nuevo libro como hijo directo de la raíz.
 * Rechaza la inserción si se superó el límite k de hijos.
 * @return true si se insertó correctamente, false en caso contrario.
 */
bool Tree::insertBook(std::string id, std::string title, std::string isbn,
                      int year, std::string lang, std::string desc,
                      double rating, int pages,
                      const std::vector<SimilarBook> &similares) {
    if (!rootNode)
        return false;

    if (static_cast<int>(rootNode->children.size()) >= k) {
        std::cerr << "[WARN] No se pudo insertar el libro " << id
                  << ". Se alcanzó el límite k = " << k << " en la raíz."
                  << std::endl;
        return false;
    }

    Node *newNode =
        new Node(id, title, isbn, year, lang, desc, rating, pages, rootNode);
    newNode->similarBooks = similares;
    rootNode->children.push_back(newNode);
    treeSize++;
    return true;
}

// ── Carga desde archivo
// ───────────────────────────────────────────────────────

/**
 * Parsea el archivo de texto limpio e inserta cada registro en el árbol.
 * Formato de línea: id|titulo|isbn|anio|idioma|desc|rating|paginas|similares
 * Similares: titulo;isbn;anio separados por '#', o "0" si no hay.
 * @param rutaArchivo Ruta al archivo cleaned_books.txt.
 */
void Tree::cargarDesdeArchivo(const std::string &rutaArchivo) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir el archivo: " << rutaArchivo
                  << std::endl;
        return;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        if (linea.empty())
            continue;

        std::stringstream ss(linea);
        std::string id, titulo, isbn, s_anio, idioma, desc, s_rating, s_paginas,
            s_similares;

        std::getline(ss, id, '|');
        std::getline(ss, titulo, '|');
        std::getline(ss, isbn, '|');
        std::getline(ss, s_anio, '|');
        std::getline(ss, idioma, '|');
        std::getline(ss, desc, '|');
        std::getline(ss, s_rating, '|');
        std::getline(ss, s_paginas, '|');
        std::getline(ss, s_similares);

        int anio = (s_anio.empty() || s_anio == "N/A") ? 0 : std::stoi(s_anio);
        double rating = s_rating.empty() ? 0.0 : std::stod(s_rating);
        int paginas = (s_paginas.empty() || s_paginas == "N/A")
                          ? 0
                          : std::stoi(s_paginas);

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
                int anio_sim = (a_sim.empty() || a_sim == "N/A" || a_sim == "0")
                                   ? 0
                                   : std::stoi(a_sim);
                listaSimilares.push_back({t_sim, i_sim, anio_sim});
            }
        }

        if (!insertBook(id, titulo, isbn, anio, idioma, desc, rating, paginas,
                        listaSimilares))
            break;
    }

    archivo.close();
    std::cout
        << "[INFO] Proceso de carga finalizado. Nodos totales en el árbol: "
        << treeSize << std::endl;
}

// ── listar
// ────────────────────────────────────────────────────────────────────

/**
 * Recorre el árbol en PreOrder de forma recursiva acumulando los IDs.
 * Se omite la raíz artificial ya que no representa un libro real.
 * @param node  Nodo actual de la recursión.
 * @param result Vector donde se acumulan los IDs en orden PreOrder.
 */
void Tree::preOrder(Node *node, std::vector<std::string> &result) {
    if (!node)
        return;
    if (node != rootNode)
        result.push_back(node->id);
    for (auto child : node->children)
        preOrder(child, result);
}

/**
 * Lista los IDs de todos los libros en recorrido PreOrder.
 * @return Vector con los IDs en el orden de visita PreOrder.
 */
std::vector<std::string> Tree::listar() {
    std::vector<std::string> result;
    preOrder(rootNode, result);
    return result;
}

// ── borrar_ratings
// ────────────────────────────────────────────────────────────

/**
 * Elimina del árbol todos los libros con rating promedio <= r.
 * @param r Umbral de calificación. Se eliminan los libros con rating <= r.
 */
void Tree::borrar_ratings(double r) { borrar_ratings(rootNode, r); }

/**
 * Recorre los hijos de cada nodo y elimina los que no superen el umbral.
 * El recorrido es de derecha a izquierda para no invalidar índices al borrar.
 * Si el hijo supera el umbral, se evalúa recursivamente su descendencia.
 * @param node Nodo padre cuyos hijos se evaluarán.
 * @param r    Umbral de calificación.
 */
void Tree::borrar_ratings(Node *node, double r) {
    if (!node)
        return;

    for (int i = static_cast<int>(node->children.size()) - 1; i >= 0; --i) {
        Node *child = node->children[i];
        if (child->averageRating <= r) {
            deleteSubtree(child);
            node->children.erase(node->children.begin() + i);
            treeSize--;
        } else {
            borrar_ratings(child, r);
        }
    }
}

// ── precursores
// ───────────────────────────────────────────────────────────────

/**
 * Lista los IDs de libros cuyos similares fueron publicados todos en años
 * posteriores. Se ignoran similares con año desconocido (0). Se requiere al
 * menos un similar con año conocido para que el libro sea considerado
 * precursor.
 * @return Vector con los IDs de los libros precursores.
 */
std::vector<std::string> Tree::precursores() {
    std::vector<std::string> result;
    precursores(rootNode, result);
    return result;
}

/**
 * Evalúa recursivamente en PreOrder si cada libro es precursor de sus
 * similares. Un libro es precursor si todos sus similares con año conocido
 * fueron publicados en un año estrictamente posterior al suyo.
 * @param node   Nodo actual bajo análisis.
 * @param result Vector donde se acumulan los IDs que cumplen la condición.
 */
void Tree::precursores(Node *node, std::vector<std::string> &result) {
    if (!node)
        return;

    if (node != rootNode && !node->similarBooks.empty() &&
        node->publicationYear != 0) {
        bool todosPosteriores = true;
        bool hayAlMenosUnoValido = false;

        for (const auto &sim : node->similarBooks) {
            if (sim.publicationYear == 0)
                continue; // año desconocido: se ignora
            hayAlMenosUnoValido = true;
            if (sim.publicationYear <= node->publicationYear) {
                todosPosteriores = false;
                break;
            }
        }

        if (todosPosteriores && hayAlMenosUnoValido)
            result.push_back(node->id);
    }

    for (auto child : node->children)
        precursores(child, result);
}