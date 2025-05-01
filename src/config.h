#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials
const int WEBSOCKET_PORT = 8888;
inline const char* WIFI_SSID = "ND-guest";
inline const char* WIFI_PASSWORD = "";
inline const char* WEBSOCKET_HOST = "10.12.153.200";

// I2S Microphone pins
#define I2S_SD 42  // Serial Data
#define I2S_WS 41  // Word Select (LRCLK)
#define I2S_SCK 47 // Serial Clock

// Speaker pins
#define I2S_SPEAKER_BCLK 21//18 // Bit Clock
#define I2S_SPEAKER_LRC 35  // Left Right Clock (Word Select)
#define I2S_SPEAKER_DIN 45  // Data Input

// LED pins
#define LED_MIC 3   // RED LED for microphone activity
#define LED_SPKR 41 // BLUE LED for speaker activity
#define LED_BLINK 13

// Button pin
#define BUTTON_PIN 46

// Battery percentage pin
#define BATTERY_ADC_PIN 39

// Motor pins
#define BRIDGE_A1_PIN 4
#define BRIDGE_A2_PIN 5
#define BRIDGE_B1_PIN 6
#define BRIDGE_B2_PIN 7
#define SERVO_PIN0 9
#define SERVO_PIN1 10

// I2S Microphone configuration
// #define SAMPLE_RATE 44100
#define SAMPLE_RATE 16000
#define SAMPLE_BITS 32
#define CHANNELS 1

#define I2S_PORT_MIC I2S_NUM_0
#define I2S_PORT_SPEAKER I2S_NUM_1

// Buffer configuration
#define bufferCnt 10
#define bufferLen 1024

// Audio detection thresholds
#define MIC_THRESHOLD 2300 // Adjust based on testing
#define LED_DELAY 1        // ms to keep LED on after sound stops

struct LedThreshold
{
    int ledPin;
    int threshold;
};

const LedThreshold ledThresholds[] = {
    {LED_MIC, 100},
    // {LED_SPKR, 200},
};

enum AudioQuality
{
  LOW_DEFINITION = 16000,
  OPENAI_DEFINITION = 22050,
  MID_DEFINITION = 24000,
  HIGH_DEFINITION = 44100,
  ULTRA_HIGH_DEFINITION = 96000
};

const AudioQuality AUDIO_QUALITY_SPEAKER = AudioQuality::LOW_DEFINITION;
const AudioQuality AUDIO_QUALITY = AudioQuality::HIGH_DEFINITION;
const AudioQuality AUDIO_QUALITY_MIC = AudioQuality::LOW_DEFINITION;

#endif