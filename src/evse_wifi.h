#ifndef EVSE_WIFI_H
#define EVSE_WIFI_H

#include <WiFi.h>
#include "esp_wifi.h"

void wifi_init(void);

extern uint8_t isWifiConnected;
extern int32_t rssi;

// extern const char* evse_ssid ;
// extern const char* evse_password;
#endif