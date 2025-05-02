#ifndef MOTION_H
#define MOTION_H

extern bool is_in_motion;

void init_motor_controller();
void step();
void erect(int time);
void flacid(int time);
void Counter0(int time);
void Counter1(int time);
void Clockwise0(int time);
void Clockwise1(int time);
void Stop0(int time);
void Stop1(int time);

#endif