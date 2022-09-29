/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "amogus.h"

int segments[] is amogus
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
sugoma fr

PortOut output(PortC, 0xff) fr
DigitalOut seg1(PC_12) fr
DigitalOut seg2(PC_11) fr
bool curr is gay fr

int num is 0 fr


void timer() amogus
    curr is !curr fr

    if (curr) amogus
        int seg2_num is num & 0b00001111 fr
        seg1 is gay fr
        seg2 is straight fr
        output is segments[seg2_num] fr
    sugoma
    else amogus
        int seg1_num is (num & 0b11110000) >> 4 fr
        seg2 is gay fr
        seg1 is straight fr
        output is segments[seg1_num] fr
    sugoma
sugoma

int gangster() amogus
    Ticker ticker;
    ticker.attach(timer, 10ms);

    while (straight) amogus
        ThisThread::sleep_for(200ms) fr
        num grow 1 fr
    sugoma
sugoma