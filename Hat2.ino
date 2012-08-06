#include <stdint.h>
#include <ffft.h>



#include "SPI.h"
#include "LPD8806.h"

#include "Settings.h"
#include "Writer.h"

#include "HMRainbow.h"
#include "HMFullRainbow.h"
#include "HMSolid.h"
#include "HMBoxes.h"
#include "HMAudio.h"
#include "HMRandomSquares.h"

#include "HMRandomStripes.h"


#define NUM_LIGHTS 96
#define NUM_MODES 6

//----------- PINS ----------- 
int dataPin  = 2;    // Yellow wire on Adafruit Pixels
int clockPin = 3;    // Green wire on Adafruit Pixels

int button1Pin = 9;
int led1Pin = 8;


int brightnessPin = A0;
int ratePin = A1;
int microphonePin = A5;


//----------- AUDIO STUFF ------------------
#define  IR_AUDIO  5 // ADC channel to capture
#define NUM_BUCKETS 18

volatile  byte  position = 0;

int16_t capture[FFT_N]; 		/* Wave capturing buffer */
complex_t bfly_buff[FFT_N]; 		/* FFT buffer */
uint16_t spectrum[FFT_N/2]; 		/* Spectrum output buffer */

float sBuffer[NUM_BUCKETS];

float minima[NUM_BUCKETS];
float maxima[NUM_BUCKETS];


//-------------- INIT ------------------

LPD8806 strip = LPD8806(NUM_LIGHTS, dataPin, clockPin);
Settings settings = Settings();
Writer writer = Writer();


HM_Base *modes[NUM_MODES];


int btn1State = 0;
int mode = 0;


void setup() {

  pinMode(button1Pin, INPUT);
  digitalWrite(button1Pin, HIGH); //enable pullup
  
  
  pinMode(led1Pin, OUTPUT);
  digitalWrite(led1Pin, HIGH);


  //init modes
  modes[0] = new HMRainbow();
  modes[1] = new HMFullRainbow();
  modes[2] = new HMRandomSquares();
  modes[3] = new HMRandomStripes();
  modes[4] = new HMBoxes();
  modes[5] = new HMSolid();
  
  strip.begin();
  reset();
  
  modes[mode]->init(&strip, &settings);  


  //adcInit();



  Serial.begin(57600);
}


void loop() {
  int b = digitalRead(button1Pin);

  if (btn1State == 0 && b == HIGH) {
    btn1State = 1;
    digitalWrite(led1Pin, LOW);
  } 
  else if (btn1State == 1 && b == LOW) {
    incrementMode();
    btn1State = 0;
    digitalWrite(led1Pin, HIGH);
  }

  settings.brightness = analogRead(brightnessPin) / 1024.0;
  settings.rate = analogRead(ratePin) / 1024.0;

//Serial.println(settings.rate, DEC);

  modes[mode]->loop();

}


void incrementMode() {
   mode++;
    if (mode >= NUM_MODES) {
      mode = 0;
    }
    
    
    reset();
    modes[mode]->init(&strip, &settings);  
}


void reset() {
  for (int i = 0; i < NUM_LIGHTS; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}





void adcInit(){
  /*  REFS0 : VCC use as a ref, IR_AUDIO : channel selection, ADEN : ADC Enable, ADSC : ADC Start, ADATE : ADC Auto Trigger Enable, ADIE : ADC Interrupt Enable,  ADPS : ADC Prescaler  */
  // free running ADC mode, f = ( 16MHz / prescaler ) / 13 cycles per conversion 
  ADMUX = _BV(REFS0) | IR_AUDIO; // | _BV(ADLAR); 
  //  ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) //prescaler 64 : 19231 Hz - 300Hz per 64 divisins
    ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // prescaler 128 : 9615 Hz - 150 Hz per 64 divisions, better for most music
  sei();
}


// free running ADC fills capture buffer
ISR(ADC_vect)
{
  if (position >= FFT_N)
    return;

  capture[position] = ADC; // + zero;
  if (capture[position] == -1 || capture[position] == 1)
    capture[position] = 0;

  position++;
}



