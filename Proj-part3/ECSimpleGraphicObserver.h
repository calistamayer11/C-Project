#ifndef ECSimpleGraphicObserver_h
#define ECSimpleGraphicObserver_h

#include "ECObserver.h"
#include "ECGraphicViewImp.h"
#include "ECElevatorSim.h"
#include <vector>
#include <string>

// Struct for passenger
struct Passenger
{
    int floorIndex;    // Floor where the passenger is currently
    bool isOnElevator; // Flag to check if the passenger is on the elevator
    int x, y;          // Passenger's position relative to the elevator
    bool isVisible;    // If the passenger is visible on the screen
};

// Elevator Observer Class
class ECSimpleGraphicObserver : public ECObserver
{
public:
    // Constructor: Takes a reference to the graphic view and elevator simulation
    ECSimpleGraphicObserver(ECGraphicViewImp &viewIn, ECElevatorSim &elevatorSimIn);

    // Main update function to process events and render UI
    virtual void Update();

private:
    ECGraphicViewImp &view;     // Reference to the graphical view
    ECElevatorSim &elevatorSim; // Reference to the elevator simulation
    // Elevator rendering properties
    int elevatorX, elevatorY;  // Elevator's current position
    int elevatorWidth = 50;    // Width of the elevator
    int elevatorHeight = 50;   // Height of the elevator
    int elevatorSpeed = 3;     // Speed of the elevator movement
    int currentFloor;          // Current floor the elevator is at
    bool isMovingDown = false; // Whether the elevator is moving
    bool isMovingUp = false;   // Direction of movement

    // UI rendering properties
    int floorHeight = 150; // Height of each floor
    // int floorSpacing = 20; // Spacing between floors in the UI
    int minY = 20;  // Minimum Y position
    int maxY = 790; // Maximum Y position
    // int topLeftX = 20, topLeftY = 770;
    // int bottomRightX = 400, bottomRightY = 920;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;
    int stepSize = 5;
    // Dynamic Floor Heights
    std::vector<int> floorHeights; // Store Y positions of floors dynamically

    // Passenger management
    std::vector<Passenger> passengers; // Passengers in the simulation
    int passengerWidth = 15;           // Width of the passenger
    int passengerHeight = 30;          // Height of the passenger

    // Internal timer variables
    int tickCount = 0;      // Timer tick counter
    int ticksPerFloor = 10; // How many ticks to move one floor

    // Signal lights and UI states
    std::vector<bool> signalLights; // Signal lights for each floor
    std::vector<bool> lightStates;  // Light states for each floor (top or bottom light)

    // Elevator control and UI update methods
    void MoveElevator();     // Move the elevator based on the timer ticks
    void DrawElevator();     // Draw the elevator on the screen
    void DrawPassengers();   // Draw passengers on the screen
    void DrawFloors();       // Draw floor lines and labels on the screen
    void DrawSignalLights(); // Draw signal lights for each floor

    // Helper functions for processing events
    void ProcessEvent(const std::string &event); // Process each event from the history vector
};

#endif
