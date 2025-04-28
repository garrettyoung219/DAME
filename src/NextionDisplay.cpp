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
