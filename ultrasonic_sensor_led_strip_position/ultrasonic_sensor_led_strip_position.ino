#include <FastLED.h>

// How many leds are in the strip?
#define NUM_LEDS 193

// Data pin that led data will be written out over
#define DATA_PIN 4

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

// defines pins numbers
const int trigPin = 2;  //D4
const int echoPin = 0;  //D3

// defines variables
long duration;
int distance;

void setup() {
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
Serial.begin(9600); // Starts the serial communication
FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {
// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);

FastLED.clear();
if(distance - 2 < NUM_LEDS){
  leds[distance - 2] = CRGB::Blue;
  leds[distance - 1] = CRGB::Green;
  leds[distance] = CRGB::Red;
  leds[distance + 1] = CRGB::Green;
  leds[distance + 2] = CRGB::Blue;
}
FastLED.show();
}
