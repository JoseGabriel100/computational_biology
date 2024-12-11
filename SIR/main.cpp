#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <tcl.h>
#include <tk.h>
#include <atomic>
#include <thread>
#include <mutex>

// Estructura para representar una persona
struct Person
{
  double x, y; // Posición
  char state;  // 'S' para susceptible, 'I' para infectado, 'R' para recuperado, 'D' para fallecido
};

// Estructura para contener la población y el intérprete de Tcl
struct SimulationData
{
  std::vector<Person> people;
  Tcl_Interp *interp;
};

// Parámetros del modelo SIR
const double beta = 0.9;             // Tasa de transmisión
const double gamma_ = 0.1;           // Tasa de recuperación
const double mu = 0.0;               // Tasa de mortalidad
const int numPeople = 100;           // Número de personas
const double infectionRadius = 25.0; // Radio de infección

// Variables globales para controlar la simulación
std::atomic<bool> simulationRunning(false);
std::mutex mtx;

// Función para inicializar la población
void initializePopulation(SimulationData &data, int initialInfected)
{
  data.people.clear();
  for (int i = 0; i < numPeople; ++i)
  {
    Person person;
    person.x = rand() % 500;
    person.y = rand() % 500;
    person.state = (i < initialInfected) ? 'I' : 'S'; // Inicializar con el número especificado de personas infectadas
    data.people.push_back(person);
  }
}

// Función para actualizar el estado de la población
void updatePopulation(SimulationData &data)
{
  int susceptibleCount = 0;
  int infectedCount = 0;
  int recoveredCount = 0;
  int deadCount = 0;

  for (auto &person : data.people)
  {
    if (person.state == 'I')
    {
      // Recuperación
      if ((rand() % 100) < (gamma_ * 100))
      {
        person.state = 'R';
      }
      // Muerte
      else if ((rand() % 100) < (mu * 100))
      {
        person.state = 'D';
      }
    }
  }

  for (auto &person : data.people)
  {
    if (person.state == 'S' || person.state == 'R')
    {
      for (const auto &other : data.people)
      {
        if (other.state == 'I')
        {
          double distance = std::sqrt(std::pow(person.x - other.x, 2) + std::pow(person.y - other.y, 2));
          if (distance < infectionRadius)
          {
            if ((rand() % 100) < (beta * 100))
            {
              person.state = 'I';
              break;
            }
          }
        }
      }
    }
  }

  // Movimiento aleatorio con distribución uniforme
  for (auto &person : data.people)
  {
    if (person.state != 'D')
    {                                 // Solo las personas vivas se mueven
      person.x += (rand() % 51 - 25); // Movimiento en x en el rango [-15, 15]
      person.y += (rand() % 51 - 25); // Movimiento en y en el rango [-15, 15]
      if (person.x < 0)
        person.x = 0;
      if (person.x > 500)
        person.x = 500;
      if (person.y < 0)
        person.y = 0;
      if (person.y > 500)
        person.y = 500;
    }
  }

  // Contar el número de personas en cada estado
  for (const auto &person : data.people)
  {
    switch (person.state)
    {
    case 'S':
      susceptibleCount++;
      break;
    case 'I':
      infectedCount++;
      break;
    case 'R':
      recoveredCount++;
      break;
    case 'D':
      deadCount++;
      break;
    }
  }

  // Imprimir los resultados
  std::cout << "Susceptible: " << susceptibleCount
            << ", Infected: " << infectedCount
            << ", Recovered: " << recoveredCount
            << ", Dead: " << deadCount << std::endl;
}

// Función para actualizar la GUI
void updateGUI(void *clientData)
{
  SimulationData *data = reinterpret_cast<SimulationData *>(clientData);
  Tcl_Interp *interp = data->interp;

  // Actualizar la población
  if (simulationRunning)
  {
    updatePopulation(*data);
  }

  // Actualizar la GUI
  Tcl_Eval(interp, ".canvas delete all");
  for (const auto &person : data->people)
  {
    std::string color;
    switch (person.state)
    {
    case 'S':
      color = "blue";
      break;
    case 'I':
      color = "red";
      break;
    case 'R':
      color = "green";
      break;
    case 'D':
      color = "gray";
      break;
    }
    std::string command = ".canvas create oval " + std::to_string(person.x - 5) + " " + std::to_string(person.y - 5) + " " +
                          std::to_string(person.x + 5) + " " + std::to_string(person.y + 5) + " -fill " + color;
    Tcl_Eval(interp, command.c_str());

    // Dibujar el radio de infección para las personas infectadas
    if (person.state == 'I')
    {
      std::string radiusCommand = ".canvas create oval " + std::to_string(person.x - infectionRadius) + " " +
                                  std::to_string(person.y - infectionRadius) + " " +
                                  std::to_string(person.x + infectionRadius) + " " +
                                  std::to_string(person.y + infectionRadius) + " -outline red";
      Tcl_Eval(interp, radiusCommand.c_str());
    }
  }

  // Programar la siguiente actualización
  Tcl_CreateTimerHandler(100, updateGUI, clientData);
}

// Función para iniciar la simulación
int startSimulation(ClientData clientData, Tcl_Interp *interp, int argc, const char *argv[])
{
  simulationRunning = true;
  Tcl_SetResult(interp, const_cast<char *>("Simulation Running"), TCL_STATIC);
  return TCL_OK;
}

// Función para detener la simulación
int stopSimulation(ClientData clientData, Tcl_Interp *interp, int argc, const char *argv[])
{
  simulationRunning = false;
  Tcl_SetResult(interp, const_cast<char *>("Simulation Stopped"), TCL_STATIC);
  return TCL_OK;
}

// Función principal
int main(int argc, char *argv[])
{
  Tcl_Interp *interp = Tcl_CreateInterp();
  if (Tcl_Init(interp) == TCL_ERROR)
  {
    std::cerr << "Error initializing Tcl" << std::endl;
    return 1;
  }
  if (Tk_Init(interp) == TCL_ERROR)
  {
    std::cerr << "Error initializing Tk" << std::endl;
    return 1;
  }

  // Crear datos de la simulación
  SimulationData data;
  data.interp = interp;

  // Inicializar la población con un número específico de personas infectadas
  int initialInfected = 5; // Puedes cambiar este valor según sea necesario
  initializePopulation(data, initialInfected);

  // Crear comandos Tcl para actualizar la GUI
  Tcl_CreateCommand(interp, "startSimulation", startSimulation, reinterpret_cast<void *>(&data), NULL);
  Tcl_CreateCommand(interp, "stopSimulation", stopSimulation, reinterpret_cast<void *>(&data), NULL);

  // Configurar la GUI
  Tcl_Eval(interp, "wm title . {SIR Model Simulation}");
  Tcl_Eval(interp, "canvas .canvas -width 500 -height 500 -bg white");
  Tcl_Eval(interp, "button .start -text {Start Simulation} -command {startSimulation}");
  Tcl_Eval(interp, "button .stop -text {Stop Simulation} -command {stopSimulation}");
  Tcl_Eval(interp, "pack .start .stop .canvas");

  // Programar la primera actualización de la GUI
  Tcl_CreateTimerHandler(100, updateGUI, reinterpret_cast<void *>(&data));

  // Iniciar el bucle principal de Tk
  Tk_MainLoop();

  return 0;
}