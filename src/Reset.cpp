// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\Reset.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "Reset.h"
#include "OcppEngine.h"
#include "EVSE.h"

extern bool flag_rebootRequired;
extern bool flag_evseIsBooted;
extern bool flag_evseReadIdTag;
extern bool flag_evseAuthenticate;
extern bool flag_evseStartTransaction;
extern bool flag_evRequestsCharge;
extern bool flag_evseStopTransaction;
extern bool flag_evseUnauthorise;
extern bool flag_evseSoftReset; //added new flag @Wamique

extern uint8_t reasonForStop;

Reset re_set;

Reset::Reset() {

}

const char* Reset::getOcppOperationType(){
	return "Reset";
}

void Reset::processReq(JsonObject payload) {
	/*
	* Process the application data here. Note: you have to implement the device reset procedure in your client code. You have to set
	* a onSendConfListener in which you initiate a reset (e.g. calling ESP.reset() )
	*/
	const char *type = payload["type"] | "Invalid";
	if (!strcmp(type, "Hard")){
		Serial.print(F("[Reset] Warning: received request to perform hard reset, but this implementation is only capable of soft reset!\n"));
		//Hard_Reset(); To be implemented
		//delay(5000);
		//ESP.restart();
		reasonForStop = 2;
		softReset();
	} 
	else if (!strcmp(type, "Soft")){
		if(DEBUG_OUT) Serial.println(F("Soft Reset is Requested"));
		reasonForStop = 8;
		softReset();
	}
}

DynamicJsonDocument* Reset::createConf(){
	DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	payload["status"] = "Accepted";
	return doc;
}



/**********New Function added @Wamique****************/

void softReset(){

	ChargePointStatus inferencedStatus = getChargePointStatusService()->inferenceStatus();
	if(inferencedStatus == ChargePointStatus::Preparing ||
	inferencedStatus == ChargePointStatus::SuspendedEVSE ||
	inferencedStatus == ChargePointStatus::SuspendedEV ||
	inferencedStatus == ChargePointStatus::Charging ){
		if(DEBUG_OUT) Serial.println(F("Current Status::Charging/Preparing/SuspendedEV"));
		flag_evseReadIdTag = false;
		flag_evseAuthenticate = false;
		flag_evseStartTransaction = false;
		flag_evRequestsCharge = false;
		flag_evseStopTransaction = true;
		EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;
		flag_evseUnauthorise = false;
		flag_evseSoftReset = true;
		flag_rebootRequired = false;
	} else if(inferencedStatus == ChargePointStatus::Available){
		flag_evseReadIdTag = false;
		flag_evseAuthenticate = false;
		flag_evseStartTransaction = false;
		flag_evRequestsCharge = false;
		flag_evseStopTransaction = false;
		flag_evseUnauthorise = false;
		flag_rebootRequired = true;
		if(DEBUG_OUT) Serial.println(F("Current::Available"));
	}
}

