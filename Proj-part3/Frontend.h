#ifndef FRONTEND_H
#define FRONTEND_H

#include "ECGraphicViewImp.h"

//************************************************************
struct Passenger
{
    int floorIndex;    // Floor where the passenger is currently
    bool isOnElevator; // Flag to check if the passenger is on the elevator
    int x, y;          // Y position relative to the elevator
    bool isVisible;
};
class ECSimpleGraphicObserver
{
public:
    ECSimpleGraphicObserver(ECGraphicViewImp &viewIn);
    virtual void Update();

private:
    ECGraphicViewImp &view;
    bool fIsSpaceBarPressed;
    // adding new keys
    bool fIsUpArrowPressed;
    bool fIsDownArrowPressed;
    bool drawPassenger;
    bool onElevator;
    bool leave;
    int topLeftX = 20, topLeftY = 770;
    int bottomRightX = 400, bottomRightY = 920;
    bool isMovingUp = false;
    bool isMovingDown = false;
    int stepSize = 3;
    int minY = 20, maxY = 790;
    std::vector<Passenger> passengers;
    int currentFloorIndex;
    std::vector<bool> signalLights;
    std::vector<bool> lightStates;
};

#endif