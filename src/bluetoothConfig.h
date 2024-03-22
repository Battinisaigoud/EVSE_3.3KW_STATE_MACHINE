// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\bluetoothConfig.h"
#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <BluetoothSerial.h> //@mwh
#include<EEPROM.h> //@mwh
#include<ArduinoJson.h> //@mwh
#include<WiFi.h>
#include <Preferences.h>
//void wifiConnectWithStoredCredential();
void bluetooth_Loop();
void eepromStoreCred();
bool VerifyCred(String message);
void startingBTConfig();

#endif