#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

// ****************
// **** setup *****
// ****************

// Specify a radio driver
#define DRIVER_RF22
//#define DRIVER_RF95

#define MAX_MESSAGE_LEN     		50
#ifdef DRIVER_RF22
    #define DEFAULT_TX_POW          RH_RF22_TXPOW_17DBM
#elif defined DRIVER_RF95
    #define DEFAULT_TX_POW          10
#endif

#define SENT_FAILED_THRESHOLD		3

#define SERIAL_BAUD         		57600
#define SERVER_ADDRESS      		200
#define CLIENT_ADDRESS      		202

#define DEFAULT_SLEEP_CYCLES		15
#define INITIAL_LOOP_CYCLES			100
#define MAX_SLEEP_CYCLES			100

// ********************
// **** setup end *****
// ********************

#define ERROR_HEADER    F("[ERROR] ")
#define WARN_HEADER     F("[WARN] ")
#define INFO_HEADER     F("[INFO] ")
#define MQTT_HEADER     F("[MQTT] ")

#define RADIO_SHDN	4
#define LED_PIN			8

#define MQTT_STAT_OK_TOPIC	F("rf/0/ok")
#define MQTT_STAT_ERR_TOPIC	F("rf/0/err")

#ifdef __cplusplus
extern "C" {
#endif
void radioOn();
void radioOff();
void initBlink();
void goodBlink();
void badBlink();
#ifdef __cplusplus
}
#endif

#endif /* UTILS_UTILS_H_ */
