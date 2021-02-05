/*
    Copyright (C) 2021 Marc Ketel
    This file is part of Reset-timeout. <https://github.com/atoomnetmarc/Reset-timeout>.

    Reset-timeout is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Reset-timeout is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Reset-timeout.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include <avr/sleep.h>

//PB4
#define PIN_RESET 4

//PB3
#define PIN_DTR 3

//PB1
#define LED_PIN 1

//Reset will be ignored after this many units of time (s)
#define RESET_TIMEOUT 120u

void setup() {
  pinMode(PIN_DTR, INPUT);
  pinMode(PIN_RESET, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}

void loop() {

  static uint64_t timeout = millis();
  static uint8_t lastpinstate = 0;

  uint8_t newpinstate = digitalRead(PIN_DTR);

  //Do thing on HIGH to LOW transition
  if (lastpinstate == HIGH && newpinstate == LOW)
  {
    //Trigger reset.
    pinMode(PIN_RESET, OUTPUT);
    digitalWrite(PIN_RESET, LOW);
    delayMicroseconds(50);

    //Release reset.
    digitalWrite(PIN_RESET, HIGH);
    delayMicroseconds(10);
    pinMode(PIN_RESET, INPUT_PULLUP);
    timeout = millis();
  }

  lastpinstate = newpinstate;

  //After a while, ignore DTR changes.
  if (millis() - timeout > RESET_TIMEOUT * 1000)
  {
    digitalWrite(LED_PIN, LOW);
    wdt_disable();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_cpu();
    //Only reset or power cycle will reset CPU.
  }


}