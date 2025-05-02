#ifndef XBOXCONTROL_H
#define XBOXCONTROL_H

#include <XboxSeriesXControllerESP32_asukiaaa.hpp>

#define XBOX_CONTROLLER_BLE_ADDRESS "3C:FA:06:3D:A9:FF"

extern XboxSeriesXControllerESP32_asukiaaa::Core xboxController;

void initXboxButtonStates();
void updateXboxButtonStates();

bool justPressedA();
bool justPressedB();
bool justPressedX();
bool justPressedY();
bool justPressedUp();
bool justPressedDown();
bool justPressedLeft();
bool justPressedRight();
bool justPressedRB();
bool justPressedStart();

bool xboxTrigJustPressed();
bool xboxTrigJustReleased();



extern bool lastTickState;
extern bool thisTickState;

#endif