#include "NextionDisplay.h"

NextionDisplay::NextionDisplay(HardwareSerial& serial, uint8_t tx, uint8_t rx)
  : nextion(serial) {
  nextion.begin(9600, SERIAL_8N1, rx, tx);
}

void NextionDisplay::begin(uint32_t baud) {
  nextion.begin(baud, SERIAL_8N1);
  currentLine = "";
  displayText = "";
  updateNextionText("");
}

void NextionDisplay::printDelta(const String& delta) {
  // static String currentLine = "";
  // static String displayText = "";

  String word = delta;

  // Check if it's the beginning of a word (leading space)
  bool startsWithSpace = word.length() > 0 && word.charAt(0) == ' ';

  // Count projected line length if we added the word
  int projectedLength = currentLine.length() + word.length();

  // If it would overflow, wrap to new line
  if (projectedLength > CHARS_PER_LINE) {
    displayText += "\r\n";

    // Remove leading space so it doesn't indent
    if (startsWithSpace) {
      word = word.substring(1);
    }

    currentLine = word;
  } else {
    currentLine += word;
  }

  displayText += word;

  // Optional: limit text size to prevent overflow
  if (displayText.length() > MAX_TEXT_LENGTH) {
    Serial.println("Exceeded max length!!!!!!!!!!!!!!!!!!!!");
    displayText = displayText.substring(displayText.length() - MAX_TEXT_LENGTH);
  }

  updateNextionText(displayText);
}

void NextionDisplay::printRaw(const String& text) {
  displayText = text;
  updateNextionText(displayText);
}

void NextionDisplay::updateNextionText(const String& partial) {
  nextion.print("t0.txt=\"" + partial + "\"");
  nextion.write(0xFF); nextion.write(0xFF); nextion.write(0xFF);
}

void NextionDisplay::clear() {
  currentLine = "";
  displayText = "";
  updateNextionText(""); // clear
}

void NextionDisplay::setGifVisible(bool visible) {
  nextion.print("vis p0,");
  nextion.print(visible ? "1" : "0");
  nextion.write(0xFF); nextion.write(0xFF); nextion.write(0xFF);
}

void NextionDisplay::updateTimeTask(void *param) {
  NextionDisplay* self = static_cast<NextionDisplay*>(param);
  while (true) {
      struct tm timeinfo;
      if (getLocalTime(&timeinfo)) {
          char timeStr[9]; // HH:MM:SS
          strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);

          String cmd = "tTime.txt=\"" + String(timeStr) + "\"";
          self->nextion.print(cmd);
          self->nextion.write(0xFF); self->nextion.write(0xFF); self->nextion.write(0xFF);
      }

      vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait 1 second
  }
}

void NextionDisplay::updateBatteryTask(void *param) {
  NextionDisplay* self = static_cast<NextionDisplay*>(param);

  int batteryPercentage = 100;  // or pass this as part of a struct if dynamic

  while (true) {
      if (batteryPercentage > 0) {
          batteryPercentage--;

          String cmd = "tBattery.txt=\"" + String(batteryPercentage) + "%\"";
          self->nextion.print(cmd);
          self->nextion.write(0xFF); self->nextion.write(0xFF); self->nextion.write(0xFF);

          Serial.printf("Battery: %d%%\n", batteryPercentage);
      }

      vTaskDelay(600000 / portTICK_PERIOD_MS); // 10 minutes
  }
}

