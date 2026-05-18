# Entregable 2: Procesador de Dataset GoodReads (Árbol General)

Este proyecto procesa un dataset de 10.000 archivos XML de reseñas de libros de GoodReads, limpia y estructura la información mediante un script en Python, y posteriormente construye y consulta un árbol general en C++.

## Parser e Integración (Python)

Para optimizar el rendimiento y evitar dependencias complejas en C++, se desarrolló un script extractor que consolida los 10.000 archivos XML en un único archivo plano de texto estructurado (`cleaned_books.txt`) escrito en Python.

### Requisitos previos

- Python 3.x instalado.
- Los 10.000 archivos XML deben estar dentro de una carpeta llamada `books_xml` en la raíz del proyecto.

### Instrucciones de ejecución

Para ejecutar el parser y limpiar los datos, ejecute el siguiente comando en la terminal:

```bash
python parser.py
```

### Archivo de Salida

El archivo generado contiene exactamente una línea por libro, utilizamos delimitadores específicos para evitar conflictos con los textos:

- `|` Pipe: separa los campos principales del libro.
- `#` Numeral: separa cada libro similar dentro del listado.
- `;` Punto y coma: separa los subcampos de un libro similar (Título;ISBN;Año).

### Formato de línea

`ID|Título|ISBN|Año|Idioma|Descripción|Rating|Páginas|TítuloSim1;ISBNSim1;AñoSim1#TítuloSim2;ISBNSim2;AñoSim2`

## Compilación y Ejecución en C++

### Instrucciones de Compilación

El código de C++ se compila utilizando el estándar C++11 o superior. Ejecute el siguiente comando en su terminal.

`g++ -std=c++11 main.cpp -o programa`

### Instrucciones de ejecución

Una vez compilado, ejecute el binario asegurándose de que el archivo `cleaned_books.txt` se encuentra en el mismo directorio:

```bash
./programa
```
