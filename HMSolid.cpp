#include "HMSolid.h"
#include "StripUtils.h"
#include "Settings.h"


HMSolid::HMSolid() {};


void HMSolid::init(LPD8806 *strip, Settings *settings) {

};


void HMSolid::loop() {
  uint32_t c = StripUtils().getWheelColor(settings->brightness, 128 * settings -> rate);

  for (int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, c);
  }
  strip->show();





};



