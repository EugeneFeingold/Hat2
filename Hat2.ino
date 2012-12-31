#include <stdint.h>
#include <ffft.h>


#define ENCODER_OPTIMIZE_INTERRUPTS



#include "SPI.h"
#include "LPD8806.h"
#include "Encoder.h"

#include "Settings.h"
#include "StripUtils.h"
#include "Writer.h"

#define NUM_LIGHTS 96

//----------- PINS ----------- 

const short encPinA = 2; 
const short encPinB = 3;

const short dataPin  = 6;    // white wire on Adafruit Pixels
const short clockPin = 7;    // Green wire on Adafruit Pixels

const short button1Pin = 12;
const short led1Pin = 10;


const short brightnessPin = A0;
const short ratePin = A3;
const short microphonePin = A5;




//-------------- INIT ------------------

LPD8806 strip = LPD8806(NUM_LIGHTS, dataPin, clockPin);
Settings settings = Settings();
Writer writer = Writer();

Encoder modeEnc(encPinA, encPinB);



#define NUM_MODES 5

//  init - loop - shutdown

void (*modes[NUM_MODES][3])(void) = { 
  //{ &hmAudioInit, &hmAudioLoop, &hmAudioShutdown}, 
  { &hmRainbowInit, &hmRainbowLoop, NULL } ,
  //{ &hmSidewaysRainbowInit, &hmSidewaysRainbowLoop, NULL } ,
  //{ &hmFullRainbowInit, &hmFullRainbowLoop, NULL },
  //{ NULL, &hmRandomSquaresLoop, NULL},
  { NULL, &hmRandomStripesLoop, NULL},
  //{ &hmRotatingStripeInit, &hmRotatingStripeLoop, NULL},
  //{ NULL, &hmSolidLoop, NULL},
  { NULL, &hmWhiteSparkleLoop, NULL},
  { NULL, &hmSparkleLoop, NULL},
  { NULL, &hmWaves, NULL},
  //{ &hmMatricesInit, &hmMatricesLoop, NULL }

};


#define NUM_AUDIO_MODES 3

void (*audioModes[NUM_AUDIO_MODES])(void) = {
  &hmAudioEffect2,
  &hmAudioEffect3,
  hmAudioSparkle
};



int btn1State = 0;
long oldModePos = -999;
long modeTimer = 0;
short modeDir = 0;
boolean audioMode = false;
int mode = 0;
unsigned long m;



void setup() {
  pinMode(button1Pin, INPUT);
  digitalWrite(button1Pin, HIGH); //enable pullup
  
  pinMode(led1Pin, OUTPUT);
  digitalWrite(led1Pin, LOW);
  
  strip.begin();
  reset();
  
  randomSeed(analogRead(A1));
  
  mode = random(NUM_MODES);
  

  if (modes[mode][0] != NULL) {
    modes[mode][0]();
  }
  
  oldModePos = modeEnc.read();

  Serial.begin(57600);
}


void loop() {
  long newModePos = modeEnc.read();
  if (newModePos != oldModePos) {
    modeDir = (newModePos > oldModePos) ? 1 : -1;
    oldModePos = newModePos;
    modeTimer = millis() + 200;
  }
  if (modeDir != 0) {
    if (modeTimer > millis()) {
      reset();
      return;
    }
    incrementMode(modeDir);
    modeTimer = 0;
    modeDir = 0;
  }

  int b = digitalRead(button1Pin);

  if (btn1State == 0 && b == HIGH) {
    btn1State = 1;
  } 
  else if (btn1State == 1 && b == LOW) {
    audioMode = !audioMode;
    audioMode ? hmAudioInit() : hmAudioShutdown();
    mode = 0;
    btn1State = 0;
    digitalWrite(led1Pin, audioMode);
  }

  settings.brightness = analogRead(brightnessPin) / 1024.0;
  settings.rate = analogRead(ratePin) / 1024.0;

  audioMode ? hmAudioLoop() : modes[mode][1]();
}


void incrementMode(short dir) {
  if (!audioMode && modes[mode][2] != NULL) {
    modes[mode][2]();
  }
 
 Serial.println(mode);
 
  mode+=dir;
  if (mode >= (audioMode ? NUM_AUDIO_MODES : NUM_MODES)) {
    mode = 0;
  } else if (mode < 0) {
    mode = (audioMode ? NUM_AUDIO_MODES : NUM_MODES - 1);
  }
  
  reset();
  
  if (!audioMode && modes[mode][0] != NULL) {
    modes[mode][0]();
  }
  
  m = 0;
}




void reset() {
  for (int i = 0; i < NUM_LIGHTS; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}










//============ ============  common mode vars ============ ============ 


int hmj;
int hmj1;
float hmk;
uint32_t hmcol;


//============ ============  Rainbow ============ ============ 

void hmRainbowInit() {
  hmk = 0;
}

void hmRainbowLoop() {
   for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, StripUtils().getWheelColor(settings.brightness, ((i * 128 / strip.numPixels()) + (int)hmk) % 128));
  }
  strip.show();

  hmk += 16 - 32 * settings.rate;
  if (hmk > 128 * 5) {
    hmk = 0;
  } 
}

//============ ============ Sideways Rainbow ============ ============ 

void hmSidewaysRainbowInit() {
  hmk = 0;
}

void hmSidewaysRainbowLoop() {
 
  for (int x = 0; x < 18; x++) {
     for (int y = 0; y < 5; y++) {
       strip.setPixelColor(y * 18 + x, StripUtils().getWheelColor(settings.brightness, (x * 5 + y) * 128 / strip.numPixels() + (int)hmk));
     } 
   }
  strip.show();

  hmk += 8 - 16 * settings.rate;
  if (hmk > 128 * 5) {
    hmk = 0;
  } 
}


//============ ============  Full Rainbow ============ ============ 

void hmFullRainbowInit() {
  hmk = 0;
}

void hmFullRainbowLoop() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, StripUtils().getWheelColor(settings.brightness, (int)hmk));
  }
  strip.show();

  hmk+= 8 * settings.rate;
  if (hmk > 127) {
    hmk = 0;
  }
}

//============ ============  Matrices ============ ============ 
#define MATRIX_WIDTH 6

/*
static int heartPattern[6][MATRIX_WIDTH] = {
  { 0, 1, 0, 1, 0, 0  },
  { 1, 1, 1, 1, 1, 0  },
  { 1, 1, 1, 1, 1, 0  },
  { 0, 1, 1, 1, 0, 0  },
  { 0, 0, 1, 0, 0, 0  }    
};
*/

static int matrixPattern[6][MATRIX_WIDTH] = {
  { 1, 0, 1, 0, 1, 0  },
  { 0, 1, 0, 1, 0, 0  },
  { 0, 0, 1, 0, 0, 0  },
  { 0, 1, 0, 1, 0, 0  },
  { 1, 0, 0, 0, 1, 0  }    
};

void hmMatricesInit() {
  hmj = MATRIX_WIDTH * 10;
}

void hmMatricesLoop() {
  if (millis() < m + 50 - 50 * settings.rate) {
    return;
  }
  
  uint32_t c = StripUtils().getColor(settings.brightness, 127, 16, 0);

  for (int i = 0; i < strip.numPixels(); i++) {
    int p = i + hmj / 10;
    strip.setPixelColor(i, (matrixPattern[i / 18][p % MATRIX_WIDTH] == 1) ? c : 0);
  }
  strip.show();

  hmj--;
  if (hmj < 0) {
    hmj = MATRIX_WIDTH * 10;
  }
  
  m = millis();
}


//============ ============  Random Squares ============ ============ 

void hmRandomSquaresLoop() {
  if (millis() < m + 1000 - settings.rate * 1000) {
    return;
  }
  
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
  m = millis();
}


//============ ============  Random Stripes ============ ============ 

void hmRandomStripesLoop() {  
  if (millis() < m + 1000 - settings.rate * 1000) {
    return;
  }
  for (int x = 0; x < 18; x++) {
    //uint32_t col = StripUtils().getRandomBalancedColor(settings -> brightness);
    uint32_t col = StripUtils().getWheelColor(settings.brightness, random(128));
    for (int y = 0; y < 5; y++) {
      strip.setPixelColor(y * 18 + x, col);
    }
  }
  strip.show();  
  m = millis();
}


//============ ============  Solid ============ ============ 
 
void hmSolidLoop() {
  uint32_t c = StripUtils().getWheelColor(settings.brightness, 128 * settings.rate);
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}



//============ ============  Rotating Stripe ============ ============ 
 
 void hmRotatingStripeInit() {
   hmcol = StripUtils().getWheelColor(settings.brightness, 128 * settings.rate);
   hmj = 0; 
   hmj1 = 0;
 }

void hmRotatingStripeLoop() {
  if (millis() < m + 1000 - settings.rate * 1000) {
    return;
  }
  
  if (hmj > 17) {
    hmj = 0;
  } else if (hmj < 0) {
    hmj = 17;
  }
  if (hmj1 > 128) {
    hmj1 = 0;
  }
  for (int i = 0; i < strip.numPixels(); i++) {
    //strip.setPixelColor(i, (i % 18 == hmj) ? hmcol : 0);      
    strip.setPixelColor(i, (i % 18 == hmj) ? StripUtils().getWheelColor(settings.brightness, hmj1) : StripUtils().getIntermediateColor(strip.getPixelColor(i), 0, 0.3));

  }
  strip.show();
  hmj++;
  hmj1++;
  m = millis();

}


//============ ============ White Sparkle ============ ============ 

void hmWhiteSparkleLoop() {
  if (millis() < m + 1000 - settings.rate * 1000) {
    return;
  }
  
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, (random(100) > 70) ? StripUtils().getColor(settings.brightness, 255, 255, 255) : 0);
  }
  strip.show();
  m = millis();
}

//============ ============ Sparkle ============ ============ 

void hmSparkleLoop() {
  if (millis() < m + 1000 - settings.rate * 1000) {
    float fadeRate = 0.08 * settings.rate;
    for (int i = 0; i < strip.numPixels(); i++) {
          strip.setPixelColor(i, StripUtils().getIntermediateColor(strip.getPixelColor(i), 0, fadeRate));
    }
    strip.show();
    return;
  }
  
  for (int i = 0; i < strip.numPixels(); i++) {
    if (random(100) > 70) {
      strip.setPixelColor(i, StripUtils().getWheelColor(settings.brightness, random(255)));
    }

  }
  strip.show();
  m = millis();
}


//============ ============ Wave ============ ============ 

void hmWaves() {
  if (millis() < m + 1000 - settings.rate * 1000) {
    return;
  }
  
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, ((i + hmj) % 9 == (i / 18) || (i + hmj) % 9 == 9 - (i / 18)) ? StripUtils().getWheelColor(settings.brightness, random(128)) : 0);
  }
  strip.show();
  
  hmj++;
  m = millis();
}



//============ ============  AUDIO ============ ============ 
//============ ============  AUDIO ============ ============ 
//============ ============  AUDIO ============ ============ 
//============ ============  AUDIO ============ ============ 
//============ ============  AUDIO ============ ============ 



#define IR_AUDIO 5 // ADC channel to capture

#define BUCKETS 4

 #define N_WAVE          1024    /* full length of Sinewave[] */
 #define LOG2_N_WAVE     10      /* log2(N_WAVE) */

 #define FFT_SIZE       64 
 #define log2FFT       6
 #define N             (2 * FFT_SIZE)
 #define log2N         (log2FFT + 1)


int       x[N], fx[N];
int      incomingByte; 



const int sdvig = 32768; //DC bias of the ADC, approxim +2.5V. 
int minima[BUCKETS] = {1023,1023,1023,1023};
int maxima[BUCKETS] = {0,0,0,0};
const float decay = 0.9;  //AGC time constant. 
                      //AGC affects visual display only, no AGC on analog part of the system


void hmAudioInit() {
   ADCSRA = 0x87;

   // turn on adc, freq = 1/128 , 125 kHz.  
   //ADMUX = 0x60 | IR_AUDIO;
   ADMUX = _BV(REFS0) | IR_AUDIO | _BV(ADLAR); 
   //Bit 5 – ADLAR: ADC Left Adjust Result
   
   ADCSRA |= (1<<ADSC);
}

void hmAudioLoop() {
   int   i;

   //Filling up input raw data array x[];
 // 14.6 msec for ADC and 12.4 msec everything else, TOTAL = 27 msec if FFT size = 64 (N=128).
 // 28.8 msec for ADC and 27.1 msec everything else, TOTAL = 55.9 msec if FFT size = 128 (N).

   //    while((ADCSRA&(1<<ADIF)) == 0); //Discard first conversion result.
   while(!(ADCSRA & 0x10));

   for(i=0; i<N; i++ ) {
     ADCSRA |= (1<<ADSC);
     //    while((ADCSRA&(1<<ADIF)) == 0);
     while(!(ADCSRA & 0x10));

     x[i] = ADCL;
     x[i] += (ADCH << 8);  
   }  


   for (i=0; i<N; i++){
     x[i] -=  sdvig;
     if (i & 0x01)
       fx[(N+i)>>1] = x[i] ;
     else
       fx[i>>1] = x[i] ;
   }  

 //Performing FFT, getting fx[] array, where each element represents
 //frequency bin with width 65 Hz.

    fix_fftr( fx, log2N );

 // Calculation of the magnitude:
   // calcMagnitude(N/2);
   
  audioModes[mode]();
}

void calcMagnitude(short n) {
  //maximum *= decay;
  
  int minMax = 50 * settings.rate;
  
  for (short i = 0; i < n; i++) {
     fx[i] = sqrt((long)fx[i] * (long)fx[i] + (long)fx[i+N/2] * (long)fx[i+N/2]);   
     //maximum = max(50, max(maximum, fx[i]));
     maxima[i] = max(minMax, max((int)(maxima[i] * decay), fx[i]));
     minima[i] = max(1, min((int)(minima[i]*(1-decay)), fx[i]));
   }
  
}

void hmAudioEffect1() {
   calcMagnitude(18);
 
   for (short i = 0; i < 18; i++) {
     for (short y = 0; y < 5; y++) {
       strip.setPixelColor(i + y * 18, StripUtils().getColor(settings.brightness, (fx[i]/(5-y) > maxima[y]/5 ? 127 : 0), 0, 0));
     }
   }
   strip.show();
}




void hmAudioEffect2() {

  hmk+=  settings.rate;
  if (hmk > 127) {
    hmk = 0;
  }
  
  calcMagnitude(BUCKETS + 1);
 
   for (short i = 0; i < 18; i++) {
     Serial.print(fx[1], DEC);
     Serial.print("-");
     Serial.print(minima[1]);
     Serial.print("-");
     Serial.print(maxima[1]);
     Serial.println("");
     
     
     short ix = i % BUCKETS + 1;
//     int percentage = 100 * fx[ix] / maxima[ix];
    int percentage = 100 * (fx[ix] - minima[ix]) / (maxima[ix] - minima[ix]);

     for (short y = 0; y < 5; y++) {
       uint32_t col = percentage > (y * 20) ? 0 : StripUtils().getWheelColor(settings.brightness, map(constrain(percentage, 0, y * 20), 0, y * 20, hmk, hmk + 64));
       col = StripUtils().getIntermediateColor(strip.getPixelColor(i + y * 18), col, 0.4);
       strip.setPixelColor(i + y * 18, col);
     }
   }
   strip.show(); 
}



void hmAudioEffect3() {
  calcMagnitude(BUCKETS);
  
  uint32_t col = StripUtils().getWheelColor(settings.brightness, map(fx[1] + fx[2], 0, maxima[1], 43, 128));
  col = StripUtils().getIntermediateColor(strip.getPixelColor(0), col, 0.6);

  for (short i = 0; i < strip.numPixels(); i++) { 
    
    
    strip.setPixelColor(i, col);
  }
  strip.show();
  
}



void hmAudioSparkle() {
 
  calcMagnitude(BUCKETS);

  int percentage = 100 * (fx[1] - minima[1]) / (maxima[1] - minima[1]);
  
  
  //if (millis() < m + 1000 -  settings.rate * 1000) {
    float fadeRate = 0.08 * settings.rate;
    for (int i = 0; i < strip.numPixels(); i++) {
          strip.setPixelColor(i, StripUtils().getIntermediateColor(strip.getPixelColor(i), 0, fadeRate));
    }
    strip.show();
    return;
  //}
  
  
  
  
  for (int i = 0; i < strip.numPixels(); i++) {
    if (random(100) > (percentage * settings.rate)) {
      strip.setPixelColor(i, StripUtils().getWheelColor(settings.brightness, random(255)));
    }

  }
  strip.show();
  m = millis(); 
}





void hmAudioShutdown() {
   ADCSRA = 0x97;
   ADMUX = 0x00;

}










 //**************************************************************************************************


 const prog_int16_t Sinewave[N_WAVE-N_WAVE/4] PROGMEM = {
   0,    201,    402,    603,    804,   1005,   1206,   1406,
   1607,   1808,   2009,   2209,   2410,   2610,   2811,   3011,
   3211,   3411,   3611,   3811,   4011,   4210,   4409,   4608,
   4807,   5006,   5205,   5403,   5601,   5799,   5997,   6195,
   6392,   6589,   6786,   6982,   7179,   7375,   7571,   7766,
   7961,   8156,   8351,   8545,   8739,   8932,   9126,   9319,
   9511,   9703,   9895,  10087,  10278,  10469,  10659,  10849,
   11038,  11227,  11416,  11604,  11792,  11980,  12166,  12353,
   12539,  12724,  12909,  13094,  13278,  13462,  13645,  13827,
   14009,  14191,  14372,  14552,  14732,  14911,  15090,  15268,
   15446,  15623,  15799,  15975,  16150,  16325,  16499,  16672,
   16845,  17017,  17189,  17360,  17530,  17699,  17868,  18036,
   18204,  18371,  18537,  18702,  18867,  19031,  19194,  19357,
   19519,  19680,  19840,  20000,  20159,  20317,  20474,  20631,
   20787,  20942,  21096,  21249,  21402,  21554,  21705,  21855,
   22004,  22153,  22301,  22448,  22594,  22739,  22883,  23027,
   23169,  23311,  23452,  23592,  23731,  23869,  24006,  24143,
   24278,  24413,  24546,  24679,  24811,  24942,  25072,  25201,
   25329,  25456,  25582,  25707,  25831,  25954,  26077,  26198,
   26318,  26437,  26556,  26673,  26789,  26905,  27019,  27132,
   27244,  27355,  27466,  27575,  27683,  27790,  27896,  28001,
   28105,  28208,  28309,  28410,  28510,  28608,  28706,  28802,
   28897,  28992,  29085,  29177,  29268,  29358,  29446,  29534,
   29621,  29706,  29790,  29873,  29955,  30036,  30116,  30195,
   30272,  30349,  30424,  30498,  30571,  30643,  30713,  30783,
   30851,  30918,  30984,  31049,  31113,  31175,  31236,  31297,
   31356,  31413,  31470,  31525,  31580,  31633,  31684,  31735,
   31785,  31833,  31880,  31926,  31970,  32014,  32056,  32097,
   32137,  32176,  32213,  32249,  32284,  32318,  32350,  32382,
   32412,  32441,  32468,  32495,  32520,  32544,  32567,  32588,
   32609,  32628,  32646,  32662,  32678,  32692,  32705,  32717,
   32727,  32736,  32744,  32751,  32757,  32761,  32764,  32766,
   32767,  32766,  32764,  32761,  32757,  32751,  32744,  32736,
   32727,  32717,  32705,  32692,  32678,  32662,  32646,  32628,
   32609,  32588,  32567,  32544,  32520,  32495,  32468,  32441,
   32412,  32382,  32350,  32318,  32284,  32249,  32213,  32176,
   32137,  32097,  32056,  32014,  31970,  31926,  31880,  31833,
   31785,  31735,  31684,  31633,  31580,  31525,  31470,  31413,
   31356,  31297,  31236,  31175,  31113,  31049,  30984,  30918,
   30851,  30783,  30713,  30643,  30571,  30498,  30424,  30349,
   30272,  30195,  30116,  30036,  29955,  29873,  29790,  29706,
   29621,  29534,  29446,  29358,  29268,  29177,  29085,  28992,
   28897,  28802,  28706,  28608,  28510,  28410,  28309,  28208,
   28105,  28001,  27896,  27790,  27683,  27575,  27466,  27355,
   27244,  27132,  27019,  26905,  26789,  26673,  26556,  26437,
   26318,  26198,  26077,  25954,  25831,  25707,  25582,  25456,
   25329,  25201,  25072,  24942,  24811,  24679,  24546,  24413,
   24278,  24143,  24006,  23869,  23731,  23592,  23452,  23311,
   23169,  23027,  22883,  22739,  22594,  22448,  22301,  22153,
   22004,  21855,  21705,  21554,  21402,  21249,  21096,  20942,
   20787,  20631,  20474,  20317,  20159,  20000,  19840,  19680,
   19519,  19357,  19194,  19031,  18867,  18702,  18537,  18371,
   18204,  18036,  17868,  17699,  17530,  17360,  17189,  17017,
   16845,  16672,  16499,  16325,  16150,  15975,  15799,  15623,
   15446,  15268,  15090,  14911,  14732,  14552,  14372,  14191,
   14009,  13827,  13645,  13462,  13278,  13094,  12909,  12724,
   12539,  12353,  12166,  11980,  11792,  11604,  11416,  11227,
   11038,  10849,  10659,  10469,  10278,  10087,   9895,   9703,
   9511,   9319,   9126,   8932,   8739,   8545,   8351,   8156,
   7961,   7766,   7571,   7375,   7179,   6982,   6786,   6589,
   6392,   6195,   5997,   5799,   5601,   5403,   5205,   5006,
   4807,   4608,   4409,   4210,   4011,   3811,   3611,   3411,
   3211,   3011,   2811,   2610,   2410,   2209,   2009,   1808,
   1607,   1406,   1206,   1005,    804,    603,    402,    201,
   0,   -201,   -402,   -603,   -804,  -1005,  -1206,  -1406,
   -1607,  -1808,  -2009,  -2209,  -2410,  -2610,  -2811,  -3011,
   -3211,  -3411,  -3611,  -3811,  -4011,  -4210,  -4409,  -4608,
   -4807,  -5006,  -5205,  -5403,  -5601,  -5799,  -5997,  -6195,
   -6392,  -6589,  -6786,  -6982,  -7179,  -7375,  -7571,  -7766,
   -7961,  -8156,  -8351,  -8545,  -8739,  -8932,  -9126,  -9319,
   -9511,  -9703,  -9895, -10087, -10278, -10469, -10659, -10849,
   -11038, -11227, -11416, -11604, -11792, -11980, -12166, -12353,
   -12539, -12724, -12909, -13094, -13278, -13462, -13645, -13827,
   -14009, -14191, -14372, -14552, -14732, -14911, -15090, -15268,
   -15446, -15623, -15799, -15975, -16150, -16325, -16499, -16672,
   -16845, -17017, -17189, -17360, -17530, -17699, -17868, -18036,
   -18204, -18371, -18537, -18702, -18867, -19031, -19194, -19357,
   -19519, -19680, -19840, -20000, -20159, -20317, -20474, -20631,
   -20787, -20942, -21096, -21249, -21402, -21554, -21705, -21855,
   -22004, -22153, -22301, -22448, -22594, -22739, -22883, -23027,
   -23169, -23311, -23452, -23592, -23731, -23869, -24006, -24143,
   -24278, -24413, -24546, -24679, -24811, -24942, -25072, -25201,
   -25329, -25456, -25582, -25707, -25831, -25954, -26077, -26198,
   -26318, -26437, -26556, -26673, -26789, -26905, -27019, -27132,
   -27244, -27355, -27466, -27575, -27683, -27790, -27896, -28001,
   -28105, -28208, -28309, -28410, -28510, -28608, -28706, -28802,
   -28897, -28992, -29085, -29177, -29268, -29358, -29446, -29534,
   -29621, -29706, -29790, -29873, -29955, -30036, -30116, -30195,
   -30272, -30349, -30424, -30498, -30571, -30643, -30713, -30783,
   -30851, -30918, -30984, -31049, -31113, -31175, -31236, -31297,
   -31356, -31413, -31470, -31525, -31580, -31633, -31684, -31735,
   -31785, -31833, -31880, -31926, -31970, -32014, -32056, -32097,
   -32137, -32176, -32213, -32249, -32284, -32318, -32350, -32382,
   -32412, -32441, -32468, -32495, -32520, -32544, -32567, -32588,
   -32609, -32628, -32646, -32662, -32678, -32692, -32705, -32717,
   -32727, -32736, -32744, -32751, -32757, -32761, -32764, -32766
 };

 int fix_fft(int fr[], int fi[], int m )
 {
   int mr, nn, i, j, l, k, istep, n, scale, shift;
   int qr, qi, tr, ti, wr, wi;

   n = 1 << m;

   /* max FFT size = N_WAVE */
   if (n > N_WAVE)
     return -1;

   mr = 0;
   nn = n - 1;
   scale = 0;

   /* decimation in time - re-order data */
   for (m=1; m<=nn; ++m) {
     l = n;
     do {
       l >>= 1;
     } 
     while (mr+l > nn);
     mr = (mr & (l-1)) + l;

     if (mr <= m)
       continue;
     tr = fr[m];
     fr[m] = fr[mr];
     fr[mr] = tr;
     ti = fi[m];
     fi[m] = fi[mr];
     fi[mr] = ti;
   }

   l = 1;
   k = LOG2_N_WAVE-1;
   while (l < n) {
     shift = 1;
     istep = l << 1;
     for (m=0; m<l; ++m) {
       j = m << k;
       /* 0 <= j < N_WAVE/2 */
       wr =  pgm_read_word(&Sinewave[j+N_WAVE/4]);
       wi = -pgm_read_word(&Sinewave[j]);

       wr >>= 1;
       wi >>= 1;

       for (i=m; i<n; i+=istep) {
         j = i + l;
         tr = ((long)wr*(long)fr[j] - (long)wi*(long)fi[j])>>15;
         ti = ((long)wr*(long)fi[j] + (long)wi*(long)fr[j])>>15;
         qr = fr[i];
         qi = fi[i];

         qr >>= 1;
         qi >>= 1;

         fr[j] = qr - tr;
         fi[j] = qi - ti;
         fr[i] = qr + tr;
         fi[i] = qi + ti;
       }
     }
     --k;
     l = istep;
   }
   return scale;
 }

 int fix_fftr(int f[], int m )
 {
   int i, Nt = 1<<(m-1),  scale = 0;
   int tt, *fr=f, *fi=&f[Nt];

   scale = fix_fft(fi, fr, m-1 );
   return scale;
 }
 //**************************************************************************************************
