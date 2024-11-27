#include <iostream>
#include <string>
#include <vector>
#include <cstdlib> // For random numbers
#include <ctime>   // For seeding random

enum class State {
    Idle,
    Moving,
    Searching
};

class Agent {
private:
    int x, y; // Position in 2D space
    State currentState;

public:
    Agent(int startX, int startY) : x(startX), y(startY), currentState(State::Idle) {}

    void move(int width, int height) {
        if (currentState == State::Moving) {
            // Move to a random adjacent cell
            x += (rand() % 3) - 1; // -1, 0, or 1
            y += (rand() % 3) - 1;

            // Keep position within bounds
            x = std::max(0, std::min(width - 1, x));
            y = std::max(0, std::min(height - 1, y));

            std::cout << "Agent moved to (" << x << ", " << y << ")\n";

            // Example rule: if reaches (5,5), switch to Searching
            if (x == 5 && y == 5) {
                changeState(State::Searching);
            }
        }
    }

    void changeState(State newState) {
        currentState = newState;
        std::cout << "State changed to: " << stateToString(currentState) << "\n";
    }

    void act() {
        if (currentState == State::Searching) {
            std::cout << "Agent is searching at (" << x << ", " << y << ")\n";
            // After "searching," go back to Idle
            changeState(State::Idle);
        }
    }

    std::string stateToString(State state) {
        switch (state) {
            case State::Idle: return "Idle";
            case State::Moving: return "Moving";
            case State::Searching: return "Searching";
            default: return "Unknown";
        }
    }

    void startMoving() {
        if (currentState == State::Idle) {
            changeState(State::Moving);
        }
    }
};

class Environment {
private:
    int width, height;
    std::vector<Agent> agents;

public:
    Environment(int w, int h) : width(w), height(h) {}

    void addAgent(Agent agent) {
        agents.push_back(agent);
    }

    void update() {
        for (auto& agent : agents) {
            agent.startMoving(); // Ensure agents can start moving if idle
            agent.move(width, height);
            agent.act();
        }
    }
};

int main() {
    srand(static_cast<unsigned>(time(0))); // Seed for random numbers

    Environment env(10, 10); // Create a 10x10 grid

    // Create agents
    Agent agent1(0, 0);
    Agent agent2(9, 9);

    // Add agents to the environment
    env.addAgent(agent1);
    env.addAgent(agent2);

    // Simulate for a few steps
    for (int step = 0; step < 10; ++step) {
        std::cout << "Step " << step + 1 << ":\n";
        env.update();
        std::cout << "-------------------------\n";
    }

    return 0;
}
