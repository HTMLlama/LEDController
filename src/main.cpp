#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
#include <RotaryEncoder.h>

#define NUM_LEDS 900
#define LED_PIN 5
#define COLOR_ORDER RGB
#define BGT_PIN A4
int bgt = 20;
CRGB leds[NUM_LEDS];

#define SOLID 0
#define PCMR 1
#define SMALL 2
#define MED 3
#define LARGE 4
#define XL 5
#define XXL 6
#define COLOR_FADE 7
#define PRIDE 8
#define XMAS 9
#define SOLID2 10
#define MURICA 11
#define FX_TOTAL 12
int fx = SMALL;
bool isXmasSet = false;
int fadeHue = 100;

#define ENCODER_SW 4
#define ENCODER_DT 3
#define ENCODER_CLK 2
RotaryEncoder *encoder = nullptr;

#define HSV_1_SW 9
#define HSV_2_SW 10
#define HSV_S_SW 11
#define HSV_V_SW 12
int hue = 255;
int hue2 = 100;
int sat = 255;
int sat2 = 255;
int val = 255;
int val2 = 255;
int pcmrHue = 0;

// -----------------------------------------------------

int roller(int item) {
  if (item > 255) {
    item = 0;
  } else if (item < 0) {
    item = 255;
  }
  return item;
}

void lightSection(int blockSize) {
  for (size_t i = 0; i < NUM_LEDS; i+=blockSize*2) {
      for (size_t n = 0; n < blockSize; n++) {
        int indexA = i+n;
        int indexB = i+n+blockSize;
        if (indexA < NUM_LEDS) {
          leds[indexA] = CRGB().setHSV(hue, sat, val);
        }
        if (indexB < NUM_LEDS) {
          leds[indexB] = CRGB().setHSV(hue2, sat2, val2);;
        }
      } 
    }
}

void maybeBuildXmas() {
  if (!isXmasSet) {
    for (size_t i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB().setHSV(random(0, 255), random(200, 255), random(200, 255));
    }
    isXmasSet = true;
  }
}

void readEncoder() {
  int direction = (int)encoder->getDirection();


  if(digitalRead(HSV_S_SW) == LOW) {

    if (digitalRead(HSV_1_SW) == LOW) {
      sat = roller(sat + direction);
    }
    if (digitalRead(HSV_2_SW) == LOW) {
      sat2 = roller(sat2 + direction);
    }

  } else if (digitalRead(HSV_V_SW) == LOW) {

    if (digitalRead(HSV_1_SW) == LOW) {
      val = roller(val + direction);
    }
    if (digitalRead(HSV_2_SW) == LOW) {
      val2 = roller(val2 + direction);
    }

  } else {

    if (digitalRead(HSV_1_SW) == LOW) {
      hue = roller(hue + direction);
    }
    if (digitalRead(HSV_2_SW) == LOW) {
      hue2 = roller(hue2 + direction);
    }

  }
  
}

void readEncoderBtn() {
  if (digitalRead(ENCODER_SW) == LOW) {
    fx++;
    if (fx >= FX_TOTAL) {
      fx = 0;
    }
    isXmasSet = false;
    delay(500);
  }
  
}

void displayLights() {
  bgt = map(analogRead(BGT_PIN), 1023, 0, 0, 255);
  FastLED.setBrightness(bgt);
  switch (fx) {

    case PCMR:
        fill_rainbow(leds, NUM_LEDS, pcmrHue, 2);
        pcmrHue++;
        if (pcmrHue > 255) {
          pcmrHue = 0;
        }
        
      break;

    case SMALL:
      lightSection(8);
      break;
    
    case MED:
      lightSection(12);
      break;

    case LARGE:
      lightSection(18);
      break;

    case XL:
      lightSection(26);
      break;

    case XXL:
      lightSection(34);
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

    case XMAS:
      maybeBuildXmas();
      break;

    case SOLID2:
      for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB().setHSV(hue2, sat2, val2);
      }
      break;

    case COLOR_FADE:
      for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB().setHSV(fadeHue, 250, 250);
      }
      break;
    
    default:

      for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB().setHSV(hue, sat, val);
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

  maybeBuildXmas();

  // Serial.begin(9600);
}

void loop() {
  readEncoder();
  readEncoderBtn();

  EVERY_N_MILLISECONDS(20) { displayLights(); }
  EVERY_N_MINUTES(2) { fadeHue = roller(fadeHue + 1); }
}