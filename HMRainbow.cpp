
#include "HMrainbow.h"
#include "StripUtils.h"
#include "Settings.h"



HMRainbow::HMRainbow(LPD8806 *strip, Settings *settings) {
  this->strip = strip;
  this->settings = settings;
};


void HMRainbow::init() {
  j = 0;
};


void HMRainbow::loop() {
    for (int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, StripUtils().getWheelColor(settings->brightness, ((i * 128 / strip->numPixels()) + j) % 128));
  }
  strip->show();
  
  
  
  j+=5;
  if (j > 128 * 5) {
    j = 0;
  }
  
  
};
