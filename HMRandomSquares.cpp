
#include "HMRandomSquares.h"
#include "StripUtils.h"
#include "Settings.h"



HMRandomSquares::HMRandomSquares() {
};

void HMRandomSquares::init(LPD8806 *strip, Settings *settings) {
  super::init(strip, settings);
};


void HMRandomSquares::loop() {

  //uint32_t col = StripUtils().getWheelColor(settings->brightness, (int)random(128));

for (int x = 0; x < 18; x+=2) {
  for (int y = 0; y < 5; y+=2) {


    uint32_t col = StripUtils().getRandomBalancedColor(settings -> brightness);
    //uint32_t col = StripUtils().getRandomColor(settings -> brightness);
  

  
    strip->setPixelColor(y * 18 + x, col);
    strip->setPixelColor((y + 1) * 18 + x, col);
    strip->setPixelColor(y * 18 + x + 1, col);
    strip->setPixelColor((y + 1) * 18 + x + 1, col);

    }
  }
 strip->show();

  delay(1000 - settings->rate * 1000);



};


