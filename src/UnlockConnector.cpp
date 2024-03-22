// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\UnlockConnector.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License



#include "UnlockConnector.h"
#include "OcppEngine.h"

UnlockConnector Unlock_Connector;

UnlockConnector::UnlockConnector() {

}

const char* UnlockConnector::getOcppOperationType(){
	return "UnlockConnector";
}

void UnlockConnector::processReq(JsonObject payload) {
	
	connectorId = payload["connectorId"].as<int>();
	if(connectorId!=1)
    {
        accepted = false;
    }
    else
    {
        accepted = true;
    }
}

DynamicJsonDocument* UnlockConnector::createConf(){
	DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
    if(accepted)
	payload["status"] = "NotSupported";
    else
    payload["status"] = "NotSupported";
	
	return doc;
}

DynamicJsonDocument* UnlockConnector::createReq() {
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

	payload["idTag"] = "fefed1d19876";
	payload["connectorId"] = 1;

	return doc;
}

void UnlockConnector::processConf(JsonObject payload){
	String status = payload["status"] | "Invalid";

	if (status.equals("Accepted")) {
		if (DEBUG_OUT) Serial.println("[UnlockConnector] Request has been accepted!\n");
		} else {
			Serial.print(F("[UnlockConnector] Request has been denied!"));
	}
}
