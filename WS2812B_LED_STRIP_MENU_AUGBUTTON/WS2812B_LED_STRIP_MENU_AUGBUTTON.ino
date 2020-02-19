#include <LinkedList.h>
#include <LiquidCrystal.h>

#include "FastLED.h"
#include "LEDList.h"
#include "LEDList.cpp"

using namespace std;

#define DATA_PIN 3
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 240
#define MAX_BRIGHTNESS 255
#define BRIGHTNESS 92

CRGB leds[NUM_LEDS];
CRGB colorsUSA[] = {CRGB::Red, CRGB::White, CRGB::Blue, CRGB::Black};
LiquidCrystal LcDriver(11, 9, 5, 6, 7, 8);
//  THOUGHTS:  IF I CHANGE THIS LIST TO AN OBJECT WITH A STRING AND A PROGRAM I WOULD BE
//              ABLE TO JUST USE ONE LIST.
LinkedList<String*> myMenu = LinkedList<String*>();

LEDList<CRGB> colors;
String m0 = "RANDOM NORMAL";
String m1 = "RANDOM FAST";
String m2 = "SINGLE ZIPPER";
unsigned long Timer;
double encoderPosition;
int curMode;
enum MStates { ModeSelect, BrightnessAdjust };
MStates MenuState;
enum BStates { Idle, Wait, Low };
BStates ButtonState;
unsigned long ButtonTime;



/**
   ButtonNextState
   @param int Input - either 1 or 0

   @return int - always returns 0
*/
int ButtonNextState(int Input)
{

  switch (ButtonState)                  //Begins case-switch
  {
    case Idle:                            //Idle case
      if (Input == LOW)                   //If input is low
      {
        ButtonTime = millis();            //Set ButtonTime to current running time
        ButtonState = Wait;               //Change State to Wait
      }
      break;                              //End of Idle Case

    case Wait:
      if (Input == HIGH)
      {
        ButtonState = Idle;
      }
      else if (millis() - ButtonTime >= 5)
      {
        ButtonState = Low;
        return 1;
      }
      break;

    case Low:
      if (Input == HIGH)
      {
        ButtonState = Idle;
      } else if (millis() - ButtonTime >= 1000)
      {
        if (MenuState == ModeSelect)
        {
          MenuState = BrightnessAdjust;
          ButtonTime = millis();
          return 0;
        } else if (MenuState == BrightnessAdjust)
        {
          MenuState = ModeSelect;
          ButtonTime = millis();
         return 0;
        }
      }
      break;
  }
  return 0;
}

void MonitorA()
{
  if (digitalRead(2) == digitalRead(3))     //If inputA and inputB are equal
  {
    if (MenuState == ModeSelect)
    {
      if (encoderPosition < myMenu.size() - 1)
        encoderPosition += .25;    //Increment encoderPosition
    } else if (MenuState == BrightnessAdjust)
    {
      if (FastLED.getBrightness() < MAX_BRIGHTNESS)
        FastLED.setBrightness(FastLED.getBrightness() + 1);
    }
  }
  else
  {
    if (MenuState == ModeSelect)
    {
      if (encoderPosition > 0)
        encoderPosition -= .25;    //Else, decrement encoderPosition
    } else if (MenuState == BrightnessAdjust)
    {
      if (FastLED.getBrightness() > 0)
        FastLED.setBrightness(FastLED.getBrightness() - 1);
    }
  }
}

void MonitorB()
{
  if (digitalRead(2) == digitalRead(3))     //If inputA and inputB are equal
  {
    if (MenuState == ModeSelect)
    {
      if (encoderPosition > 0)
        encoderPosition -= .25;    //Else, decrement encoderPosition
    } else if (MenuState == BrightnessAdjust)
    {
      if (FastLED.getBrightness() > 0)
        FastLED.setBrightness(FastLED.getBrightness() - 1);
    }
  }
  else
  {
    if (MenuState == ModeSelect)
    {
      if (encoderPosition < myMenu.size() - 1)
        encoderPosition += .25;    //Increment encoderPosition
    } else if (MenuState == BrightnessAdjust)
    {
      if (FastLED.getBrightness() < MAX_BRIGHTNESS)
        FastLED.setBrightness(FastLED.getBrightness() + 1);
    }
  }
}





void setup()
{
  LcDriver.begin(16, 2);
  LcDriver.clear();
  LcDriver.setCursor(0, 0);               //Set up the LcDriver and put cursor at 0, 0
  LcDriver.print("WELCOME");

  delay(3000); // initial delay of a few seconds is recommended
  //  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(BRIGHTNESS);// global brightness
  //  showProgramCleanUp(1000); // clean up
  //  FastLED.show();
  //
  //  for (int i = 0; i < 4; i++)
  //  {
  //    colors.add(colorsUSA[i]);
  //  }


  pinMode(4, INPUT);
  ButtonState = Idle;
  Timer = millis();                       //Set timer to the current millis
  attachInterrupt(0, MonitorA, CHANGE);   //Set MonitorA() to interrupt 0
  attachInterrupt(1, MonitorB, CHANGE);   //Set MonitorB() to interrupt 1

  myMenu.add(&m0);
  myMenu.add(&m1);
  myMenu.add(&m2);
  curMode = 0;
}



//
//// switches off all LEDs
//void showProgramCleanUp(long delayTime) {
//  for (int i = 0; i < NUM_LEDS; ++i) {
//    leds[i] = CRGB::Black;
//  }
//  FastLED.show();
//  delay(delayTime);
//}
//
//
//// switches on all LEDs. Each LED is shown in random color.
//// numIterations: indicates how often LEDs are switched on in random colors
//// delayTime: indicates for how long LEDs are switched on.
//void showProgramRandom(int numIterations, long delayTime) {
//  for (int iteration = 0; iteration < numIterations; ++iteration) {
//    for (int i = 0; i < NUM_LEDS; ++i) {
//      leds[i] = CHSV(random8(), 255, 255); // hue, saturation, value
//    }
//    FastLED.show();
//    delay(delayTime);
//  }
//}
//
//
//void showProgramSingleZipper(CRGB crgb, long delayTime) {
//  for (int i = 0; i < NUM_LEDS; i++) {
//    leds[i] = crgb;
//    FastLED.show();
//    delay(delayTime);
//    leds[i] = CRGB::Black;
//  }
//  for (int i = NUM_LEDS - 1; i >= 0; i--) {
//    leds[i] = crgb;
//    FastLED.show();
//    delay(delayTime);
//    leds[i] = CRGB::Black;
//  }
//}
//
//
//// Shifts a single pixel from the start of strip to the end.
//// crgb: color of shifted pixel
//// delayTime: indicates how long the pixel is shown on each LED
//void showProgramShiftSinglePixel(CRGB crgb, long delayTime) {
//  for (int i = 0; i < NUM_LEDS; ++i) {
//    leds[i] = crgb;
//    FastLED.show();
//    delay(delayTime);
//    leds[i] = CRGB::Black;
//  }
//}
//
//
//void showProgramOneColor(CRGB crgb, long delayTime) {
//  for (int i = 0; i < NUM_LEDS; ++i) {
//    leds[i] = crgb;
//  }
//  FastLED.show();
//  delay(delayTime);
//}
//
//
////void showProgramMultiColor(LEDList<CRGB> c, long dTime, long delayTime) {
//void showProgramMultiColor(long dTime, long delayTime) {
//  for (int i = 0; i < colors.count; i++) {
//    showProgramOneColor(colors[i], dTime);
//  }
//  delay(delayTime);
//}
//
//
//
//// Shifts multiple pixel from the start of strip to the end. The color of each pixel is randomized.
//// delayTime: indicates how long the pixels are shown on each LED
//void showProgramShiftMultiPixel(long delayTime) {
//  for (int i = 0; i < NUM_LEDS; ++i) {
//    for (int j = i; j > 0; --j) {
//      leds[j] = leds[j - 1];
//    }
//    CRGB newPixel = CHSV(random8(), 255, 255);
//    leds[0] = newPixel;
//    FastLED.show();
//    delay(delayTime);
//  }
//}
//
//
//void showProgramThreeArray(CRGB crgb1, CRGB crgb2, CRGB crgb3, long delayTime) {
//  for (int i = 0; i < NUM_LEDS - 3; i += 3) {
//    leds[i] = crgb1;
//    leds[i + 1] = crgb2;
//    leds[i + 2] = crgb3;
//  }
//  FastLED.show();
//  delay(delayTime);
//}
//
//
//void showProgramDimmer(CRGB crgb, int decay, long delayTime) {
//  FastLED.setBrightness(BRIGHTNESS);
//  for (int i = 0; i < NUM_LEDS; ++i) {
//    leds[i] = crgb;
//  }
//  FastLED.show();
//  int curBright = BRIGHTNESS;
//
//  while (curBright > 0) {
//    curBright -=decay;
//    FastLED.setBrightness(curBright);
//    for (int i = 0; i < NUM_LEDS; ++i) {
//      leds[i] = crgb;
//    }
//    FastLED.show();
//    delay(delayTime);
//  }
//}
//
//
//void showProgramDimInOut(CRGB crgb, int decay, long delayTime) {
//  FastLED.setBrightness(BRIGHTNESS);
//  for (int i = 0; i < NUM_LEDS; ++i) {
//    leds[i] = crgb;
//  }
//  FastLED.show();
//  int curBright = BRIGHTNESS;
//
//  while (curBright > 0) {
//    curBright -=decay;
//    FastLED.setBrightness(curBright);
//    for (int i = 0; i < NUM_LEDS; ++i) {
//      leds[i] = crgb;
//    }
//    FastLED.show();
//    delay(delayTime);
//  }
//
//   while (curBright < 255 - decay) {
//    curBright +=decay;
//    FastLED.setBrightness(curBright);
//    for (int i = 0; i < NUM_LEDS; ++i) {
//      leds[i] = crgb;
//    }
//    FastLED.show();
//    delay(delayTime);
//  }
//}





void loop()
{
  if (millis() - Timer > 100)             //Every 100 milliseconds
  {
    LcDriver.clear();
    LcDriver.setCursor(0, 0);
    if (MenuState == ModeSelect)
    {
      LcDriver.print("Mode ");
      LcDriver.print((int)encoderPosition);      //Clear and Print the encoderPosition to the LcDriver
      LcDriver.setCursor(0, 1);
      LcDriver.print(*myMenu.get((int)encoderPosition));
    } else if (MenuState == BrightnessAdjust)
    {
      LcDriver.print("BRIGHTNESS:");
      LcDriver.setCursor(0,1);
      LcDriver.print(FastLED.getBrightness());
    }

    Timer += 100;                         //Increase Timer by 100
  }
  if (ButtonNextState(digitalRead(4)))    //If Button Pressed (Selects Menu Item)
  {
    curMode = (int)encoderPosition;
    LcDriver.clear();
    LcDriver.setCursor(0, 0);
    encoderPosition = 0;
    LcDriver.print("Mode ");
    LcDriver.print((int)encoderPosition);
  }

  //  showProgramCleanUp(100); // clean up
  switch (curMode)
  {
    case 0:
      //      showProgramRandom(100, 100); // show "random" program
      break;

    case 1:
      //      showProgramRandom(10, 10); // show "random" program
      break;

    case 2:
      //      showProgramSingleZipper(CRGB::Purple, 10); // show "zipper" program
      break;
  }
}
