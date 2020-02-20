#include <LiquidCrystal.h>
#include <FastLED.h>
#include <ListLib.h>

using namespace std;

#define ONBOARD_LED_PIN 13
#define DATA_PIN 3
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 240
#define BRIGHTNESS 92
#define MAX_BRIGHTNESS 255

CRGB leds[NUM_LEDS];
CRGB colorsUSA[] = {CRGB::Red, CRGB::White, CRGB::Blue, CRGB::Black};
CRGB colorsTest[] = {CRGB::Red, CRGB::Green, CRGB::Blue};
CRGB colorsKSU[] = {0x512888, CRGB::Silver, CRGB::White};
LiquidCrystal LCDriver(11, 9, 5, 6, 7, 8);
List<CRGB> colors;

typedef enum {CLEAN_UP = 0, RANDOM = 1, SINGLE_ZIPPER = 2, SHIFT_SINGLE_PIXEL = 3,
              ONE_COLOR = 4, ONE_COLOR_STROBE = 5, MULTI_COLOR = 6, MULTI_COLOR_STROBE = 7,
              SHIFT_MULTI_PIXEL = 8, THREE_ARRAY = 9, DIMMER = 10, DIM_IN_OUT = 11} ShowType;
ShowType showType;

unsigned long durationTime;
CRGB crgb;
CRGB three_colors[] = {colorsTest[0], colorsTest[1], colorsTest[2]};
unsigned long intervalTime;
unsigned int decay;
void showProgramCleanUp(unsigned long);
void showProgramRandom(int , unsigned long );
void showProgramSingleZipper(CRGB , unsigned long );
void showProgramShiftSinglePixel(CRGB , unsigned long );
void showProgramOneColor(CRGB , unsigned long );
void showProgramOneColorStrobe(CRGB , unsigned long , unsigned long );
void showProgramMultiColor(unsigned long , unsigned long );
void showProgramMultiColorStrobe(unsigned long , unsigned long );
void showProgramShiftMultiPixel(unsigned long );
void showProgramThreeArray(CRGB , CRGB , CRGB , unsigned long );
void showProgramDimmer(CRGB , unsigned int , unsigned long );
void showProgramDimInOut(CRGB , unsigned int , unsigned long );


void setup() {
  Serial.begin(9600);
  delay(3000); // initial delay of a few seconds is recommended
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(BRIGHTNESS);// global brightness
  showProgramCleanUp(1); // clean up
  showType = CLEAN_UP;
  FastLED.show();

  for (int i = 0; i < 4; i++)
  {
    colors.Add(colorsUSA[i]);
  }
}

void loop() {
  testShowPrograms();

//  switch(showType)
//  {
//    case CLEAN_UP: 
//      break;
//    else: 
//      break;
//  }
}


// switches off all LEDs
void showProgramCleanUp(unsigned long durationTime) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  digitalWrite(13, HIGH); // sets the digital pin 13 on
  delay(durationTime);            // waits for a second
  digitalWrite(13, LOW);  // sets the digital pin 13 off
}

void testShowPrograms()
{
//    showProgramCleanUp(100); // clean up
//    showProgramRandom(100, 100); // show "random" program
//  
//    showProgramCleanUp(1000); // clean up
//    showProgramRandom(10, 10); // show "random" program
//  
//    showProgramCleanUp(1); // clean up
//    showProgramSingleZipper(CRGB::Purple, 10); // show "zipper" program
//  
//    showProgramCleanUp(2500); // clean up
//    showProgramShiftSinglePixel(CRGB::Blue, 100); // show "shift single pixel program" with blue pixel
//
//    showProgramCleanUp(100);
//    showProgramOneColor(CRGB::Purple, 50);  // show "one color" program
//  
//    showProgramCleanUp(1000);
//    showProgramOneColorStrobe(CRGB::Purple, 66, 7000);
//
//    showProgramCleanUp(1000);
//    showProgramMultiColorStrobe(100, 10000);
//      
//    showProgramCleanUp(100);
//    showProgramMultiColor(1000, 1);  //show "multi color" program
//  
//    showProgramCleanUp(2500); // clean up
//    showProgramShiftSinglePixel(CRGB::Red, 100); // show "shift single pixel program" with red pixel
//  
//    showProgramCleanUp(2500); // clean up
//    showProgramShiftMultiPixel(100); // show "shift multi pixel" program
//  
//    showProgramCleanUp(2500); // clean up
//    showProgramShiftMultiPixel(25); // show "shift multi pixel" program
//  
//    showProgramCleanUp(100);
//    showProgramThreeArray(CRGB::Blue, CRGB::Red, CRGB::Green, 1000); //show "RGB" program
//  
//    showProgramCleanUp(100);
//    showProgramDimmer(CRGB::Purple, 1, 5);  //show "dimmer" program
//
//    showProgramDimInOut(CRGB::Purple, 1, 5);  //show "dim in/out" program
}


// switches on all LEDs. Each LED is shown in random color.
// numIterations: indicates how often LEDs are switched on in random colors
// durationTime: indicates for how long LEDs are switched on.
void showProgramRandom(int numIterations, unsigned long durationTime) {
  for (int iteration = 0; iteration < numIterations; ++iteration) {
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = CHSV(random8(), 255, 255); // hue, saturation, value
    }
    FastLED.show();
    delay(durationTime);
  }
}


void showProgramSingleZipper(CRGB crgb, unsigned long durationTime) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = crgb;
    FastLED.show();
    delay(durationTime);
    leds[i] = CRGB::Black;
  }
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    leds[i] = crgb;
    FastLED.show();
    delay(durationTime);
    leds[i] = CRGB::Black;
  }
}


// Shifts a single pixel from the start of strip to the end.
// crgb: color of shifted pixel
// durationTime: indicates how long the pixel is shown on each LED
void showProgramShiftSinglePixel(CRGB crgb, unsigned long durationTime) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = crgb;
    FastLED.show();
    delay(durationTime);
    leds[i] = CRGB::Black;
  }
}


void showProgramOneColor(CRGB crgb, unsigned long durationTime) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = crgb;
  }
  FastLED.show();
  delay(durationTime);
}


void showProgramOneColorStrobe(CRGB crgb, unsigned long intervalTime, unsigned long durationTime) {
  unsigned long timer = millis();
  while(millis() - timer < durationTime)
  {
    showProgramOneColor(crgb, intervalTime);
    showProgramCleanUp(intervalTime);
  }
}


//Cycles through the list of colors
void showProgramMultiColor(unsigned long dTime, unsigned long durationTime) {
  for (unsigned int i = 0; i < colors.Count(); i++) {
    showProgramOneColor(colors[i], dTime);
  }
  delay(durationTime);
}


void showProgramMultiColorStrobe(unsigned long intervalTime, unsigned long durationTime) {
  unsigned long timer = millis();
  int i = 0;
  while(millis() - timer < durationTime)
  {
      showProgramOneColor(colors[i], intervalTime);
      i = (i + 1) % colors.Count();
  }
}


// Shifts multiple pixel from the start of strip to the end. The color of each pixel is randomized.
// durationTime: indicates how long the pixels are shown on each LED
void showProgramShiftMultiPixel(unsigned long durationTime) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    for (int j = i; j > 0; --j) {
      leds[j] = leds[j - 1];
    }
    CRGB newPixel = CHSV(random8(), 255, 255);
    leds[0] = newPixel;
    FastLED.show();
    delay(durationTime);
  }
}


void showProgramThreeArray(CRGB crgb1, CRGB crgb2, CRGB crgb3, unsigned long durationTime) {
  for (int i = 0; i < NUM_LEDS - 3; i += 3) {
    leds[i] = crgb1;
    leds[i + 1] = crgb2;
    leds[i + 2] = crgb3;
  }
  FastLED.show();
  delay(durationTime);
}


void showProgramDimmer(CRGB crgb, unsigned int decay, unsigned long durationTime) {
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = crgb;
  }
  FastLED.show();
  int curBright = BRIGHTNESS;
  
  while (curBright > 0) {
    curBright -=decay;
    FastLED.setBrightness(curBright);
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = crgb;
    }
    FastLED.show();
    delay(durationTime);
  }
}


void showProgramDimInOut(CRGB crgb, unsigned int decay, unsigned long durationTime) {
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = crgb;
  }
  FastLED.show();
  unsigned int curBright = BRIGHTNESS;
  
  while (curBright > 0) {
    curBright -=decay;
    FastLED.setBrightness(curBright);
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = crgb;
    }
    FastLED.show();
    delay(durationTime);
  }

   while (curBright < 255 - decay) {
    curBright +=decay;
    FastLED.setBrightness(curBright);
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = crgb;
    }
    FastLED.show();
    delay(durationTime);
  }
}
