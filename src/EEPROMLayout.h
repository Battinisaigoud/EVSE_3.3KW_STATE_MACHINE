// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\EEPROMLayout.h"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef EEPROMLAYOUT_H
#define EEPROMLAYOUT_H

#include "ChargePointStatusService.h"

typedef struct {
  int isInitialized; // 0
  float lastEnergy; // 4
  char currentStatus[20]; // 8
  char idTag[40]; // 28
  int tranId; // 68
  //int connectorId;
} EEPROM_Data;

#endif
