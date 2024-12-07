#include "ECSimpleGraphicObserver.h"

ECSimpleGraphicObserver::ECSimpleGraphicObserver(ECGraphicViewImp &viewIn)
    : view(viewIn), elevatorX(200), elevatorY(750), currentFloor(1), targetFloor(1),
      floorHeight(100), isMoving(false), isMovingUp(false), tickCount(0), ticksPerFloor(10)
{
    // Initialize passenger data or other observer-specific setup if needed ??
}

void ECSimpleGraphicObserver::Update()
{
    // Check the current event from the graphic view
    ECGVEventType eventType = view.GetCurrEvent();

    if (eventType == ECGV_EV_TIMER)
    {
        // Timer tick event: move the elevator if it's moving
        if (isMoving)
        {
            MoveElevator();       // Update the position
            view.SetRedraw(true); // Request a redraw
        }
    }
}

void ECSimpleGraphicObserver::MoveElevator()
{
    // Determine the direction of movement
    if (currentFloor < targetFloor)
    {
        isMovingUp = true;
    }
    else if (currentFloor > targetFloor)
    {
        isMovingUp = false;
    }
    else
    {
        // Reached target floor
        isMoving = false;
        return;
    }

    // Increment position gradually
    if (isMovingUp)
    {
        elevatorY -= floorHeight / ticksPerFloor; // Move up in small increments
    }
    else
    {
        elevatorY += floorHeight / ticksPerFloor; // Move down in small increments
    }

    // Track ticks and update floor when enough ticks have passed
    tickCount++;
    if (tickCount >= ticksPerFloor)
    {
        tickCount = 0;
        currentFloor += (isMovingUp ? 1 : -1);

        // Stop moving if we've reached the target floor
        if (currentFloor == targetFloor)
        {
            isMoving = false;
        }
    }
}

void ECSimpleGraphicObserver::DrawElevator()
{
    // Example elevator drawing logic
    int elevatorWidth = 100;
    int elevatorHeight = 80;
    view.DrawFilledRectangle(elevatorX, elevatorY, elevatorX + elevatorWidth,
                             elevatorY + elevatorHeight, ECGV_BLUE);
}

void ECSimpleGraphicObserver::DrawPassengers()
{
    for (const Passenger &p : passengers)
    {
        if (p.isVisible)
        {
            view.DrawFilledCircle(p.x, p.y, 10, ECGV_GREEN); // Draw passengers as green circles
        }
    }
}
