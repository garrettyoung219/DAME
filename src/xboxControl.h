#ifndef XBOXCONTROL_H
#define XBOXCONTROL_H

#include <XboxSeriesXControllerESP32_asukiaaa.hpp>

#define XBOX_CONTROLLER_BLE_ADDRESS "3C:FA:06:3D:A9:FF"

extern XboxSeriesXControllerESP32_asukiaaa::Core xboxController;

void xboxButtonLoop();
bool xboxButtonYJustPressed();
bool xboxButtonYJustReleased();
void initXboxButtonChecker();

extern bool lastTickState;
extern bool thisTickState;

#endif