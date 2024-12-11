# SIR Model Simulation

Este proyecto implementa una simulación del modelo SIR (Susceptible, Infectado, Recuperado) utilizando C++ y Tcl/Tk para la visualización. La simulación muestra cómo una enfermedad infecciosa se propaga en una población y permite visualizar el estado de cada individuo en la población.

## Requisitos

- C++11 o superior
- Tcl/Tk
- CMake

## Instalación

1. **Instalar los paquetes de desarrollo de Tcl/Tk**:
   ```sh
   sudo apt-get install tcl-dev tk-dev
   ```

2. **Clonar el repositorio del proyecto**:
   ```sh
   git clone https://github.com/tu-usuario/sir-simulation.git
   cd sir-simulation
   ```

3. **Crear un directorio de construcción y navegar a él**:
   ```sh
   mkdir build
   cd build
   ```

4. **Configurar el proyecto con CMake**:
   ```sh
   cmake ..
   ```

5. **Construir el proyecto**:
   ```sh
   make
   ```

## Ejecución

1. **Ejecutar la simulación**:
   ```sh
   ./SIRSimulation
   ```

2. **Interacción con la GUI**:
   - **Start Simulation**: Inicia la simulación.
   - **Stop Simulation**: Detiene la simulación.

## Descripción del Código

### Estructura del Proyecto

- `main.cpp`: Contiene la implementación principal de la simulación del modelo SIR.
- `CMakeLists.txt`: Archivo de configuración de CMake para construir el proyecto.
- `build.sh`: Script para configurar y construir el proyecto.

### Parámetros del Modelo SIR

- `beta`: Tasa de transmisión.
- `gamma_`: Tasa de recuperación.
- `mu`: Tasa de mortalidad.
- `dt`: Paso de tiempo.
- `numPeople`: Número de personas en la población.
- `infectionRadius`: Radio de infección.

### Funciones Principales

- `initializePopulation(SimulationData& data, int initialInfected)`: Inicializa la población con un número específico de personas infectadas.
- `updatePopulation(SimulationData& data)`: Actualiza el estado de la población en cada paso de tiempo.
- `updateGUI(void* clientData)`: Actualiza la GUI para reflejar el estado actual de la población.
- `startSimulation(ClientData clientData, Tcl_Interp* interp, int argc, const char* argv[])`: Inicia la simulación.
- `stopSimulation(ClientData clientData, Tcl_Interp* interp, int argc, const char* argv[])`: Detiene la simulación.

### Ejemplo de Uso

El siguiente fragmento de código muestra cómo inicializar la población y crear los comandos Tcl para iniciar y detener la simulación:

```cpp
// Inicializar la población con un número específico de personas infectadas
int initialInfected = 5; // Puedes cambiar este valor según sea necesario
initializePopulation(data, initialInfected);

// Crear comandos Tcl para actualizar la GUI
Tcl_CreateCommand(interp, "startSimulation", startSimulation, reinterpret_cast<void *>(&data), NULL);
Tcl_CreateCommand(interp, "stopSimulation", stopSimulation, reinterpret_cast<void *>(&data), NULL);
```
