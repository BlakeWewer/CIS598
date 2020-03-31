#include <LiquidCrystal.h>
#include <FastLED.h>
#include <ListLib.h>

using namespace std;

#define ONBOARD_LED_PIN 13
#define DATA_PIN 6
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 240
#define BRIGHTNESS 75
#define MIN_BRIGHTNESS 5
#define MAX_BRIGHTNESS 200

CRGB leds[NUM_LEDS];
CRGB colorsUSA[] = {CRGB::Red, CRGB::White, CRGB::Blue, CRGB::Black};
CRGB colorsTest[] = {CRGB::Red, CRGB::Green, CRGB::Blue};
CRGB colorsKSU[] = {0x512888, CRGB::Silver, CRGB::White};
List<CRGB> colors;
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
#define MIC_PIN A0
#define MIC_RATIO 65.3846154    //Brightness = 65.3846154*Reading     //Reading = 0.01529412*Brightness

#define POT_PIN A1
#define POT_RATIO 0.24926686    //Brightness = 0.24926686*Reading     //Reading = 4.01176471*Brightness

typedef enum {CLEAN_UP = 0, RANDOM = 1, SINGLE_ZIPPER = 2, SHIFT_SINGLE_PIXEL = 3,
              ONE_COLOR = 4, ONE_COLOR_STROBE = 5, MULTI_COLOR = 6, MULTI_COLOR_STROBE = 7,
              SHIFT_MULTI_PIXEL = 8, THREE_ARRAY = 9, DIMMER = 10, DIM_IN_OUT = 11, POT_ONE = 12} ShowType;
ShowType showType;

LiquidCrystal LCD(12, 11, 5, 4, 3, 2);
#define LCD_BACKLIGHT_PIN A5
unsigned long menuTimer;

#define BUTTON_PIN A2
int buttonValue = 0;

unsigned long durationTime;
CRGB crgb;
CRGB three_colors[] = {colorsTest[0], colorsTest[1], colorsTest[2]};
unsigned long intervalTime;
unsigned int decay;
void testShowPrograms();
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
void showProgramPotentiometerOne(CRGB , unsigned long );
void showProgramMicrophoneOne(CRGB, unsigned long);


void setup() {
  Serial.begin(9600);
  delay(3000); // initial delay of a few seconds is recommended
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(BRIGHTNESS);// global brightness
  showProgramCleanUp(1); // clean up
  showType = CLEAN_UP;
  FastLED.show();

  for (unsigned int i = 0; i < (sizeof(colorsKSU) / sizeof(CRGB)); i++)
  {
    colors.Add(colorsKSU[i]);
  }

  LCD.begin(16, 2);
  LCD.clear();
  LCD.setCursor(0,0);
  LCD.print("HELLO USER");
  menuTimer = millis();
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  analogWrite(LCD_BACKLIGHT_PIN, HIGH);
}

void loop() {
  testShowPrograms();
//  onlyLEDModes();
  manageButtons();
//  manageMenu();
}


void manageMenu()
{
  if(millis() - menuTimer > 100)
  {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("HELLO USER");
    LCD.setCursor(0, 1);
    LCD.print("TEST");
    menuTimer+=100;
  }  
}

void manageButtons()
{
  buttonValue = analogRead(BUTTON_PIN);

  Serial.println(buttonValue);               //Display the read value in the Serial monitor
  if (buttonValue < 100)                     //Lower limit for first button - if below this limit then no button is pushed and LEDs are turned off
  {
    LCD.clear();
    LCD.setCursor(0,0);
    LCD.print("No Button");
    LCD.setCursor(0, 1);
    LCD.print(buttonValue);
  }
  else if (buttonValue < 200)                //First button limit
  {
    LCD.clear();
    LCD.setCursor(0,0);
    LCD.print("Button 1");
    LCD.setCursor(0, 1);
    LCD.print(buttonValue);
  }
  else if (buttonValue < 350)                //Second button limit
  {
    LCD.clear();
    LCD.setCursor(0,0);
    LCD.print("Button 2");
    LCD.setCursor(0, 1);
    LCD.print(buttonValue);
  }
  else if (buttonValue < 750)                //Third button limit
  {
    LCD.clear();
    LCD.setCursor(0,0);
    LCD.print("Button 3");
    LCD.setCursor(0, 1);
    LCD.print(buttonValue);
  }
  else                                       //Fourth button limit
  {
    LCD.clear();
    LCD.setCursor(0,0);
    LCD.print("Button 4");
    LCD.setCursor(0, 1);
    LCD.print(buttonValue);
  }
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

void onlyLEDModes()
{
  switch(showType)
  {
    case CLEAN_UP: 
      showProgramCleanUp(1);
      break;
    case RANDOM: 
      showProgramRandom(100, 100);
      break;
    case SINGLE_ZIPPER: 
      showProgramSingleZipper(CRGB::Purple, 10);
      break;
    case SHIFT_SINGLE_PIXEL: 
      showProgramShiftSinglePixel(CRGB::Blue, 100);
      break;
    case ONE_COLOR: 
      showProgramOneColor(CRGB::Purple, 50);
      break;
    case ONE_COLOR_STROBE: 
      showProgramOneColorStrobe(CRGB::Purple, 66, 7000);
      break;
    case MULTI_COLOR: 
      showProgramMultiColor(1000, 1);
      break;
    case MULTI_COLOR_STROBE: 
      showProgramMultiColorStrobe(100, 10000);
      break;
    case SHIFT_MULTI_PIXEL: 
      showProgramShiftMultiPixel(25);
      break;
    case THREE_ARRAY: 
      showProgramThreeArray(CRGB::Blue, CRGB::Red, CRGB::Green, 1000);
      break;
    case DIMMER: 
      showProgramDimmer(CRGB::Purple, 1, 5);
      break;
    case DIM_IN_OUT: 
      showProgramDimInOut(CRGB::Purple, 1, 5);
      break;
    case POT_ONE:
      showProgramPotentiometerOne(CRGB::Purple, 10000);
      break;
    default: 
      showProgramCleanUp(100); // clean up
      break;
  }
}

void testShowPrograms()
{
//    showProgramCleanUp(100); // clean up
//    showProgramRandom(100, 100); // show "random" program
//  
//    showProgramCleanUp(1000); // clean up
//    showProgramRandom(100, 66); // show "random" program
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
//    showProgramOneColorStrobe(CRGB::Purple, 66, 7000); // show "one color strobe" program
//
//    showProgramCleanUp(1000);
//    showProgramMultiColorStrobe(100, 10000); // MAY HAVE SOME PROBLEMS STILL
//      
//    showProgramCleanUp(100);
//    showProgramMultiColor(1000, 1);   // MAY HAVE SOME PROBLEMS STILL //show "multi color" program
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
//
    showProgramCleanUp(100);
    showProgramMicrophoneOne(CRGB::Purple, 1000);  // show "microphone one color" program
//
//    showProgramCleanUp(100);
//    showProgramPotentiometerOne(CRGB::Purple, 1000);
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

void showProgramMicrophoneOne(CRGB crgb, unsigned long duration) {
  unsigned long Timer;
  double value = MIN_BRIGHTNESS;
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  unsigned long startMillis;
  unsigned int peakToPeak = 0;
  double volts;
  
  for (int i = 0; i < NUM_LEDS; ++i) { 
    leds[i] = crgb;
  }

  Timer = millis();
  
  while (millis() - Timer < duration) {
    if(value < MIN_BRIGHTNESS)
      value = MIN_BRIGHTNESS;
    if(value > MAX_BRIGHTNESS)
      value = MAX_BRIGHTNESS;
    
//    Serial.print("    ");
//    Serial.println(value);
    FastLED.setBrightness(value);
    FastLED.show();
    
    startMillis = millis(); // Start of sample window
    signalMax = 0;
    signalMin = 1023;
    while (millis() - startMillis < sampleWindow)
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
    volts = (peakToPeak * 3.3) / 1024; // convert to volts
//    Serial.print(volts);
//    Serial.print("    ");
//    Serial.print(signalMin);
//    Serial.print("    ");
//    Serial.println(signalMax);
//
    value = (int)(volts * MIC_RATIO);
  }
}

void showProgramPotentiometerOne(CRGB crgb, unsigned long duration) {
  unsigned long Timer;
  double value = analogRead(POT_PIN);
  value = (int)(value*POT_RATIO); 
  if(value < MIN_BRIGHTNESS) value = MIN_BRIGHTNESS;
  if(value > MAX_BRIGHTNESS) value = MAX_BRIGHTNESS; 
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = crgb;
  }
  Timer = millis();
  while(millis() - Timer < duration) {
    FastLED.setBrightness(value);
    FastLED.show();
    value = (int)(analogRead(POT_PIN)*POT_RATIO);
    Serial.println(value);
  }
}
