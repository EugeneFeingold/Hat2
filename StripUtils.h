#pragma once
#ifndef inclStripUtils
#define inclStripUtils


#include "LPD8806.h"

class StripUtils {
public:

  static uint32_t getColor(byte r, byte g, byte b);
  static uint32_t getColor(float brightness, byte r, byte g, byte b);

  static uint32_t getWheelColor(float brightness, byte WheelPos);

  static float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve);

private: 


};



#endif

