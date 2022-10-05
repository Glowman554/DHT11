/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

int segments[] = {
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00000111,
    0b01111111,
    0b01101111,
    0b01110111,
    0b01111100,
    0b00111001,
    0b01011110,
    0b01111001,
    0b01110001
};

PortOut output(PortC, 0xff);
DigitalOut seg1(PC_12);
DigitalOut seg2(PC_11);
bool curr = false;

int num = 0;


void timer() {
    curr = !curr;

    if (curr) {
        int seg2_num = num & 0b00001111;
        seg1 = false;
        seg2 = true;
        output = segments[seg2_num];
    } else {
        int seg1_num = (num & 0b11110000) >> 4;
        seg2 = false;
        seg1 = true;
        output = segments[seg1_num];
    }
}

int main() {
    Ticker ticker;
    ticker.attach(timer, 10ms);

    while (true) {
        ThisThread::sleep_for(200ms);
        num++;
    }
}
