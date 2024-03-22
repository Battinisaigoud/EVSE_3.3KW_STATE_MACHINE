// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\OTA.h"
#ifndef OTA_H
#define OTA_H
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>


void setupOTA();
void performUpdate(WiFiClient& updateSource, size_t updateSize);
void ota_Loop();

#endif