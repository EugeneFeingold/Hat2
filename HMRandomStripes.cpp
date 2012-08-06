
#include "HMRandomStripes.h"
#include "StripUtils.h"
#include "Settings.h"



HMRandomStripes::HMRandomStripes() {
};

void HMRandomStripes::init(LPD8806 *strip, Settings *settings) {
  super::init(strip, settings);
};


void HMRandomStripes::loop() {

  //uint32_t col = StripUtils().getWheelColor(settings->brightness, (int)random(128));
  
  for (int x = 0; x < 18; x++) {
  
    //uint32_t col = StripUtils().getRandomBalancedColor(settings -> brightness);
    uint32_t col = StripUtils().getWheelColor(settings -> brightness, random(128));
    
    
    for (int y = 0; y < 5; y++) {
        strip->setPixelColor(y * 18 + x, col);
    }
  
  }
  
 strip->show();

  delay(1000 - settings->rate * 1000);



};


