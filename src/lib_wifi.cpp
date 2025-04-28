
#include "wifi.h"
#include "config.h"
#include <WiFi.h>
#include <Arduino.h>

// WiFi credentials
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

void connectToWiFi()
{
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid);//, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}