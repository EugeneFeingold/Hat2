
#include "SPI.h"
#include "LPD8806.h"

#include "Settings.h"

#include "HMRainbow.h"
#include "HMSolid.h"
#include "HMBoxes.h"



#define NUM_LIGHTS 96
#define NUM_MODES 3

//----------- PINS ----------- 
int dataPin  = 2;    // Yellow wire on Adafruit Pixels
int clockPin = 3;    // Green wire on Adafruit Pixels

int button1Pin = 9;
int led1Pin = 8;


int brightnessPin = A0;
int microphonePin = A5;


LPD8806 strip = LPD8806(NUM_LIGHTS, dataPin, clockPin);
Settings settings = Settings();


IHatMode *modes[NUM_MODES];


int btn1State = 0;
int mode = 0;





void setup() {

  pinMode(button1Pin, INPUT);
  pinMode(led1Pin, OUTPUT);

  //init modes
  modes[0] = new HMRainbow(&strip, &settings);
  modes[1] = new HMSolid(&strip, &settings);
  modes[2] = new HMBoxes(&strip, &settings);


  //init Settings
  //Settings().setBrightness(0.0);

  for (int i = 0; i < NUM_MODES; i++) {
    modes[i]->init();
  }


  strip.begin();

  Serial.begin(57600);
}


void loop() {
  int b = digitalRead(button1Pin);

  if (btn1State == 0 && b == HIGH) {
    btn1State = 1;
    digitalWrite(led1Pin, HIGH);

    mode++;
    if (mode >= NUM_MODES) {
      mode = 0;
    }

    modes[mode]->init(); 

  } 
  else if (btn1State == 1 && b == LOW) {
    btn1State = 0;
    digitalWrite(led1Pin, LOW);
  }

  settings.brightness = analogRead(brightnessPin) / 1024.0;

  modes[mode]->loop();

}


