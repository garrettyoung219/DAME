#include <Arduino.h>
#include <Audio.h>
#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <driver/i2s.h>
#include <math.h>
#include "mic.h"
#include "config.h"
#include "lib_wifi.h"
#include "utils.h"
#include "speaker.h"
#include "lib_button.h"
#include "lib_websocket.h"
#include <esp_task_wdt.h>  // Required for watchdog control
#include "esp_heap_caps.h"
#include "NextionDisplay.h"
#include "NimBLEDevice.h"
#include <XboxSeriesXControllerESP32_asukiaaa.hpp>
#include "xboxControl.h"

int16_t sBuffer[bufferLen];
ButtonChecker button;
NextionDisplay display(Serial2, 17, 16);
XboxSeriesXControllerESP32_asukiaaa::Core xboxController(XBOX_CONTROLLER_BLE_ADDRESS);

void setupAudioIO() {
    setRecording(false);

    // Uninstall any existing I2S drivers
    i2s_driver_uninstall(I2S_PORT_MIC);
   
    setupSpeakerI2S();
    delay(200);  // Increased delay for better initialization
    
    setupMicrophone();
    delay(200);  // Increased delay for better initialization
}

void setup() {
    pinMode(BUILTIN_LED, OUTPUT);
    Serial.begin(115200);
    display.begin();
    xboxController.begin();
    initXboxButtonChecker();
    connectToWiFi();
    connectToWebSocket();
    setupAudioIO();

    xTaskCreatePinnedToCore(micTask, "micTask", 16000, NULL, 1, NULL, 1);
}

void loop() {
    xboxController.onLoop();
    button.loop();
    xboxButtonLoop();
    if (xboxButtonYJustPressed()) {
        display.clear();
        display.printRaw("Button Y just pressed.");
        //digitalWrite(BUILTIN_LED, HIGH);
    }
    else if (xboxButtonYJustReleased()){
        display.clear();
        display.printRaw("Button Y just released.");
    }
    if (xboxController.xboxNotif.btnA) {
        digitalWrite(BUILTIN_LED, HIGH);
    }
    else {
        digitalWrite(BUILTIN_LED, LOW);
    }
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
    }
    else if (button.justReleased() || xboxButtonYJustReleased()) {
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

    loopWebsocket();
}
