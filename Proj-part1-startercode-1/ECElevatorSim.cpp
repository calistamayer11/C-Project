//  Elevator simulation backend for part 3

#include "ECElevatorSim.h"

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
    // go through the active requests and find the floor that should be serviced next
    int closestFloor = -1;
    EC_ELEVATOR_MOVEMENT move = ELEVATOR_STOP;

    getActiveRequests();

    if (!activeRequests.empty()) // go through the active requests and find the next floor.
    {
        // set to the direction of the next request
        // iterate over active requests to find the closest floor
        for (const auto &request : activeRequests)
        {
            int targetFloor = request.GetRequestedFloor();
            if (closestFloor == -1 ||
                (std::abs(targetFloor - elevatorSim.GetCurrFloor()) < std::abs(closestFloor - elevatorSim.GetCurrFloor())))
            {
                closestFloor = targetFloor;
                EC_ELEVATOR_DIR move = targetFloor > elevatorSim.GetCurrFloor() ? EC_ELEVATOR_UP : EC_ELEVATOR_DOWN;

                // set to the direction of the closest floor
                elevatorSim.SetCurrDir(move);

                // transition the elevator to move state
                elevatorSim.transitionToState(ELEVATOR_MOVE);
                break;
            }
        }
    }
};
void Loading::onTimeTick()
{
    getActiveRequests();
    for (auto &request : activeRequests)
    {
        if (request.GetFloorSrc() == elevatorSim.GetCurrFloor()) // is the request to enter here?
        {
            // let the passenger on and record the time
            request.SetArriveTime(elevatorSim.GetCurrTime());
        }
        else if (request.GetFloorDest() == elevatorSim.GetCurrFloor()) // is the request to exit here?
        {
            request.SetArriveTime(elevatorSim.GetCurrTime()); // mark the time that the request was completed
            request.SetServiced(true);                        // mark the request as complete
        }
    }
};
void Moving::onTimeTick()
{
    int currentFloor = elevatorSim.GetCurrFloor();
    EC_ELEVATOR_DIR currentDir = elevatorSim.GetCurrDir();
    bool transitionToLoading = false;

    getActiveRequests();
    // loop through active requests to see if there's a request at the current floor in the direction of movement
    for (auto &request : activeRequests)
    {
        int requestedFloor = request.GetRequestedFloor();
        // check if the request is at the current floor and the elevator is moving in the correct direction
        if (requestedFloor == currentFloor)
        {
            // transition to loading state if the request is at the current floor
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
};

void ElevatorState::getActiveRequests()
{
    currentTime = elevatorSim.GetCurrTime();
    activeRequests.clear();
    for (const auto &request : elevatorSim.listRequests)
    {
        if (request.GetTime() <= currentTime) // check if the current request time is less than the current time that the elevator is on
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