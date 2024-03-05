#define hallSensor1 A12
#define hallSensor2 A15
#define hallSensor3 A14
#define hallSensor4 A13

#define pwmALeft 8
#define pwmARight 12
#define pwmBLeft 7
#define pwmBRight 6
#define pwmCLeft 5
#define pwmCRight 11
#define pwmDLeft 2
#define pwmDRight 3

#define pwmLEDA 4
#define pwmLEDB 9
#define pwmLEDC 10
#define pwmLEDD 13

#define enableALeft 22
#define enableARight 23
#define enableBLeft 24
#define enableBRight 25
#define enableCLeft 26
#define enableCRight 27
#define enableDLeft 28
#define enableDRight 29

#define button 44

#define potentiometer1 A12
#define potentiometer2 A11
#define potentiometer3 A10

int permanentMagnetVerticalMovementDirection = 0;
const uint32_t debounceTime = 100;
int previousButtonValue = 1;
bool buttonPressed = false;
int electroMagnetFieldDirection = 0;
int buttonValue = 1;

//int positionBuffer = 0;
int magicPosition1 = 185;
int magicPosition2 = 185;
int magicPosition3 = 185;
int magicPosition4 = 185;
int verticalMagicPosition = 185;

int previousPositionError1 = 0;
int previousPositionError2 = 0;
int previousPositionError3 = 0;
int previousPositionError4 = 0;
int previousVerticalPositionError = 0;

int previousVelocity1 = 0;
int previousVelocity2 = 0;
int previousVelocity3 = 0;
int previousVelocity4 = 0;
int previousVerticalVelocity = 0;

void analogWrite25k(int pin, int value)
{
  bool printDebugForAnalogWrite25k = false;
  //printDebugForAnalogWrite25k = true;

  if (printDebugForAnalogWrite25k) {
    Serial.print("analogWrite25k called with pin: ");
    Serial.print(pin);
    Serial.print(" and pwm value: ");
    Serial.print(value);
    Serial.println();
  }
  
  switch (pin) {
    //might be able to optimize this by using only 4 pins and sharing 2 for each motor driver? since only 1 pwm is active at a time per driver. but with the modules, both enables must be on, so not sure how to select right vs left.
    case 2:
      OCR3B = value;
      break;
    case 3:
      OCR3C = value;
      break;
    case 5:
      OCR3A = value;
      break;
    case 6:
      OCR4A = value;
      break;
    case 7:
      OCR4B = value;
      break;
    case 8:
      OCR4C = value;
      break;
    case 11:
      OCR1A = value;
      break;
    case 12:
      OCR1B = value;
      break;
    default:
      // no other pins will work
      break;
  }
}

int outputElectromagnetWaveform(int pin) {
  // get the time since arduino stared and use math function to calc wave from time
  unsigned long elapsedTime = millis();
  //int outputValue = 511 * sin(2.0 * M_PI * (elapsedTime / 1000.0) / 10.0) + 512;
  //Serial.println(outputValue);
  int outputValue = 60 * sin(2.0 * M_PI * (elapsedTime / 1000.0) / 5.0) + 220;
  //Serial.println(outputValue);
  analogWrite25k(pin, outputValue);
}

void outputSingleChannel(int positionError, int pwmLeftPin, int pwmRightPin, int enableLeftPin, int enableRightPin, int pwmLEDPin, int electroMagnetOutput, int LEDOutput) {
  if (positionError > 0) {
      analogWrite25k(pwmLeftPin, electroMagnetOutput);
      analogWrite25k(pwmRightPin, 0);
      analogWrite(pwmLEDPin, LEDOutput);
      digitalWrite(enableLeftPin, HIGH);
      digitalWrite(enableRightPin, HIGH);
    }
    if (positionError < 0) {
      analogWrite25k(pwmRightPin, electroMagnetOutput);
      analogWrite25k(pwmLeftPin, 0);
      analogWrite(pwmLEDPin, LEDOutput);
      digitalWrite(enableLeftPin, HIGH);
      digitalWrite(enableRightPin, HIGH);
    }
    if (positionError == 0) {
      analogWrite(pwmLEDPin, 0);
      digitalWrite(enableLeftPin, LOW);
      digitalWrite(enableRightPin, LOW);
    }
}

void setup() {
  // Start serial coms
  Serial.begin(9600);

  // Setup inputs and outputs
  pinMode(pwmALeft, OUTPUT);
  pinMode(pwmARight, OUTPUT);
  pinMode(pwmBLeft, OUTPUT);
  pinMode(pwmBRight, OUTPUT);
  pinMode(pwmCLeft, OUTPUT);
  pinMode(pwmCRight, OUTPUT);
  pinMode(pwmDLeft, OUTPUT);
  pinMode(pwmDRight, OUTPUT);
  pinMode(pwmLEDA, OUTPUT);
  pinMode(pwmLEDB, OUTPUT);
  pinMode(pwmLEDC, OUTPUT);
  pinMode(pwmLEDD, OUTPUT);
  pinMode(enableALeft, OUTPUT);
  pinMode(enableARight, OUTPUT);
  pinMode(enableBLeft, OUTPUT);
  pinMode(enableBRight, OUTPUT);
  pinMode(enableCLeft, OUTPUT);
  pinMode(enableCRight, OUTPUT);
  pinMode(enableDLeft, OUTPUT);
  pinMode(enableDRight, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  // Set initial permanentMagnetVerticalMovementDirection direction
  digitalWrite(enableALeft, LOW);
  digitalWrite(enableARight, LOW);
  digitalWrite(enableBLeft, LOW);
  digitalWrite(enableBRight, LOW);
  digitalWrite(enableCLeft, LOW);
  digitalWrite(enableCRight, LOW);
  digitalWrite(enableDLeft, LOW);
  digitalWrite(enableDRight, LOW);

  // Configure Timers 1, 3, 4 and 5 for PWM @ 25 kHz.
  // undo the configuration done by the Arduino core library to reset timers
  TCCR1A = TCCR1B = TCNT1 = TCCR3A = TCCR3B = TCNT3 = TCCR4A = TCCR4B = TCNT4 = TCCR5A = TCCR5B = TCNT5 = 0;
  // non-inverted PWM on channels A and B. mode 10: ph. correct PWM, TOP = ICR1, prescaler = 1.
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11);
  TCCR3A = _BV(COM3A1) | _BV(COM3B1) | _BV(WGM11);
  TCCR4A = _BV(COM4A1) | _BV(COM4B1) | _BV(WGM11);
  TCCR5A = _BV(COM5A1) | _BV(COM5B1) | _BV(WGM11);
  TCCR1B = TCCR3B = TCCR4B = TCCR5B = _BV(WGM13) | _BV(CS10);
  // TOP = 320. max value for timers to use for duty cycle in custom analogWrite
  ICR1 = 320;
}

void loop() {
  // Read hall sensors values
  int fieldStrength1 = analogRead(hallSensor1);
  int fieldStrength2 = analogRead(hallSensor2);
  int fieldStrength3 = analogRead(hallSensor3);
  int fieldStrength4 = analogRead(hallSensor4);
  double averageFieldStrength = (fieldStrength1 + fieldStrength2 + fieldStrength3 + fieldStrength4) / 4.0;

  // Read potentiometer values
  int potentiometer1Value = analogRead(potentiometer1);
  int potentiometer2Value = analogRead(potentiometer2);
  int potentiometer3Value = analogRead(potentiometer3);

  bool printDebugForFieldStrength = false;
  //printDebugForFieldStrength = true;

  if (printDebugForFieldStrength) {
    Serial.println("fieldStrength1: ");
    Serial.print(fieldStrength1);
    Serial.println("fieldStrength2: ");
    Serial.print(fieldStrength2);
    Serial.println("fieldStrength3: ");
    Serial.print(fieldStrength3);
    Serial.println("fieldStrength4: ");
    Serial.print(fieldStrength4);
    Serial.print("averageFieldStrength: ");
    Serial.println(averageFieldStrength);
    Serial.println();
  }

  //if button is pressed, store values of field strengths as "magic" values for floating position reference
  buttonValue = digitalRead(button);

  if (buttonValue == 0) {
    buttonPressed = true;
    magicPosition1 = fieldStrength1;
    magicPosition2 = fieldStrength2;
    magicPosition3 = fieldStrength3;
    magicPosition4 = fieldStrength4;
    verticalMagicPosition = averageFieldStrength;
    Serial.println("Magic positions set");
  }

  int positionError1 = magicPosition1 - fieldStrength1;
  int positionError2 = magicPosition2 - fieldStrength2;
  int positionError3 = magicPosition3 - fieldStrength3;
  int positionError4 = magicPosition4 - fieldStrength4;

  int verticalPositionError = verticalMagicPosition - averageFieldStrength;

  int velocity1 = previousPositionError1 - positionError1;
  int velocity2 = previousPositionError2 - positionError2;
  int velocity3 = previousPositionError3 - positionError3;
  int velocity4 = previousPositionError4 - positionError4;

  //int verticalVelocity = previousVerticalPositionError - verticalPositionError;
  int verticalVelocity = 0;

  //int acceleration = previousVerticalVelocity - velocity;
  //Serial.println(acceleration);

  int constantFieldOffset = 0; //170 in working downward levitation project

  if (buttonPressed) { //&& abs(verticalPositionError) > 1
    int calculatedOutput1 = constantFieldOffset + velocity1 + verticalVelocity;
    int calculatedOutput2 = constantFieldOffset + velocity2 + verticalVelocity;
    int calculatedOutput3 = constantFieldOffset + velocity3 + verticalVelocity;
    int calculatedOutput4 = constantFieldOffset + velocity4 + verticalVelocity;
    
    int LED1Brightness = map(calculatedOutput1, 0, 320, 0, 255);
    int LED2Brightness = map(calculatedOutput2, 0, 320, 0, 255);
    int LED3Brightness = map(calculatedOutput3, 0, 320, 0, 255);
    int LED4Brightness = map(calculatedOutput4, 0, 320, 0, 255);

    bool printDebugForOutput = false;
    //printDebugForOutput = true;

    if (printDebugForOutput) {
      Serial.print("Calculated outputs: 1: ");
      Serial.print(calculatedOutput1);
      Serial.print(", 2: ");
      Serial.print(calculatedOutput2);
      Serial.print(", 3: ");
      Serial.print(calculatedOutput3);
      Serial.print(", 4: ");
      Serial.print(calculatedOutput4);
      Serial.println();
    }

    outputSingleChannel(positionError1, pwmCLeft, pwmCRight, enableCLeft, enableCRight, pwmLEDC, calculatedOutput1, LED1Brightness);
    outputSingleChannel(positionError2, pwmBLeft, pwmBRight, enableBLeft, enableBRight, pwmLEDB, calculatedOutput2, LED2Brightness);
    outputSingleChannel(positionError3, pwmDLeft, pwmDRight, enableDLeft, enableDRight, pwmLEDD, calculatedOutput3, LED3Brightness);
    outputSingleChannel(positionError4, pwmALeft, pwmARight, enableALeft, enableARight, pwmLEDA, calculatedOutput4, LED4Brightness);
  }
    
  previousPositionError1 = positionError1;
  previousPositionError2 = positionError2;
  previousPositionError3 = positionError3;
  previousPositionError4 = positionError4;
  previousVerticalPositionError = verticalPositionError;
  //previousVerticalVelocity = velocity;

  // Use elapsed time to generated a waveform to control EM strength for debug purposes
  //outputElectromagnetWaveform(pwmALeft);
  //outputElectromagnetWaveform(pwmBLeft);
  //outputElectromagnetWaveform(pwmCLeft);
  //outputElectromagnetWaveform(pwmDLeft);
}
