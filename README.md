# DAME: Interstellar's TARS Brought to Life

**Senior Design Project | Department of Electrical Engineering | University of Notre Dame**

---

## Project Overview

**DAME**  is a real-time voice assistant built on the ESP32-S3-WROOM-1 platform. Inspired by the robot **TARS** from *Interstellar*, DAME is a compact and responsive system capable of real-time two-way audio communication via WebSocket to a server using OpenAI's Realtime API. Designed for portability, modularity, and fast interaction, DAME brings a playful yet powerful interface between human and machine.

---

## Team Members

- Garrett Young
- Matthew Sims
- Jack McGarrity
- Xander Steele
- Jack Corrao

---

## Features

- Real-time voice communication using OpenAI's Realtime API.
- WebSocket-based communication with dynamic text/audio streaming.
- Xbox Series X BLE controller integration for user commands.
- Live transcription and text display using a Nextion screen.
- I2S-based microphone and speaker audio system.
- Modular code structure for easy expansion.

---

## Hardware Overview

- **Microphone:** [Adafruit I2S MEMS Microphone Breakout - SPH0645LM4H](https://www.adafruit.com/product/3421)
- **Speaker:** MAX98357A I2S DAC and Amplifier
- **Controller:** Xbox Series X Controller (Firmware v5.x+, BLE Mode)
- **Display:** [Nextion Intelligent Series 7.0" HMI Touch Display NX8048P070-011R](https://nextion.tech/datasheets/nx8048p070-011r/)
- **MCU:** ESP32-S3-WROOM-1

---

## Pin Mapping

| Peripheral      | Signal          | ESP32-S3 Pin |
|-----------------|-----------------|--------------|
| Microphone (I2S) | Serial Data (SD) | GPIO 42      |
|                 | Word Select (WS) | GPIO 41      |
|                 | Serial Clock (SCK) | GPIO 47    |
| Speaker (I2S)    | Bit Clock (BCLK) | GPIO 18      |
|                 | Left/Right Clock (LRC) | GPIO 35 |
|                 | Data Input (DIN) | GPIO 45      |
| Nextion Display  | TX (ESP32 -> Nextion RX) | GPIO 17 |
|                 | RX (ESP32 <- Nextion TX) | GPIO 16 |
| Button           | Button Input    | GPIO 46      |
| LEDs             | Microphone Activity LED | GPIO 3 |
|                 | Speaker Activity LED | GPIO 41   |

---

## Software Overview

### Main Components

- `mic.cpp/.h`: Handles microphone setup, recording, and sound detection.
- `speaker.cpp/.h`: Plays received audio with pitch adjustment.
- `websocket.cpp/.h`: Manages WebSocket connection and message handling.
- `NextionDisplay.cpp/.h`: Controls text output on the Nextion screen.
- `xboxControl.cpp/.h`: Integrates Xbox controller input.
- `button.cpp/.h`: Handles a local hardware button for manual recording control.
- `lib_wifi.cpp/.h`: Manages WiFi connection.
- `config.h`: Stores all pin mappings, audio settings, and WiFi/WebSocket configuration.

### Platform

- **Framework:** Arduino
- **Platform:** ESP-IDF (via PlatformIO)
- **Dependencies:** ArduinoWebsockets, ESP32-audioI2S, ArduinoJson, NimBLE-Arduino, XboxSeriesXControllerESP32_asukiaaa

---

## Quick Installation

1. Clone the project to your local machine.
2. Open the project in PlatformIO (VSCode).
3. Connect your ESP32-S3 board.
4. Flash the firmware (`platformio run --target upload`).
5. Ensure your WiFi and WebSocket server IP are properly set in `config.h`.
6. Attach hardware according to the pin mappings provided.

---

## How to Use

- **Power on DAME**: The system boots and displays "Ready for listening".
- **Send Commands**:
  - **Press the "Y" button** on the Xbox controller or the physical button to start recording.
  - **Release the button** to stop recording and send the captured audio to the server.
  - **Audio Response**: DAME plays the AI's response through the speaker and prints text to the Nextion screen.
  - **Press the "Start" button** on the Xbox controller to reset the system if it is unresponsive.

---

## Acknowledgments

This project would not have been possible without the work of the open-source community. Special thanks to:

- [FabrikappAgency - esp32-realtime-voice-assistant](https://github.com/FabrikappAgency/esp32-realtime-voice-assistant) for voice assistant code structure.
- [Asuki Kono - XboxSeriesXControllerESP32_asukiaaa](https://github.com/asukiaaa/arduino-XboxSeriesXControllerESP32) for BLE Xbox controller library.

---

## Future Improvements

- **Wake Word Functionality**: Implement voice activation (e.g., say "TARS" to trigger listening mode) to eliminate button presses.
- **Noise Reduction**: Improve microphone input quality with adaptive filtering.
- **Battery Integration**: Add portable power solutions for true field deployment.

---

## License

This project is for educational purposes under the University of Notre Dame's Senior Design Capstone. Open-source components are credited above under their respective licenses.
