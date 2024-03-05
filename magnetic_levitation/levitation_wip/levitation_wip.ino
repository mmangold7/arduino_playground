#define enB 9
#define in3 6
#define in4 7
#define button 4

int movementDirection = 0;
const uint32_t debounceTime = 100;
int previousButtonValue = 1;
bool pressed = false;
int electroMagnetDirection = 0;

//int positionBuffer = 0;
int magicPosition = 185;
int buttonValue = 1;
int previousPositionError = 0;
int previousVelocity = 0;

void analogWrite25k(int pin, int value)
{
    switch (pin) {
        case 9:
            OCR1A = value;
            break;
        case 10:
            OCR1B = value;
            break;
        default:
            // no other pin will work
            break;
    }
}

int outputElectromagnetWaveform() {
  //get the time since arduino stared and use math function to calc wave from time
  unsigned long elapsedTime = millis();
  //int outputValue = 511 * sin(2.0 * M_PI * (elapsedTime / 1000.0) / 10.0) + 512;
  //Serial.println(outputValue);
  int outputValue = 60 * sin(2.0 * M_PI * (elapsedTime / 1000.0) / 5.0) + 220;
  //Serial.println(outputValue);
  analogWrite25k(enB, outputValue);
}

void setup() {
  // Start serial coms
  Serial.begin(9600);

  // Setup inputs and outputs
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  // Set initial movementDirection direction
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  // Configure Timer 1 for PWM @ 25 kHz.
  TCCR1A = 0;           // undo the configuration done by...
  TCCR1B = 0;           // ...the Arduino core library
  TCNT1  = 0;           // reset timer
  TCCR1A = _BV(COM1A1)  // non-inverted PWM on ch. A
         | _BV(COM1B1)  // same on ch; B
         | _BV(WGM11);  // mode 10: ph. correct PWM, TOP = ICR1
  TCCR1B = _BV(WGM13)   // ditto
         | _BV(CS10);   // prescaler = 1
  ICR1   = 320;         // TOP = 320

}

void loop() {

  // My own button logic, not been using this anymore
  //flip the bool any time the button goes low (aka is pressed, since it's set to be high by default) and the button state has changed (i.e. isn't still low from current press and hold, or still high from being high by default)
  //int buttonValue = digitalRead(button);
  //if(buttonValue != previousButtonValue && buttonValue == 0){
  //  delay(debounceTime);
  //  buttonToggle = !buttonToggle;
  //}
  //previousButtonValue = buttonValue;

  // Button logic I got from others' code
  // Read button - Debounce
  /*
  if (digitalRead(button) == false) {
    pressed = !pressed;
  }
  while (digitalRead(button) == false){
    delay(20);
  }
  
  // If button is pressed - change rotation direction
  if (pressed == true  & electroMagnetDirection == 0) {
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    electroMagnetDirection = 1;
    delay(20);
  }
  // If button is pressed - change rotation direction
  if (pressed == false & electroMagnetDirection == 1) {
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    electroMagnetDirection = 0;
    delay(20);
  }
  */
  
  int fieldStrength = analogRead(A0); // Read hall sensor value
  //Serial.println(fieldStrength);

  //if button is pressed, store value of field strength as magic value for floating position reference
  buttonValue = digitalRead(button);

  if (buttonValue == 0) {
    pressed = true;
    magicPosition = fieldStrength;
  }  

  int positionError = magicPosition - fieldStrength;
  int velocity = previousPositionError - positionError;
  //int acceleration = previousVelocity - velocity;
  //Serial.println(acceleration);

  if(positionError > 0){
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }

  if(positionError < 0){
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  }

  if(pressed ){ //&& abs(positionError) > 1
    analogWrite25k(enB, 170 + velocity);
  }

  previousPositionError = positionError;
  //previousVelocity = velocity;

  // Use elapsed time to generated a waveform to controll EM strength
  //outputElectromagnetWaveform();

  // Use potentiometer to controll EM strength
  //int potValue = analogRead(A1); // Read potentiometer value
  //int pwmOutput = map(potValue, 0, 1023, 0 , 320); // Map the potentiometer value from 0 to 320
  //analogWrite25k(enB, pwmOutput);

  // Normal arduino analog output mapped from potentiometer value
  //int pwmOutput = map(potValue, 0, 1023, 0 , 255); // Map the potentiometer value from 0 to 255
  //analogWrite(enB, pwmOutput); // Send PWM signal to L298N enBble pin

  //retrieve previous position from memory
  //int previousPosition = positionBuffer;

  //save new position to memory for later use, now that you've already retrieved the old value
  //positionBuffer = fieldStrength;

  //int currentVelocity = fieldStrength - previousPosition;

  ///////////int positionError = magicPosition - fieldStrength;
  //int positionError = 10;

  //int counterFieldStrenth = positionError;

  //Serial.println(positionError);

  //reverse counter-field depending on if the target is located too close or far from the sensor
  //if (positionError < 0) {
    //reverse the counter-field direction
  //  digitalWrite(in1, HIGH);
  //  digitalWrite(in2, LOW);

  //  movementDirection = 0;

    //flip the sign to positive so it can be mapped properly for intensity
  //  positionError = -positionError;
  //} else {
    //reverse the counter-field direction
   // digitalWrite(in1, LOW);
  //  digitalWrite(in2, HIGH);

   // movementDirection = 1;
  //}

  //if (false) {
    //int pwmOutput = 255;
   // int pwmOutput = map(positionError, 0, 3, 0 , 100); // Map the sensor value from 0 to 255
   // analogWrite(enB, pwmOutput); // Send PWM signal to L298N enBble pin
  //}

  //guess at good max value for sensor range, needs revising
  //int pwmOutput = map(positionError, 0, 100, 0 , 255); // Map the sensor value from 0 to 255
  //analogWrite(enB, pwmOutput); // Send PWM signal to L298N enBble pin

  //todo: account for the counter-field affecting the sensor. probably could have it calibrate itself by trying different intensities and recording the resulting field. could either use those recorded values directly as new reference points, or could derive a function from it to use later. especially if that's too much memory.

  //direct-feed velocity into field intensity. probably not a good idea, velocity technically independent of relative position

  //reverse counter-field depending on if the target is moving towards or away from the sensor
  //if(currentVelocity < 0){
  //  digitalWrite(in3, LOW);
  //  digitalWrite(in4, HIGH);
  //  rotDirection = 0;
    //delay(20);
  //}

  //int pwmOutput = map(currentVelocity, 0, 1023, 0 , 255); // Map the sensor value from 0 to 255
  //analogWrite(enB, pwmOutput); // Send PWM signal to L298N enBble pin
}
