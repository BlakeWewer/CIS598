#include "FastLED.h"
#include "LEDList.h"
#include "LEDList.cpp"
using namespace std;

#define BUTTON_A_PIN 9
#define DATA_PIN 3
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 240
#define BRIGHTNESS 92
CRGB leds[NUM_LEDS];
CRGB colorsUSA[] = {CRGB::Red, CRGB::White, CRGB::Blue, CRGB::Black};
LEDList<CRGB> colors;
enum States {On, Off};
States state = On;
unsigned long Timer = millis();

void setup() {
  Serial.begin(9600);
  delay(3000); // initial delay of a few seconds is recommended
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(BRIGHTNESS);// global brightness
  showProgramCleanUp(1000); // clean up
  FastLED.show();

  for (int i = 0; i < 4; i++)
  {
    colors.add(colorsUSA[i]);
  }

  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
}


// switches off all LEDs
void showProgramCleanUp(long delayTime) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  delay(delayTime);
}


// switches on all LEDs. Each LED is shown in random color.
// numIterations: indicates how often LEDs are switched on in random colors
// delayTime: indicates for how long LEDs are switched on.
void showProgramRandom(int numIterations, long delayTime) {
  for (int iteration = 0; iteration < numIterations; ++iteration) {
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = CHSV(random8(), 255, 255); // hue, saturation, value
    }
    FastLED.show();
    delay(delayTime);
  }
}


void showProgramSingleZipper(CRGB crgb, long delayTime) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = crgb;
    FastLED.show();
    delay(delayTime);
    leds[i] = CRGB::Black;
  }
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    leds[i] = crgb;
    FastLED.show();
    delay(delayTime);
    leds[i] = CRGB::Black;
  }
}


// Shifts a single pixel from the start of strip to the end.
// crgb: color of shifted pixel
// delayTime: indicates how long the pixel is shown on each LED
void showProgramShiftSinglePixel(CRGB crgb, long delayTime) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = crgb;
    FastLED.show();
    delay(delayTime);
    leds[i] = CRGB::Black;
  }
}


void showProgramOneColor(CRGB crgb, long delayTime) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = crgb;
  }
  FastLED.show();
  delay(delayTime);
}


//void showProgramMultiColor(LEDList<CRGB> c, long dTime, long delayTime) {
void showProgramMultiColor(long dTime, long delayTime) {
  for (int i = 0; i < colors.count; i++) {
    showProgramOneColor(colors[i], dTime);
  }
  delay(delayTime);
}



// Shifts multiple pixel from the start of strip to the end. The color of each pixel is randomized.
// delayTime: indicates how long the pixels are shown on each LED
void showProgramShiftMultiPixel(long delayTime) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    for (int j = i; j > 0; --j) {
      leds[j] = leds[j - 1];
    }
    CRGB newPixel = CHSV(random8(), 255, 255);
    leds[0] = newPixel;
    FastLED.show();
    delay(delayTime);
  }
}


void showProgramThreeArray(CRGB crgb1, CRGB crgb2, CRGB crgb3, long delayTime) {
  for (int i = 0; i < NUM_LEDS - 3; i += 3) {
    leds[i] = crgb1;
    leds[i + 1] = crgb2;
    leds[i + 2] = crgb3;
  }
  FastLED.show();
  delay(delayTime);
}


void showProgramDimmer(CRGB crgb, int decay, long delayTime) {
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = crgb;
  }
  FastLED.show();
  int curBright = BRIGHTNESS;

  while (curBright > 0) {
    curBright -= decay;
    FastLED.setBrightness(curBright);
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = crgb;
    }
    FastLED.show();
    delay(delayTime);
  }
}


void showProgramDimInOut(CRGB crgb, int decay, long delayTime) {
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = crgb;
  }
  FastLED.show();
  int curBright = BRIGHTNESS;

  while (curBright > 0) {
    curBright -= decay;
    FastLED.setBrightness(curBright);
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = crgb;
    }
    FastLED.show();
    delay(delayTime);
  }

  while (curBright < 255 - decay) {
    curBright += decay;
    FastLED.setBrightness(curBright);
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = crgb;
    }
    FastLED.show();
    delay(delayTime);
  }
}

void ChangeState()
{
  switch (state) {
    case On:
      state = Off;
      break;
    case Off:
      state = On;
  }
}

// main program
void loop() {
  Serial.println(state);
  if (digitalRead(BUTTON_A_PIN) == HIGH)
  {
    if (millis() - Timer > 500)
    {
      ChangeState();
      Timer = millis();
    }
  }

  while (state == Off) {
    showProgramCleanUp(10);
    if (digitalRead(BUTTON_A_PIN) == LOW)
    {
      if (millis() - Timer > 500)
      {
        ChangeState();
        Timer = millis();
      }
    }
  }

  while (state == On) {
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
    showProgramOneColor(CRGB::Purple, 100);  // show "one color strobe" program
    //
    //    showProgramCleanUp(100);
    //    showProgramMultiColor(1000, 1);  //show "multi color strobe" program
    //
    //    showProgramCleanUp(2500); // clean up
    //    showProgramShiftSinglePixel(CRGB::Red, 100); // show "shift single pixel program" with maroon pixel
    //
    //    showProgramCleanUp(2500); // clean up
    //    showProgramShiftMultiPixel(100); // show "shift multi pixel" program
    //
    //    showProgramCleanUp(2500); // clean up
    //    showProgramShiftMultiPixel(25); // show "shift multi pixel" program
    //
    //    showProgramCleanUp(100);
    //    showProgramThreeArray(CRGB::Blue, CRGB::Red, CRGB::Green, 100); //show "RGB" program
    //
    //    showProgramCleanUp(100);
    //    showProgramDimmer(CRGB::Purple, 1, 5);  //show "dimmer" program
    //
    //    showProgramDimInOut(CRGB::Purple, 1, 5);  //show "dim in/out" program

    if (digitalRead(BUTTON_A_PIN) == LOW)
    {
      if (millis() - Timer > 500)
      {
        ChangeState();
        Timer = millis();
      }
    }
  }
  /*
    switch (state) {
      case Off:
        showProgramCleanUp(1);
        break;
      case On:
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
            showProgramOneColor(CRGB::Purple, 100);  // show "one color strobe" program
        //
        //    showProgramCleanUp(100);
        //    showProgramMultiColor(1000, 1);  //show "multi color strobe" program
        //
        //    showProgramCleanUp(2500); // clean up
        //    showProgramShiftSinglePixel(CRGB::Red, 100); // show "shift single pixel program" with maroon pixel
        //
        //    showProgramCleanUp(2500); // clean up
        //    showProgramShiftMultiPixel(100); // show "shift multi pixel" program
        //
        //    showProgramCleanUp(2500); // clean up
        //    showProgramShiftMultiPixel(25); // show "shift multi pixel" program
        //
        //    showProgramCleanUp(100);
        //    showProgramThreeArray(CRGB::Blue, CRGB::Red, CRGB::Green, 100); //show "RGB" program
        //
        //    showProgramCleanUp(100);
        //    showProgramDimmer(CRGB::Purple, 1, 5);  //show "dimmer" program
        //
        //    showProgramDimInOut(CRGB::Purple, 1, 5);  //show "dim in/out" program
        break;
    }*/
}

