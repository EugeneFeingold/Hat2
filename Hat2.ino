
#include "SPI.h"
#include "LPD8806.h"

#include "Settings.h"

#include "HMRainbow.h"
#include "HMSolid.h"



#define NUM_LIGHTS 96
#define NUM_MODES 2

//----------- PINS ----------- 
int dataPin  = 2;    // Yellow wire on Adafruit Pixels
int clockPin = 3;    // Green wire on Adafruit Pixels

int brightnessPin = A0;
int microphonePin = A5;


LPD8806 strip = LPD8806(NUM_LIGHTS, dataPin, clockPin);
Settings settings = Settings();


IHatMode *modes[NUM_MODES];







void setup() {
  //init modes
  modes[0] = new HMRainbow(&strip, &settings);
  modes[1] = new HMSolid(&strip, &settings);
  
  
  //init Settings
  //Settings().setBrightness(0.0);

  for (int i = 0; i < NUM_MODES; i++) {
    modes[i]->init();
  }
  
  
  strip.begin();
  
  Serial.begin(57600);
}


void loop() {
  settings.brightness = analogRead(brightnessPin) / 1024.0;
    
  modes[1]->loop();
  
}
