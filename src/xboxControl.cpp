#include <XboxSeriesXControllerESP32_asukiaaa.hpp>
#include "NimBLEDevice.h"
#include "xboxControl.h"

bool thisTickState;
bool lastTickState;

void initXboxButtonChecker() {
    lastTickState = false;
    thisTickState = false;
}

void xboxButtonLoop() {
    lastTickState = thisTickState;
    thisTickState = xboxController.xboxNotif.btnY;
}

bool xboxButtonYJustPressed() {
    return thisTickState && !lastTickState;
}

bool xboxButtonYJustReleased() {
    return !thisTickState && lastTickState;
}
