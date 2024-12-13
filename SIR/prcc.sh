#!/bin/bash

# Ingresar al directorio de construcción
cd build

# Ejecutar prcc
prcc 20

# Ver resultados de correlacion
gv prccvals.ps