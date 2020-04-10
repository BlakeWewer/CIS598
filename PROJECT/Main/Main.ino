#include <LiquidCrystal.h>
#include <FastLED.h>
#include <ListLib.h>

using namespace std;

#define ONBOARD_LED_PIN 13
#define DATA_PIN 6
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 240
#define DEFAULT_BRIGHTNESS 92
byte BRIGHTNESS = 92;
#define DEFAULT_MIN_BRIGHTNESS 5
#define DEFAULT_MAX_BRIGHTNESS 200
int MIN_BRIGHTNESS = 5;
int MAX_BRIGHTNESS = 200;

#define BUTTON_1 7
#define BUTTON_2 8
#define BUTTON_3 9
#define BUTTON_4 10
#define NUM_BUTTONS 4
// I want to switch the pins for Buttons 1 and 2 to D2 and D3 in order to enable interrupts

typedef enum {Idle = 0, Wait = 1, High = 2} ButtonState;

typedef struct {
  int Pin;
  int Value;
  int PrevValue;
  bool Active;
  ButtonState State;
  unsigned long Timer;
} Button;

Button buttons[] = {{BUTTON_1, 0, 0, false, Idle, 0},
  {BUTTON_2, 0, 0, false, Idle, 0},
  {BUTTON_3, 0, 0, false, Idle, 0},
  {BUTTON_4, 0, 0, false, Idle, 0}
};

CRGB leds[NUM_LEDS];
CRGB colorsUSA[] = {CRGB::Red, CRGB::White, CRGB::Blue, CRGB::Black};
CRGB colorsTest[] = {CRGB::Red, CRGB::Green, CRGB::Blue};
CRGB colorsKSU[] = {0x512888, CRGB::Silver, CRGB::White};
int num_colors;

List<CRGB> colors;
typedef struct {
  CRGB Value;
  String Name;
} Color;

Color BasicColors[] = {{CRGB::AliceBlue, "AliceBlue"}, {0x512888, "Royal Purple"}};
//CRGB BasicColors[] = {CRGB::AliceBlue, CRGB::Amethyst, CRGB::AntiqueWhite, CRGB::Aqua, CRGB::Aquamarine, CRGB::Azure, CRGB::Beige, CRGB::Bisque, CRGB::Black, CRGB::BlanchedAlmond, CRGB::Blue, CRGB::BlueViolet, CRGB::Brown, CRGB::BurlyWood, CRGB::CadetBlue, CRGB::Chartreuse, CRGB::Chocolate, CRGB::Coral, CRGB::CornflowerBlue, CRGB::Cornsilk, CRGB::Crimson, CRGB::Cyan, CRGB::DarkBlue, CRGB::DarkCyan, CRGB::DarkGoldenrod, CRGB::DarkGray, CRGB::DarkGrey, CRGB::DarkGreen, CRGB::DarkKhaki, CRGB::DarkMagenta, CRGB::DarkOliveGreen, CRGB::DarkOrange, CRGB::DarkOrchid, CRGB::DarkRed, CRGB::DarkSalmon, CRGB::DarkSeaGreen, CRGB::DarkSlateBlue, CRGB::DarkSlateGray, CRGB::DarkSlateGrey, CRGB::DarkTurquoise, CRGB::DarkViolet, CRGB::DeepPink, CRGB::DeepSkyBlue, CRGB::DimGray, CRGB::DimGrey, CRGB::DodgerBlue, CRGB::FireBrick, CRGB::FloralWhite, CRGB::ForestGreen, CRGB::Fuchsia, CRGB::Gainsboro, CRGB::GhostWhite, CRGB::Gold, CRGB::Goldenrod, CRGB::Gray, CRGB::Grey, CRGB::Green, CRGB::GreenYellow, CRGB::Honeydew, CRGB::HotPink, CRGB::IndianRed, CRGB::Indigo, CRGB::Ivory, CRGB::Khaki, CRGB::Lavender, CRGB::LavenderBlush, CRGB::LawnGreen, CRGB::LemonChiffon, CRGB::LightBlue, CRGB::LightCoral, CRGB::LightCyan, CRGB::LightGoldenrodYellow, CRGB::LightGreen, CRGB::LightGrey, CRGB::LightPink, CRGB::LightSalmon, CRGB::LightSeaGreen, CRGB::LightSkyBlue, CRGB::LightSlateGray, CRGB::LightSlateGrey, CRGB::LightSteelBlue, CRGB::LightYellow, CRGB::Lime, CRGB::LimeGreen, CRGB::Linen, CRGB::Magenta, CRGB::Maroon, CRGB::MediumAquamarine, CRGB::MediumBlue, CRGB::MediumOrchid, CRGB::MediumPurple, CRGB::MediumSeaGreen, CRGB::MediumSlateBlue, CRGB::MediumSpringGreen, CRGB::MediumTurquoise, CRGB::MediumVioletRed, CRGB::MidnightBlue, CRGB::MintCream, CRGB::MistyRose, CRGB::Moccasin, CRGB::NavajoWhite, CRGB::Navy, CRGB::OldLace, CRGB::Olive, CRGB::OliveDrab, CRGB::Orange, CRGB::OrangeRed, CRGB::Orchid, CRGB::PaleGoldenrod, CRGB::PaleGreen, CRGB::PaleTurquoise, CRGB::PaleVioletRed, CRGB::PapayaWhip, CRGB::PeachPuff, CRGB::Peru, CRGB::Pink, CRGB::Plaid, CRGB::Plum, CRGB::PowderBlue, CRGB::Purple, CRGB::Red, CRGB::RosyBrown, CRGB::RoyalBlue, CRGB::SaddleBrown, CRGB::Salmon, CRGB::SandyBrown, CRGB::SeaGreen, CRGB::Seashell, CRGB::Sienna, CRGB::Silver, CRGB::SkyBlue, CRGB::SlateBlue, CRGB::SlateGray, CRGB::SlateGrey, CRGB::Snow, CRGB::SpringGreen, CRGB::SteelBlue, CRGB::Tan, CRGB::Teal, CRGB::Thistle, CRGB::Tomato, CRGB::Turquoise, CRGB::Violet, CRGB::Wheat, CRGB::White, CRGB::WhiteSmoke, CRGB::Yellow, CRGB::YellowGreen, CRGB::FairyLight, CRGB::FairyLightNCC};
//String BasicColorsNames[] = {"AliceBlue", "Amethyst", "AntiqueWhite", "Aqua", "Aquamarine", "Azure", "Beige", "Bisque", "Black", "BlanchedAlmond", "Blue", "BlueViolet", "Brown", "BurlyWood", "CadetBlue", "Chartreuse", "Chocolate", "Coral", "CornflowerBlue", "Cornsilk", "Crimson", "Cyan", "DarkBlue", "DarkCyan", "DarkGoldenrod", "DarkGray", "DarkGrey", "DarkGreen", "DarkKhaki", "DarkMagenta", "DarkOliveGreen", "DarkOrange", "DarkOrchid", "DarkRed", "DarkSalmon", "DarkSeaGreen", "DarkSlateBlue", "DarkSlateGray", "DarkSlateGrey", "DarkTurquoise", "DarkViolet", "DeepPink", "DeepSkyBlue", "DimGray", "DimGrey", "DodgerBlue", "FireBrick", "FloralWhite", "ForestGreen", "Fuchsia", "Gainsboro", "GhostWhite", "Gold", "Goldenrod", "Gray", "Grey", "Green", "GreenYellow", "Honeydew", "HotPink", "IndianRed", "Indigo", "Ivory", "Khaki", "Lavender", "LavenderBlush", "LawnGreen", "LemonChiffon", "LightBlue", "LightCoral", "LightCyan", "LightGoldenrodYellow", "LightGreen", "LightGrey", "LightPink", "LightSalmon", "LightSeaGreen", "LightSkyBlue", "LightSlateGray", "LightSlateGrey", "LightSteelBlue", "LightYellow", "Lime", "LimeGreen", "Linen", "Magenta", "Maroon", "MediumAquamarine", "MediumBlue", "MediumOrchid", "MediumPurple", "MediumSeaGreen", "MediumSlateBlue", "MediumSpringGreen", "MediumTurquoise", "MediumVioletRed", "MidnightBlue", "MintCream", "MistyRose", "Moccasin", "NavajoWhite", "Navy", "OldLace", "Olive", "OliveDrab", "Orange", "OrangeRed", "Orchid", "PaleGoldenrod", "PaleGreen", "PaleTurquoise", "PaleVioletRed", "PapayaWhip", "PeachPuff", "Peru", "Pink", "Plaid", "Plum", "PowderBlue", "Purple", "Red", "RosyBrown", "RoyalBlue", "SaddleBrown", "Salmon", "SandyBrown", "SeaGreen", "Seashell", "Sienna", "Silver", "SkyBlue", "SlateBlue", "SlateGray", "SlateGrey", "Snow", "SpringGreen", "SteelBlue", "Tan", "Teal", "Thistle", "Tomato", "Turquoise", "Violet", "Wheat", "White", "WhiteSmoke", "Yellow", "YellowGreen", "FairyLight", "FairyLightNCC"};
const byte MIC_SAMPLE_WINDOW_DURATION = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
#define MIC_PIN A0
#define MIC_RATIO 65.3846154    //Brightness = 65.3846154*Reading     //Reading = 0.01529412*Brightness

#define POT_PIN A1
#define POT_RATIO 0.24926686    //Brightness = 0.24926686*Reading     //Reading = 4.01176471*Brightness

#define NUM_SHOWTYPES 16
typedef enum {CLEAN_UP = 0, RANDOM = 1, SINGLE_ZIPPER = 2, SHIFT_SINGLE_PIXEL = 3,
              ONE_COLOR = 4, ONE_COLOR_STROBE = 5, MULTI_COLOR = 6, MULTI_COLOR_STROBE = 7,
              SHIFT_MULTI_PIXEL = 8, THREE_ARRAY = 9, DIMMER = 10, DIM_IN_OUT = 11, POT_ONE = 12,
              MIC_ONE = 13, MIC_MULTI = 14, CYCLE_COLORS = 15
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
  {MIC_MULTI, "MIC_MULTI"},
  {CYCLE_COLORS, "CYCLE_COLORS"}
};

typedef enum { AUDIO3 = 0, AUDIO5 = 1 } ModeType;
ModeType mode;

LiquidCrystal LCD(12, 11, 5, 4, 3, 2);  // I will need to switch pins 3 and to to 8 and 7
#define LCD_BACKLIGHT_PIN A5
unsigned long menuTimer;

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
  LCD.begin(16, 2);
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("HELLO USER");
  delay(1000); // initial delay of a few seconds is recommended
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(BRIGHTNESS);// global brightness
  showProgramCleanUp(1); // clean up
  showType = CLEAN_UP;
  FastLED.show();

  menuTimer = millis();
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(BUTTON_4, INPUT);
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  analogWrite(LCD_BACKLIGHT_PIN, HIGH);
}

void loop() {
  manageButtons();
  manageMenu();
  adjustBrightnessPot();
  onlyLEDModes();
  //  testShowPrograms();
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
  for(int i = 0; i < NUM_BUTTONS; i++)
  {
    buttons[i].PrevValue = buttons[i].Value;
    buttons[i].Value = digitalRead(buttons[i].Pin);
  }

  if(buttons[0].Value)
  {
     if(buttonPressed(buttons[0]))  buttons[0].Active = true;
     else buttons[0].Active = false;
  }
  else if (buttons[1].Value)
  {
    buttons[0].Active = false;
    if(buttonPressed(buttons[1])) buttons[1].Active = true;
    else buttons[1].Active = false;
  }
  else if(buttons[2].Value)
  {
    buttons[0].Active = false;
    buttons[1].Active = false;
    if(buttonPressed(buttons[2]))
    {
      buttons[2].Active = true;
      if(buttons[3].Value)
      {
          if(buttonPressed(buttons[3])) buttons[3].Active = true;
          else buttons[3].Active = false;
      }
    }
    else
    {
      buttons[2].Active = false;
    }
  }
  else if(buttons[3].Value)
  {
    buttons[0].Active = false;
    buttons[1].Active = false;
    buttons[2].Active = false;
    if(buttonPressed(buttons[3])) buttons[3].Active = true;
    else buttons[3].Active = false;
  }
  else{
    buttons[0].Active = false;
    buttons[1].Active = false;
    buttons[2].Active = false;
    buttons[3].Active = false;
  }
}

int buttonPressed(Button button)
{
  if(button.Value && !button.PrevValue) return 1;
  else return 0;
}

void incrementShowType()
{
  FastLED.setBrightness(DEFAULT_BRIGHTNESS);
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
      showType = MIC_MULTI;
      break;
    case MIC_MULTI:
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

//void manageButtonsAnalog()
//{
//  buttonValue = analogRead(BUTTON_PIN);
//
//  Serial.println(buttonValue);               //Display the read value in the Serial monitor
//  if (buttonValue < 100)                     //Lower limit for first button - if below this limit then no button is pushed and LEDs are turned off
//  {
//    LCD.clear();
//    LCD.setCursor(0, 0);
//    LCD.print("No Button");
//    LCD.setCursor(0, 1);
//    LCD.print(buttonValue);
//  }
//  else if (buttonValue < 325)                //First button limit
//  {
//    LCD.clear();
//    LCD.setCursor(0, 0);
//    LCD.print("Button 1");
//    LCD.setCursor(0, 1);
//    LCD.print(buttonValue);
//  }
//  else if (buttonValue < 450)                //Second button limit
//  {
//    LCD.clear();
//    LCD.setCursor(0, 0);
//    LCD.print("Button 2");
//    LCD.setCursor(0, 1);
//    LCD.print(buttonValue);
//  }
//  else if (buttonValue < 900)                //Third button limit
//  {
//    LCD.clear();
//    LCD.setCursor(0, 0);
//    LCD.print("Button 3");
//    LCD.setCursor(0, 1);
//    LCD.print(buttonValue);
//  }
//  else                                       //Fourth button limit
//  {
//    LCD.clear();
//    LCD.setCursor(0, 0);
//    LCD.print("Button 4");
//    LCD.setCursor(0, 1);
//    LCD.print(buttonValue);
//  }
//}


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
      showProgramRandom(100, 1);
      break;
    case SINGLE_ZIPPER:
      showProgramSingleZipper(CRGB::Purple, 1);
      break;
    case SHIFT_SINGLE_PIXEL:
      showProgramShiftSinglePixel(CRGB::Blue, 1);
      break;
    case ONE_COLOR:
      showProgramOneColor(CRGB::Purple, 1);
      break;
    case ONE_COLOR_STROBE:
      showProgramOneColorStrobe(CRGB::Purple, 1, 1);
      break;
    case MULTI_COLOR:
      showProgramMultiColor(1, 1);
      break;
    case MULTI_COLOR_STROBE:
      showProgramMultiColorStrobe(1, 1);
      break;
    case SHIFT_MULTI_PIXEL:
      showProgramShiftMultiPixel(1);
      break;
    case THREE_ARRAY:
      showProgramThreeArray(CRGB::Blue, CRGB::Red, CRGB::Green, 1);
      break;
    case DIMMER:
      showProgramDimmer(CRGB::Purple, 1, 5);
      break;
    case DIM_IN_OUT:
      showProgramDimInOut(CRGB::Purple, 1, 5);
      break;
    case POT_ONE:
      showProgramPotentiometerOne(CRGB::Purple, 1);
      break;
    case MIC_ONE:
      showProgramMicrophoneOne(CRGB::Purple, 1);
      break;
    case MIC_MULTI:
      //      showProgramMicrophoneOne(CRGB::Purple, 10000);    Still need to implement function
      break;
    case CYCLE_COLORS:
      //      cycleCRGB();
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
  //    showProgramMultiColorStrobe(50, 10000); // show "multi color strobe" program
  //
  //    showProgramCleanUp(100);
  //    showProgramMultiColor(1000, 1);   //show "multi color" program
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
  //    showProgramCleanUp(100);
  //    showProgramMicrophoneOne(CRGB::Purple, 1000);  // show "microphone one color" program
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
  while (millis() - timer < durationTime)
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
  while (millis() - timer < durationTime)
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

  while (curBright > 0) {
    curBright -= decay;
    FastLED.setBrightness(curBright);
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = crgb;
    }
    FastLED.show();
    delay(durationTime);
  }

  while (curBright < 255 - decay) {
    curBright += decay;
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
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  unsigned long startMillis;
  unsigned int peakToPeak = 0;
  double volts;


  startMillis = millis(); // Start of sample window
  signalMax = 0;
  signalMin = 1023;
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
  volts = (peakToPeak * 3.3) / 1024; // convert to volts
  //    Serial.print(volts);
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

void adjustBrightnessPot()
{
  double value = (analogRead(POT_PIN) * POT_RATIO);
  double ratio = value / 127.5;
  MIN_BRIGHTNESS = (int)(DEFAULT_MIN_BRIGHTNESS * ratio);
  MAX_BRIGHTNESS = (int)(DEFAULT_MAX_BRIGHTNESS * ratio);
  if (MIN_BRIGHTNESS < 0) MIN_BRIGHTNESS = 0;
  if (MAX_BRIGHTNESS > 255) MAX_BRIGHTNESS = 255;

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

//void cycleCRGB()
//{
//  for(CRGB i : BasicColors)
//  {
//    for(int j = 0; j < NUM_LEDS; j++)
//    {
//      leds[j] = i;
//    }
//    FastLED.show();
//    unsigned long Timer = millis();
//    LCD.clear();
//    LCD.setCursor(0, 0);
//    LCD.print(BasicColorsNames[i]);
//    while(millis() - Timer < 100)
//    {
//
//    }
//  }
//}
