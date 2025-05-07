#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include "pwm_utils.h"
#include "timer_utils.h"
#include "sensor_utils.h"
#include "BluetoothSerial.h"


int motorPins[4] = {26, 27, 25, 33};
const int triggerPins[5] = {22,19,18,17,4}; //LF LB RF RB BB
const int echoPins[5] = {23,21,5,16,2}; //LF LB RF RB BB
long distances[5];
int counterLeft;
int counterRight;
int initializePark;

char cmd;
BluetoothSerial serialBt;

void setup() {
    Serial.begin(115200);
    initHardwareTimer();

    for(int i=0;i< 5;i++) {
        setOutput(triggerPins[i]);
        distances[i] = 0;
    }

    serialBt.begin("ESP32-BT");

    counterLeft = 0;
    counterRight = 0;
    initializePark = -1;

    motorsInit(motorPins);
    //carForward(70);
}

void loop() {

    if(serialBt.available()){
        cmd = serialBt.read();
    }
    if(cmd=='1'){
        initializePark= 0;
        carForward(70);
    }else if (cmd == '0'){
        initializePark = -1;
        carStop();
    }




    for(int i=0;i< 5; i++) {
        distances[i] = measureDistance(triggerPins[i], echoPins[i]);
        Serial.print("sensor: ");
        Serial.print(i+1);
        Serial.print(" value: ");
        Serial.println(distances[i]);
    }

    if(initializePark == 0) {
        if(distances[0] > 23 && distances[1] > 23) {
            if(counterLeft >= 6) {
                initializePark = 1;
                Serial.println("parking left");
                delay(10);
            } else {
                counterLeft++;
                delay(2);
            }
        } else if (!(distances[0] > 23 && distances[1] > 23)) {
            counterLeft = 0;
        }

        if(distances[2] > 23 && distances[3] > 23) {
            if(counterRight >= 6) {
                initializePark = 2;
                Serial.println("parking right");
                delay(10);
            } else {
                counterRight++;
            }
        } else if (!(distances[2] > 23 && distances[3] > 23)) {
            counterRight = 0;
        }
    }

    else if(initializePark == 1) {
        carStop();
        delay(100);
        carSpinLeft(740);
        carBackward(60);
        delay(300);
        while(true) {
            if(measureDistance(triggerPins[4], echoPins[4]) < 2)
                break;
        }
        carStop();
        initializePark = 3;
    }
    
    else if(initializePark == 2) {
        carStop();
        delay(100);
        carSpinRight(740);
        carBackward(60);
        delay(300);
        while(true) {
            if(measureDistance(triggerPins[4], echoPins[4]) < 2)
                break;
        }
        carStop();
        initializePark = 3;
    }
}