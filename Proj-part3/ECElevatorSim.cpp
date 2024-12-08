// //
// //  ECElevatorSim.cpp
// //
// //
// //  Created by Yufeng Wu on 6/27/23.
// //  Elevator simulation

#include "ECElevatorSim.h"
#include "ECGraphicViewImp.h"
#include "ECSimpleGraphicObserver.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;
ECElevatorSim::ECElevatorSim(int numFloors, std::vector<ECElevatorSimRequest> &listRequests)
    : numFloors(numFloors), currentFloor(1), currentTime(0), currentDirection(EC_ELEVATOR_STOPPED), currentMove(ELEVATOR_STOP), listRequests(std::move(listRequests))
{
    elevator = new Stopped(*this);
}

ECElevatorSim::~ECElevatorSim()
{
    delete elevator;
}
int ECElevatorSim::GetCurrFloor() const
{
    if (!elevator)
    {
        std::cerr << "Error: elevator is not initialized!" << std::endl;
        return -1;
    }
    return currentFloor;
}
void ECElevatorSim::SetCurrDir(EC_ELEVATOR_DIR dir)
{
    currentDirection = dir;
}
void ECElevatorSim::SetCurrFloor(int floor)
{
    currentFloor = floor;
}

EC_ELEVATOR_DIR ECElevatorSim::GetCurrDir() const
{
    return currentDirection;
}

int ECElevatorSim::GetCurrTime()
{
    return currentTime;
};
void Stopped::onTimeTick()
{
    // const auto &activeRequests = elevatorSim.getActiveRequests();
    getActiveRequests(); // Update activeRequests

    std::ostringstream oss;
    oss << "Time: " << elevatorSim.GetCurrTime()
        << ", Floor: " << elevatorSim.GetCurrFloor()
        << ", Direction: " << (elevatorSim.GetCurrDir() == EC_ELEVATOR_UP ? "UP" : (elevatorSim.GetCurrDir() == EC_ELEVATOR_DOWN ? "DOWN" : "STOPPED"))
        << ", State: STOPPED, Requests: ";

    for (auto &request : activeRequests)
    {
        // Dereference the request to access the actual object
        ECElevatorSimRequest &req = request.get();

        // Now you can access and modify `req` directly
        oss << "[Src: " << req.GetFloorSrc()
            << ", Dest: " << req.GetFloorDest() << "] ";
    }

    elevatorSim.LogEvent(oss.str());

    int closestFloor = -1;
    EC_ELEVATOR_DIR move = EC_ELEVATOR_STOPPED;

    if (!activeRequests.empty())
    {
        for (auto &request : activeRequests)
        {
            // Dereference the request to access the actual object
            ECElevatorSimRequest &req = request.get();

            int targetFloor = req.GetRequestedFloor();
            if (closestFloor == -1 || std::abs(targetFloor - elevatorSim.GetCurrFloor()) < std::abs(closestFloor - elevatorSim.GetCurrFloor()))
            {
                closestFloor = targetFloor;
                move = targetFloor > elevatorSim.GetCurrFloor() ? EC_ELEVATOR_UP : EC_ELEVATOR_DOWN;
                elevatorSim.SetCurrDir(move);
                elevatorSim.transitionToState(ELEVATOR_MOVE);
                break;
            }
        }
    }
}

void Loading::onTimeTick()
{
    // Fetch active requests
    getActiveRequests();

    // Iterate through each active request
    for (auto &request : activeRequests)
    {
        // Dereference to access the actual request
        ECElevatorSimRequest &req = request.get();

        // Passenger boarding condition: if the request is to enter here (on the current floor)
        if (req.GetRequestedFloor() == elevatorSim.GetCurrFloor() && !req.IsFloorRequestDone()) // Only board if not yet boarded
        {
            // Let the passenger board and record the time
            req.SetArriveTime(elevatorSim.GetCurrTime()); // Record the time passenger boards
            req.SetFloorRequestDone(true);                // Mark floor request as done (passenger is on board)

            // Log the boarding event
            std::ostringstream oss;
            oss << "Passenger boards at floor " << elevatorSim.GetCurrFloor()
                << " - Request Src: " << req.GetFloorSrc() << ", Dest: " << req.GetFloorDest();
            elevatorSim.LogEvent(oss.str());

            // Determine the next target floor and direction
            int targetFloor = req.GetRequestedFloor();
            EC_ELEVATOR_DIR move = targetFloor > elevatorSim.GetCurrFloor() ? EC_ELEVATOR_UP : EC_ELEVATOR_DOWN;
            elevatorSim.SetCurrDir(move);

            // Transition to moving state
            elevatorSim.transitionToState(ELEVATOR_MOVE);
            break; // Break after processing this request to move to the next one (if any)
        }

        // Passenger exiting condition: if the request is to exit here (on the current floor)
        else if (req.GetFloorDest() == elevatorSim.GetCurrFloor() && req.IsFloorRequestDone() && !req.IsServiced())
        {
            // Mark the passenger as serviced (they have reached their destination)
            req.SetArriveTime(elevatorSim.GetCurrTime()); // Record the time they exit
            req.SetServiced(true);                        // Mark request as completed

            // Log the exiting event
            std::ostringstream oss;
            oss << "Passenger exits at floor " << elevatorSim.GetCurrFloor()
                << " - Request Src: " << req.GetFloorSrc() << ", Dest: " << req.GetFloorDest()
                << ", Time: " << elevatorSim.GetCurrTime() << ", Floor: " << elevatorSim.GetCurrFloor();
            elevatorSim.LogEvent(oss.str());

            // Transition to moving state
            int targetFloor = req.GetRequestedFloor();
            EC_ELEVATOR_DIR move = targetFloor > elevatorSim.GetCurrFloor() ? EC_ELEVATOR_UP : EC_ELEVATOR_DOWN;
            elevatorSim.SetCurrDir(move);

            // Move to the next request (after servicing this one)
            elevatorSim.transitionToState(ELEVATOR_MOVE);
            break; // Stop further processing for this tick, transition happens
        }
    }
}

void Moving::onTimeTick()
{
    int currentFloor = elevatorSim.GetCurrFloor();
    EC_ELEVATOR_DIR currentDir = elevatorSim.GetCurrDir();
    bool transitionToLoading = false;

    // Fetch the active requests
    getActiveRequests();

    // Iterate through each active request
    for (auto &request : activeRequests)
    {
        // Dereference to access the actual request
        ECElevatorSimRequest &req = request.get();

        int requestedFloor = req.GetRequestedFloor();

        // If the request is at the current floor and not yet boarded, transition to loading state
        if (requestedFloor == currentFloor)
        {
            elevatorSim.transitionToState(ELEVATOR_LOAD);
            transitionToLoading = true;
            break;
        }
    }

    // If no request was processed, continue moving the elevator
    if (!transitionToLoading)
    {
        // Move the elevator up or down based on direction
        if (currentDir == EC_ELEVATOR_UP)
        {
            currentFloor += 1;
        }
        else if (currentDir == EC_ELEVATOR_DOWN)
        {
            currentFloor -= 1;
        }

        if (currentFloor < 1)
        {
            currentFloor = 1; // Prevent moving below floor 1
        }
        else if (currentFloor > elevatorSim.GetNumFloors())
        {
            currentFloor = elevatorSim.GetNumFloors(); // Prevent moving above max floor
        }

        elevatorSim.SetCurrFloor(currentFloor);
    }

    // Log the elevator's state (time, floor, direction)
    std::ostringstream oss;
    oss << "Time: " << elevatorSim.GetCurrTime()
        << ", Floor: " << elevatorSim.GetCurrFloor()
        << ", Direction: " << (currentDir == EC_ELEVATOR_UP ? "Moving UP" : (currentDir == EC_ELEVATOR_DOWN ? "Moving DOWN" : "STOPPED"));
    elevatorSim.LogEvent(oss.str());
}

void ECElevatorSim::transitionToState(EC_ELEVATOR_MOVEMENT newState)
{
    if (elevator)
    {
        delete elevator;
    }

    // Transition based on the new state
    switch (newState)
    {
    case ELEVATOR_STOP:
        elevator = new Stopped(*this);
        break;

    case ELEVATOR_MOVE:
        elevator = new Moving(*this);
        break;

    case ELEVATOR_LOAD:
        elevator = new Loading(*this);
        break;

    default:
        std::cerr << "Invalid state transition" << std::endl;
        return;
    }

    currentMove = newState;
    elevator->getActiveRequests();
}

void ECElevatorSim::Simulate(int lenSim)
{
    for (int time = 0; time < lenSim; ++time)
    {
        currentTime++;
        elevator->onTimeTick();
    }
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <simulation_file>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream infile(filename);
    if (!infile)
    {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return 1;
    }

    // Read the first line: number of floors and number of simulation steps
    int numFloors, simLength;
    std::string line;
    while (std::getline(infile, line))
    {
        if (line[0] == '#')
            continue; // Skip this line if it's a comment

        std::istringstream iss(line);
        if (iss >> numFloors >> simLength)
        {
            break; // If successfully read numFloors and simLength, stop
        }
        else
        {
            std::cerr << "Error: Invalid format for the first line in the file." << std::endl;
            return 1;
        }
    }

    std::vector<ECElevatorSimRequest> listRequests;
    std::string requestLine;
    while (std::getline(infile, requestLine))
    {
        if (requestLine.empty() || requestLine[0] == '#')
            continue; // Skip empty lines or comments

        std::istringstream iss(requestLine);
        int time, srcFloor, destFloor;
        if (iss >> time >> srcFloor >> destFloor)
        {
            listRequests.emplace_back(time, srcFloor, destFloor);
        }
        else
        {
            std::cerr << "Invalid request format: " << requestLine << "\n";
        }
    }

    // Run the simulation
    ECElevatorSim elevatorSim(numFloors, listRequests);
    elevatorSim.Simulate(simLength);

    // Create the graphical view
    ECGraphicViewImp view(600, 900); // Create or pass your view object here

    // Create the graphic observer and pass the view and simulation object
    ECSimpleGraphicObserver graphicObserver(view, elevatorSim);
    view.Attach(&graphicObserver);
    // Now that the history is filled, process it and start showing the simulation
    view.Show(); // Let the view run the event loop and display the simulation

    return 0;
}
