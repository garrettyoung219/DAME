#include "battery.h"
#include "config.h"
#include <Arduino.h>

float readBatteryVoltage() {
    int adc_raw = analogRead(BATTERY_ADC_PIN);
    float adc_voltage = (adc_raw / 4095.0) * 3.3;  // ADC counts to voltage
    float battery_voltage = adc_voltage * ((1000000.0 + 470000.0) / 470000.0);  // undo divider
    return battery_voltage;
}

int getBatteryPercentage() {
    float voltage = readBatteryVoltage();

    // Define battery voltage range for 2S Li-Ion (typical values)
    const float max_voltage = 8.4;   // Full charge
    const float min_voltage = 6.0;   // Dead battery (don't want to discharge below ~3.0V/cell)

    if (voltage >= max_voltage) return 100;
    if (voltage <= min_voltage) return 0;

    // Linear interpolation between min and max
    int percentage = (int)(((voltage - min_voltage) / (max_voltage - min_voltage)) * 100);
    return percentage;
}