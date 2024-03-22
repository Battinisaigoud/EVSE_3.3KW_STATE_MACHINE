// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\GetLocalListVersion.h"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef GETLOCALLISTVERSION_H
#define GETLOCALLISTVERSION_H

#include "OcppMessage.h"

class GetLocalListVersion : public OcppMessage {
private:
	int localListVersionNumber;
public:
	GetLocalListVersion();

	const char* getOcppOperationType();

	/*DynamicJsonDocument* createReq();*/

	/*void processConf(JsonObject payload);*/

	void processReq(JsonObject payload);

	DynamicJsonDocument* createConf();

};

#endif
