// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\StatusNotification.h"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef STATUSNOTIFICATION_H
#define STATUSNOTIFICATION_H

#include <EEPROM.h>

#include "OcppMessage.h"
#include "OcppEngine.h"
#include "ChargePointStatusService.h"
#include "TimeHelper.h"
#include "EEPROMLayout.h"

class StatusNotification : public OcppMessage {
private:
	ChargePointStatus currentStatus = ChargePointStatus::NOT_SET;
	char timestamp[JSONDATE_LENGTH + 1] = {'\0'};
public:
	StatusNotification(ChargePointStatus currentStatus);

	StatusNotification();

	const char* getOcppOperationType();

	DynamicJsonDocument* createReq();

	void processConf(JsonObject payload);

	void processReq(JsonObject payload);

	DynamicJsonDocument* createConf();
};

#endif
