#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include "speaker.h"
#include "config.h"
#include <ArduinoJson.h>
#include "NextionDisplay.h"

using namespace websockets;

WebsocketsClient client;
extern NextionDisplay display;

void handleTextMessage(String message) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
        Serial.println("Raw message: ");
        Serial.println(message);
        return;
    }

    const char* type = doc["type"];
    if (type && strcmp(type, "text") == 0) {
        const char* text = doc["text"];
        if (text) {
            Serial.print("[TEXT] ");
            Serial.println(text);

            display.printDelta(String(text));
        }
    }
    // Add other message type handlers as needed
}

void handleBinaryMessage(WebsocketsMessage message) {
    // Otherwise treat as pure audio data
    uint8_t* payload = (uint8_t*)message.c_str();
    size_t length = message.length();

    Serial.printf("[AUDIO] Received %zu bytes\n", length);
    speaker_play(payload, length);
}

void onMessageCallback(WebsocketsMessage message) {
    if (message.isBinary()) {
        Serial.println("Received binary message");
        handleBinaryMessage(message);
    } else {
        Serial.println("Received text message");
        handleTextMessage(message.data());
    }
}

void onEventsCallback(WebsocketsEvent event, String data)
{
    if (event == WebsocketsEvent::ConnectionOpened)
    {
        Serial.println("Connection Opened");
    }
    else if (event == WebsocketsEvent::ConnectionClosed)
    {
        Serial.println("Connection Closed");
    }
    else if (event == WebsocketsEvent::GotPing)
    {
        Serial.println("Got a Ping!");
    }
    else if (event == WebsocketsEvent::GotPong)
    {
        Serial.println("Got a Pong!");
    }
}

void connectToWebSocket()
{
    // Configure WebSocket callbacks
    client.onMessage(onMessageCallback);
    client.onEvent(onEventsCallback);

    const char *websockets_server_host = WEBSOCKET_HOST;
    const uint16_t websockets_server_port = WEBSOCKET_PORT;

    // Try to connect to WebSocket server
    bool connected = false;
    while (!connected)
    {
        if (client.connect(websockets_server_host, websockets_server_port, "/device"))
        {
            connected = true;
            Serial.println("WebSocket Connected!");
            client.send("Hello Server");
            client.ping();
        }
        else
        {
            Serial.println("WebSocket Connection Failed! Retrying in 2 seconds...");
            delay(2000);
        }
    }
}

void checkWebSocketConnection()
{
    if (!client.available())
    {
        Serial.println("WebSocket connection lost. Reconnecting...");
        connectToWebSocket();
    }
    client.poll();
}

void sendMessage(const char *message)
{
    if (client.available())
    {
        client.send(message);
    }
    else
    {
        Serial.println("WebSocket not connected - cannot send message");
    }
}

void sendButtonState(bool buttonState)
{
    if (client.available())
    {
        uint8_t buttonMessage = buttonState ? 1 : 0;
        client.sendBinary((const char *)&buttonMessage, sizeof(buttonMessage));
    }
    else
    {
        Serial.println("WebSocket not connected - cannot send button state");
    }
}

void sendBinaryData(const int16_t *buffer, size_t bytesIn)
{
    if (client.available())
    {
        // Convert int16_t buffer to char buffer
        const char *charBuffer = reinterpret_cast<const char *>(buffer);
        client.sendBinary(charBuffer, bytesIn);
    }
    else
    {
        Serial.println("WebSocket not connected - cannot send binary data");
        delay(1000);
    }
}
void reconnectWSServer()
{
    if (!client.available())
    {
        Serial.println("WebSocket connection lost. Attempting to reconnect...");
        connectToWebSocket();
    }
}

void loopWebsocket()
{
    client.poll();
    //   static unsigned long lastReconnectAttempt = 0;
    //   unsigned long currentMillis = millis();

    //   if (currentMillis - lastReconnectAttempt >= 5000)
    //   {
    //     lastReconnectAttempt = currentMillis;
    //     reconnectWSServer();
    //   }

    //   setButtonState();
    //   delay(1);
}