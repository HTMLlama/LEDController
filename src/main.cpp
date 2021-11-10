#include <Arduino.h>
#include <FastLED.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <RotaryEncoder.h>

#define NUM_LEDS 60
#define LED_PIN 6
#define COLOR_ORDER RGB
int bgt = 20;
CRGB leds[NUM_LEDS];

#define ENCODER_SW 4
#define ENCODER_DT 3
#define ENCODER_CLK 2
RotaryEncoder *encoder = nullptr;

int hue = 100;

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
  // int pos = 0;
  // int newPos = encoder->getPosition();
  // if (pos != newPos) {
  //   Serial.print("pos:");
  //   Serial.print(newPos);
  //   Serial.print(" dir:");
  //   Serial.println((int)(encoder->getDirection()));
  //   pos = newPos;
  // }
  return newItem;
}

void displayLights() {
  for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB().setHSV(hue, 255, 255);
  }
  FastLED.show();
}

// -----------------------------------------------------

void checkPosition() {
  encoder->tick(); // just call tick() to check the state.
}

void setup() {
  delay(1000);
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(bgt);

  // encoder = new RotaryEncoder(ENCODER_CLK, ENCODER_DT, RotaryEncoder::LatchMode::TWO03);
  // encoder = new RotaryEncoder(ENCODER_CLK, ENCODER_DT, RotaryEncoder::LatchMode::FOUR3);
  encoder = new RotaryEncoder(ENCODER_CLK, ENCODER_DT, RotaryEncoder::LatchMode::FOUR0);
  attachInterrupt(digitalPinToInterrupt(ENCODER_DT), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), checkPosition, CHANGE);

  Serial.begin(9600);
}

void loop() {
  hue = readEncoder(hue, true);
  displayLights();

  Serial.println(hue);
}