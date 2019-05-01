// external libraries
#include <avr/wdt.h>
#include <DallasTemperature.h>
#include <RHReliableDatagram.h>
#include <LowPower.h>
#include <OneWire.h>
#if defined DRIVER_RF22
#include <RH_RF22.h>
#elif defined DRIVER_RF95
#include <RH_RF95.h>
#endif
#include <SPI.h>

//#define RAM_DEBUG

// project includes
#include "utils/utils.h"
#include "utils/Measurement.h"
#include "utils/Config.h"
#include "Streaming.h"

#if defined DRIVER_RF22
RH_RF22 radioDriver;
#elif defined DRIVER_RF95
RH_RF95 radioDriver;
#endif

RHReliableDatagram radio(radioDriver, CLIENT_ADDRESS);
Measurement measurement;
uint8_t outBuffer[MAX_MESSAGE_LEN];
uint8_t inBuffer[MAX_MESSAGE_LEN];
unsigned long sentOk = 0;
unsigned long sentFailed = 0;
int sentFailedPower = 0;
uint8_t sleepCycles = DEFAULT_SLEEP_CYCLES;

Config *config;

// ********************
// *** DECLARATIONS ***
// ********************
void measure(MeasurementType type) {
    measurement.doMeasure(type);
}

void printStat() {
	Serial << INFO_HEADER << F("ok/failed: ") << sentOk << "/" << sentFailed << endl;
}

void sendRF(String message) {
	radioDriver.setModeIdle();
    Serial << INFO_HEADER << F("Sending over RF: ") << message << endl;

    if (message.length() < MAX_MESSAGE_LEN) {
        strcpy((char *) outBuffer, (const char*) message.c_str());
    } else {
        Serial << ERROR_HEADER << F("Sending over RF failed - message too long!") << endl;
        return;
    }

    if (radio.sendtoWait(outBuffer, sizeof(outBuffer), SERVER_ADDRESS)) {
        sentOk++;
        Serial << INFO_HEADER << F("send OK") << endl;
        goodBlink();

        Serial << INFO_HEADER << F("waiting for response ...") << endl;

        uint8_t len = sizeof(inBuffer);
        uint8_t from;
        if (radio.recvfromAckTimeout(inBuffer, &len, 800, &from)) {
        	Serial << INFO_HEADER << F("Response received.") << endl;

        	sleepCycles = config->getTxPeriod();
        	sleepCycles = sleepCycles > MAX_SLEEP_CYCLES ? MAX_SLEEP_CYCLES : sleepCycles;
        	sleepCycles = sleepCycles < 1 ? 1 : sleepCycles;
        	Serial << INFO_HEADER << F("Setting sleepCycles to: ") << sleepCycles << endl;

        	uint8_t power = config->getTxPower();
        	// limit settings
#ifdef DRIVER_RF22
        	power = power > RH_RF22_TXPOW_20DBM ? RH_RF22_TXPOW_20DBM : power;
#elif defined DRIVER_RF95
        	power = power > 25 ? 25 : power;
#endif
        	Serial << INFO_HEADER << F("Setting txPower to: ") << power << endl;
        	radioDriver.setTxPower(power);
#ifdef RAM_DEBUG
        	Serial << freeRam() << endl;
#endif
        } else {
        	Serial << WARN_HEADER << F("Waiting for response timed out") << endl;
        }

    } else {
        sentFailed++;
        sentFailedPower++;
        if (sentFailedPower > SENT_FAILED_THRESHOLD) {
        	Serial << WARN_HEADER << F("sentFailedPower > ") << SENT_FAILED_THRESHOLD << endl;
        	Serial << WARN_HEADER << F("setting power to default: ") << DEFAULT_TX_POW << endl;
        	radioDriver.setTxPower(DEFAULT_TX_POW);
        	sentFailedPower = 0;
        }
        Serial << WARN_HEADER << F("send FAILED") << endl;
        badBlink();
#ifdef RAM_DEBUG
        Serial << freeRam() << endl;
#endif
    }
#ifdef RAM_DEBUG
    Serial << freeRam() << endl;
#endif
    radioDriver.sleep();
}

void sendRFStat() {
	String message;
	message = String(MQTT_STAT_OK_TOPIC) + " ";
	message += sentOk;
	sendRF(message);

	message = String(MQTT_STAT_ERR_TOPIC) + " ";
	message += sentFailed;
	sendRF(message);
}

void doAndSendAllMeasurements() {
	Serial << F("-----") << endl;

#ifdef DS18B20_ENABLED
	if (dsOk) {
		measure(MeasurementType::dsTemp);
		wdt_reset();
	}
#endif
#ifdef DHT22_ENABLED
	if (dhOk) {
		measure(MeasurementType::dhTemp);
		wdt_reset();
		measure(MeasurementType::dhHum);
		wdt_reset();
	}
#endif
#ifdef SHT_ENABLED
	if (shtOk) {
		measure(MeasurementType::shtTemp);
		wdt_reset();
		measure(MeasurementType::shtHum);
		wdt_reset();
	}
#endif
#ifdef HTU21D_ENABLED
	measure(MeasurementType::htuTemp);
	wdt_reset();
	measure(MeasurementType::htuHum);
	wdt_reset();
#endif
	measure(MeasurementType::battVoltage);
	wdt_reset();

	String measurementContent =
#if defined BATT_PIN
			String(measurement.getBattVoltage()) + "/" +
#else
			"0/" +
#endif
			String(sentOk) + "/" +
			String(sentFailed) + "/" +
#ifdef DS18B20_ENABLED
			String(measurement.getDsTemp()) + "/" +
#else
			"0/" +
#endif
#ifdef DHT22_ENABLED
			String(measurement.getDhTemp()) + "/" +
			String(measurement.getDhHum()) + "/" +
#else
			"0/0/" +
#endif
#ifdef SHT_ENABLED
			String(measurement.getShtTemp()) + "/" +
			String(measurement.getShtHum()) + "/" +
#else
			"0/0/" +
#endif
#ifdef HTU21D_ENABLED
			String(measurement.getHtuTemp()) + "/" +
			String(measurement.getHtuHum());
#else
			"0/0";
#endif

	sendRF(measurementContent);
	wdt_reset();

	printStat();
	wdt_reset();
#ifdef RAM_DEBUG
	Serial << freeRam() << endl;
#endif
	delay(100);
}

void initialSendingLoop() {
	for (int i=0; i<INITIAL_LOOP_CYCLES; i++) {
		Serial << endl << INFO_HEADER << F("Initial loop ") << i << F("/") << INITIAL_LOOP_CYCLES << endl;
		doAndSendAllMeasurements();
		delay(5000);
	}
}

// ********************
// **** EXECUTION *****
// ********************
void setup()
{
	config = new Config(inBuffer);

    Serial.begin(SERIAL_BAUD);
    Serial << endl << endl << INFO_HEADER << F("Sensor rf22 start ...") << endl;
#ifdef RAM_DEBUG
    Serial << freeRam() << endl;
#endif

    // LED
    pinMode(LED_PIN, OUTPUT);
    initBlink();

    // watchdog
    wdt_reset();
	Serial << INFO_HEADER << "enabling WDTO_8S" << endl;
	wdt_enable(WDTO_8S);

    // init measurement
    if (!measurement.begin()) {
        Serial << ERROR_HEADER << F("measurement init failed! Stopping.") << endl;
    } else {
        Serial << INFO_HEADER << F("measurement init ok ...") << endl;
    }

    // init radio
	pinMode(RADIO_SHDN, OUTPUT);
	radioOff();
	radioOn();
	delay(200);
    if (!radio.init()) {
        Serial << ERROR_HEADER << F("radio init failed! Stopping.") << endl;
        while(1);
    } else {
        Serial << INFO_HEADER << F("radio init ok ...") << endl;
    }

    radioDriver.setTxPower(DEFAULT_TX_POW);

    delay(200);
    wdt_reset();

    // initialSendingLoop();
}

void loop()
{
	wdt_reset();
	wdt_enable(WDTO_8S);
	static unsigned char sleepCounter = 0;
	if (sleepCounter%sleepCycles == 0) {
		sleepCounter = 0;
		doAndSendAllMeasurements();
	}
	sleepCounter++;

	wdt_reset();
	wdt_disable();
	LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

#ifdef RAM_DEBUG
int freeRam()
{
     extern int __heap_start, *__brkval;
     int v;
     return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
#endif
