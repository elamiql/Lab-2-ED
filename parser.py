import os
import xml.etree.ElementTree as ET


def limpiar(texto):
    if texto is None:
        return "N/A"
    limpio = texto.replace("\n", " ").replace("\r", " ").replace("|", " ")
    return " ".join(limpio.split())


def get_text(element, tag, default="0"):
    """findtext devuelve None si el tag no existe, o string vacio si existe pero esta vacio."""
    val = element.findtext(tag, default=default)
    return val.strip() if val and val.strip() else default


def barra_progreso(procesados, total, escritos, errores, ancho=40):
    porcentaje = procesados / total
    lleno      = int(ancho * porcentaje)
    barra      = "█" * lleno + "░" * (ancho - lleno)
    print(f"\r[{barra}] {procesados}/{total}  Correctos: {escritos}  Errores: {errores}", end="", flush=True)


def leer_xml(ruta):
    try:
        tree = ET.parse(ruta)
        root = tree.getroot()

        book = root.find("book")
        if book is None:
            return None

        id_libro = get_text(book, "id")
        titulo   = limpiar(get_text(book, "title", default="N/A"))
        isbn     = get_text(book, "isbn")
        anio     = get_text(book, "publication_year")
        idioma   = get_text(book, "language_code", default="N/A")
        desc     = limpiar(get_text(book, "description", default="Sin desc"))
        rating   = get_text(book, "average_rating", default="0.0")
        paginas  = get_text(book, "num_pages")

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
        print(f"\n[ERROR] XML malformado en {ruta}: {e}")
        return None
    except Exception as e:
        print(f"\n[ERROR] Fallo inesperado en {ruta}: {e}")
        return None


def main():
    carpeta    = "./books_xml"
    salida     = "cleaned_books.txt"
    salida_tmp = salida + ".tmp"

    archivos_xml   = sorted([f for f in os.listdir(carpeta) if f.endswith(".xml")])
    total_archivos = len(archivos_xml)

    print(f"[INFO] Parseando {total_archivos} archivos...")

    procesados = 0
    escritos   = 0
    errores    = 0

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

            barra_progreso(procesados, total_archivos, escritos, errores)

    print()  # salto de línea al terminar
    os.replace(salida_tmp, salida)
    print(f"[OK] Terminado. Procesados: {procesados} | Escritos: {escritos} | Errores: {errores}")
    print(f"[OK] Guardado en '{salida}'")


if __name__ == "__main__":
    main()