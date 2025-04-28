#ifndef NEXTIONDISPLAY_H
#define NEXTIONDISPLAY_H

#include <Arduino.h>
#include <HardwareSerial.h>

class NextionDisplay {
public:
  NextionDisplay(HardwareSerial& serial, uint8_t tx, uint8_t rx);

  void begin(uint32_t baud = 9600);
  void printDelta(const String& delta); // Called with each delta.text chunk
  void printRaw(const String& text);
  void clear();
  void setGifVisible(bool visible);

private:
  void updateNextionText(const String& partial);

  HardwareSerial& nextion;
  String currentLine;
  String displayText;

  static const int CHARS_PER_LINE = 28;
  static const int MAX_TEXT_LENGTH = 1800; // Adjust for your Nextion textbox height

};

#endif
