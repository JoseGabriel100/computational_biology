#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <mutex>
#include <cstdlib>
#include <tcl.h>
#include <tk.h>

// Enum para los estados del semáforo
enum TrafficLightState { GREEN, YELLOW, RED };

// Clase TrafficLight para representar un semáforo
class TrafficLight {
public:
    TrafficLight(int greenTime, int yellowTime, int redTime, TrafficLightState initialState, int offset)
        : greenTime(greenTime), yellowTime(yellowTime), redTime(redTime), state(initialState), offset(offset) {}

    // Actualizar el estado del semáforo basado en el tiempo actual
    void update(int currentTime) {
        int cycleTime = greenTime + yellowTime + redTime;
        int timeInCycle = (currentTime + offset) % cycleTime;

        if (timeInCycle < greenTime) {
            state = GREEN;
        } else if (timeInCycle < greenTime + yellowTime) {
            state = YELLOW;
        } else {
            state = RED;
        }
    }

    // Obtener el estado actual del semáforo
    TrafficLightState getState() const {
        return state;
    }

private:
    int greenTime;
    int yellowTime;
    int redTime;
    TrafficLightState state;
    int offset;
};

// Clase Car para representar un coche
class Car {
public:
    Car(int lane) : lane(lane), x(lane == 0 ? 0 : 200), y(lane == 1 ? 0 : 200), speed(rand() % 5 + 5) {}

    // Actualizar la posición del coche basado en el estado del semáforo y la posición del semáforo
    void update(TrafficLightState trafficLightState, int lightPosition) {
        bool hasReachedLight = (lane == 0 && x + speed >= lightPosition) || (lane == 1 && y + speed >= lightPosition);

        int currentSpeed = speed;
        if (trafficLightState == YELLOW) {
            currentSpeed /= 2;
        }

        if (trafficLightState == GREEN || !hasReachedLight || (lane == 0 && x > lightPosition) || (lane == 1 && y > lightPosition)) {
            if (lane == 0) {
                x += currentSpeed;
            } else if (lane == 1) {
                y += currentSpeed;
            }
        }
    }

    // Obtener la posición x del coche
    int getX() const { return x; }

    // Obtener la posición y del coche
    int getY() const { return y; }

private:
    int lane;
    int x;
    int y;
    int speed;
};

// Estructura para contener los datos de la simulación
struct SimulationData {
    std::vector<TrafficLight> trafficLights;
    std::vector<Car> cars;
    Tcl_Interp* interp; // Intérprete para la GUI
};

// Variables globales
std::atomic<bool> simulationRunning(true);
std::mutex mtx;

// Función para actualizar la simulación
void updateSimulation(SimulationData* data) {
    int startTime = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);

    while (simulationRunning) {
        int currentTime = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
        int elapsedTime = currentTime - startTime;
        int cycleTime = elapsedTime % 18;

        {
            std::lock_guard<std::mutex> lock(mtx);

            for (auto& light : data->trafficLights) {
                light.update(cycleTime);
            }

            for (auto& car : data->cars) {
                TrafficLightState lightState = data->trafficLights[car.getX() < 150 ? 0 : 1].getState();
                int lightPosition = 150;
                car.update(lightState, lightPosition);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Función para actualizar la GUI
void updateGUI(void* clientData) {
    SimulationData* data = reinterpret_cast<SimulationData*>(clientData);

    std::lock_guard<std::mutex> lock(mtx);

    // Limpiar el canvas
    Tcl_Eval(data->interp, ".canvas delete all");

    // Dibujar las carreteras
    Tcl_Eval(data->interp, ".canvas create rectangle 0 200 400 200 -fill gray"); // Carretera horizontal
    Tcl_Eval(data->interp, ".canvas create rectangle 200 0 200 400 -fill gray"); // Carretera vertical

    // Dibujar los semáforos
    for (size_t i = 0; i < data->trafficLights.size(); ++i) {
        int x = (i == 0 ? 150 : 200);
        int y = (i == 0 ? 200 : 150);
        TrafficLightState state = data->trafficLights[i].getState();
        const char* color = (state == GREEN) ? "green" : (state == YELLOW) ? "yellow" : "red";

        std::string command = ".canvas create oval " + std::to_string(x) + " " + std::to_string(y) + " " +
                              std::to_string(x + 20) + " " + std::to_string(y + 20) + " -fill " + color;
        Tcl_Eval(data->interp, command.c_str());
    }

    // Dibujar los coches
    for (const auto& car : data->cars) {
        int x = car.getX();
        int y = car.getY();
        std::string command = ".canvas create rectangle " + std::to_string(x) + " " + std::to_string(y) + " " +
                              std::to_string(x + 20) + " " + std::to_string(y + 10) + " -fill blue";
        Tcl_Eval(data->interp, command.c_str());
    }

    // Programar la siguiente actualización
    Tcl_CreateTimerHandler(100, updateGUI, clientData);
}

// Función para iniciar la simulación
int startSimulation(void* clientData, Tcl_Interp* interp, int argc, const char* argv[]) {
    SimulationData* data = reinterpret_cast<SimulationData*>(clientData);
    simulationRunning = true;
    std::thread simulationThread(updateSimulation, data);
    simulationThread.detach();
    Tcl_SetResult(interp, const_cast<char*>("Simulacion comenzada"), TCL_STATIC);
    return TCL_OK;
}

// Función para detener la simulación
int stopSimulation(void* clientData, Tcl_Interp* interp, int argc, const char* argv[]) {
    simulationRunning = false;
    Tcl_SetResult(interp, const_cast<char*>("Simulacion detenida"), TCL_STATIC);
    return TCL_OK;
}

int main(int argc, char* argv[]) {
    Tcl_Interp* interp = Tcl_CreateInterp();
    if (Tcl_Init(interp) == TCL_ERROR) {
        std::cerr << "Error initializing Tcl" << std::endl;
        return 1;
    }
    if (Tk_Init(interp) == TCL_ERROR) {
        std::cerr << "Error initializing Tk" << std::endl;
        return 1;
    }

    // Crear datos de la simulación
    SimulationData data = {
        {
            TrafficLight(10, 1, 7, GREEN, 0),
            TrafficLight(10, 1, 7, RED, 9)
        },
        {
            Car(0),
            Car(1),
            Car(0),
            Car(1)
        },
        interp
    };

    // Crear comandos
    Tcl_CreateCommand(interp, "startSimulation", startSimulation, reinterpret_cast<void*>(&data), NULL);
    Tcl_CreateCommand(interp, "stopSimulation", stopSimulation, reinterpret_cast<void*>(&data), NULL);

    if (Tk_MainWindow(interp) == NULL) {
        std::cerr << "Error creating main window" << std::endl;
        return 1;
    }

    // Configurar la GUI
    Tcl_Eval(interp, "wm title . {Traffic Simulation}");
    Tcl_Eval(interp, "button .start -text {Iniciar} -command {startSimulation}");
    Tcl_Eval(interp, "button .stop -text {Detener} -command {stopSimulation}");
    Tcl_Eval(interp, "canvas .canvas -width 400 -height 400 -bg white");
    Tcl_Eval(interp, "pack .start .stop .canvas");

    // Programar actualizaciones de la GUI
    Tcl_CreateTimerHandler(100, updateGUI, reinterpret_cast<void*>(&data));

    // Iniciar el bucle principal de Tk
    Tk_MainLoop();

    return 0;
}