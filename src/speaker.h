#ifndef SPEAKER_H
#define SPEAKER_H

#include <Arduino.h>

esp_err_t setupSpeakerI2S();
void speaker_play(uint8_t *payload, uint32_t len);

#endif