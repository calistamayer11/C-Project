#include "ECSimpleGraphicObserver.h"
#include "ECElevatorSim.h"
#include <thread>
#include <chrono>

// compile with:  g++ ECGraphicViewImp.cpp ECElevatorSim.cpp ECSimpleGraphicObserver.cpp $(pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 --libs --cflags) -o ElevatorSim -std=c++17
ECSimpleGraphicObserver::ECSimpleGraphicObserver(ECGraphicViewImp &viewIn, ECElevatorSim &elevatorSimIn)
    : view(viewIn), elevatorSim(elevatorSimIn), signalLights(elevatorSim.GetNumFloors(), false), lightStates(elevatorSim.GetNumFloors(), false)
{
    int floorHeight = 120; // Height per floor
    floorHeights.resize(elevatorSim.GetNumFloors());

    for (int i = 0; i < elevatorSim.GetNumFloors(); ++i)
    {
        floorHeights[i] = (i * (floorHeight)); // Starting Y-position for each floor
    }
}
void ECSimpleGraphicObserver::Update()
{
    const int passengerOffsetX = 460;
    static size_t currentHistoryIndex = 0; // Keeps track of the current event in the history vector
    const int floorWidth = 400;            // Width of each floor
    int floorHeight = 120;                 // Height of each floor
    for (int i = 0; i < elevatorSim.GetNumFloors(); ++i)
    {
        view.DrawRectangle(20, floorHeights[i], floorWidth, floorHeights[i] + floorHeight, 3);
    }
    int elevatorWidth = 400;  // Elevator width
    int elevatorHeight = 100; // Elevator height
    int elevatorX = 0;        // Elevator's X position (horizontal position)

    // Initial position for the elevator
    if (currentHistoryIndex == 0)
    {
        topLeftY = floorHeights[elevatorSim.GetNumFloors() - 1];
        bottomRightY = topLeftY + elevatorHeight;
    }

    int topLeftX = elevatorX;
    int bottomRightX = topLeftX + elevatorWidth;

    view.DrawFilledRectangle(topLeftX, topLeftY, bottomRightX, bottomRightY, ECGV_CYAN);
    // Ensure there are events to process
    if (currentHistoryIndex < elevatorSim.GetHistory().size())
    {
        // Retrieve the current event from history
        std::string currentEvent = elevatorSim.GetHistory()[currentHistoryIndex];

        int currentFloor = -1;
        if (currentEvent.find("Floor:") != std::string::npos)
        {
            std::cout << currentEvent << std::endl;
            size_t floorPos = currentEvent.find("Floor:") + 6;    // Find the position where the floor number starts
            std::string floorStr = currentEvent.substr(floorPos); // Extract the full substring after "Floor:"

            // Trim spaces from the extracted floor string
            floorStr = std::string(floorStr.begin(), std::find_if(floorStr.rbegin(), floorStr.rend(), [](unsigned char ch)
                                                                  { return !std::isspace(ch); })
                                                         .base());
            try
            {
                currentFloor = std::stoi(floorStr); // Convert the entire string to an integer
                // std::cout << currentFloor << std::endl;
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << "Error: Invalid floor number in event: " << currentEvent << std::endl;
            }
        }
        // Define constants for drawing
        const int passengerWidth = 15;
        const int passengerHeight = 30;
        const int passengerOffsetX = 460; // Position for passengers outside the elevator

        // If the event is related to floor movement (up/down)
        // If the event is related to moving up
        if (currentEvent.find("Moving UP") != std::string::npos)
        {
            isMovingUp = true;    // Set the flag for moving up
            isMovingDown = false; // Ensure the down flag is false

            float targetFloorHeight = floorHeights[currentFloor - 1]; // Get the current target floor height

            // Calculate the distance to the target floor
            float distanceToNextFloor = topLeftY - targetFloorHeight;

            // Move the elevator in small steps towards the next floor
            if (distanceToNextFloor > 0)
            {
                // Move the elevator by the stepSize or the remaining distance to the floor, whichever is smaller
                float moveDistance = std::min(static_cast<float>(stepSize), distanceToNextFloor);
                topLeftY -= moveDistance; // move the elevator up
                bottomRightY -= moveDistance;
                distanceToNextFloor = topLeftY - targetFloorHeight;
                // Draw the elevator at its new position
                view.DrawFilledRectangle(topLeftX, topLeftY, bottomRightX, bottomRightY, ECGV_CYAN);
                view.SetRedraw(true);
            }
            else
            {
                // Once the elevator reaches the next floor, stop and set its position exactly on the floor
                topLeftY = floorHeights[0];
                bottomRightY = topLeftY + floorHeight;

                // Move to the next floor in the list
                floorHeights.erase(floorHeights.begin()); // Remove the current floor

                // If there are still more floors, continue moving up
                if (!floorHeights.empty())
                {
                    // Set the elevator to keep moving upwards if there are more floors to go
                    isMovingUp = true;
                }
                else
                {
                    // If no more floors, stop the elevator
                    isMovingUp = false;
                }
            }

            // Add a delay to control the speed of the elevator
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            // float targetFloorHeight = floorHeights[currentFloor] - stepSize;
            // while (topLeftY > targetFloorHeight) // Check if the elevator is above the target floor
            // {
            //     // Move the elevator up by a smaller increment for smoother movement
            //     // float moveDistance = std::min(static_cast<float>(stepSize), topLeftY - targetFloorHeight); // Avoid overshooting
            //     // topLeftY -= moveDistance;
            //     // bottomRightY -= moveDistance;
            //     topLeftY -= stepSize;
            //     bottomRightY -= stepSize;
            //     // Draw the elevator at its new position
            //     view.DrawFilledRectangle(topLeftX, topLeftY, bottomRightX, bottomRightY, ECGV_CYAN);
            //     view.SetRedraw(true);                                       // Trigger a redraw after updating the position
            //     std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Adjust the time to control the speed (smaller value for smoother movement)
            // }

            // Once the elevator reaches the next floor, set its position to exactly the next floor height
            // topLeftY = floorHeights[currentFloor]; // Set position to the current floor height
            // bottomRightY = topLeftY + floorHeight;

            // // Update the current floor after moving to the next one
            // currentFloor++;

            // // If the elevator has reached the last floor, stop the movement
            // if (currentFloor >= floorHeights.size())
            // {
            //     isMovingUp = false; // Stop moving up when it reaches the last floor
            // }

            // // Draw the elevator at the final position on the current floor
            // view.DrawFilledRectangle(topLeftX, topLeftY, bottomRightX, bottomRightY, ECGV_CYAN);
            // view.SetRedraw(true);
        }

        // If the event is related to moving down
        // else if (currentEvent.find("Moving DOWN") != std::string::npos)
        // {
        //     isMovingDown = true; // Set the flag for moving down
        //     isMovingUp = false;  // Ensure the up flag is false

        //     // Move the elevator down
        //     topLeftY += floorHeight;
        //     bottomRightY += floorHeight;

        //     // Ensure the elevator doesn't move past the last floor
        //     if (bottomRightY >= floorHeights[elevatorSim.GetNumFloors() - 1] + floorHeight)
        //     {
        //         bottomRightY = floorHeights[elevatorSim.GetNumFloors() - 1] + floorHeight;
        //         topLeftY = floorHeights[elevatorSim.GetNumFloors() - 1];
        //         isMovingDown = false; // Stop moving down when it reaches the last floor
        //     }

        //     // Draw the elevator at its new position
        //     view.DrawFilledRectangle(topLeftX, topLeftY, bottomRightX, bottomRightY, ECGV_CYAN);
        //     view.SetRedraw(true); // Trigger a redraw after updating the position
        // }
        // if (currentEvent.find("waiting") != std::string::npos)
        // {
        //     size_t floorPos = currentEvent.find("at floor ") + 9; // Find the position where the floor number starts
        //     std::string floorStr = currentEvent.substr(floorPos); // Get the substring starting from the floor number
        //     int waitingFloor = std::stoi(floorStr);
        //     int passengerX = passengerOffsetX;                                                           // Position outside the elevator
        //     int passengerY = floorHeights[waitingFloor - 1] + (floorHeight / 2) - (passengerHeight / 2); // Position vertically aligned to the waiting floor

        //     // Draw the passenger as a rectangle and ellipse
        //     view.DrawRectangle(passengerX, passengerY, passengerX + passengerWidth, passengerY + passengerHeight, 3);
        //     view.DrawEllipse(passengerX + passengerWidth / 2, passengerY - passengerHeight / 2, passengerWidth / 2, passengerWidth / 2, 3);
        //     view.SetRedraw(true);
        // }
        // // // Handle passenger boarding or exiting
        // if (currentEvent.find("boards") != std::string::npos)
        // {
        //     int boardingFloor = std::stoi(currentEvent.substr(currentEvent.find("floor ") + 6, 1));
        //     for (auto &passenger : passengers)
        //     {
        //         if (passenger.floorIndex == boardingFloor && !passenger.isOnElevator)
        //         {
        //             passenger.isOnElevator = true;
        //             passenger.x = elevatorX + 20;
        //             passenger.y = floorHeights[boardingFloor - 1] + (floorHeight / 2) - (passengerHeight / 2);
        //             view.DrawRectangle(passenger.x, passenger.y, passenger.x + passengerWidth, passenger.y + passengerHeight, 3);
        //             view.DrawEllipse(passenger.x + passengerWidth / 2, passenger.y - passengerHeight / 2, passengerWidth / 2, passengerWidth / 2, 3);
        //             break;
        //         }
        //     }
        // }

        // if (currentEvent.find("exits") != std::string::npos)
        // {
        //     int exitFloor = std::stoi(currentEvent.substr(currentEvent.find("floor ") + 6, 1));
        //     for (auto &passenger : passengers)
        //     {
        //         if (passenger.isOnElevator && passenger.floorIndex == exitFloor)
        //         {
        //             passenger.isOnElevator = false;
        //             passenger.x = -100;
        //             passenger.y = -100;
        //             view.SetRedraw(true);
        //             break;
        //         }
        //     }
        // }

        // Increment history index and redraw
        currentHistoryIndex++;
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Adjust the time to control the speed
        view.SetRedraw(true);
    }
}
