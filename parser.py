import os
import xml.etree.ElementTree as ET


def limpiar(texto):
    if texto is None:
        return "N/A"
    # Quitar saltos de linea y separadores para no romper el formato pipe/hash/punto y coma
    limpio = texto.replace("\n", " ").replace("\r", " ").replace("|", " ")
    return " ".join(limpio.split())


def leer_xml(ruta):
    try:
        tree = ET.parse(ruta)
        root = tree.getroot()

        # Datos basicos del libro
        id_libro = root.findtext("id", default="0")
        titulo    = limpiar(root.findtext("title", default="N/A"))
        isbn      = root.findtext("isbn", default="0")
        anio      = root.findtext("publication_year", default="0")
        idioma    = root.findtext("language_code", default="N/A")
        desc      = limpiar(root.findtext("description", default="Sin desc"))
        rating    = root.findtext("average_rating", default="0.0")
        paginas   = root.findtext("num_pages", default="0")

        # Libros similares
        # Separador anidado: campos internos con ";" y libros entre si con "#"
        # Se sanitizan ";" y "#" en los titulos para no romper el formato
        similares = []
        tag_similares = root.find("similar_books")

        if tag_similares is not None:
            for b in tag_similares.findall("book"):
                t = limpiar(b.findtext("title", default="N/A")).replace(";", " ").replace("#", " ")
                i = b.findtext("isbn", default="0")
                a = b.findtext("publication_year", default="0")
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
    carpeta      = "./data_xml"
    salida       = "cleaned_books.txt"
    salida_tmp   = salida + ".tmp"

    archivos_xml = sorted(
        [f for f in os.listdir(carpeta) if f.endswith(".xml")]
    )
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