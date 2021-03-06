/*
   Copyright 2021 Marc Ketel

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License
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