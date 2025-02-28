#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

#define EESD_C6_USB
#define EESD_C6

// User LED
static const uint8_t LED_BUILTIN = 3;
#define BUILTIN_LED LED_BUILTIN  // backward compatibility
#define LED_BUILTIN LED_BUILTIN  // allow testing #ifdef LED_BUILTIN

static const uint8_t BOOT_PIN = 9;

static const uint8_t TX = 16;
static const uint8_t RX = 17;

static const uint8_t SDA = 19;
static const uint8_t SCL = 18;
/* fix later */
static const uint8_t SS = 11;
static const uint8_t MOSI = 7;
static const uint8_t MISO = 2;
static const uint8_t SCK = 6;

static const uint8_t A0 = 2;
static const uint8_t A1 = 3;
static const uint8_t A2 = 4;
static const uint8_t A3 = 5;

static const uint8_t D0 = 2;
static const uint8_t D1 = 3;
static const uint8_t D2 = 4;
static const uint8_t D3 = 5;
static const uint8_t D4 = 6;
static const uint8_t D5 = 7;
static const uint8_t D6 = 21;
static const uint8_t D7 = 20;
static const uint8_t D8 = 8;
static const uint8_t D9 = 9;
static const uint8_t D10 = 10;

#endif /* Pins_Arduino_h */
