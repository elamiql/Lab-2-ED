import os
import xml.etree.ElementTree as ET


def limpiar(texto):
    if texto is None:
        return "N/A"
    # Quitar saltos de linea y separadores para no romper el formato pipe/hash/punto y coma
    limpio = texto.replace("\n", " ").replace("\r", " ").replace("|", " ")
    return " ".join(limpio.split())


def get_text(element, tag, default="0"):
    """findtext devuelve None si el tag no existe, o string vacio si existe pero esta vacio."""
    val = element.findtext(tag, default=default)  # findtext, sin guion bajo
    return val.strip() if val and val.strip() else default


def leer_xml(ruta):
    try:
        tree = ET.parse(ruta)
        root = tree.getroot()

        book = root.find("book")
        if book is None:
            print(f"[WARN] Sin tag <book> en {ruta}")
            return None

        # Datos basicos del libro
        id_libro = get_text(book, "id")
        titulo   = limpiar(get_text(book, "title", default="N/A"))
        isbn     = get_text(book, "isbn")
        anio     = get_text(book, "publication_year")
        idioma   = get_text(book, "language_code", default="N/A")
        desc     = limpiar(get_text(book, "description", default="Sin desc"))
        rating   = get_text(book, "average_rating", default="0.0")
        paginas  = get_text(book, "num_pages")

        # Libros similares
        # Separador anidado: campos internos con ";" y libros entre si con "#"
        # Se sanitizan ";" y "#" en los titulos para no romper el formato
        similares = []
        tag_similares = book.find("similar_books")

        if tag_similares is not None:
            for b in tag_similares.findall("book"):
                t = limpiar(get_text(b, "title", default="N/A")).replace(";", " ").replace("#", " ")
                i = get_text(b, "isbn")
                a = get_text(b, "publication_year")
                similares.append(f"{t};{i};{a}")

        str_similares = "#".join(similares) if similares else "0"

        return f"{id_libro}|{titulo}|{isbn}|{anio}|{idioma}|{desc}|{rating}|{paginas}|{str_similares}\n"

    except ET.ParseError as e:
        print(f"[ERROR] XML malformado en {ruta}: {e}")
        return None
    except Exception as e:
        print(f"[ERROR] Fallo inesperado en {ruta}: {e}")
        return None


def main():
    carpeta    = "./books_xml"
    salida     = "cleaned_books.txt"
    salida_tmp = salida + ".tmp"

    archivos_xml   = sorted([f for f in os.listdir(carpeta) if f.endswith(".xml")])
    total_archivos = len(archivos_xml)

    print(f"[INFO] Empezando parseo — {total_archivos} archivos encontrados")

    procesados = 0
    escritos   = 0
    errores    = 0

    # Se escribe en .tmp primero; si el script se interrumpe no se pierde el archivo anterior
    with open(salida_tmp, "w", encoding="utf-8") as f:
        for archivo in archivos_xml:
            ruta  = os.path.join(carpeta, archivo)
            linea = leer_xml(ruta)
            procesados += 1

            if linea:
                f.write(linea)
                escritos += 1
            else:
                errores += 1

            if procesados % 1000 == 0:
                print(f"[PROGRESO] {procesados}/{total_archivos} — escritos: {escritos}, errores: {errores}")

    # Solo reemplaza el archivo final si todo termino sin excepcion
    os.replace(salida_tmp, salida)
    print(f"[OK] Terminado. Procesados: {procesados} | Escritos: {escritos} | Errores: {errores}")
    print(f"[OK] Guardado en '{salida}'")


if __name__ == "__main__":
    main()