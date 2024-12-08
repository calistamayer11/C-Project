#include "ECSimpleGraphicObserver.h"
#include "ECElevatorSim.h"
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

    // Draw the floors immediately after initialization
    const int floorWidth = 400; // Width of each floor
    for (int i = 0; i < elevatorSim.GetNumFloors(); ++i)
    {
        view.DrawRectangle(20, floorHeights[i], floorWidth, floorHeights[i] + floorHeight, 3, ECGV_BLACK);
    }
}
void ECSimpleGraphicObserver::Update()
{
    static size_t currentHistoryIndex = 0; // Keeps track of the current event in the history vector
    const int floorWidth = 400;            // Width of each floor
    int floorHeight = 120;                 // Height of each floor
    for (int i = 0; i < elevatorSim.GetNumFloors(); ++i)
    {
        view.DrawRectangle(20, floorHeights[i], floorWidth, floorHeights[i] + floorHeight, 3);
    }
    // Ensure there are events to process
    if (currentHistoryIndex < elevatorSim.GetHistory().size())
    {
        // Retrieve the current event from history
        std::string currentEvent = elevatorSim.GetHistory()[currentHistoryIndex];

        std::cout << currentEvent << std::endl;
        // Define constants for drawing
        const int passengerWidth = 15;
        const int passengerHeight = 30;
        const int passengerOffsetX = 460; // Position for passengers outside the elevator

        // If the event is related to floor movement (up/down)
        if (currentEvent.find("Moving UP") != std::string::npos)
        {
            // Move the elevator and render the new floor position
            elevatorY -= elevatorSpeed;
            if (elevatorY <= floorHeights[0])
            {
                elevatorY = floorHeights[0];
            }
            view.DrawFilledRectangle(elevatorX, elevatorY, elevatorX + elevatorWidth, elevatorY + elevatorHeight, ECGV_CYAN);
            view.SetRedraw(true);
        }
        else if (currentEvent.find("Moving DOWN") != std::string::npos)
        {
            elevatorY += elevatorSpeed;
            if (elevatorY >= floorHeights[elevatorSim.GetNumFloors() - 1])
            {
                elevatorY = floorHeights[elevatorSim.GetNumFloors() - 1];
            }
            view.DrawFilledRectangle(elevatorX, elevatorY, elevatorX + elevatorWidth, elevatorY + elevatorHeight, ECGV_CYAN);
            view.SetRedraw(true);
        }

        // Handle passenger boarding or exiting
        if (currentEvent.find("boards") != std::string::npos)
        {
            int boardingFloor = std::stoi(currentEvent.substr(currentEvent.find("floor ") + 6, 1));
            for (auto &passenger : passengers)
            {
                if (passenger.floorIndex == boardingFloor && !passenger.isOnElevator)
                {
                    passenger.isOnElevator = true;
                    passenger.x = elevatorX + 10;
                    passenger.y = floorHeights[boardingFloor] + (floorHeight / 2) - (passengerHeight / 2);
                    view.DrawRectangle(passenger.x, passenger.y, passenger.x + passengerWidth, passenger.y + passengerHeight, 3);
                    view.DrawEllipse(passenger.x + passengerWidth / 2, passenger.y - passengerHeight / 2, passengerWidth / 2, passengerWidth / 2, 3);
                    view.SetRedraw(true);
                    break;
                }
            }
        }

        if (currentEvent.find("exits") != std::string::npos)
        {
            int exitFloor = std::stoi(currentEvent.substr(currentEvent.find("floor ") + 6, 1));
            for (auto &passenger : passengers)
            {
                if (passenger.isOnElevator && passenger.floorIndex == exitFloor)
                {
                    passenger.isOnElevator = false;
                    passenger.x = -100;
                    passenger.y = -100;
                    view.SetRedraw(true);
                    break;
                }
            }
        }

        // Increment history index and redraw
        currentHistoryIndex++;
        view.SetRedraw(true);
    }
}
