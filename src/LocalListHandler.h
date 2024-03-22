// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\LocalListHandler.h"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License


#ifndef LOCALLISTHANDLER_H
#define LOCALLISTHANDLER_H

#include <ArduinoJson.h>
#include <Arduino.h>
class IdTagInfoClass {
private:
	String idTag;
	String idTagStatus;
	String idExpDate;
	
public:
	void setIdTagInfo (String idTag, String idTagStatus, String idExpDate);
	void getIdTagInfo();
	void processJson(char *json);

};






#endif