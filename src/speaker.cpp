#include <Arduino.h>
#include <driver/i2s.h>
#include <math.h>
#include "config.h"
#include "speaker.h"
#include "websocket.h"
#include "button.h"
#include "mic.h"

esp_err_t setupSpeakerI2S()
{
  // Uninstall any existing I2S drivers
  i2s_driver_uninstall(I2S_PORT_MIC);
  //i2s_driver_uninstall(I2S_PORT_SPEAKER);
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = AUDIO_QUALITY_SPEAKER,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = bufferCnt,
      .dma_buf_len = bufferLen,
      .use_apll = false,
      .tx_desc_auto_clear = true,
      .fixed_mclk = 0};

  i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_SPEAKER_BCLK,
      .ws_io_num = I2S_SPEAKER_LRC,
      .data_out_num = I2S_SPEAKER_DIN,
      .data_in_num = I2S_PIN_NO_CHANGE};

  // Check if I2S port is valid
  if (I2S_PORT_SPEAKER < I2S_NUM_0 || I2S_PORT_SPEAKER >= I2S_NUM_MAX)
  {
    Serial.println("Invalid I2S port");
    return ESP_ERR_INVALID_ARG;
  }

  // Install I2S driver
  esp_err_t i2s_err = i2s_driver_install(I2S_PORT_SPEAKER, &i2s_config, 0, NULL);
  if (i2s_err != ESP_OK)
  {
    Serial.printf("Failed to install I2S driver: %s\n", esp_err_to_name(i2s_err));
    return i2s_err;
  }

  // Set I2S pins
  i2s_err = i2s_set_pin(I2S_PORT_SPEAKER, &pin_config);
  if (i2s_err != ESP_OK)
  {
    Serial.printf("Failed to set I2S pins: %s\n", esp_err_to_name(i2s_err));
    i2s_driver_uninstall(I2S_PORT_SPEAKER);
    return i2s_err;
  }

  Serial.println("I2S initialized successfully");
  return ESP_OK;
}

void speaker_play(uint8_t *payload, uint32_t len)
{
  const float volume = 1.2f;
  const float pitch = 0.8f; // 1.0 = normal speed, >1 = faster, <1 = slower
  //Serial.printf("received %lu bytes", len);
  Serial.println();
  size_t bytes_written;

  // Create a buffer to store modified samples
  int16_t *samples = (int16_t *)payload;
  size_t num_samples = len / sizeof(int16_t);

  // Calculate new buffer size based on pitch
  size_t new_num_samples = (size_t)(num_samples / pitch);
  int16_t *pitched_samples = new int16_t[new_num_samples];

  // Resample audio for pitch/speed adjustment
  for (size_t i = 0; i < new_num_samples; i++)
  {
    float original_index = i * pitch;
    size_t index = (size_t)original_index;
    if (index < num_samples)
    {
      pitched_samples[i] = (int16_t)(samples[index] * volume);
    }
  }

  i2s_write(I2S_PORT_SPEAKER, pitched_samples, new_num_samples * sizeof(int16_t),
            &bytes_written, portMAX_DELAY);

  delete[] pitched_samples;
}