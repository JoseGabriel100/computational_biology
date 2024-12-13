#!/bin/bash

# Eliminar el directorio de construcción anterior
rm -rf build

# Crear un nuevo directorio de construcción
mkdir build
cd build

# Copiar el generador del lhsmatrix
cp ../lhsmatrix .

# Configurar el proyecto con CMake
cmake ..

# Construir el proyecto
make

# Ejecutar la simulación
./SIRSimulation --no-gui