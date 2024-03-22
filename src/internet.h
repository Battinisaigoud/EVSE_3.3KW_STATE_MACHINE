// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\internet.h"
#ifndef INTERNET_H
#define INTERNET_H

#include"CustomGsm.h"
#include "libraries/arduinoWebSockets-master/src/WebSocketsClient.h"
#include <WiFi.h>

void internetLoop();
void wifi_gsm_connect();

#endif
