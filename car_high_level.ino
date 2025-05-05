// Define trigger and echo pins
const int trigPins[5] = {18, 2, 22, 17, 12}; //LF LB RF RB BB
const int echoPins[5] = {19, 4, 21, 16, 14}; //LF LB RF RB BB

const int motorPwm[2] = {27,25};
const int motorEnable[2] = {26,33};
int go;
char stop;

// Store distances
long duration[5];
float distance[5];

void motorSpin(int power, int direction, int index) {
  if(direction == 1) {
    analogWrite(motorEnable[index], LOW);
    analogWrite(motorPwm[index], power);
  } else {
    analogWrite(motorEnable[index], power);
    analogWrite(motorPwm[index], LOW);
  }
}


void carForward() {
  motorSpin(80, 1, 0);
  motorSpin(80, 1, 1);
}

void carBackward() {
  motorSpin(80, 0, 0);
  motorSpin(80, 0, 1);
}

void carSpinRight(int time) {
  motorSpin(40, 1, 1);
  motorSpin(200, 1, 0);
  delay(time);
  carStop();
}

void motorOff(int index) {
    analogWrite(motorEnable[index], LOW);
    analogWrite(motorPwm[index], LOW);
}

void carStop() {
  motorOff(0);
  motorOff(1);
}

void setup() {
  Serial.begin(115200);
  go = 1;
  stop = 'n';
  // Set pin modes
  for (int i = 0; i < 5; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }
  for (int i=0;i < 2;i++) {
    pinMode(motorPwm[i], OUTPUT);
    pinMode(motorEnable[i], OUTPUT);
  }

  carBackward();
}

void loop() {
    if(stop == 'y') {
    } else {
    for(int i=0;i< 5; i++) {
    // Send 10us pulse to trigger
    digitalWrite(trigPins[i], LOW);
    delayMicroseconds(2);
    digitalWrite(trigPins[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPins[i], LOW);

    // Read echo time
    duration[i] = pulseIn(echoPins[i], HIGH, 30000); // Timeout at 30ms (max ~5m)

    // Calculate distance in cm
    distance[i] = duration[i] * 0.034 / 2;
    }

    if(distance[2] > 23) {
      delay(500);
      carStop();
      delay(500);
      carSpinRight(400);
      carForward();
      delay(80);
      carStop();
      stop = 'y';
    }
    }
}