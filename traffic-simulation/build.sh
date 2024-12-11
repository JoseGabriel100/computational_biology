#!/bin/bash

# Eliminar el directorio de construcción anterior
rm -rf build

# Crear un nuevo directorio de construcción
mkdir build
cd build

# Configurar el proyecto con CMake
cmake ..

# Construir el proyecto
make

./TrafficSimulation