#pragma once
#ifndef inclStripUtils
#define inclStripUtils


#include "LPD8806.h"

class StripUtils {
public:

  static uint32_t getColor(byte r, byte g, byte b);
  static uint32_t getColor(float brightness, byte r, byte g, byte b);

  static uint32_t getWheelColor(float brightness, byte WheelPos);
  static uint32_t getRandomColor(float brightness);
  static uint32_t getRandomBalancedColor(float brightness);

  static uint32_t getIntermediateColor(uint32_t first, uint32_t second, float frac);


private: 


};



#endif

