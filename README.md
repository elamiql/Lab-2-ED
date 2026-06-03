# Entregable 2: XML

**Alumnos:**

- Ignacio Silva
- Agustín Salgado

Este proyecto procesa un dataset de 10.000 archivos XML de reseñas de libros de GoodReads, limpia y estructura la información mediante un script en Python, y posteriormente construye y consulta un árbol general en C++.

## Estructura del proyecto

```
.
├── books_xml/          # Carpeta con los 10.000 archivos XML (no incluida en el repo)
├── parser.py           # Script Python que parsea los XML y genera cleaned_books.txt
├── cleaned_books.txt   # Archivo generado por el parser (no incluido en el repo)
├── tree.hpp            # Declaración de la clase Tree y estructuras auxiliares
├── tree.cpp            # Implementación del árbol general
└── main.cpp            # Punto de entrada: carga el árbol y ejecuta las consultas
```

---

## Parser (Python)

Para evitar dependencias complejas en C++, se desarrolló un script en Python que consolida los 10.000 archivos XML en un único archivo de texto estructurado (`cleaned_books.txt`).

### Requisitos

- Python 3.x
- Carpeta `books_xml/` con los 10.000 archivos XML en la raíz del proyecto

### Ejecución

```bash
python parser.py
```

### Formato de salida

Una línea por libro, con los siguientes delimitadores:

- `|` separa los campos principales del libro
- `#` separa libros similares entre sí
- `;` separa los subcampos de cada libro similar (Título;ISBN;Año)

**Ejemplo de línea:**

```
ID|Título|ISBN|Año|Idioma|Descripción|Rating|Páginas|TítuloSim1;ISBNSim1;AñoSim1#TítuloSim2;ISBNSim2;AñoSim2
```

Si el libro no tiene similares, el último campo es `0`.

---

## Compilación y ejecución (C++)

### Requisitos

- Compilador g++ con soporte C++17
- Archivo `cleaned_books.txt` generado previamente por el parser en el mismo directorio

### Compilación

```bash
g++ -std=c++17 -o tree main.cpp tree.cpp
```

### Ejecución

**Linux / macOS:**

```bash
./tree
```

**Windows:**

```bash
tree.exe
```

---

## Consultas implementadas

| Función             | Descripción                                                                           |
| ------------------- | ------------------------------------------------------------------------------------- |
| `listar()`          | Retorna los IDs de todos los libros en recorrido PreOrder                             |
| `borrar_ratings(r)` | Elimina del árbol todos los libros con rating promedio ≤ r                            |
| `precursores()`     | Retorna los IDs de libros cuyos similares fueron publicados todos en años posteriores |

---

## Compilación rápida

**Desde cero (parsear XMLs + compilar + ejecutar):**

```bash
python parser.py && g++ -std=c++17 -o tree main.cpp tree.cpp && ./tree
```

**Solo compilar y ejecutar (si ya tienes `cleaned_books.txt`):**

```bash
g++ -std=c++17 -o tree main.cpp tree.cpp && ./tree
```

**Windows:**

```bash
python parser.py && g++ -std=c++17 -o tree main.cpp tree.cpp && tree.exe
g++ -std=c++17 -o tree main.cpp tree.cpp && tree.exe
```

---

## Ejemplo de salida

```
--- CARGANDO EL DATASET DE LIBROS AL ARBOL ---
[INFO] Proceso de carga finalizado. Nodos totales en el árbol: 10000
Tamaño inicial del árbol: 10000 libros.

--- EJECUTANDO CONSULTA: listar() ---
Total de IDs recuperados en PreOrder: 10000
Primeros 5 IDs del recorrido: 1 10 10006 1000751 10008056

--- EJECUTANDO CONSULTA: precursores() ---
Cantidad de libros que son precursores de sus similares: 402
Muestra de IDs precursores (primeros 5): 1015311 102327 1027760 102953 102954

--- EJECUTANDO CONSULTA: borrar_ratings(3.5) ---
Tamaño del árbol después del borrado masivo: 9647 libros.
Nuevo total de IDs en el árbol: 9647
```
