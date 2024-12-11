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
double beta = 0.9;                   // Tasa de transmisión
double gamma_ = 0.1;                 // Tasa de recuperación
double mu = 0.0;                     // Tasa de mortalidad
const int numPeople = 100;           // Número de personas
const double infectionRadius = 25.0; // Radio de infección

// Global variables for LHS matrix
const int NVAR = 20;
const int NRUNS = 100;
double datalhs[NVAR + 1][NRUNS + 1];

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
    if (person.state == 'S')
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
      person.x += (rand() % 31 - 15); // Movimiento en x en el rango [-15, 15]
      person.y += (rand() % 31 - 15); // Movimiento en y en el rango [-15, 15]
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

// Function to read LHS matrix
void readLHSMatrix()
{
  std::ifstream lhsmatrix("lhsmatrix", std::ios::in);
  if (!lhsmatrix)
  {
    std::cerr << "Lhsmatrix file not found!" << std::endl;
    exit(1);
  }
  int nvar, nruns, x;
  lhsmatrix >> nvar >> nruns >> x >> x;
  if (nruns != NRUNS || nvar > NVAR)
  {
    std::cerr << "Check -nruns- or -nvar- values in lhsmatrix and this program" << std::endl;
    exit(1);
  }
  for (int i = 1; i <= nvar; i++)
  {
    for (int j = 1; j <= nruns; j++)
    {
      lhsmatrix >> datalhs[i][j];
    }
  }
  lhsmatrix.close();
}

// Function to run the simulation for a specific run
void runSimulation(int run, int print)
{
  // Set parameters from LHS matrix
  beta = datalhs[1][run];
  gamma_ = datalhs[2][run];
  mu = datalhs[3][run];
  // ...set other parameters as needed...

  // Initialize population
  SimulationData data;
  data.interp = Tcl_CreateInterp();
  initializePopulation(data, 5); // Example: 5 initial infected

  // Run the simulation for a specified number of steps
  for (int step = 0; step < 1000; ++step)
  {
    updatePopulation(data);
  }

  // Save output variables
  int susceptibleCount = 0, infectedCount = 0, recoveredCount = 0, deadCount = 0;
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

  // Print or save results
  if (print)
  {
    std::cout << "Run " << run << ": "
              << "S: " << susceptibleCount << ", "
              << "I: " << infectedCount << ", "
              << "R: " << recoveredCount << ", "
              << "D: " << deadCount << std::endl;
  }
  // Save to file
  std::ofstream stream(std::to_string(run) + ".txt");
  stream << "S: " << susceptibleCount << "\n"
         << "I: " << infectedCount << "\n"
         << "R: " << recoveredCount << "\n"
         << "D: " << deadCount << std::endl;
  stream.close();
}

// Main function
int main(int argc, char *argv[])
{
  int print = (argc >= 2) ? atoi(argv[1]) : 0;
  readLHSMatrix(); // Add this line to read the LHS matrix
  for (int run = 1; run <= NRUNS; ++run)
  {
    runSimulation(run, print);
  }
  return 0;
}