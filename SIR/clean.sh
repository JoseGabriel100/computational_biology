#!/bin/bash

# Lista de archivos a conservar
keep_files=("build.sh" "clean.sh" "genlhsmatrix.txt" "lhsdata" "lhsmatrix" "lhsoutcome" "main.cpp" "CMakeLists.txt" "prcc.sh" "output.pdf" "Readme.md")

# Eliminar todos los archivos excepto los especificados en la lista y omitir la carpeta assets
for file in *; do
    if [[ ! " ${keep_files[@]} " =~ " ${file} " ]] && [[ "$file" != "assets" ]]; then
        rm -rf "$file"
    fi
done

rm -f output1.pdf

echo "Limpieza completada. Se han conservado los archivos especificados y la carpeta assets."