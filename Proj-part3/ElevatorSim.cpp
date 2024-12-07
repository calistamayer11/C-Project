// //
// //  ECElevatorSim.cpp
// //
// //
// //  Created by Yufeng Wu on 6/27/23.
// //  Elevator simulation

#include "ElevatorSim.h"
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
    getActiveRequests();

    std::ostringstream oss;
    oss << "Time: " << elevatorSim.GetCurrTime()
        << ", Floor: " << elevatorSim.GetCurrFloor()
        << ", Direction: " << (elevatorSim.GetCurrDir() == EC_ELEVATOR_UP ? "UP" : (elevatorSim.GetCurrDir() == EC_ELEVATOR_DOWN ? "DOWN" : "STOPPED"))
        << ", State: STOPPED, Requests: ";

    for (const auto &request : activeRequests)
    {
        oss << "[Src: " << request.GetFloorSrc()
            << ", Dest: " << request.GetFloorDest() << "] ";
    }

    elevatorSim.history.push_back(oss.str());

    int closestFloor = -1;
    EC_ELEVATOR_DIR move = EC_ELEVATOR_STOPPED;

    if (!activeRequests.empty())
    {
        for (const auto &request : activeRequests)
        {
            int targetFloor = request.GetRequestedFloor();
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
    getActiveRequests();
    for (auto &request : activeRequests)
    {
        if (request.GetRequestedFloor() == elevatorSim.GetCurrFloor()) // Is the request to enter here?
        {
            // Let the passenger on and record the time
            request.SetArriveTime(elevatorSim.GetCurrTime());
            request.SetFloorRequestDone(true);
            std::ostringstream oss;
            oss << "Passenger boards at floor " << elevatorSim.GetCurrFloor()
                << " - Request Src: " << request.GetFloorSrc() << ", Dest: " << request.GetFloorDest();
            elevatorSim.history.push_back(oss.str());
            int targetFloor = request.GetRequestedFloor();
            int closestFloor = -1;
            EC_ELEVATOR_DIR move = EC_ELEVATOR_STOPPED;
            if (closestFloor == -1 || std::abs(targetFloor - elevatorSim.GetCurrFloor()) < std::abs(closestFloor - elevatorSim.GetCurrFloor()))
            {
                closestFloor = targetFloor;
                move = targetFloor > elevatorSim.GetCurrFloor() ? EC_ELEVATOR_UP : EC_ELEVATOR_DOWN;
                elevatorSim.SetCurrDir(move);
                elevatorSim.transitionToState(ELEVATOR_MOVE);
                break;
            }
        }
        else if (request.GetFloorDest() == elevatorSim.GetCurrFloor()) // Is the request to exit here?
        {
            request.SetArriveTime(elevatorSim.GetCurrTime());
            request.SetServiced(true); // Mark request as completed
            std::ostringstream oss;
            oss << "Passenger exits at floor " << elevatorSim.GetCurrFloor()
                << " - Request Src: " << request.GetFloorSrc() << ", Dest: " << "Time: " << elevatorSim.GetCurrTime()
                << ", Floor: " << elevatorSim.GetCurrFloor() << request.GetFloorDest();
            elevatorSim.history.push_back(oss.str());
            int targetFloor = request.GetRequestedFloor();
            int closestFloor = -1;
            EC_ELEVATOR_DIR move = EC_ELEVATOR_STOPPED;
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
void Moving::onTimeTick()
{
    int currentFloor = elevatorSim.GetCurrFloor();
    EC_ELEVATOR_DIR currentDir = elevatorSim.GetCurrDir();
    bool transitionToLoading = false;

    getActiveRequests();

    for (auto &request : activeRequests)
    {
        int requestedFloor = request.GetRequestedFloor();
        if (requestedFloor == currentFloor && !request.IsFloorRequestDone())
        {
            elevatorSim.transitionToState(ELEVATOR_LOAD);
            transitionToLoading = true;
            break;
        }
    }

    if (!transitionToLoading)
    {
        if (currentDir == EC_ELEVATOR_UP)
        {
            elevatorSim.SetCurrFloor(currentFloor + 1); // move up
        }
        else if (currentDir == EC_ELEVATOR_DOWN)
        {
            elevatorSim.SetCurrFloor(currentFloor - 1); // move down
        }
    }

    std::ostringstream oss;
    oss << "Time: " << elevatorSim.GetCurrTime()
        << ", Floor: " << elevatorSim.GetCurrFloor()
        << ", Direction: " << (currentDir == EC_ELEVATOR_UP ? "Moving UP" : (currentDir == EC_ELEVATOR_DOWN ? "Moving DOWN" : "STOPPED"));
    elevatorSim.history.push_back(oss.str());
}

// void Stopped::onTimeTick()
// {
//     getActiveRequests();

//     std::ostringstream oss;
//     oss << "Time: " << elevatorSim.GetCurrTime()
//         << ", Floor: " << elevatorSim.GetCurrFloor()
//         << ", Direction: " << (elevatorSim.GetCurrDir() == EC_ELEVATOR_UP ? "UP" : (elevatorSim.GetCurrDir() == EC_ELEVATOR_DOWN ? "DOWN" : "STOPPED"))
//         << ", State: STOPPED, Requests: ";

//     // Append requests to the log
//     for (const auto &request : activeRequests)
//     {
//         oss << "[Src: " << request.GetFloorSrc()
//             << ", Dest: " << request.GetFloorDest() << "] ";
//     }

//     // Add the log entry to history
//     elevatorSim.history.push_back(oss.str());

//     // go through the active requests and find the floor that should be serviced next
//     int closestFloor = -1;
//     EC_ELEVATOR_DIR move = EC_ELEVATOR_STOPPED;

//     if (!activeRequests.empty()) // go through the active requests and find the next floor.
//     {
//         // set to the direction of the next request
//         // iterate over active requests to find the closest floor
//         for (const auto &request : activeRequests)
//         {
//             int targetFloor = request.GetRequestedFloor();
//             if (closestFloor == -1 ||
//                 (std::abs(targetFloor - elevatorSim.GetCurrFloor()) < std::abs(closestFloor - elevatorSim.GetCurrFloor())))
//             {
//                 closestFloor = targetFloor;
//                 move = targetFloor > elevatorSim.GetCurrFloor() ? EC_ELEVATOR_UP : EC_ELEVATOR_DOWN;

//                 // set to the direction of the closest floor
//                 elevatorSim.SetCurrDir(move);

//                 // transition the elevator to move state
//                 elevatorSim.transitionToState(ELEVATOR_MOVE);
//                 // break;
//             }
//         }
//         if (closestFloor != -1)
//         {
//             elevatorSim.SetCurrDir(move);
//             elevatorSim.transitionToState(ELEVATOR_MOVE);
//         }
//     }
// };
// void Loading::onTimeTick()
// {
//     getActiveRequests();

//     // Log current state
//     std::ostringstream oss;
//     oss << "Time: " << elevatorSim.GetCurrTime()
//         << ", Floor: " << elevatorSim.GetCurrFloor()
//         << ", State: LOADING, Requests: ";

//     // Print request details to debug
//     for (const auto &request : activeRequests)
//     {
//         // Debug print to show which requests are being processed in the loading state
//         std::cout << "Processing request at floor " << request.GetRequestedFloor()
//                   << " - Src: " << request.GetFloorSrc()
//                   << ", Dest: " << request.GetFloorDest()
//                   << ", FloorRequestDone: " << request.IsFloorRequestDone()
//                   << ", Serviced: " << request.IsServiced() << std::endl;

//         oss << "[Src: " << request.GetFloorSrc()
//             << ", Dest: " << request.GetFloorDest() << "] ";
//     }

//     elevatorSim.history.push_back(oss.str());

//     bool processedRequest = false;

//     for (auto &request : activeRequests)
//     {
//         // If we haven't serviced this request yet
//         if (!request.IsServiced())
//         {
//             cout << "FloorRequestDone: " << request.IsFloorRequestDone() << endl;
//             // Check if this is the floor where the passenger should get on
//             if (request.GetFloorSrc() == elevatorSim.GetCurrFloor() && !request.IsFloorRequestDone())
//             {
//                 // The passenger boards the elevator
//                 std::cout << "Before: FloorRequestDone: " << request.IsFloorRequestDone()
//                           << ", Serviced: " << request.IsServiced() << std::endl;
//                 request.SetFloorRequestDone(true);
//                 std::cout << "After: FloorRequestDone: " << request.IsFloorRequestDone()
//                           << ", Serviced: " << request.IsServiced() << std::endl;
//                 // request.SetFloorRequestDone(true); // passenger is now on the elevator

//                 // Decide the direction based on passenger's destination
//                 if (request.GetFloorDest() < elevatorSim.GetCurrFloor())
//                 {
//                     elevatorSim.SetCurrDir(EC_ELEVATOR_DOWN);
//                 }
//                 else if (request.GetFloorDest() > elevatorSim.GetCurrFloor())
//                 {
//                     elevatorSim.SetCurrDir(EC_ELEVATOR_UP);
//                 }
//                 else
//                 {
//                     // If the destination is the same as current floor (unlikely), just service it
//                     request.SetServiced(true);
//                 }

//                 // Transition back to moving state now that the passenger is on board
//                 processedRequest = true;
//                 request.SetServiced(true); // Mark as serviced
//                 elevatorSim.transitionToState(ELEVATOR_MOVE);
//                 break;
//             }
//             // Check if this is the floor where the passenger should get off
//             else if (request.GetFloorDest() == elevatorSim.GetCurrFloor() && request.IsFloorRequestDone() && !request.IsServiced())
//             {
//                 // Passenger exits
//                 request.SetArriveTime(elevatorSim.GetCurrTime());
//                 request.SetServiced(true);

//                 std::cout << "Passenger exits at floor " << elevatorSim.GetCurrFloor()
//                           << " - FloorRequestDone: " << request.IsFloorRequestDone()
//                           << ", Serviced: " << request.IsServiced() << std::endl;

//                 processedRequest = true;
//                 break;
//             }
//         }
//     }

//     // If no request was processed at this floor, just resume moving
//     if (!processedRequest)
//     {
//         std::cout << "No requests processed at this floor, resuming movement." << std::endl;
//         elevatorSim.transitionToState(ELEVATOR_MOVE);
//     }
// }

// void Moving::onTimeTick()
// {
//     getActiveRequests();
//     int currentFloor = elevatorSim.GetCurrFloor();
//     EC_ELEVATOR_DIR currentDir = elevatorSim.GetCurrDir();
//     bool foundRequestToLoad = false;

//     // Check if there's a request at the current floor to transition to the Loading state
//     for (auto &request : activeRequests)
//     {
//         int reqFloor = request.GetRequestedFloor();

//         // check if the request is at the current floor and the elevator is moving in the correct direction
//         if (reqFloor == currentFloor && !request.IsFloorRequestDone() && !request.IsServiced())
//         {
//             // Need to load the passenger onto the elevator
//             foundRequestToLoad = true;
//             elevatorSim.transitionToState(ELEVATOR_LOAD);
//             break;
//         }
//     }

//     // If no valid request at the current floor, move the elevator
//     if (!foundRequestToLoad)
//     {
//         if (currentDir == EC_ELEVATOR_UP)
//         {
//             elevatorSim.SetCurrFloor(currentFloor + 1); // Move up
//         }
//         else if (currentDir == EC_ELEVATOR_DOWN)
//         {
//             elevatorSim.SetCurrFloor(currentFloor - 1); // Move down
//         }
//     }
//     // Log the movement
//     std::ostringstream oss;
//     oss << "Time: " << elevatorSim.GetCurrTime()
//         << ", Floor: " << elevatorSim.GetCurrFloor()
//         << ", Direction: "
//         << (currentDir == EC_ELEVATOR_UP ? "Moving UP"
//                                          : (currentDir == EC_ELEVATOR_DOWN ? "Moving DOWN" : "STOPPED"));
//     elevatorSim.history.push_back(oss.str());
// }

void ElevatorState::getActiveRequests()
{
    currentTime = elevatorSim.GetCurrTime();
    activeRequests.clear();
    for (const auto &request : elevatorSim.listRequests)
    {
        if (request.GetTime() <= currentTime && !request.IsServiced()) // check if the current request time is less than the current time that the elevator is on
        {
            activeRequests.push_back(request);
        }
    }
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
    // Skip lines that start with a '#'
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
            // Create a new request object and add it to the list
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

    // Output simulation history or current state
    const auto &history = elevatorSim.GetHistory();
    for (const auto &entry : history)
    {
        std::cout << entry << std::endl;
    }

    return 0;
}