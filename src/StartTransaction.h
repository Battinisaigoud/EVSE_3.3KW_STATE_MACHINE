// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\StartTransaction.h"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef STARTTRANSACTION_H
#define STARTTRANSACTION_H

#include <EEPROM.h>

#include "Variants.h"
#include "EEPROMLayout.h"

#include "OcppMessage.h"

class StartTransaction : public OcppMessage {
private:
	String idTag = String(""); //FIXME change it to /0 NULL char.
	int connectorId = 1; 
	bool accepted = false;
public:
	StartTransaction();

	StartTransaction(String &idTag);
	
	StartTransaction(String &idTag, int &connectorId);

	const char* getOcppOperationType();

	DynamicJsonDocument* createReq();

	void processConf(JsonObject payload);

	void processReq(JsonObject payload);

	DynamicJsonDocument* createConf();
};

#endif
