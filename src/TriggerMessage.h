// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\TriggerMessage.h"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef TRIGGERMESSAGE_H
#define TRIGGERMESSAGE_H

#include "Variants.h"

#include "libraries/arduinoWebSockets-master/src/WebSocketsClient.h"

#include "OcppMessage.h"
#include "OcppOperation.h"

class TriggerMessage : public OcppMessage {
private:
  WebSocketsClient *webSocket;
  OcppOperation *triggeredOperation;
  const char *statusMessage;
public:
  TriggerMessage(WebSocketsClient *webSocket);

  const char* getOcppOperationType();

  void processReq(JsonObject payload);

  DynamicJsonDocument* createConf();
};

#endif
