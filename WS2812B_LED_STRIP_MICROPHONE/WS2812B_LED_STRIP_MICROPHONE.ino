#include "FastLED.h"
#include "LEDList.h"
#include "LEDList.cpp"
using namespace std;

#define DATA_PIN 3
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 240
#define BRIGHTNESS 92
CRGB leds[NUM_LEDS];
CRGB colorsUSA[] = {CRGB::Red, CRGB::White, CRGB::Blue, CRGB::Black};
LEDList<CRGB> colors;
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
#define MIC_PIN A7
#define MIC_RATIO 65.3846154    //Brightness = 65.3846154*Reading     //Reading = 0.01529412*Brightness

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

}


// switches off all LEDs
void showProgramCleanUp(long delayTime) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  delay(delayTime);
}


void showProgramMicrophoneOne(CRGB crgb, long duration) {
  unsigned long Timer;
  double value;
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  unsigned long startMillis;
  unsigned int peakToPeak = 0;
  double volts;
  value = 100;
  
  for (int i = 0; i < NUM_LEDS; ++i) { 
    leds[i] = crgb;
  }

  Timer = millis();
  
  while (millis() - Timer < duration) {
    FastLED.setBrightness(value);
    FastLED.show();
    
    startMillis = millis(); // Start of sample window
    signalMax = 0;
    signalMin = 0;
    while (millis() - startMillis < sampleWindow)
    {
      sample = analogRead(A7);      
      if (sample < 1024) // toss out spurious readings
      {
        if (sample > signalMax)
        {
          signalMax = sample; // save just the max levels
        }
        else if (sample < signalMin)
        {
          signalMin = sample; // save just the min levels
        }
      }
    }
    peakToPeak = signalMax - signalMin; // max - min = peak-peak amplitude
    volts = (peakToPeak * 5.0) / 1024; // convert to volts
    Serial.print(volts);
    Serial.print("    ");
    Serial.print(signalMin);
    Serial.print("    ");
    Serial.println(signalMax);

    value = (int)(volts * MIC_RATIO);
  }
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


// main program
void loop() {
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
  //    showProgramOneColor(CRGB::Purple, 50);  // show "one color strobe" program
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
  //
  //    showProgramMicrophoneOne(CRGB::Red, 10000);
}

