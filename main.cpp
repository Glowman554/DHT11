/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include <lcd.hpp>

const int t_tol_start = 2;
const int t_tol_pulse = 10;
software_lcd lcd;

struct DHT11 {
    DigitalInOut io;
    InterruptIn io_irq;
    Timer timer;

    bool first_time;
    uint64_t data;
    uint32_t cnt;
    bool eod;

    void pos_edge() {
        this->timer.reset();
    }

    void neg_edge() {
        uint32_t t_pulse_us = timer.elapsed_time().count();

        // Detecting 0 if the pulse width ranges around 25 us
        if (25 - t_tol_pulse <= t_pulse_us && t_pulse_us <= 30 + t_tol_pulse) {
            this->data = this->data << 1;
            this->cnt++;
        }

        // Detecting 1 if the pulse width ranges from 70 us
        else if (70 - t_tol_pulse <= t_pulse_us && t_pulse_us <= 70 + t_tol_pulse) {
            this->data = (this->data << 1) | 1;
            this->cnt++;
        }

        if (this->cnt >= 40) {
            this->io_irq.disable_irq();
            this->eod = true;
        }
    }

    enum DHT11Error {
        OK = 0,
        READ_TOO_OFTEN = 1,
        BUS_BUSY = 2,
        NOT_PRESENT = 3,
        NOT_READY = 4,
        CHKSUM_ERR = 5,
        WATCHDOG_ERR = 6,
    };

    struct DHT11Result {
        DHT11Error status;
        int temperature;
        int humidity;

        DHT11Result(DHT11Error status) {
            this->status = status;
        }

        DHT11Result(DHT11Error status, uint64_t data) {
            this->status = status;
            this->humidity = (data & 0xff00000000) >> 32;
            this->temperature = (data & 0x0000ff0000) >> 16;

            uint32_t chksum = ((data & 0xff00000000) >> 32) + ((data & 0x00ff000000) >> 24) + ((data & 0x0000ff0000) >> 16) + ((data & 0x000000ff00) >> 8);

            if (chksum != (data & 0x00000000ff)) {
                this->status = CHKSUM_ERR;
            }
        }
    };

    DHT11(PinName pin) : io(pin, PIN_INPUT, OpenDrain, 1), io_irq(pin) {
        this->io_irq.rise(callback(this, &DHT11::pos_edge));
        this->io_irq.fall(callback(this, &DHT11::neg_edge));
        this->io_irq.disable_irq();
        this->timer.start();
        this->first_time = true;
    }

    DHT11Result readData(void) {
        if (duration_cast<std::chrono::milliseconds>(this->timer.elapsed_time()).count() < 2000 && this->first_time == false) {
            this->timer.reset();
            return DHT11Result(READ_TOO_OFTEN);
        }

        this->data = 0;
        this->cnt = 0;
        this->eod = false;
        this->timer.reset();

        // Checking the data bus
        if (this->io == 0) {
            this->timer.reset();
            return DHT11Result(BUS_BUSY);
        }

        // Sending start signal, low signal for around 10 ms
        this->timer.reset();
        this->io.output();
        this->io = 0;
        while (duration_cast<std::chrono::milliseconds>(this->timer.elapsed_time()).count() < 20 + t_tol_start);
        this->io.input();
        this->io = 1;

        // Waiting for the start of the response signal
        this->timer.reset();
        do {
            if (this->timer.elapsed_time().count() > 100) {
                this->timer.reset();
                return DHT11Result(NOT_PRESENT);
            }
        } while (this->io == 1);

        // Wainting for the start of the ready signal
        this->timer.reset();
        do {
            if (this->timer.elapsed_time().count() > 100) {
                this->timer.reset();
                return DHT11Result(NOT_READY);
            }
        } while (this->io == 0);

        // Wainting for the end of the ready signal
        this->timer.reset();
        do {
            if (this->timer.elapsed_time().count() > 100) {
                this->timer.reset();
                return DHT11Result(WATCHDOG_ERR);
            }
        } while (this->io == 1);

        // Starting the pulse width sensing
        // by the use of interruptions
        this->io_irq.enable_irq();

        int wdt = 50;
        do {
            wait_us(100);
            if (wdt < 0) {
                this->timer.reset();
                return DHT11Result(WATCHDOG_ERR);
            }

            wdt--;
        } while (this->eod == false);

        this->timer.reset();
        this->first_time = false;
        return DHT11Result(OK, this->data);
    }
};

int main() {
    lcd.set_flags(LCD_CURSOR | LCD_CURSOR_BLINK);
    lcd.clear();
    DHT11 sens(PB_0);

    while (true) {

        char buf[0xff] = {0};
        DHT11::DHT11Result res = sens.readData();
        if (res.status != DHT11::OK) {
            sprintf(buf, "ERROR %d", res.status);
        } else {
            sprintf(buf, "%d C %d%% H", res.temperature, res.humidity);
        }

        lcd.clear();
        lcd.puts(buf);
        ThisThread::sleep_for(2000ms);
    }
}
