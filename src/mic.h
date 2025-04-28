#ifndef MIC_H
#define MIC_H

#include <Arduino.h>

void detectSound(int16_t *buffer, size_t length);
esp_err_t setupMicrophone();
void micTask(void *parameter);
void setRecording(bool recording);

#endif