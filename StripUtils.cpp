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

  return getColor ((uint8_t)fr, (uint8_t)fg, (uint8_t)fb);
};



uint32_t StripUtils::getWheelColor(float brightness, byte WheelPos)
{
  if (WheelPos < 42) {
    return getColor(brightness, WheelPos * 3, 127 - WheelPos * 3, 0);
  } 
  else if (WheelPos < 170) {
    WheelPos -= 42;
    return getColor(brightness, 127 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 85; 
    return getColor(brightness, 0, WheelPos * 3, 127 - WheelPos * 3);
  }
}







float StripUtils::fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve){

  float originalRange = 0;
  float newRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine 
  if (originalMin > originalMax ) {
    return 0;
  }

  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output 
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  /*
   Serial.println(curve * 100, DEC);   // multply by 100 to preserve resolution  
   Serial.println(); 
   */

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Reference the values
  originalRange = originalMax - originalMin;
  newRange = newEnd - newBegin;

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / originalRange;   // normalize to 0 - 1 float

  /*
  Serial.print(originalRange, DEC);  
   Serial.print("   ");  
   Serial.print(newRange, DEC);  
   Serial.print("   ");  
   Serial.println(zeroRefCurVal, DEC);  
   Serial.println();  
   */


  return newBegin + pow(normalizedCurVal, curve) * newRange;
}







