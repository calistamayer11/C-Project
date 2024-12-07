//
//  SimpleObserver.cpp
//
//
//  Created by Yufeng Wu on 12/1/22.
//

#include "SimpleObserver.h"
#include <cstdlib>
#include <ctime>

//************************************************************
ECSimpleGraphicObserver ::ECSimpleGraphicObserver(ECGraphicViewImp &viewIn) : view(viewIn), fIsSpaceBarPressed(false), signalLights(6, false), lightStates(6, false)
{
}
void ECSimpleGraphicObserver::Update()
{
    ECGVEventType evt = view.GetCurrEvent();
    const int floorHeights[] = {20, 170, 320, 470, 620, 770}; // Y positions of floors
    const int numFloors = sizeof(floorHeights) / sizeof(floorHeights[0]);
    const int floorHeight = 150; // height of floor
    const int passengerWidth = 15;
    const int passengerHeight = 30;
    const int passengerOffsetX = 460; // to draw passengers outside the elevator
    const int signalLightWidth = 15;
    const int signalLightHeight = 15;
    const int signalLightOffsetX = 420;
    const int signalLightVerticalOffset = 20;
    const int elevatorOffsetX = 40; // Position for passengers inside the elevator
    const int elevatorWidth = 50;   // Width of the elevator

    if (evt == ECGV_EV_KEY_UP_SPACE)
    {
        fIsSpaceBarPressed = true;

        // Calculate the closest floor
        int currentCenterY = (topLeftY + bottomRightY) / 2;
        int closestFloorIndex = 0;
        int closestDistance = abs(currentCenterY - floorHeights[0]);

        for (int i = 1; i < numFloors; ++i)
        {
            int distance = abs(currentCenterY - floorHeights[i]);
            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestFloorIndex = i;
            }
        }

        topLeftY = floorHeights[closestFloorIndex];
        bottomRightY = topLeftY + floorHeight;

        isMovingUp = false;
        isMovingDown = false;
        currentFloorIndex = closestFloorIndex;

        view.SetRedraw(true);
        return;
    }

    if (evt == ECGV_EV_KEY_UP_UP)
    {
        isMovingUp = true;
        fIsSpaceBarPressed = false;
    }
    if (evt == ECGV_EV_KEY_DOWN_DOWN)
    {
        isMovingDown = true;
        fIsSpaceBarPressed = false;
    }

    // When the right key is pressed, create a new passenger at a random floor
    if (evt == ECGV_EV_KEY_DOWN_RIGHT)
    {
        int randomFloorIndex = std::rand() % numFloors; // random floor index
        passengers.push_back({randomFloorIndex, false, passengerOffsetX, floorHeights[randomFloorIndex] + (floorHeight / 2) - (passengerHeight / 2)});

        if (randomFloorIndex == 0)
        {
            signalLights[randomFloorIndex] = true;
            lightStates[randomFloorIndex] = false;
        }
        else if (randomFloorIndex == numFloors - 1)
        {
            signalLights[randomFloorIndex] = true;
            lightStates[randomFloorIndex] = true;
        }
        else
        {
            bool lightOn = std::rand() % 2;
            signalLights[randomFloorIndex] = true;
            lightStates[randomFloorIndex] = lightOn;
        }

        view.SetRedraw(true);
    }

    // When the left key is pressed,  move the passenger into the elevator
    if (evt == ECGV_EV_KEY_DOWN_LEFT)
    {
        onElevator = true;
    }
    if (evt == ECGV_EV_KEY_UP_ESCAPE)
    {
        // move the passenger out of the elevator
        for (auto it = passengers.begin(); it != passengers.end(); ++it)
        {
            if (it->isOnElevator)
            {
                it->isOnElevator = false;
                it->x = -100;
                it->y = -100;
                passengers.erase(it);
                break;
            }
        }
        view.SetRedraw(true);
    }
    if (onElevator && !isMovingUp && !isMovingDown)
    {
        for (auto &passenger : passengers)
        {
            if (!passenger.isOnElevator && passenger.floorIndex == currentFloorIndex)
            {
                passenger.isOnElevator = true;
                passenger.x = elevatorOffsetX;
                passenger.y = topLeftY + (floorHeight / 2) - (passengerHeight / 2);
                signalLights[currentFloorIndex] = false;

                // passenger inside the elevator
                view.DrawRectangle(passenger.x, passenger.y, passenger.x + passengerWidth, passenger.y + passengerHeight, 3);
                view.DrawEllipse(passenger.x + passengerWidth / 2, passenger.y - passengerHeight / 2, passengerWidth / 2, passengerWidth / 2, 3);

                break;
            }
        }
        onElevator = false;
    }

    // Timer event for movement and drawing
    if (evt == ECGV_EV_TIMER)
    {
        for (int i = 0; i < numFloors; ++i)
        {
            view.DrawRectangle(20, floorHeights[i], 400, floorHeights[i] + floorHeight, 3);
            int firstLightY = floorHeights[i] + (floorHeight / 2) - signalLightHeight - (signalLightVerticalOffset / 2);
            int secondLightY = floorHeights[i] + (floorHeight / 2) + (signalLightVerticalOffset / 2);
            if (signalLights[i]) // If the light for this floor is on
            {
                if (lightStates[i]) // Check if it's the top light or the bottom light that should be on
                {
                    // Top light on
                    view.DrawFilledEllipse(signalLightOffsetX, firstLightY, signalLightWidth, signalLightHeight, ECGV_BLACK);
                    view.DrawEllipse(signalLightOffsetX, secondLightY, signalLightWidth, signalLightHeight, 3); // Bottom light off
                }
                else
                {
                    // Bottom light on
                    view.DrawFilledEllipse(signalLightOffsetX, secondLightY, signalLightWidth, signalLightHeight, ECGV_BLACK);
                    view.DrawEllipse(signalLightOffsetX, firstLightY, signalLightWidth, signalLightHeight, 3); // Top light off
                }
            }
            else
            {
                // If the light for this floor is off, both lights should be off
                view.DrawEllipse(signalLightOffsetX, firstLightY, signalLightWidth, signalLightHeight, 3);  // Top light off
                view.DrawEllipse(signalLightOffsetX, secondLightY, signalLightWidth, signalLightHeight, 3); // Bottom light off
            }
        }
    }
    view.DrawFilledRectangle(topLeftX, topLeftY, bottomRightX, bottomRightY, ECGV_CYAN);
    view.SetRedraw(true);

    if (isMovingUp)
    {
        topLeftY -= stepSize;
        bottomRightY -= stepSize;

        if (topLeftY <= floorHeights[0])
        {
            topLeftY = floorHeights[0];
            bottomRightY = topLeftY + floorHeight;
            isMovingUp = false;
        }

        view.DrawFilledRectangle(topLeftX, topLeftY, bottomRightX, bottomRightY, ECGV_CYAN);
        view.SetRedraw(true);
    }
    else if (isMovingDown)
    {
        topLeftY += stepSize;
        bottomRightY += stepSize;

        if (bottomRightY >= floorHeights[numFloors - 1] + floorHeight)
        {
            bottomRightY = floorHeights[numFloors - 1] + floorHeight;
            topLeftY = floorHeights[numFloors - 1];
            isMovingDown = false;
        }

        view.DrawFilledRectangle(topLeftX, topLeftY, bottomRightX, bottomRightY, ECGV_CYAN);
        view.SetRedraw(true);
    }
    else
    {
        view.DrawFilledRectangle(topLeftX, topLeftY, bottomRightX, bottomRightY, ECGV_CYAN);
        view.SetRedraw(true);
    }

    int elevatorPassengerOffsetX = topLeftX + 10;
    const int passengerSpacing = passengerWidth + 10;

    for (auto &passenger : passengers)
    {
        if (passenger.isOnElevator)
        {
            passenger.y = topLeftY + (floorHeight / 2) - (passengerHeight / 2);
            passenger.x = elevatorPassengerOffsetX;
            elevatorPassengerOffsetX += passengerSpacing;
        }

        view.DrawRectangle(passenger.x, passenger.y, passenger.x + passengerWidth, passenger.y + passengerHeight, 3);
        view.DrawEllipse(passenger.x + passengerWidth / 2, passenger.y - passengerHeight / 2, passengerWidth / 2, passengerWidth / 2, 3);
    }
    view.SetRedraw(true);
}