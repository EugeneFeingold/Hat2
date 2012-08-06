#include <stdint.h>
#include <ffft.h>



#include "SPI.h"
#include "LPD8806.h"

#include "Settings.h"
#include "StripUtils.h"
#include "Writer.h"

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

volatile byte audioBufferPosition = 0;

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


//  init - loop - shutdown

void (*modes[NUM_MODES][3])(void) = { 
  { &hmRainbowInit, &hmRainbowLoop, NULL } ,
  { &hmFullRainbowInit, &hmFullRainbowLoop, NULL },
  { &hmMatricesInit, &hmMatricesLoop, NULL },
  { NULL, &hmRandomSquaresLoop, NULL},
  { NULL, &hmRandomStripesLoop, NULL},
  { NULL, &hmSolidLoop, NULL}
};


int btn1State = 0;
int mode = 0;


void setup() {
  pinMode(button1Pin, INPUT);
  digitalWrite(button1Pin, HIGH); //enable pullup
  
  pinMode(led1Pin, OUTPUT);
  digitalWrite(led1Pin, HIGH);
  
  strip.begin();
  reset();

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

  modes[mode][1]();
}


void incrementMode() {
  if (modes[mode][2] != NULL) {
    modes[mode][2]();
  }
 
  mode++;
  if (mode >= NUM_MODES) {
    mode = 0;
  }
  
  reset();
  
  if (modes[mode][0] != NULL) {
    modes[mode][0]();
  }
}


void reset() {
  for (int i = 0; i < NUM_LIGHTS; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}










//============ ============  common mode vars ============ ============ 

int hmj;



//============ ============  Rainbow ============ ============ 

void hmRainbowInit() {
  hmj = 0;
}

void hmRainbowLoop() {
   for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, StripUtils().getWheelColor(settings.brightness, ((i * 128 / strip.numPixels()) + (int)hmj) % 128));
  }
  strip.show();

  hmj+= 16 - 32 * settings.rate;
  if (hmj > 128 * 5) {
    hmj = 0;
  } 
}


//============ ============  Full Rainbow ============ ============ 

void hmFullRainbowInit() {
  hmj = 0;
}

void hmFullRainbowLoop() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, StripUtils().getWheelColor(settings.brightness, (int)hmj));
  }
  strip.show();

  hmj+= 8 * settings.rate;
  if (hmj > 127) {
    hmj = 0;
  }
}

//============ ============  Matrices ============ ============ 
#define MATRIX_WIDTH 6

static int matrixPattern[6][MATRIX_WIDTH] = {
  { 0, 1, 0, 1, 0, 0  },
  { 1, 1, 1, 1, 1, 0  },
  { 1, 1, 1, 1, 1, 0  },
  { 0, 1, 1, 1, 0, 0  },
  { 0, 0, 1, 0, 0, 0  }    
};

void hmMatricesInit() {
  hmj = MATRIX_WIDTH * 10;
}

void hmMatricesLoop() {
  uint32_t c = StripUtils().getColor(settings.brightness, 127, 0, 0);

  for (int i = 0; i < strip.numPixels(); i++) {
    int p = i + hmj / 10;
    strip.setPixelColor(i, (matrixPattern[i / 18][p % MATRIX_WIDTH] == 1) ? c : 0);
  }
  strip.show();

  delay(50 - 50 * settings.rate);

  hmj--;
  if (hmj < 0) {
    hmj = MATRIX_WIDTH * 10;
  }
}


//============ ============  Random Squares ============ ============ 

void hmRandomSquaresLoop() {
  for (int x = 0; x < 18; x+=2) {
    for (int y = 0; y < 5; y+=2) {
      uint32_t col = StripUtils().getRandomBalancedColor(settings.brightness);
      //uint32_t col = StripUtils().getRandomColor(settings -> brightness);
      strip.setPixelColor(y * 18 + x, col);
      strip.setPixelColor((y + 1) * 18 + x, col);
      strip.setPixelColor(y * 18 + x + 1, col);
      strip.setPixelColor((y + 1) * 18 + x + 1, col);
    }
  }
  strip.show();
  delay(1000 - settings.rate * 1000);
}


//============ ============  Random Stripes ============ ============ 

void hmRandomStripesLoop() {  
  for (int x = 0; x < 18; x++) {
    //uint32_t col = StripUtils().getRandomBalancedColor(settings -> brightness);
    uint32_t col = StripUtils().getWheelColor(settings.brightness, random(128));
    for (int y = 0; y < 5; y++) {
      strip.setPixelColor(y * 18 + x, col);
    }
  }
  strip.show();
  delay(1000 - settings.rate * 1000);
}


//============ ============  Solid ============ ============ 

void hmSolidLoop() {
  uint32_t c = StripUtils().getWheelColor(settings.brightness, 128 * settings.rate);
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}


//============ ============  AUDIO ============ ============ 

void hmAudioInit() {
  /*  REFS0 : VCC use as a ref, IR_AUDIO : channel selection, ADEN : ADC Enable, ADSC : ADC Start, ADATE : ADC Auto Trigger Enable, ADIE : ADC Interrupt Enable,  ADPS : ADC Prescaler  */
  // free running ADC mode, f = ( 16MHz / prescaler ) / 13 cycles per conversion 
  ADMUX = _BV(REFS0) | IR_AUDIO; // | _BV(ADLAR); 
  //  ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) //prescaler 64 : 19231 Hz - 300Hz per 64 divisins
  ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // prescaler 128 : 9615 Hz - 150 Hz per 64 divisions, better for most music
  sei();
}

void hmAudioLoop() {
  
}

// free running ADC fills capture buffer
ISR(ADC_vect)
{
  if (audioBufferPosition >= FFT_N)
    return;

  capture[audioBufferPosition] = ADC; // + zero;
  if (capture[audioBufferPosition] == -1 || capture[audioBufferPosition] == 1)
    capture[audioBufferPosition] = 0;

  audioBufferPosition++;
}

void hmAudioShutdown() {
  
}



