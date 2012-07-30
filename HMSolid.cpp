#include "HMSolid.h"
#include "StripUtils.h"
#include "Settings.h"


HMSolid::HMSolid(LPD8806 *strip, Settings *settings) {
  this->strip = strip;
  this->settings = settings;
};


void HMSolid::init() {

};


void HMSolid::loop() {
  uint32_t c = StripUtils().getColor(settings->brightness, 127, 0, 0);
  
  for (int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, c);
  }
  strip->show();
  
  
 
  
  
};


