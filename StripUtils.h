#ifndef inclStripUtils
#define inclStripUtils


#include "LPD8806.h"

class StripUtils {
 public:
  
  static uint32_t getColor(byte r, byte g, byte b);
  static uint32_t getColor(float brightness, byte r, byte g, byte b);

  static uint32_t getWheelColor(float brightness, byte WheelPos);


 private: 
  
  
};



#endif
