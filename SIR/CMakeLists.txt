cmake_minimum_required(VERSION 3.5)

project(SIRSimulation)

set(CMAKE_CXX_STANDARD 11)

# Especificar manualmente las rutas a las bibliotecas y directorios de inclusión de Tcl y Tk
set(TCL_INCLUDE_PATH "/usr/include/tcl")
set(TK_INCLUDE_PATH "/usr/include/tk")
set(TCL_LIBRARY "/usr/lib/x86_64-linux-gnu/libtcl.so")
set(TK_LIBRARY "/usr/lib/x86_64-linux-gnu/libtk.so")

include_directories(${TCL_INCLUDE_PATH} ${TK_INCLUDE_PATH})

add_executable(SIRSimulation main.cpp)

target_link_libraries(SIRSimulation ${TCL_LIBRARY} ${TK_LIBRARY})