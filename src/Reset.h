// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\Reset.h"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef RESET_H
#define RESET_H

#include "Variants.h"

#include "OcppMessage.h"

class Reset : public OcppMessage {
public:
	Reset();

	const char* getOcppOperationType();

	void processReq(JsonObject payload);

	DynamicJsonDocument* createConf();


};
	/****Added new Function @Wamique*****/
	void softReset();
	/***********************************/
#endif
