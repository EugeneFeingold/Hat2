
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


HM_Base *modes[NUM_MODES];


int btn1State = 0;
int mode = 0;





void setup() {

  pinMode(button1Pin, INPUT);
  pinMode(led1Pin, OUTPUT);


  digitalWrite(led1Pin, LOW);


  //init modes
  modes[0] = new HMRainbow();
  modes[1] = new HMSolid();
  modes[2] = new HMBoxes();


  //init Settings
  //Settings().setBrightness(0.0);

  strip.begin();
  reset();

  Serial.begin(57600);
}


void loop() {
  int b = digitalRead(button1Pin);

  if (btn1State == 0 && b == HIGH) {
    btn1State = 1;
    digitalWrite(led1Pin, LOW);

    mode++;
    if (mode >= NUM_MODES) {
      mode = 0;
    }

    reset();
    modes[mode]->init(&strip, &settings); 

  } 
  else if (btn1State == 1 && b == LOW) {
    btn1State = 0;
    digitalWrite(led1Pin, HIGH);
  }

  settings.brightness = analogRead(brightnessPin) / 1024.0;

  modes[mode]->loop();

}



void reset() {
  for (int i = 0; i < NUM_LIGHTS; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}


