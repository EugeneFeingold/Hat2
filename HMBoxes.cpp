#include "HMBoxes.h"
#include "StripUtils.h"
#include "Settings.h"

#define BOXES_WIDTH 6


static int pattern[6][BOXES_WIDTH] = {
  {
    0, 1, 0, 1, 0, 0  }
  ,
  {
    1, 1, 1, 1, 1, 0  }
  ,
  {
    1, 1, 1, 1, 1, 0  }
  ,
  {
    0, 1, 1, 1, 0, 0  }
  ,
  {
    0, 0, 1, 0, 0, 0  }    
};


HMBoxes::HMBoxes(LPD8806 *strip, Settings *settings) {
  this->strip = strip;
  this->settings = settings;
};




void HMBoxes::init() {
  j = BOXES_WIDTH * 10;
};


void HMBoxes::loop() {
  uint32_t c = StripUtils().getColor(settings->brightness, 127, 0, 0);

  for (int i = 0; i < strip->numPixels(); i++) {
    int p = i + j / 10;
    strip->setPixelColor(i, (pattern[i / 18][p % BOXES_WIDTH] == 1) ? c : 0);
  }
  strip->show();


  j--;
  if (j < 0) {
    j = BOXES_WIDTH * 10;
  }



};



