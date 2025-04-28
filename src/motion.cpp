#include <Arduino.h>
#include "config.h"
#include "motion.h"

bool is_in_motion = false;

void init_motor_controller() {
    pinMode(BRIDGE_A1_PIN, OUTPUT);
    pinMode(BRIDGE_A2_PIN, OUTPUT);
    pinMode(BRIDGE_B1_PIN, OUTPUT);
    pinMode(BRIDGE_B2_PIN, OUTPUT);
    pinMode(SERVO_D_M1_PIN, OUTPUT);
    pinMode(SERVO_D_M2_PIN, OUTPUT);

     // Insert code for Initialize H bridge and PWM control signals

    return;
}

void lift() {
    // Insert code for linear motion
    return;
}

void rotate(int angle) {
    // Insert code for rotating legs
    return;
}

void step() {
    if (!is_in_motion) {
        is_in_motion = true;

        // Insert code for taking one complete step
    
        is_in_motion = false;
    }
    return;
}