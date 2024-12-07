#ifndef ECSimpleGraphicObserver_h
#define ECSimpleGraphicObserver_h

#include "ECObserver.h"
#include "ECGraphicViewImp.h"
#include <vector>

// Struct for passenger
struct Passenger
{
    int floorIndex;
    bool isOnElevator;
    int x, y; // Passenger's position
    bool isVisible;
};

// Elevator Observer Class
class ECSimpleGraphicObserver : public ECObserver
{
public:
    ECSimpleGraphicObserver(ECGraphicViewImp &viewIn);
    virtual void Update();

private:
    ECGraphicViewImp &view;
    int elevatorX, elevatorY;          // Elevator's current position
    int targetFloor;                   // Target floor the elevator is moving to
    int currentFloor;                  // Current floor the elevator is at
    int floorHeight;                   // Height per floor for visualization
    bool isMoving;                     // Is the elevator moving?
    bool isMovingUp;                   // Direction of movement
    int tickCount;                     // Timer ticks
    int ticksPerFloor;                 // How many ticks to move one floor
    std::vector<Passenger> passengers; // Passengers in the simulation

    void MoveElevator();   // Update elevator position based on timer ticks
    void DrawElevator();   // Draw the elevator on the screen
    void DrawPassengers(); // Draw passengers on the screen
};

#endif
