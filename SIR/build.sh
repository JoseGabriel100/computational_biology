#!/bin/bash

# Ejecutar el generador del lhsmatrix
crun genlhsmatrix.txt

# Ejecutar el script de limpieza
./clean.sh

# Eliminar el directorio de construcción anterior
rm -rf build

# Crear un nuevo directorio de construcción
mkdir build
cd build

# Copiar el generador del lhsmatrix
cp ../lhsmatrix .
cp ../lhsdata .
cp ../lhsoutcome .

# Configurar el proyecto con CMake
cmake ..

# Construir el proyecto
make

# Ejecutar la simulación
./SIRSimulation --no-gui