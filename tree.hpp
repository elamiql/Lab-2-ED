#ifndef TREE_HPP
#define TREE_HPP

#include <vector>
#include <string>
#include <iostream>

// Estructura para almacenar los datos de un libro similar
struct SimilarBook {
    std::string title;           
    std::string isbn;            
    int publicationYear;         
};

// Clase para la gestión del árbol general de libros
class Tree {
private:
    // Estructura interna para los nodos del árbol
    struct Node {
        std::string id;              
        std::string title;           
        std::string isbn;            
        int publicationYear;         
        std::string languageCode;    
        std::string description;     
        double averageRating;        
        int numPages;                
        
        // lista de libros similares
        std::vector<SimilarBook> similarBooks;

        // Punteros para la jerarquía del árbol
        Node* parent;                
        std::vector<Node*> children; 

        // Constructor del nodo
        Node(std::string id_, std::string title_, std::string isbn_, int year_,
             std::string lang_, std::string desc_, double rating_, int pages_, Node* p = nullptr);
    };


    Node* rootNode;                  // Puntero a la raíz artificial
    int treeSize;                    // Contador total de libros insertados
    int k;                           // Límite máximo de hijos por nodo

    
    void preOrder(Node* node, std::vector<std::string>& result);   // Recorrido PreOrder
    void deleteSubtree(Node* node);                               // Libera memoria de un subárbol
    void borrar_ratings(Node* node, double r);                    // Filtro de eliminación por rating
    void precursores(Node* node, std::vector<std::string>& result); // Filtro analítico por año

public:
    // Métodos públicos de control de la estructura
    Tree(int k);                     // Constructor: define el límite de hijos k
    ~Tree();                        // Destructor: libera toda la memoria dinámica
    bool isEmpty();                  // Comprueba si el árbol no tiene elementos
    int size();                     // Retorna la cantidad de libros actuales

    // Inserta un nuevo libro validando que no se supere el límite k de la raíz
    bool insertBook(std::string id, std::string title, std::string isbn, int year,
                    std::string lang, std::string desc, double rating, int pages,
                    const std::vector<SimilarBook>& similares);

    // Parsea el archivo limpio e inserta los registros uno a uno en el árbol
    void cargarDesdeArchivo(const std::string& rutaArchivo);

    
    std::vector<std::string> listar();       // Retorna los IDs en PreOrder
    void borrar_ratings(double r);          // Elimina subárboles con rating <= r
    std::vector<std::string> precursores();  // Retorna IDs con similares estrictamente posteriores
};

#endif