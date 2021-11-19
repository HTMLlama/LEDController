#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
#include <RotaryEncoder.h>
#include <LiquidCrystal.h>

#define NUM_LEDS 300
#define LED_PIN 5
#define COLOR_ORDER RGB
#define BGT_PIN A4
int bgt = 20;
CRGB leds[NUM_LEDS];
int hue = 100;

#define SOLID 0
#define PCMR 1
#define FALL_HOLIDAY 2
#define XMAS 3
#define MURICA 4
#define PRIDE 5

#define FX_TOTAL 6
int fx = SOLID;
String fxName = "Solid";

#define ENCODER_SW 4
#define ENCODER_DT 3
#define ENCODER_CLK 2
RotaryEncoder *encoder = nullptr;

const int RS = 13, EN = 12, D4 = 11, D5 = 10, D6 = 9, D7 = 8;
LiquidCrystal lcd(RS,EN,D4,D5,D6,D7); 

// -----------------------------------------------------

int readEncoder(int item, boolean isColor) {
  int newItem = item + (int)encoder->getDirection();
  if (isColor) {
    if (newItem > 255) {
      newItem = 0;
    } else if (newItem < 0) {
      newItem = 255;
    }
  }
  return newItem;
}

void readEncoderBtn() {
  if (digitalRead(ENCODER_SW) == LOW) {
    fx++;
    if (fx >= FX_TOTAL) {
      fx = 0;
    }
    delay(200);
  }
  
}

void displayLights() {
  bgt = map(analogRead(BGT_PIN), 1023, 0, 0, 255);
  FastLED.setBrightness(bgt);
  switch (fx) {
  case PCMR:
      fxName = "PCMR";
      fill_rainbow( leds, NUM_LEDS, hue, 2);
      hue++;
      if (hue > 255) {
        hue = 0;
      }
      
    break;

  case FALL_HOLIDAY:
    fxName = "Fall";
    for (size_t i = 0; i < NUM_LEDS; i+=16) {
      for (size_t n = 0; n < 8; n++) {
        int indexA = i+n;
        int indexB = i+n+8;
        if (indexA < NUM_LEDS) {
          leds[indexA] = CRGB().setHSV(25, 255, 255);
        }
        if (indexB < NUM_LEDS) {
          leds[indexB] = CRGB::White;
        }
      }
      
    }
    
    break;
  
  case XMAS:
    fxName = "Xmas";
    for (size_t i = 0; i < NUM_LEDS; i+=16) {
      for (size_t n = 0; n < 8; n++) {
        int indexA = i+n;
        int indexB = i+n+8;
        if (indexA < NUM_LEDS) {
          leds[indexA] = CRGB::Red;
        }
        if (indexB < NUM_LEDS) {
          leds[indexB] = CRGB::Green;
        }
      }
    }
    break;
  
  case MURICA:
    fxName = "Murica!";
    for (size_t i = 0; i < NUM_LEDS; i+=27) {
      for (size_t n = 0; n < 9; n++) {
        int indexA = i+n;
        int indexB = i+n+9;
        int indexC = i+n+9+9;
        if (indexA < NUM_LEDS) {
          leds[indexA] = CRGB::Red;
        }
        if (indexB < NUM_LEDS) {
          leds[indexB] = CRGB::White;
        }
        if (indexC < NUM_LEDS) {
          leds[indexC] = CRGB::Blue;
        }
      }
    }
    break;

  case PRIDE:
  fxName = "Pride";
    fill_rainbow( leds, NUM_LEDS, hue, 2);
    break;
  
  default:

  fxName = "Solid";
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB().setHSV(hue, 255, 255);
    }
    break;
  }
  FastLED.show();
}

void displayLcd() {
  lcd.home();
  lcd.clear();
  lcd.print(fxName);
  lcd.setCursor(0, 2);
  lcd.print("hue: ");
  lcd.print(hue);
}

// -----------------------------------------------------

void checkPosition() {
  encoder->tick();
}

void setup() {
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(BGT_PIN, INPUT);

  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(bgt);

  // encoder = new RotaryEncoder(ENCODER_CLK, ENCODER_DT, RotaryEncoder::LatchMode::TWO03);
  encoder = new RotaryEncoder(ENCODER_CLK, ENCODER_DT, RotaryEncoder::LatchMode::FOUR3);
  // encoder = new RotaryEncoder(ENCODER_CLK, ENCODER_DT, RotaryEncoder::LatchMode::FOUR0);
  attachInterrupt(digitalPinToInterrupt(ENCODER_DT), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), checkPosition, CHANGE);

  lcd.begin(16, 2);

  // Serial.begin(9600);
}

void loop() {
  hue = readEncoder(hue, true);
  readEncoderBtn();
  // displayLights();
  // displayLcd();

  EVERY_N_MILLISECONDS( 20 ) { displayLights(); }

  // Serial.println(hue);
  // delay(100);
}