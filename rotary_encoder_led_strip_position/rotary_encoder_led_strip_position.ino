#include <FastLED.h>

// How many leds are in the strip?
#define NUM_LEDS 193

// Data pin that led data will be written out over
#define DATA_PIN 6

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

int val;
int encoder0PinA = 3;
int encoder0PinB = 4;
int encoder0Pos = 3;
int encoder0PinALast = LOW;
int n = LOW;

void setup() {
  pinMode (encoder0PinA, INPUT);
  pinMode (encoder0PinB, INPUT);
  Serial.begin (9600);
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {
  n = digitalRead(encoder0PinA);
  if ((encoder0PinALast == LOW) && (n == HIGH)) {
    if (digitalRead(encoder0PinB) == LOW) {
      encoder0Pos = encoder0Pos - 10;
    } else {
      encoder0Pos = encoder0Pos + 10;
    }

    if(encoder0Pos >= NUM_LEDS){
      encoder0Pos = encoder0Pos - NUM_LEDS;
    }

    if(encoder0Pos < 0){
      encoder0Pos = encoder0Pos + NUM_LEDS;
    }
    
    Serial.print (encoder0Pos);
    Serial.print ("/");
  }
  encoder0PinALast = n;

  FastLED.clear();
  leds[encoder0Pos] = CRGB::Green;
  leds[encoder0Pos + 1] = CRGB::Red;
  leds[encoder0Pos - 1] = CRGB::Red;
  leds[encoder0Pos + 2] = CRGB::Blue;
  leds[encoder0Pos - 2] = CRGB::Blue;
  FastLED.show();
}
