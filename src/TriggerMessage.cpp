// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\TriggerMessage.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "TriggerMessage.h"
#include "SimpleOcppOperationFactory.h"
#include "OcppEngine.h"

// TriggerMessage Trigger_Message;

TriggerMessage::TriggerMessage(WebSocketsClient *webSocket) : webSocket(webSocket) {
  statusMessage = "NotImplemented"; //default value if anything goes wrong
}

const char* TriggerMessage::getOcppOperationType(){
    return "TriggerMessage";
}

void TriggerMessage::processReq(JsonObject payload) {

  Serial.println("[TriggerMessage] Warning: TriggerMessage is not tested!\n");

  triggeredOperation = makeFromTriggerMessage(webSocket, payload);
  if (triggeredOperation != NULL) {
	initiateOcppOperation(triggeredOperation);
    statusMessage = "Accepted";
  } else {
    Serial.println("[TriggerMessage] Couldn't make OppOperation from TriggerMessage. Ignore request.\n");
    statusMessage = "NotImplemented";
  }
}

DynamicJsonDocument* TriggerMessage::createConf(){
  DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1) + strlen(statusMessage));
  JsonObject payload = doc->to<JsonObject>();
  
  payload["status"] = statusMessage;

  //Serial.print(F("[TriggerMessage] Triggering messages is not implemented!\n"));

  return doc;
}
