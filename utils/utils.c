/*
 * utils.c
 *
 *  Created on: Sep 29, 2018
 *      Author: josef
 */

#include <Arduino.h>
#include "utils.h"

void radioOn() {
	digitalWrite(SI_4432_SHDN, LOW);
}

void radioOff() {
	digitalWrite(SI_4432_SHDN, HIGH);
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
