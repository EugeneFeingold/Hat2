#include "HMSolid.h"
#include "StripUtils.h"
#include "Settings.h"


HMSolid::HMSolid() {};


void HMSolid::init() {

};


void HMSolid::loop() {
  uint32_t c = StripUtils().getColor(settings->brightness, 127, 0, 0);

  for (int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, c);
  }
  strip->show();





};



