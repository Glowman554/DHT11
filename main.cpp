/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"


DigitalOut led(LED1);

int main() {

    while (true) {
        ThisThread::sleep_for(200ms);
        led = !led;
    }
}
