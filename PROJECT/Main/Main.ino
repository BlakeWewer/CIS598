#include <LiquidCrystal.h>
#include <FastLED.h>
#include <ListLib.h>
#include "arduinoFFT.h"

using namespace std;

#define ONBOARD_LED_PIN 13
#define DATA_PIN 6
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 240
#define DEFAULT_BRIGHTNESS 75
byte BRIGHTNESS = 75;
#define DEFAULT_MIN_BRIGHTNESS 10
#define DEFAULT_MAX_BRIGHTNESS 200
int8_t MIN_BRIGHTNESS = 5;
int16_t MAX_BRIGHTNESS = 200;

#define BUTTON_1 2
#define BUTTON_2 3
#define BUTTON_3 9
#define BUTTON_4 10
#define NUM_BUTTONS 4

typedef struct {
  int8_t Pin;
  int8_t Value;
  int8_t PrevValue;
  bool Active;
} Button;

Button buttons[] = {{BUTTON_1, 0, 0, false},
                    {BUTTON_2, 0, 0, false},
                    {BUTTON_3, 0, 0, false},
                    {BUTTON_4, 0, 0, false}
                  };

CRGB leds[NUM_LEDS];
CHSV currentColors[5];
uint8_t num_colors;

typedef struct {
  CRGB Value;
  String Name;
} Color;

//Color BasicColors[] = {{CRGB::AliceBlue, "AliceBlue"}, {0x512888, "Royal Purple"}};

const byte MIC_SAMPLE_WINDOW_DURATION = 50; // Sample window width in mS (50 mS = 20Hz)
#define MIC_PIN A0
#define MIC_RATIO 65.3846154    //Brightness = 65.3846154*Reading     //Reading = 0.01529412*Brightness

arduinoFFT FFT = arduinoFFT();
#define SAMPLES 32
#define SAMPLING_FREQUENCY 3000
uint16_t sampling_period_us;
unsigned long micro;
double vReal[SAMPLES];
double vImag[SAMPLES];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

double freqValues3[3];
double freqValues5[5];

#define POT_PIN A1
#define POT_RATIO 0.24926686    //Brightness = 0.24926686*Reading     //Reading = 4.01176471*Brightness

#define NUM_SHOWTYPES 17
typedef enum {CLEAN_UP = 0, RANDOM = 1, SINGLE_ZIPPER = 2, SHIFT_SINGLE_PIXEL = 3,
              ONE_COLOR = 4, ONE_COLOR_STROBE = 5, MULTI_COLOR = 6, MULTI_COLOR_STROBE = 7,
              SHIFT_MULTI_PIXEL = 8, THREE_ARRAY = 9, DIMMER = 10, DIM_IN_OUT = 11, POT_ONE = 12,
              MIC_ONE = 13, MIC_MULTI_3 = 14, MIC_MULTI_5 = 15, CYCLE_COLORS = 16
             } ShowType;
ShowType showType;

typedef struct {
  ShowType Mode;
  String ModeName;
} MenuShowType;

MenuShowType menuOptions[] = {{CLEAN_UP, "CLEAN_UP"},
  {RANDOM, "RANDOM"},
  {SINGLE_ZIPPER, "SINGLE_ZIPPER"},
  {SHIFT_SINGLE_PIXEL, "SHIFT_SINGLE_PIXEL"},
  {ONE_COLOR, "ONE_COLOR"},
  {ONE_COLOR_STROBE, "ONE_COLOR_STROBE"},
  {MULTI_COLOR, "MULTI_COLOR"},
  {MULTI_COLOR_STROBE, "MULTI_COLOR_STROBE"},
  {SHIFT_MULTI_PIXEL, "SHIFT_MULTI_PIXEL"},
  {THREE_ARRAY, "THREE_ARRAY"},
  {DIMMER, "DIMMER"},
  {DIM_IN_OUT, "DIM_IN_OUT"},
  {POT_ONE, "POT_ONE"},
  {MIC_ONE, "MIC_ONE"},
  {MIC_MULTI_3, "MIC_MULTI_3"},
  {MIC_MULTI_5, "MIC_MULTI_5"},
  {CYCLE_COLORS, "CYCLE_COLORS"}
};

LiquidCrystal LCD(12, 11, 5, 4, 7, 8);  // I will need to switch pins 3 and to to 8 and 7
#define LCD_BACKLIGHT_PIN A5
unsigned long menuTimer;


void setup() {
  Serial.begin(9600);
  LCD.begin(16, 2);
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("HELLO USER");
  delay(1000); // initial delay of a few seconds is recommended
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(BRIGHTNESS);// global brightness
  num_colors = 3;
  showProgramCleanUp(1); // clean up
  showType = MIC_MULTI_3;
  FastLED.show();

  menuTimer = millis();
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(BUTTON_4, INPUT);
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  analogWrite(LCD_BACKLIGHT_PIN, HIGH);

  currentColors[0] = CHSV(0, 255, 128);   // Red
  currentColors[1] = CHSV(96, 255, 128);  // Green
  currentColors[2] = CHSV(160, 255, 128); // Blue
  currentColors[3] = CHSV(64, 255, 128);  // Yellow
  currentColors[4] = CHSV(192, 255, 128); // Purple

  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
}

void loop() {
  manageButtons();
  manageMenu();
  onlyLEDModes();
  adjustBrightnessPot();
}

void manageMenu()
{
  if (millis() - menuTimer > 100)
  {
    MenuShowType curMenuShowType = getCurrentMenuShowType();
    LCDClearAndPrintOneLine(0, curMenuShowType.ModeName);

    if (buttons[0].Active)
    {
      LCD.setCursor(0, 1);
      LCD.print("1 ");
      incrementShowType();
    }
    if (buttons[1].Active)
    {
      LCD.setCursor(1, 1);
      LCD.print("2 ");
    }
    if (buttons[2].Active)
    {
      LCD.setCursor(2, 1);
      LCD.print("3 ");
    }
    if (buttons[3].Active)
    {
      LCD.setCursor(3, 1);
      LCD.print("4 ");
    }

    menuTimer += 100;
  }
}

MenuShowType getCurrentMenuShowType()
{
  int index = -1;
  for (int i = 0; i < NUM_SHOWTYPES; i++)
  {
    if (showType == menuOptions[i].Mode) index = i;
  }
  if (index < 0) return menuOptions[0];

  return menuOptions[index];
}

void LCDClearAndPrintOneLine(byte line, String message)
{
  LCD.clear();
  LCD.setCursor(0, line);
  LCD.print(message);
}

void LCDClearAndPrintTwoLines(String line1, String line2)
{
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print(line1);
  LCD.setCursor(0, 1);
  LCD.print(line2);
}

void manageButtons()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    buttons[i].PrevValue = buttons[i].Value;
    buttons[i].Value = digitalRead(buttons[i].Pin);
  }

  if (buttons[0].Value)
  {
    if (validButtonPress(buttons[0]))  buttons[0].Active = true;
    else buttons[0].Active = false;
  }
  else if (buttons[1].Value)
  {
    buttons[0].Active = false;
    if (validButtonPress(buttons[1])) buttons[1].Active = true;
    else buttons[1].Active = false;
  }
  else if (buttons[2].Value)
  {
    buttons[0].Active = false;
    buttons[1].Active = false;
    if (validButtonPress(buttons[2]))
    {
      buttons[2].Active = true;
      if (buttons[3].Value)
      {
        if (validButtonPress(buttons[3])) buttons[3].Active = true;
        else buttons[3].Active = false;
      }
    }
    else
    {
      buttons[2].Active = false;
    }
  }
  else if (buttons[3].Value)
  {
    buttons[0].Active = false;
    buttons[1].Active = false;
    buttons[2].Active = false;
    if (validButtonPress(buttons[3])) buttons[3].Active = true;
    else buttons[3].Active = false;
  }
  else {
    buttons[0].Active = false;
    buttons[1].Active = false;
    buttons[2].Active = false;
    buttons[3].Active = false;
  }
}

int validButtonPress(Button button)
{
  if (button.Value && !button.PrevValue) return 1;
  else return 0;
}

void incrementShowType()
{
  BRIGHTNESS = DEFAULT_BRIGHTNESS;
  FastLED.setBrightness(BRIGHTNESS);
  showProgramCleanUp(1);
  switch (showType)
  {
    case CLEAN_UP:
      showType = RANDOM;
      break;
    case RANDOM:
      showType = SINGLE_ZIPPER;
      break;
    case SINGLE_ZIPPER:
      showType = SHIFT_SINGLE_PIXEL;
      break;
    case SHIFT_SINGLE_PIXEL:
      showType = ONE_COLOR;
      break;
    case ONE_COLOR:
      showType = ONE_COLOR_STROBE;
      break;
    case ONE_COLOR_STROBE:
      showType = MULTI_COLOR;
      break;
    case MULTI_COLOR:
      showType = MULTI_COLOR_STROBE;
      break;
    case MULTI_COLOR_STROBE:
      showType = SHIFT_MULTI_PIXEL;
      break;
    case SHIFT_MULTI_PIXEL:
      showType = THREE_ARRAY;
      break;
    case THREE_ARRAY:
      showType = DIMMER;
      break;
    case DIMMER:
      showType = DIM_IN_OUT;
      break;
    case DIM_IN_OUT:
      showType = POT_ONE;
      break;
    case POT_ONE:
      showType = MIC_ONE;
      break;
    case MIC_ONE:
      showType = MIC_MULTI_3;
      num_colors = 3;
      break;
    case MIC_MULTI_3:
      showType = MIC_MULTI_5;
      num_colors = 5;
      break;
    case MIC_MULTI_5:
      showType = CYCLE_COLORS;
      break;
    case CYCLE_COLORS:
      showType = RANDOM;
      break;
    default:
      showProgramCleanUp(100); // clean up
      break;
  }
}


// switches off all LEDs
void showProgramCleanUp(unsigned long durationTime) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  digitalWrite(13, HIGH); // sets the digital pin 13 on
  delay(durationTime);            // Waits for a second
  digitalWrite(13, LOW);  // sets the digital pin 13 off
}

void onlyLEDModes()
{
  switch (showType)
  {
    case CLEAN_UP:
      showProgramCleanUp(1);
      break;
    case RANDOM:
      showProgramRandom(20, 100);
      break;
    case SINGLE_ZIPPER:
//      showProgramSingleZipper(currentColors[0], 1);
      showProgramSingleZipper(CRGB::Purple, 1);
      break;
    case SHIFT_SINGLE_PIXEL:
//      showProgramShiftSinglePixel(CRGB::White, 1);
      showProgramShiftSinglePixel(CRGB::White, 1);
      break;
    case ONE_COLOR:
//      showProgramOneColor(CRGB::Purple, 1);
      showProgramOneColor(CRGB::Purple, 1);
      break;
    case ONE_COLOR_STROBE:
//      showProgramOneColorStrobe(CRGB::Purple, 10, 100);
      showProgramOneColorStrobe(CRGB::Purple, 10, 100);
      break;
    case MULTI_COLOR:
      showProgramMultiColor(90, 90);
      break;
    case MULTI_COLOR_STROBE:
      showProgramMultiColorStrobe(100, 1000);
      break;
    case SHIFT_MULTI_PIXEL:
      showProgramShiftMultiPixel(20);
      break;
    case THREE_ARRAY:
      showProgramThreeArray(1);
      break;
    case DIMMER:
//      showProgramDimmer(CRGB::Purple, 1, 5);
      showProgramDimmer(CRGB::Purple, 1, 5);
      break;
    case DIM_IN_OUT:
//      showProgramDimInOut(CRGB::Purple, 1, 5);
      showProgramDimInOut(CRGB::Purple, 1, 5);
      break;
    case POT_ONE:
//      showProgramPotentiometerOne(CRGB::Purple, 1);
      showProgramPotentiometerOne(CRGB::Purple, 1);
      break;
    case MIC_ONE:
//      showProgramMicrophoneOne(CRGB::Purple, 1000);
      showProgramMicrophoneOne(CRGB::Purple, 1000);
      break;
    case MIC_MULTI_3:
      showProgramMicrophoneMulti(1000);
      break;
    case MIC_MULTI_5:
      showProgramMicrophoneMulti(1000);
      break;
    case CYCLE_COLORS:
      cycleCRGB();
      break;
    default:
      showProgramCleanUp(100); // clean up
      break;
  }
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
  while (millis() - timer < durationTime)
  {
    showProgramOneColor(crgb, intervalTime);
    showProgramCleanUp(intervalTime);
  }
}


//Cycles through the list of colors
void showProgramMultiColor(unsigned long dTime, unsigned long durationTime) {
  for (unsigned int i = 0; i < num_colors; i++) {
    showProgramOneColor(currentColors[i], dTime);
  }
  delay(durationTime);
}


void showProgramMultiColorStrobe(unsigned long intervalTime, unsigned long durationTime) {
  unsigned long timer = millis();
  int i = 0;
  while (millis() - timer < durationTime)
  {
    showProgramOneColor(currentColors[i], intervalTime);
    showProgramCleanUp(intervalTime);
    i++;
    i %= num_colors;
  }
}


// Shifts multiple pixel from the start of strip to the end. The color of each pixel is randomized.
// durationTime: indicates how long the pixels are shown on each LED
void showProgramShiftMultiPixel(unsigned long durationTime) {
  showProgramCleanUp(1);
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


void showProgramThreeArray(unsigned long durationTime) {
  for (int i = 0; i < NUM_LEDS - 3; i += 3) {
    leds[i] = currentColors[0];;
    leds[i + 1] = currentColors[1];
    leds[i + 2] = currentColors[2];
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

  while (curBright > MIN_BRIGHTNESS) {
    curBright -= decay;
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

  while (curBright > MIN_BRIGHTNESS) {
    curBright -= decay;
    FastLED.setBrightness(curBright);
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = crgb;
    }
    FastLED.show();
    delay(durationTime);
  }

  while (curBright < MAX_BRIGHTNESS - decay) {
    curBright += decay;
    FastLED.setBrightness(curBright);
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = crgb;
    }
    FastLED.show();
    delay(durationTime);
  }

  while (curBright > BRIGHTNESS) {
    curBright -= decay;
    FastLED.setBrightness(curBright);
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = crgb;
    }
    FastLED.show();
    delay(durationTime);
  }
}

double readMic()
{
  unsigned int sample;
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  unsigned long startMillis;
  unsigned int peakToPeak = 0;
  double volts;

  startMillis = millis(); // Start of sample window
  while (millis() - startMillis < MIC_SAMPLE_WINDOW_DURATION)
  {
    sample = analogRead(MIC_PIN);
    if (sample > signalMax)
    {
      signalMax = sample; // save just the max levels
    }
    else if (sample < signalMin)
    {
      signalMin = sample; // save just the min levels
    }
  }
  peakToPeak = signalMax - signalMin; // max - min = peak-peak amplitude
  volts = ((double)peakToPeak * 5.0) / 1024.0; // convert to volts
  //    Serial.println(volts);
  //    Serial.print("    ");
  //    Serial.print(signalMin);
  //    Serial.print("    ");
  //    Serial.println(signalMax);

  return volts;
}

void showProgramMicrophoneOne(CRGB crgb, unsigned long duration) {
  unsigned long Timer;
  double brightness = MIN_BRIGHTNESS;
  double volts;

  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = crgb;
  }

  Timer = millis();

  while (millis() - Timer < duration) {
    if (brightness < MIN_BRIGHTNESS)
      brightness = MIN_BRIGHTNESS;
    if (brightness > MAX_BRIGHTNESS)
      brightness = MAX_BRIGHTNESS;

    //    Serial.print("    ");
    //    Serial.println(brightness);
    FastLED.setBrightness(brightness);
    FastLED.show();

    volts = readMic();
    brightness = (int)(volts * MIC_RATIO);
  }
}

void getAudioAndFilter()
{
  micro = micros();
  for (int i = 0; i < SAMPLES; i++)
  {
    vReal[i] = analogRead(MIC_PIN);
    vImag[i] = 0;
    while (micros() - micro < sampling_period_us)  {}
    micro += sampling_period_us;
  }
  /* Print the results of the sampling according to time */
  //  Serial.println("Data:");
  //  PrintVector(vReal, SAMPLES, SCL_TIME);
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
  //  Serial.println("Weighed data:");
  //  PrintVector(vReal, SAMPLES, SCL_TIME);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD); /* Compute FFT */
  //  Serial.println("Computed Real values:");
  //  PrintVector(vReal, SAMPLES, SCL_INDEX);
  //  Serial.println("Computed Imaginary values:");
  //  PrintVector(vImag, SAMPLES, SCL_INDEX);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES); /* Compute magnitudes */
//    Serial.println("Computed magnitudes:");
//    PrintVector(vReal, (SAMPLES >> 1), SCL_FREQUENCY);
}

void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    double abscissa;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
        break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / SAMPLING_FREQUENCY);
        break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES);
        break;
    }
    Serial.print(abscissa, 2);
    if (scaleType == SCL_FREQUENCY)
      Serial.print("Hz");
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}


void showProgramMicrophoneMulti(unsigned long duration)
{
  unsigned long Timer = millis();
//  while(millis() - Timer > duration)
//  {
//    
//  }
  getAudioAndFilter();
    if(showType == MIC_MULTI_3)
    {
      freqValues3[0] = (vReal[1] + vReal[2] + vReal[3]) / 75;
      
      double value = 0.0;
      for(int i = 4; i < 10; i++)
      {
        value +=vReal[i];
      }
      freqValues3[1] = value / 6;

      value = 0;
      for(int i = 10; i < (SAMPLES >> 1); i++)
      {
        value +=vReal[i];
      }
      freqValues3[2] = value / 6;
//      currentColors[0] = currentColors[0].
      
    }
    else if (showType == MIC_MULTI_5)
    {
      freqValues5[0] = (vReal[1] + vReal[2] + vReal[3]) / 75;
      
      double value = 0.0;
      for(int i = 4; i < 7; i++)
      {
        value +=vReal[i];
      }
      freqValues5[1] = value / 3;

      value = 0;
      for(int i = 7; i < 10; i++)
      {
        value +=vReal[i];
      }
      freqValues5[2] = value / 3;

      value = 0;
      for(int i = 10; i < 12; i++)
      {
        value +=vReal[i];
      }
      freqValues5[3] = value / 3;

      value = 0;
      for(int i = 12; i < (SAMPLES >> 1); i++)
      {
        value +=vReal[i];
      }
      freqValues5[4] = value / 3;
    }

//  for(int i = 0; i < 3; i++)
//  {
//    Serial.print(freqValues3[i]);
//    Serial.print(" - ");
//  }
//  Serial.println();
  
//  for(int i = 0; i < 5; i++)
//  {
//    Serial.print(freqValues5[i]);
//    Serial.print(" - ");
//  }
//  Serial.println();

  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t val = BRIGHTNESS / 2;
    int j = i % num_colors;
    if(num_colors == 3)
    {
      val = min(min(freqValues3[j], MAX_BRIGHTNESS), val);
      val = max(freqValues3[j], MIN_BRIGHTNESS);
      currentColors[j].val = val;
//      currentColors[j].val = freqValues3[j] + MIN_BRIGHTNESS;
    }
    else if(num_colors == 5)
    {
      val = min(min(freqValues5[j], MAX_BRIGHTNESS), val);
      val = max(freqValues5[j], MIN_BRIGHTNESS);
      currentColors[j].val = val;
//      currentColors[j].val = freqValues5[j] + MIN_BRIGHTNESS;
    }
    leds[i] = currentColors[j];
  }
    FastLED.show();
}

//void showProgramMicrophoneMulti(unsigned long duration)
//{
//  unsigned long Timer;
//  double brightness = MIN_BRIGHTNESS;
//  double volts;
//
//  for (int i = 0; i < NUM_LEDS; ++i) {
//    int j = i % num_colors;
//    leds[i] = currentColors[j];
//  }
//
//  Timer = millis();
//
//  while (millis() - Timer < duration) {
//    if (brightness < MIN_BRIGHTNESS)
//      brightness = MIN_BRIGHTNESS;
//    if (brightness > MAX_BRIGHTNESS)
//      brightness = MAX_BRIGHTNESS;
//
//    FastLED.setBrightness(brightness);
//    FastLED.show();
//
//    volts = readMic();
//    brightness = (int)(volts * MIC_RATIO);
//  }
//}

void adjustBrightnessPot()
{
  double value = (analogRead(POT_PIN) * POT_RATIO);
  double ratio = value / 100;
  MIN_BRIGHTNESS = (int)(DEFAULT_MIN_BRIGHTNESS * ratio);
  MAX_BRIGHTNESS = (int)(DEFAULT_MAX_BRIGHTNESS * ratio);
  if (MIN_BRIGHTNESS < 0) MIN_BRIGHTNESS = 0;
  if (MAX_BRIGHTNESS > 200) MAX_BRIGHTNESS = 200;

//  Serial.println(value);
//  Serial.print("MIN: ");
//  Serial.println(MIN_BRIGHTNESS);
//  Serial.print("MAX: ");
//  Serial.println(MAX_BRIGHTNESS);
}

void showProgramPotentiometerOne(CRGB crgb, unsigned long duration) {
  unsigned long Timer;
  double value = analogRead(POT_PIN);
  value = (int)(value * POT_RATIO);
  if (value < MIN_BRIGHTNESS) value = MIN_BRIGHTNESS;
  if (value > MAX_BRIGHTNESS) value = MAX_BRIGHTNESS;
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = crgb;
  }
  Timer = millis();
  while (millis() - Timer < duration) {
    FastLED.setBrightness(value);
    FastLED.show();
    value = (int)(analogRead(POT_PIN) * POT_RATIO);
    Serial.println(value);
  }
}

void cycleCRGB()
{
//  for (Color i : BasicColors)
//  {
//    for (int j = 0; j < NUM_LEDS; j++)
//    {
//      leds[j] = i.Value;
//    }
//    BRIGHTNESS = DEFAULT_BRIGHTNESS;
//    FastLED.show(BRIGHTNESS);
//    FastLED.show();
//    LCD.clear();
//    LCD.setCursor(0, 0);
//    LCD.print(i.Name);
//    delay(750);
//  }
}
