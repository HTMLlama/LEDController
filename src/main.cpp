#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
#include <RotaryEncoder.h>

#define NUM_LEDS 300
#define LED_PIN 5
#define COLOR_ORDER RGB
#define BGT_PIN A4
int bgt = 20;
CRGB leds[NUM_LEDS];
int hue = 100;
int hue2 = 200;

#define SOLID 0
#define PCMR 1
#define FALL_HOLIDAY 2
#define XMAS 3
#define MURICA 4
#define PRIDE 5
#define DUAL 6

#define FX_TOTAL 7
int fx = DUAL;

#define ENCODER_SW 4
#define ENCODER_DT 3
#define ENCODER_CLK 2
RotaryEncoder *encoder = nullptr;

#define HSV_1_SW 9
#define HSV_2_SW 10
#define HSV_S_SW 11
#define HSV_V_SW 12

// -----------------------------------------------------

int roller(int item) {
  if (item > 255) {
    item = 0;
  } else if (item < 0) {
    item = 255;
  }
  return item;
}

void readEncoder() {
  int direction = (int)encoder->getDirection();
  if (digitalRead(HSV_1_SW) == LOW) {
    hue = roller(hue + direction);
  }
  if (digitalRead(HSV_2_SW) == LOW) {
    hue2 = roller(hue2 + direction);
  }
}

void readEncoderBtn() {
  if (digitalRead(ENCODER_SW) == LOW) {
    fx++;
    if (fx >= FX_TOTAL) {
      fx = 0;
    }
    delay(500);
  }
  
}

void displayLights() {
  bgt = map(analogRead(BGT_PIN), 1023, 0, 0, 255);
  FastLED.setBrightness(bgt);
  switch (fx) {
  case PCMR:
      fill_rainbow( leds, NUM_LEDS, hue, 2);
      hue++;
      if (hue > 255) {
        hue = 0;
      }
      
    break;

  case FALL_HOLIDAY:
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
    fill_rainbow( leds, NUM_LEDS, hue, 2);
    break;

  case DUAL:
    for (size_t i = 0; i < NUM_LEDS; i+=16) {
      for (size_t n = 0; n < 8; n++) {
        int indexA = i+n;
        int indexB = i+n+8;
        if (indexA < NUM_LEDS) {
          leds[indexA] = CRGB().setHSV(hue, 200, 200);
        }
        if (indexB < NUM_LEDS) {
          leds[indexB] = CRGB().setHSV(hue2, 200, 200);;
        }
      } 
    }
    break;
  
  default:

    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB().setHSV(hue, 255, 255);
    }
    break;
  }

  FastLED.show();
}

// -----------------------------------------------------

void checkPosition() {
  encoder->tick();
}

void setup() {
  pinMode(HSV_1_SW ,INPUT_PULLUP);
  pinMode(HSV_2_SW ,INPUT_PULLUP);
  pinMode(HSV_S_SW ,INPUT_PULLUP);
  pinMode(HSV_V_SW ,INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(BGT_PIN, INPUT);

  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(bgt);

  // encoder = new RotaryEncoder(ENCODER_CLK, ENCODER_DT, RotaryEncoder::LatchMode::TWO03);
  encoder = new RotaryEncoder(ENCODER_CLK, ENCODER_DT, RotaryEncoder::LatchMode::FOUR3);
  // encoder = new RotaryEncoder(ENCODER_CLK, ENCODER_DT, RotaryEncoder::LatchMode::FOUR0);
  attachInterrupt(digitalPinToInterrupt(ENCODER_DT), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), checkPosition, CHANGE);

  // Serial.begin(9600);
}

void loop() {
  readEncoder();
  readEncoderBtn();

  EVERY_N_MILLISECONDS( 20 ) { displayLights(); }

  // Serial.print(digitalRead(HSV_1_SW));
  // Serial.print(digitalRead(HSV_2_SW));
  // Serial.print(hue);
  // Serial.print(hue2);
  // Serial.println(fx);
}