// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\SimpleOcppOperationFactory.h"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef SIMPLEOCPPOPERATIONFACTORY_H
#define SIMPLEOCPPOPERATIONFACTORY_H

#include <ArduinoJson.h>
#include "libraries/arduinoWebSockets-master/src/WebSocketsClient.h"

#include "OcppOperation.h"

OcppOperation* makeFromTriggerMessage(WebSocketsClient *ws, JsonObject payload);

OcppOperation* makeFromJson(WebSocketsClient *ws, JsonDocument *request);

OcppOperation* makeOcppOperation(WebSocketsClient *ws);

OcppOperation* makeOcppOperation(WebSocketsClient *ws, OcppMessage *msg);

OcppOperation *makeOcppOperation(WebSocketsClient *ws, const char *actionCode);

void setOnAuthorizeRequestListener(void listener(JsonObject payload));
void setOnBootNotificationRequestListener(void listener(JsonObject payload));
void setOnTargetValuesRequestListener(void listener(JsonObject payload));
void setOnSetChargingProfileRequestListener(void listener(JsonObject payload));
void setOnStartTransactionRequestListener(void listener(JsonObject payload));
void setOnTriggerMessageRequestListener(void listener(JsonObject payload));
void setOnRemoteStartTransactionReceiveRequestListener(void listener(JsonObject payload));
void setOnRemoteStartTransactionSendConfListener(void listener(JsonObject payload));
void setOnResetSendConfListener(void listener(JsonObject payload));
#endif
