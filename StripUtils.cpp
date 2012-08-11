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
  brightness = min(brightness, 1.0);
  
  float fr = brightness * r;
  float fg = brightness * g;
  float fb = brightness * b;

  return getColor ((uint8_t)fr, (uint8_t)fg, (uint8_t)fb);
};



uint32_t StripUtils::getWheelColor(float brightness, byte WheelPos)
{
  WheelPos = WheelPos & 0x7F;
  if (WheelPos < 42) {
    return getColor(brightness, WheelPos * 3, 127 - WheelPos * 3, 0);
  } 
  else if (WheelPos < 85) {
    WheelPos -= 42;
    return getColor(brightness, 127 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 85; 
    return getColor(brightness, 0, WheelPos * 3, 127 - WheelPos * 3);
  }
}



uint32_t StripUtils::getRandomColor(float brightness) {
  return getColor(brightness, random(128), random(128), random(128));
}

uint32_t StripUtils::getRandomBalancedColor(float brightness) {
  unsigned int r = random(128);
  unsigned int g = random(128);
  return getColor(brightness, r, g, 128-(r + g)/2);
}



uint32_t StripUtils::getIntermediateColor(uint32_t first, uint32_t second, float frac) {
  //color order is grb
  uint8_t r1 = first >> 8;
  uint8_t g1 = first >> 16;
  uint8_t b1 = first ;
  
  uint8_t r2 = second >> 8;
  uint8_t g2 = second >> 16;
  uint8_t b2 = second;
    
  return getColor(r1 + (r2 - r1) * frac, g1 + (g2 - g1) * frac, b1 + (b2 - b1) * frac);
}






