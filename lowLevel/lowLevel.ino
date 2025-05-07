#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include "pwm_utils.h"
#include "timer_utils.h"
#include "sensor_utils.h"

int motorPins[4] = {26, 27, 25, 33};
const int triggerPins[5] = {22,19,18,17,4}; //LF LB RF RB BB
const int echoPins[5] = {23,21,5,16,2}; //LF LB RF RB BB
long distances[5];
int counterLeft;
int counterRight;
int initializePark;


void setup() {
    Serial.begin(115200);
    initHardwareTimer();

    for(int i=0;i< 5;i++) {
        setOutput(triggerPins[i]);
        distances[i] = 0;
    }

    counterLeft = 0;
    counterRight = 0;
    initializePark = 0;

    motorsInit(motorPins);
    carForward(100);
}

void loop() {
    for(int i=0;i< 5; i++) {
        distances[i] = measureDistance(triggerPins[i], echoPins[i]);
        Serial.print("sensor: ");
        Serial.print(i+1);
        Serial.print(" value: ");
        Serial.println(distances[i]);
    }

    if(initializePark == 0) {
        if(distances[0] > 23) {
            if(counterLeft >= 10) {
                initializePark = 1;
                Serial.println("parking left");
                delay(50);
            } else {
                counterLeft++;
            }
        } else if (distances[0] < 23) {
            counterLeft = 0;
            Serial.println("fjoidjfiods");
        }

        /*if(distances[2] > 23 && initializePark != 1) {
            if(counterRight >= 10) {
                initializePark = 2;
            } else {
                counterRight++;
            }
        } else if (!(distances[2] > 23 && initializePark != 1)) {
            counterRight = 0;
        }*/
    }

    if(initializePark == 1) {
        if(distances[1] < 23) {
            carStop();
            delay(50);
            carBackward(150);
            delay(50);
            carSpinLeft(570);
            carBackward(80);
            while(true) {
                distances[4] = measureDistance(triggerPins[4], echoPins[4]);
                if(distances[4] < 9)
                    break;
            }
            carStop();
            initializePark = 3;
        }
    }
}