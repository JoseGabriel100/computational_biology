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
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unistd.h> // Para getcwd

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
const double dt = 0.1;               // Paso de tiempo
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
void updatePopulation(SimulationData &data, double beta, double gamma_, double mu, int &susceptibleCount, int &infectedCount, int &recoveredCount, int &deadCount)
{
  susceptibleCount = 0;
  infectedCount = 0;
  recoveredCount = 0;
  deadCount = 0;

  // Primero, procesar las transiciones de estado
  std::vector<char> newStates(data.people.size());
  for (size_t i = 0; i < data.people.size(); ++i)
  {
    newStates[i] = data.people[i].state;
    if (data.people[i].state == 'I')
    {
      // Recuperación
      if ((rand() % 100) < (gamma_ * 100))
      {
        newStates[i] = 'R';
      }
      // Muerte
      else if ((rand() % 100) < (mu * 100))
      {
        newStates[i] = 'D';
      }
    }
  }

  // Luego, procesar las infecciones
  for (size_t i = 0; i < data.people.size(); ++i)
  {
    if (newStates[i] == 'S' || newStates[i] == 'R')
    {
      for (const auto &other : data.people)
      {
        if (other.state == 'I')
        {
          double distance = std::sqrt(std::pow(data.people[i].x - other.x, 2) + std::pow(data.people[i].y - other.y, 2));
          if (distance < infectionRadius)
          {
            if ((rand() % 100) < (beta * 100))
            {
              newStates[i] = 'I';
              break;
            }
          }
        }
      }
    }
  }

  // Aplicar los nuevos estados
  for (size_t i = 0; i < data.people.size(); ++i)
  {
    data.people[i].state = newStates[i];
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
}

// Función para actualizar la GUI
void updateGUI(void *clientData)
{
  SimulationData *data = reinterpret_cast<SimulationData *>(clientData);
  Tcl_Interp *interp = data->interp;

  // Obtener los valores de beta, gamma_ y mu desde los datos de la simulación
  double beta = 0.9;   // Valor por defecto, debería ser actualizado
  double gamma_ = 0.1; // Valor por defecto, debería ser actualizado
  double mu = 0.0;     // Valor por defecto, debería ser actualizado

  // Actualizar la población
  if (simulationRunning)
  {
    int susceptibleCount, infectedCount, recoveredCount, deadCount;
    updatePopulation(*data, beta, gamma_, mu, susceptibleCount, infectedCount, recoveredCount, deadCount);
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

// Función para ejecutar la simulación sin GUI
void runSimulationWithoutGUI(SimulationData &data, int initialInfected, double **datalhs, int nvar, int nruns)
{
  // Ejecutar la simulación para cada conjunto de parámetros
  for (int run = 1; run <= nruns; ++run)
  {
    double beta = datalhs[0][run];
    double gamma_ = datalhs[1][run];
    double mu = datalhs[2][run];

    // Inicializar la población para cada ejecución
    initializePopulation(data, initialInfected);

    // Crear un archivo para guardar los resultados de esta ejecución
    std::ostringstream filename;
    filename << std::setw(4) << std::setfill('0') << run;
    std::ofstream outfile(filename.str());
    if (!outfile)
    {
      std::cerr << "Error: No se pudo crear el archivo " << filename.str() << std::endl;
      return;
    }

    // Ejecutar la simulación para el conjunto de parámetros actual
    for (int t = 0; t <= 100; ++t)
    { // Simulación de 100 pasos de tiempo
      int susceptibleCount, infectedCount, recoveredCount, deadCount;
      updatePopulation(data, beta, gamma_, mu, susceptibleCount, infectedCount, recoveredCount, deadCount);

      // Guardar los resultados en el archivo
      outfile << t << "\t" << susceptibleCount << "\t" << infectedCount << "\t" << recoveredCount << "\t" << deadCount << "\n";
    }

    outfile.close();

    // Imprimir los resultados para el conjunto de parámetros actual
    std::cout << "Run " << run << ": beta=" << beta << ", gamma_=" << gamma_ << ", mu=" << mu << std::endl;
  }
}

// Función principal
int main(int argc, char *argv[])
{
  bool showGUI = true;
  if (argc > 1 && std::string(argv[1]) == "--no-gui")
  {
    showGUI = false;
  }

  // Crear datos de la simulación
  SimulationData data;

  // Leer el archivo lhsmatrix y ejecutar la simulación para cada conjunto de parámetros
  std::ifstream lhsmatrix("lhsmatrix", std::ios::in);
  if (!lhsmatrix)
  {
    std::cerr << "Error: Lhsmatrix file not found!" << std::endl;
    return 1;
  }

  int nvar, nruns;
  lhsmatrix >> nvar >> nruns;

  // Crear matriz dinámica para almacenar los datos de LHS
  double **datalhs = new double *[nvar];
  for (int i = 0; i < nvar; ++i)
  {
    datalhs[i] = new double[nruns + 1];
  }

  // Ignorar la primera fila del archivo
  std::string line;
  std::getline(lhsmatrix, line);

  for (int x = 0; x < nvar; x++)
  {
    for (int y = 1; y <= nruns; y++)
    {
      lhsmatrix >> datalhs[x][y];
    }
  }
  lhsmatrix.close();

  // Ejecutar la simulación sin GUI
  int initialInfected = 10; // Puedes cambiar este valor según sea necesario
  runSimulationWithoutGUI(data, initialInfected, datalhs, nvar, nruns);

  if (showGUI)
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
    data.interp = interp;

    // Inicializar la población con un número específico de personas infectadas
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
  }

  // Liberar memoria de la matriz dinámica
  for (int i = 0; i < nvar; ++i)
  {
    delete[] datalhs[i];
  }
  delete[] datalhs;

  return 0;
}