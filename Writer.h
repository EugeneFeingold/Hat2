#pragma once
#ifndef inclWriter
#define inclWriter


#include <Arduino.h>
#include "LPD8806.h"
#include "Settings.h"


class Writer {

   static byte digits[10][5];


public:
  void write(LPD8806 *strip, Settings *settings, char text[], unsigned short length);



private:


};






#endif

