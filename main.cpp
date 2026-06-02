#include <iostream>
#include "tree.hpp"

using namespace std;

int main() {
    // Instanciamos indicando que la raíz soportará hasta 15,000 hijos directos
    Tree libroTree(15000); 

    cout << "--- CARGANDO EL DATASET DE LIBROS AL ARBOL ---" << endl;
    libroTree.cargarDesdeArchivo("cleaned_books.txt");
    cout << "Tamaño inicial del árbol: " << libroTree.size() << " libros." << endl << endl;

    cout << "--- EJECUTANDO CONSULTA: listar() ---" << endl;
    vector<string> listaPreorder = libroTree.listar();
    cout << "Total de IDs recuperados en PreOrder: " << listaPreorder.size() << endl;
    if (!listaPreorder.empty()) {
        cout << "Primeros 5 IDs del recorrido: ";
        for (size_t i = 0; i < min(listaPreorder.size(), size_t(5)); ++i) {
            cout << listaPreorder[i] << " ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "--- EJECUTANDO CONSULTA: precursores() ---" << endl;
    vector<string> listaPrecursores = libroTree.precursores();
    cout << "Cantidad de libros que son precursores de sus similares: " << listaPrecursores.size() << endl;
    if (!listaPrecursores.empty()) {
        cout << "Muestra de IDs precursores (primeros 5): ";
        for (size_t i = 0; i < min(listaPrecursores.size(), size_t(5)); ++i) {
            cout << listaPrecursores[i] << " ";
        }
        cout << endl;
    }
    cout << endl;

    double filtroRating = 3.5;
    cout << "--- EJECUTANDO CONSULTA: borrar_ratings(" << filtroRating << ") ---" << endl;
    libroTree.borrar_ratings(filtroRating);
    cout << "Tamaño del árbol después del borrado masivo: " << libroTree.size() << " libros." << endl;

    vector<string> listaPostBorrado = libroTree.listar();
    cout << "Nuevo total de IDs en el árbol: " << listaPostBorrado.size() << endl;

    return 0;
}