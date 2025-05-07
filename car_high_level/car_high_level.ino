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





#include <Arduino.h>

// === GPIO Configuration ===
const uint8_t triggerPins[5] = {18,27,22,17,33};
const uint8_t echoPins[5]    = {19,26,21,16,32};

#define GPIO_ENABLE_REG      (*((volatile uint32_t *)0x3FF44020))
#define GPIO_ENABLE1_REG     (*((volatile uint32_t *)0x3FF4402C))

#define GPIO_OUT_W1TS_REG    (*((volatile uint32_t *)0x3FF44008))
#define GPIO_OUT_W1TC_REG    (*((volatile uint32_t *)0x3FF4400C))

#define GPIO_OUT1_W1TS_REG   (*((volatile uint32_t *)0x3FF44014))
#define GPIO_OUT1_W1TC_REG   (*((volatile uint32_t *)0x3FF44018))

#define GPIO_IN_REG          (*((volatile uint32_t *)0x3FF4403C))
#define GPIO_IN1_REG         (*((volatile uint32_t *)0x3FF44040))

#define IO_MUX_GPIO2_REG   (*((volatile uint32_t *)0x3FF49040))


void setOutput(uint8_t gpio) {
  if (gpio < 32)
    GPIO_ENABLE_REG |= (1 << gpio);
  else
    GPIO_ENABLE1_REG |= (1 << (gpio - 32));
}

void triggerPulse(uint8_t gpio) {
  if (gpio < 32) {
    GPIO_OUT_W1TC_REG = (1 << gpio);
    delayMicroseconds(10);
    GPIO_OUT_W1TS_REG = (1 << gpio);
    delayMicroseconds(10);
    GPIO_OUT_W1TC_REG = (1 << gpio);
  } else {
    uint8_t bit = gpio - 32;
    GPIO_OUT1_W1TC_REG = (1 << bit);
    delayMicroseconds(10);
    GPIO_OUT1_W1TS_REG = (1 << bit);
    delayMicroseconds(10);
    GPIO_OUT1_W1TC_REG = (1 << bit);
  }
}

bool readInput(uint8_t gpio) {
  if (gpio < 32)
    return GPIO_IN_REG & (1 << gpio);
  else
    return GPIO_IN1_REG & (1 << (gpio - 32));
}

long measureDistance(uint8_t trigPin, uint8_t echoPin) {
  triggerPulse(trigPin);

  // Wait for echo pin to go HIGH (start)
  unsigned long start = micros();
  while (!readInput(echoPin)) {
    if (micros() - start > 30000) return -1; // Timeout
  }

  unsigned long echo_start = micros();

  // Wait for echo pin to go LOW (end)
  while (readInput(echoPin)) {
    if (micros() - echo_start > 30000) return -1; // Timeout
  }

  unsigned long duration = micros() - echo_start;
  return duration * 0.0343 / 2; // Distance in cm
}

void setup() {
  Serial.begin(115200);

  // Set all trigger pins as output
/*   for (int i = 0; i < 5; i++) {
    setOutput(triggerPins[i]);
  } */

  IO_MUX_GPIO2_REG = 0;

  // 2) Enable GPIO2 as an output
  GPIO_ENABLE_REG |= (1 << 2);

  // 3) Drive GPIO2 HIGH
  GPIO_OUT_W1TS_REG = (1 << 2);

  Serial.println("Starting 5 Ultrasonic Sensors (Low-Level)");
}

void loop() {
/*   for (int i = 0; i < 5; i++) {
    long dist = measureDistance(triggerPins[i], echoPins[i]);
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    if (dist >= 0)
      Serial.print(dist);
    else
      Serial.print("Timeout");
    Serial.println(" cm");
  }

  Serial.println("----------------------"); */


  delay(1000); // Wait 1 second between measurements

  GPIO_OUT_W1TS_REG = (1 << 2);

  delay(1000);

  GPIO_OUT_W1TC_REG = (1 << 2);

  



}