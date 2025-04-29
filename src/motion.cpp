#include <Arduino.h>
#include <ESP32Servo.h>
#include "config.h"
#include "motion.h"

bool is_in_motion = false;

#define CW   2000  // Clockwise (2ms pulse)
#define STOP 1500  // Stop (1.5ms pulse, neutral position)
#define CCW  1000  // Counterclockwise (1ms pulse)

Servo myServo0;
Servo myServo1;

void init_motor_controller() {
    // Insert code for Initialize H bridge and PWM control signals
    pinMode(BRIDGE_A1_PIN,OUTPUT);
    pinMode(BRIDGE_B1_PIN,OUTPUT);
    pinMode(BRIDGE_A2_PIN,OUTPUT);
    pinMode(BRIDGE_B2_PIN,OUTPUT);
    pinMode(SERVO_PIN0, OUTPUT);
    pinMode(SERVO_PIN1, OUTPUT);
    
    Serial.println("Initializing Actuators... ");
    digitalWrite(BRIDGE_A1_PIN, HIGH);
    digitalWrite(BRIDGE_B1_PIN, HIGH);
    digitalWrite(BRIDGE_A2_PIN, LOW);
    digitalWrite(BRIDGE_B2_PIN, LOW);
    Serial.println("PMOS Set HIGH\nNmos Set LOW\nAll Actuators OFF");

    myServo0.attach(SERVO_PIN0);
    myServo0.writeMicroseconds(STOP);  // Initially stop the servo
    myServo1.attach(SERVO_PIN1);
    myServo1.writeMicroseconds(STOP); 

    delay(10000);
    Serial.println("50 Seconds Remaining...");
    delay(10000);
    Serial.println("40 Seconds Remaining...");
    delay(10000);
    Serial.println("30 Seconds Remaining...");
    delay(10000);
    Serial.println("20 Seconds Remaining...");
    delay(10000);
    Serial.println("10 Seconds Remaining...");
    delay(10000);
    Serial.println("0 Seconds Remaining...");
    return;
}

void erect(int time){
  //reset the H-Bridge
  digitalWrite(BRIDGE_A1_PIN, HIGH);
  digitalWrite(BRIDGE_B1_PIN, HIGH);
  digitalWrite(BRIDGE_A2_PIN, LOW);
  digitalWrite(BRIDGE_B2_PIN, LOW);

  Serial.println("Erecting for " + char(time));
  //erect 
  digitalWrite(BRIDGE_A1_PIN, LOW);
  digitalWrite(BRIDGE_B2_PIN, HIGH);

  delay(time);

  //reset the H-Bridge
  digitalWrite(BRIDGE_A1_PIN, HIGH);
  digitalWrite(BRIDGE_B1_PIN, HIGH);
  digitalWrite(BRIDGE_A2_PIN, LOW);
  digitalWrite(BRIDGE_B2_PIN, LOW);
  return;
}

void flacid(int time){
  //reset the H-Bridge
  digitalWrite(BRIDGE_A1_PIN, HIGH);
  digitalWrite(BRIDGE_B1_PIN, HIGH);
  digitalWrite(BRIDGE_A2_PIN, LOW);
  digitalWrite(BRIDGE_B2_PIN, LOW);

  Serial.println("Flaciding for " + char(time));
  //flacid 
  digitalWrite(BRIDGE_B1_PIN, LOW);
  digitalWrite(BRIDGE_A2_PIN, HIGH);

  delay(time);

  //reset the H-Bridge
  digitalWrite(BRIDGE_A1_PIN, HIGH);
  digitalWrite(BRIDGE_B1_PIN, HIGH);
  digitalWrite(BRIDGE_A2_PIN, LOW);
  digitalWrite(BRIDGE_B2_PIN, LOW);
  return;
}

void Clockwise0(int time) {
    // Move clockwise for time seconds
    Serial.println("Moving Clockwise");
    myServo0.writeMicroseconds(CW);
    delay(time);  // Wait for time seconds
    return;
}

void Counter0(int time) {
    // Move counterclockwise for time seconds
    Serial.println("Moving Counterclockwise");
    myServo0.writeMicroseconds(CCW);
    delay(time);  // Wait for time seconds
    return;
}

void Stop0(int time){
    // Stop the servo for time seconds
    Serial.println("Stopping Servo");
    myServo0.writeMicroseconds(STOP);
    delay(time);  // Wait for time seconds
    return;
}

void Clockwise1(int time) {
    // Move clockwise for time seconds
    Serial.println("Moving Clockwise");
    myServo1.writeMicroseconds(CW);
    delay(time);  // Wait for time seconds
    return;
}

void Counter1(int time) {
    // Move counterclockwise for time seconds
    Serial.println("Moving Counterclockwise");
    myServo1.writeMicroseconds(CCW);
    delay(time);  // Wait for time seconds
    return;
}

void Stop1(int time){
    // Stop the servo for time seconds
    Serial.println("Stopping Servo");
    myServo1.writeMicroseconds(STOP);
    delay(time);  // Wait for time seconds
    return;
}

void step() {
    if (!is_in_motion) {
        is_in_motion = true;

        erect(3000);
        delay(2000);
        Counter1(80);
        Stop1(30); 
        Clockwise0(80);
        Stop0(30);
        delay(3000);
        flacid(3000);
        
        Counter0(140);
        Stop0(30); 
        Clockwise1(140);
        Stop1(30);
        delay(1000);
        erect(1000);
    
        is_in_motion = false;
    }
    return;
}
