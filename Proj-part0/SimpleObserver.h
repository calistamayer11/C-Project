//
//  SimpleObserver.h
//
//
//  Created by Yufeng Wu on 12/1/22.
//  Demonstrate how a simple observer would work
//  This observer waits for the space bar key;
//  after the user presses (and releases) the space bar
//  it displays a rectangle in the middle of the window
//  That is it!
//  This code is meant to demonstrate the functionalities
//  especially the Observer pattern implemented in
//  ECGraphicViewImp.
//

#ifndef SimpleObserver_h
#define SimpleObserver_h

#include "ECObserver.h"
#include "ECGraphicViewImp.h"

//************************************************************
struct Passenger
{
    int floorIndex;    // Floor where the passenger is currently
    bool isOnElevator; // Flag to check if the passenger is on the elevator
    int x, y;          // Y position relative to the elevator
    bool isVisible;
};
class ECSimpleGraphicObserver : public ECObserver
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

#endif /* SimpleObserver_h */