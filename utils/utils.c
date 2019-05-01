/*
 * utils.c
 *
 *  Created on: Sep 29, 2018
 *      Author: josef
 */

#include <Arduino.h>
#include "utils.h"

void radioOn() {
#ifdef DRIVER_RF22
	digitalWrite(RADIO_SHDN, LOW);
#elif defined DRIVER_RF95
	digitalWrite(RADIO_SHDN, HIGH);
#endif
}

void radioOff() {
#ifdef DRIVER_RF22
	digitalWrite(RADIO_SHDN, HIGH);
#elif defined DRIVER_RF95
	digitalWrite(RADIO_SHDN, LOW);
#endif
}

void initBlink() {
    for (int i=0; i<3; i++) {
        for (int j=0; j<10; j++) {
            digitalWrite(LED_PIN, HIGH);
			delay(20);
            digitalWrite(LED_PIN, LOW);
            delay(20);
		}
        delay(500);
    }
}

void goodBlink() {
	digitalWrite(LED_PIN, HIGH);
	delay(50);
	digitalWrite(LED_PIN, LOW);
}

void badBlink() {
	for (int i=0; i<3; i++) {
		digitalWrite(LED_PIN, HIGH);
		delay(50);
		digitalWrite(LED_PIN, LOW);
		delay(100);
	}
}
