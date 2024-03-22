// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\SendLocalList.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "SendLocalList.h"
#include "OcppEngine.h"
#include "Variants.h"
#include "Master.h"

bool send_ll_status = false;

SendLocalList Send_LocalList;

SendLocalList::SendLocalList() {
	
}


const char* SendLocalList::getOcppOperationType(){
	return "SendLocalList";
}

DynamicJsonDocument* SendLocalList::createReq() {
	
}

void SendLocalList::processConf(JsonObject payload){
	
	send_ll_status = requestSendLocalList(payload);
	
}

void SendLocalList::processReq(JsonObject payload){
	
	//listVersion = payload["listVersion"];
	//localAuthorizationList[100] = payload["localAuthorizationList"];
	
}

DynamicJsonDocument* SendLocalList::createConf(){
	DynamicJsonDocument* doc = new DynamicJsonDocument(2 * JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	payload["status"] = "Accepted";
	//JsonObject idTagInfo = payload.createNestedObject("idTagInfo");
	//idTagInfo["status"] = "Accepted";
	int ret = send_ll_status;
    if(!ret)
	{
		payload["status"] = "Accepted";
	}
	else
	{
		
		payload["status"] = "Rejected";
	}
	return doc;
}
