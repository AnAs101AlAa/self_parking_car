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
  for (int i = 0; i < 5; i++) {
    setOutput(triggerPins[i]);
  }

  Serial.println("Starting 5 Ultrasonic Sensors (Low-Level)");
}

void loop() {
  for (int i = 0; i < 5; i++) {
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

  Serial.println("----------------------");

}
