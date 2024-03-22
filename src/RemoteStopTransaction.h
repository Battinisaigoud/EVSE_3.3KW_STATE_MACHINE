// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\RemoteStopTransaction.h"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef REMOTESTOPTRANSACTION_H
#define REMOTESTOPTRANSACTION_H

#include "Variants.h"

#include "OcppMessage.h"
#include "ChargePointStatusService.h"

class RemoteStopTransaction : public OcppMessage {
private:
	int transactionId;
	bool accepted = false;
public:
	RemoteStopTransaction();

	const char* getOcppOperationType();

	DynamicJsonDocument* createReq();

	void processConf(JsonObject payload);

	void processReq(JsonObject payload);

	DynamicJsonDocument* createConf();
};

#endif
