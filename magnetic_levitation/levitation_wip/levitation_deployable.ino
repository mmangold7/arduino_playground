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

void setup() {
  // Start serial coms
  //Serial.begin(9600);

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
  int fieldStrength = analogRead(A0); // Read hall sensor value

  //if button is pressed, store value of field strength as magic value for floating position reference
  buttonValue = digitalRead(button);

  if (buttonValue == 0) {
    pressed = true;
    magicPosition = fieldStrength;
  }  

  int positionError = magicPosition - fieldStrength;
  int velocity = previousPositionError - positionError;

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
}
