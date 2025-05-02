#include <XboxSeriesXControllerESP32_asukiaaa.hpp>
#include "xboxControl.h"

// Xbox controller instance should be declared externally
extern XboxSeriesXControllerESP32_asukiaaa::Core xboxController;

// Per-button tracking
bool currA, lastA;
bool currB, lastB;
bool currX, lastX;
bool currY, lastY;
bool currUp, lastUp;
bool currDown, lastDown;
bool currLeft, lastLeft;
bool currRight, lastRight;
bool currRB, lastRB;
bool currStart, lastStart;

// RT trigger tracking
bool currRT, lastRT;

void initXboxButtonStates() {
    currA = lastA = false;
    currB = lastB = false;
    currX = lastX = false;
    currY = lastY = false;
    currUp = lastUp = false;
    currDown = lastDown = false;
    currLeft = lastLeft = false;
    currRight = lastRight = false;
    currRB = lastRB = false;
    currStart = lastStart = false;

    currRT = lastRT = false;
}

void updateXboxButtonStates() {
    // Save old states
    lastA = currA;
    lastB = currB;
    lastX = currX;
    lastY = currY;
    lastUp = currUp;
    lastDown = currDown;
    lastLeft = currLeft;
    lastRight = currRight;
    lastRB = currRB;
    lastStart = currStart;
    lastRT = currRT;

    // Read current states
    currA = xboxController.xboxNotif.btnA;
    currB = xboxController.xboxNotif.btnB;
    currX = xboxController.xboxNotif.btnX;
    currY = xboxController.xboxNotif.btnY;
    currUp = xboxController.xboxNotif.btnDirUp;
    currDown = xboxController.xboxNotif.btnDirDown;
    currLeft = xboxController.xboxNotif.btnDirLeft;
    currRight = xboxController.xboxNotif.btnDirRight;
    currRB = xboxController.xboxNotif.btnRB;
    currStart = xboxController.xboxNotif.btnStart;
    currRT = xboxController.xboxNotif.trigRT > 50;
}

// Just pressed functions
bool justPressedA()      { return  currA && !lastA; }
bool justPressedB()      { return  currB && !lastB; }
bool justPressedX()      { return  currX && !lastX; }
bool justPressedY()      { return  currY && !lastY; }
bool justPressedUp()     { return  currUp && !lastUp; }
bool justPressedDown()   { return  currDown && !lastDown; }
bool justPressedLeft()   { return  currLeft && !lastLeft; }
bool justPressedRight()  { return  currRight && !lastRight; }
bool justPressedRB()     { return  currRB && !lastRB; }
bool justPressedStart()  { return  currStart && !lastStart; }
bool xboxTrigJustPressed()   { return  currRT && !lastRT; }
bool xboxTrigJustReleased()  { return !currRT &&  lastRT; }
