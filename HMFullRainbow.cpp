
#include "HMFullRainbow.h"
#include "StripUtils.h"
#include "Settings.h"



HMFullRainbow::HMFullRainbow() {};

void HMFullRainbow::init(LPD8806 *strip, Settings *settings) {
  j = 0;
  super::init(strip, settings);
};


void HMFullRainbow::loop() {
  for (int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, StripUtils().getWheelColor(settings->brightness, (int)j));
  }
  strip->show();

//Serial.println(j);

  j+= 8 * settings->rate;
  if (j > 127) {
    j = 0;
  }


};

