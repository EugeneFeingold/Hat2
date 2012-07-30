#include "StripUtils.h"


uint32_t StripUtils::getColor(byte r, byte g, byte b) {
  uint32_t c;
  c = g;
  c <<= 8;
  c |= r;
  c <<= 8;
  c |= b;
  return c;
};


uint32_t StripUtils::getColor(float brightness, byte r, byte g, byte b) {
  float fr = brightness * r;
  float fg = brightness * g;
  float fb = brightness * b;
  
  Serial.println(getColor ((uint8_t)fr, (uint8_t)fg, (uint8_t)fb), HEX);
  
  return getColor ((uint8_t)fr, (uint8_t)fg, (uint8_t)fb);
};



uint32_t StripUtils::getWheelColor(float brightness, byte WheelPos)
{
  if (WheelPos < 42) {
   return getColor(brightness, WheelPos * 3, 127 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 42;
   return getColor(brightness, 127 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 85; 
   return getColor(brightness, 0, WheelPos * 3, 127 - WheelPos * 3);
  }
}
