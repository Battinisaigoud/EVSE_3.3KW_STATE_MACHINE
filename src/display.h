// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\display.h"
#ifndef DISPLAY_H
#define DISPLAY_H

#include<SoftwareSerial.h>
#include<ArduinoJson.h>
#include<StreamUtils.h>

void setupDisplay_Disp();
void defaultScreen_Disp();
void statusOfCharger_Disp(String);
void authenticatingRFID_Disp();
void authoriseStatus_Disp(bool);
void displayEnergyValues_Disp(String, String, String);
void cloudConnect_Disp(bool);
bool checkForResponse_Disp();
void thanks_Disp(String value);
void startmastertxn();
int checkForPin();
bool checkForBtn();

#endif
