#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <driver/i2s.h>
#include "mic.h"
#include "config.h"
#include "lib_wifi.h"
#include "speaker.h"
#include "button.h"
#include "websocket.h"
#include "NextionDisplay.h"
#include <XboxSeriesXControllerESP32_asukiaaa.hpp>
#include "xboxControl.h"
#include "motion.h"

int16_t sBuffer[bufferLen];
ButtonChecker button;
XboxSeriesXControllerESP32_asukiaaa::Core xboxController(XBOX_CONTROLLER_BLE_ADDRESS);
NextionDisplay display(Serial2, 17, 16);

void setupAudioIO() {
    setRecording(false);
    setupSpeakerI2S();
    delay(200);  // Increased delay for better initialization
    setupMicrophone();
    delay(200);  // Increased delay for better initialization
}

void setup() {
    pinMode(LED_BLINK, OUTPUT);
    pinMode(BATTERY_ADC_PIN, INPUT);
    pinMode(BUILTIN_LED, OUTPUT);
    Serial.begin(115200);
    init_motor_controller();
    display.begin();
    display.printRaw("Booting...\r\n");
    xboxController.begin();
    initXboxButtonChecker();
    connectToWiFi();
    connectToWebSocket();
    setupAudioIO();

    xTaskCreatePinnedToCore(micTask, "micTask", 16000, NULL, 1, NULL, 1);
    display.clear();
    display.printRaw("Ready for listening.");
}

void loop() {
    xboxController.onLoop();
    button.loop();
    xboxButtonLoop();
   
    // Hold Button A -> LED On
    if (xboxController.xboxNotif.btnA) {
        digitalWrite(BUILTIN_LED, HIGH);
    } else {
        digitalWrite(BUILTIN_LED, LOW);
    }

    // Hold Button Y -> Record audio
    if (button.justPressed() || xboxButtonYJustPressed()) {
        Serial.println("Recording...");
        sendMessage("START_RECORD");
        sendButtonState(1);
        
        // Stop speaker and clear buffer before starting mic
        i2s_stop(I2S_PORT_SPEAKER);
        i2s_zero_dma_buffer(I2S_PORT_SPEAKER);
        delay(100);

        i2s_start(I2S_PORT_MIC);
        delay(100);
        
        setRecording(true);
        Serial.println("Recording ready.");
    } else if (button.justReleased() || xboxButtonYJustReleased()) {
        Serial.println("Stopped recording.");
        sendButtonState(0);
        sendMessage("STOP_RECORD");
        setRecording(false);

        // Stop microphone and clear buffer before starting speaker
        i2s_stop(I2S_PORT_MIC);
        i2s_zero_dma_buffer(I2S_PORT_MIC);
        delay(100);

        display.clear(); // clear display
        
        i2s_start(I2S_PORT_SPEAKER);
        delay(100);
    }

    // Up arrow -> Take a step forward
    if (xboxController.xboxNotif.btnDirUp) {
        step();
    }

    // Press Start Button -> Reset ESP32
    if (xboxController.xboxNotif.btnStart) {
        ESP.restart();
    }

   loopWebsocket();
}
