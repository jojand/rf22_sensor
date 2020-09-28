#include "Measurement.h"
#include "Streaming.h"
#include "utils.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_HTU21DF.h>
#include "SHTSensor.h"
#include "DHT.h"

#ifdef DS18B20_ENABLEDN
OneWire oneWireDS(DS_DATA_PIN);
DallasTemperature ds_sensors(&oneWireDS);
#endif

#ifdef SHT_ENABLED
SHTSensor sht(SHTSensor::SHT3X);
#endif

#ifdef DHT22_ENABLED
DHT dht(DH_DATA_PIN, DHT22);
#endif

#ifdef HTU21D_ENABLED
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
#endif


Measurement::Measurement() {

}

bool Measurement::begin() {

    // init dht22
#ifdef DHT22_ENABLED
	Serial << INFO_HEADER << F("dht22 init") << endl;
    pinMode(DH_VCC_PIN, OUTPUT);
    pinMode(DH_GND_PIN, OUTPUT);
    digitalWrite(DH_VCC_PIN, HIGH);
    digitalWrite(DH_GND_PIN, LOW);
    pinMode(DH_DATA_PIN, INPUT_PULLUP);
    dht.begin();
#endif

    // init ds18b20
#ifdef DS18B20_ENABLED
    Serial << INFO_HEADER << F("ds18b20 init") << endl;
    pinMode(DS_VCC_PIN, OUTPUT);
    pinMode(DS_GND_PIN, OUTPUT);
    digitalWrite(DS_VCC_PIN, HIGH);
    digitalWrite(DS_GND_PIN, LOW);
    pinMode(DS_DATA_PIN, INPUT_PULLUP);
    ds_sensors.begin();
#endif

    // SHT
#ifdef SHT_ENABLED
    Serial << INFO_HEADER << F("sht init") << endl;
    pinMode(SHT_VCC_PIN, OUTPUT);
    digitalWrite(SHT_VCC_PIN, HIGH);
    pinMode(SHT_GND_PIN, OUTPUT);
    digitalWrite(SHT_GND_PIN, LOW);
    if (sht.init()) {
    	Serial << INFO_HEADER << F("sht init OK.") << endl;
    } else {
    	Serial << ERROR_HEADER << F("sht init ERROR.") << endl;
    }
    sht.setAccuracy(SHTSensor::SHT_ACCURACY_HIGH);
#endif

#ifdef HTU21D_ENABLED
    if (htu.begin()) {
    	Serial << INFO_HEADER << F("htu21d init OK.") << endl;
    } else {
    	Serial << ERROR_HEADER << F("htu21d init ERROR!") << endl;
    }
#endif

#if defined BATT_PIN
    analogReference(INTERNAL);
#endif

    return true;
}

float Measurement::doMeasure(MeasurementType measurementType) {
    switch (measurementType) {

#ifdef DS18B20_ENABLED
        case MeasurementType::dsTemp:
            ds_sensors.requestTemperatures();
            dsTemp = ds_sensors.getTempCByIndex(0);
            Serial << INFO_HEADER << F("DS_TEMP: ") << dsTemp << endl;
            return dsTemp;
#endif

#ifdef DHT22_ENABLED
        case MeasurementType::dhTemp:
        	dhTemp = dht.readTemperature(false);
        	Serial << INFO_HEADER << F("dhTemp: ") << dhTemp << endl;
            return dhTemp;

        case MeasurementType::dhHum:
        	dhHum = dht.readHumidity();
        	Serial << INFO_HEADER << F("dhHum: ") << dhHum << endl;
        	return dhHum;
#endif

#ifdef SHT_ENABLED
        case MeasurementType::shtTemp:
        	sht.readSample();
        	shtTemp = sht.getTemperature();
        	Serial << INFO_HEADER << F("shtTemp: ") << shtTemp << endl;
        	return shtTemp;

        case MeasurementType::shtHum:
        	sht.readSample();
        	shtHum = sht.getHumidity();
        	Serial << INFO_HEADER << F("shtHum: ") << shtHum << endl;
        	return shtHum;
#endif

#ifdef HTU21D_ENABLED
        case MeasurementType::htuTemp:
        	htuTemp = htu.readTemperature();
        	Serial << INFO_HEADER << F("htuTemp: ") << htuTemp << endl;
        	return htuTemp;

        case MeasurementType::htuHum:
        	htuHum = htu.readHumidity();
        	Serial << INFO_HEADER << F("htuHum: ") << htuHum << endl;
			return htuHum;
#endif

#if defined BATT_PIN
        case MeasurementType::battVoltage:
        	int actualRead;

        	// discard the first measurement ...
        	analogRead(BATT_PIN);
        	actualRead = analogRead(BATT_PIN);
//        	Serial << INFO_HEADER << F("actualRead: ") << actualRead << endl;
        	battVoltage += actualRead;
        	actualRead = analogRead(BATT_PIN);
//        	Serial << INFO_HEADER << F("actualRead: ") << actualRead << endl;
        	battVoltage += actualRead;
        	actualRead = analogRead(BATT_PIN);
//        	Serial << INFO_HEADER << F("actual.Read: ") << actualRead << endl;
        	battVoltage += actualRead;
        	battVoltage = battVoltage / 3 * ADC_BATT_CALIB;
        	Serial << INFO_HEADER << F("battVoltage: ") << battVoltage << endl;
            return battVoltage;
#endif

        default:
            return 0.0;
    }
}

float Measurement::getLastMeasurement(MeasurementType measurementType) {
    switch (measurementType) {
#ifdef DS18B20_ENABLED
        case MeasurementType::dsTemp:
            return dsTemp;
            break;
#endif

#ifdef SHT_ENABLED
        case MeasurementType::shtTemp:
            return shtTemp;
            break;

        case MeasurementType::shtHum:
            return shtHum;
            break;
#endif

#ifdef DHT22_ENABLED
        case MeasurementType::dhTemp:
            return dhTemp;
            break;

        case MeasurementType::dhHum:
            return dhHum;
            break;
#endif

#ifdef HTU21D_ENABLED
        case MeasurementType::htuTemp:
            return htuTemp;
            break;


        case MeasurementType::htuHum:
            return htuHum;
            break;
#endif

#if defined BATT_PIN
        case MeasurementType::battVoltage:
            return battVoltage;
            break;
#endif

        default:
            return 0.0;
            break;
    }
}

Measurement::~Measurement() {

}

