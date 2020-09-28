#ifndef UTILS_MEASUREMENT_H_
#define UTILS_MEASUREMENT_H_

#include "Arduino.h"

#define ADC_BATT_CALIB        	0.00605

#define MQTT_DS_TEMPERATURE 	F("rf/0/dst")
#define MQTT_DH_TEMPERATURE 	F("rf/0/dht")
#define MQTT_DH_HUMIDITY    	F("rf/0/dhh")
#define MQTT_SHT_TEMPERATURE 	F("rf/0/sht")
#define MQTT_SHT_HUMIDITY    	F("rf/0/shh")
#define MQTT_BAT_VOLTAGE    	F("rf/0/bat")

// DS18B20
//#define DS18B20_ENABLED
#ifdef DS18B20_ENABLED
#define DS_VCC_PIN   7
#define DS_DATA_PIN  8
#define DS_GND_PIN   9
#endif

// DHT22
//#define DHT22_ENABLED
#ifdef DHT22_ENABLED
#define DH_VCC_PIN   A2
#define DH_DATA_PIN  A4
#define DH_GND_PIN   A3
#endif

// SHT
//#define SHT_ENABLED
#ifdef SHT_ENABLED
#define SHT_VCC_PIN  A2
#define SHT_GND_PIN  A3
#endif

// HTU21D
#define HTU21D_ENABLED

// ADC
#define BATT_PIN     A0

enum MeasurementType {
    dsTemp,
    dhTemp,
    dhHum,
	shtTemp,
	shtHum,
	htuTemp,
	htuHum,
    battVoltage
};

class Measurement {
public:
    Measurement();

    bool begin();
    float doMeasure(MeasurementType measurementType);
    float getLastMeasurement(MeasurementType measurementType);
    String getMqttString(MeasurementType measurementType);

#ifdef DS18B20_ENABLED
    // DS
    float getDsTemp() {return dsTemp;}
#endif

#ifdef DHT22_ENABLED
    // DH
    float getDhTemp() {return dhTemp;}
	float getDhHum() {return dhHum;}
#endif

#ifdef SHT_ENABLED
	// SHT
	float getShtTemp() {return shtTemp;}
	float getShtHum() {return shtHum;}
#endif

#ifdef HTU21D_ENABLED
	// HTU
	float getHtuTemp() {return htuTemp;}
	float getHtuHum() {return htuHum;}
#endif

#if defined BATT_PIN
	// ADC
	float getBattVoltage() {return battVoltage;}
#endif

    virtual ~Measurement();

private:
#ifdef DS18B20_ENABLED
    // DS
    bool dsOk = false;
    float dsTemp = 0;
#endif

#ifdef DHT22_ENABLED
    // DH
    bool dhOk = false;
    float dhTemp = 0;
    float dhHum = 0;
#endif

#ifdef SHT_ENABLED
    // SHT
    bool shtOk = false;
    float shtTemp = 0;
    float shtHum = 0;
#endif

#ifdef HTU21D_ENABLED
    // HTU
    bool htuOk = false;
    float htuTemp = 0;
    float htuHum = 0;
#endif

#if defined BATT_PIN
    // ADC
    float battVoltage = 0;
#endif
};

#endif /* UTILS_MEASUREMENT_H_ */
