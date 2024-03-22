// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\OcppMessage.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "OcppMessage.h"

OcppMessage Ocpp_Message;

OcppMessage::OcppMessage(){}

OcppMessage::~OcppMessage(){}
  
const char* OcppMessage::getOcppOperationType(){
    Serial.println("[OcppMessage]  Unsupported operation: getOcppOperationType() is not implemented!\n");
    return "CustomOperation";
}

DynamicJsonDocument* OcppMessage::createReq() {
    Serial.println("[OcppMessage]  Unsupported operation: createReq() is not implemented!\n");
    return new DynamicJsonDocument(0);
}

void OcppMessage::processConf(JsonObject payload) {
    Serial.println("[OcppMessage]  Unsupported operation: processConf() is not implemented!\n");
}

void OcppMessage::processReq(JsonObject payload) {
    Serial.println("[OcppMessage]  Unsupported operation: processReq() is not implemented!\n");
}

DynamicJsonDocument* OcppMessage::createConf() {
    Serial.println("[OcppMessage]  Unsupported operation: createConf() is not implemented!\n");
    return NULL;
}
