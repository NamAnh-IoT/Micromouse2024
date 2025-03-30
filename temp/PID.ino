// Encoder pins
const int encoderPinA1 = 17; // Encoder trái
const int encoderPinB1 = 5;
const int encoderPinA2 = 22; // Encoder phải
const int encoderPinB2 = 23;

// Motor control pins
int IN3 = 2;  // Motor trái
int IN4 = 0;
int IN1 = 4;  // Motor phải
int IN2 = 16;

// PID constants
float Kp = 2.0;  // Proportional constant
float Ki = 0.01; // Integral constant
float Kd = 0.5;  // Derivative constant

// Target and encoder counts
volatile int encoder1Count = 0; // Encoder bên trái
volatile int encoder2Count = 0; // Encoder bên phải
const int target = 5000;        // Target cố định

// PID variables
float integral1 = 0, integral2 = 0;
float previousError1 = 0, previousError2 = 0;

void setup() {
  // Set up encoder pins
  pinMode(encoderPinA1, INPUT);
  pinMode(encoderPinB1, INPUT);
  pinMode(encoderPinA2, INPUT);
  pinMode(encoderPinB2, INPUT);

  attachInterrupt(digitalPinToInterrupt(encoderPinA1), encoder1ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinA2), encoder2ISR, CHANGE);

  // Set up motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Stop motors initially
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);

  Serial.begin(9600); // Debug
}

void loop() {
  // Compute PID for motor 1 (trái)
  
  // Debug info
  Serial.print("Encoder 1 Count: ");
  Serial.println(encoder1Count);
  Serial.print("Encoder 2 Count: ");
  Serial.println(encoder2Count);

  delay(50); // Small delay for stability
}


// Interrupt service routine for encoder 1
void encoder1ISR() {
  int stateA = digitalRead(encoderPinA1);
  int stateB = digitalRead(encoderPinB1);
  if (stateA == HIGH) {
    if (stateB == LOW) {
      encoder1Count++; // Clockwise
    } else {
      encoder1Count--; // Counter-clockwise
    }
  } else {
    if (stateB == HIGH) {
      encoder1Count++; // Clockwise
    } else {
      encoder1Count--; // Counter-clockwise
    }
  }
}

// Interrupt service routine for encoder 2
void encoder2ISR() {
  int stateA = digitalRead(encoderPinA2);
  int stateB = digitalRead(encoderPinB2);
  if (stateA == HIGH) {
    if (stateB == LOW) {
      encoder2Count++; // Clockwise
    } else {
      encoder2Count--; // Counter-clockwise
    }
  } else {
    if (stateB == HIGH) {
      encoder2Count++; // Clockwise
    } else {
      encoder2Count--; // Counter-clockwise
    }
  }
}
