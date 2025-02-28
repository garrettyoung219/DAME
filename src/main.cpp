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

int16_t sBuffer[bufferLen];
ButtonChecker button;

// Function declarations
void setupLEDs();
void setupAudioIO();

void printMemoryStats() {
    Serial.printf("🔍 Free Heap (Internal RAM): %d bytes | Free PSRAM: %d bytes\n", 
                  heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
                  heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
}

// void printTaskStats() {
//     TaskStatus_t *taskArray;
//     UBaseType_t taskCount = 2;//uxTaskGetNumberOfTasks();
    
//     taskArray = (TaskStatus_t *)heap_caps_malloc(taskCount * sizeof(TaskStatus_t), MALLOC_CAP_INTERNAL);
//     if (taskArray == NULL) {
//         Serial.println("❌ Failed to allocate memory for task stats!");
//         return;
//     }

//     uxTaskGetSystemState(taskArray, taskCount, NULL);
    
//     Serial.println("\n🔍 Task Stack Usage:");
//     for (UBaseType_t i = 0; i < taskCount; i++) {
//         Serial.printf("📌 Task: %-16s | Free Stack: %d bytes\n",
//                       taskArray[i].pcTaskName, taskArray[i].usStackHighWaterMark);
//     }
    
//     free(taskArray);
// }

void monitorMemoryTask(void *pvParameters) {
    while (true) {
        printMemoryStats();
       // printTaskStats();
        vTaskDelay(pdMS_TO_TICKS(100));  // Print every 5 seconds
    }
}

void setupLEDs() {
    pinMode(LED_MIC, OUTPUT);
    pinMode(LED_SPKR, OUTPUT);
    digitalWrite(LED_MIC, LOW);
    digitalWrite(LED_SPKR, LOW);
}

void setupAudioIO() {
    // Uninstall any existing I2S drivers
    i2s_driver_uninstall(I2S_PORT_MIC);
    
    // Setup speaker first
    setupSpeakerI2S();
    delay(200);  // Increased delay for better initialization
    
    // Then setup microphone
    setupMicrophone();
    delay(200);  // Increased delay for better initialization
}

void setup() {
    Serial.begin(115200);
    //xTaskCreatePinnedToCore(monitorMemoryTask, "MemoryMonitor", 4096, NULL, 1, NULL, 0);

    setupLEDs();
    connectToWiFi();
    connectToWebSocket();
    
    setRecording(false);
    setupAudioIO();
    Serial.println("Finished setting up Audio IO");

    xTaskCreatePinnedToCore(micTask, "micTask", 16000, NULL, 1, NULL, 1);
}

void loop() {
    button.loop();
    if (button.justPressed()) {
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
    else if (button.justReleased()) {
        Serial.println("Stopped recording.");
        sendButtonState(0);
        sendMessage("STOP_RECORD");
        setRecording(false);

        // Stop microphone and clear buffer before starting speaker
        i2s_stop(I2S_PORT_MIC);
        i2s_zero_dma_buffer(I2S_PORT_MIC);
        delay(100);
        
        i2s_start(I2S_PORT_SPEAKER);
        delay(100);
    }

    loopWebsocket();
}
