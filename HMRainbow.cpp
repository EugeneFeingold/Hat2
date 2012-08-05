
#include "HMrainbow.h"
#include "StripUtils.h"
#include "Settings.h"



HMRainbow::HMRainbow() {};

void HMRainbow::init(LPD8806 *strip, Settings *settings) {
  j = 0;
  super::init(strip, settings);
};


void HMRainbow::loop() {
  for (int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, StripUtils().getWheelColor(settings->brightness, ((i * 128 / strip->numPixels()) + (int)j) % 128));
  }
  strip->show();



  j+= 16 - 32 * settings->rate;
  if (j > 128 * 5) {
    j = 0;
  }


};

