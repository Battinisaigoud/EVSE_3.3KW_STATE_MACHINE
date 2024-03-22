// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\Heartbeat.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Heartbeat.h"
#include "TimeHelper.h"
#include <string.h>


Heartbeat Heart_beat;

Heartbeat::Heartbeat()  {
  
}

const char* Heartbeat::getOcppOperationType(){
    return "Heartbeat";
}

DynamicJsonDocument* Heartbeat::createReq() {
  DynamicJsonDocument *doc = new DynamicJsonDocument(0);
  JsonObject payload = doc->to<JsonObject>();
  /*
   * Empty payload
   */
  return doc;
}

void Heartbeat::processConf(JsonObject payload) {
  
  const char* currentTime = payload["currentTime"] | "Invalid";
  if (strcmp(currentTime, "Invalid")) {
    if (setTimeFromJsonDateString(currentTime)) {
      if (DEBUG_OUT) Serial.println("[Heartbeat] Request has been accepted!\n");
    } else {
      Serial.println("[Heartbeat] Request accepted. But Error reading time string. Expect format like 2020-02-01T20:53:32.486Z\n");
    }
  } else {
    Serial.println("[Heartbeat] Request denied. Missing field currentTime. Expect format like 2020-02-01T20:53:32.486Z\n");
  }

}

void Heartbeat::processReq(JsonObject payload) {

  /**
   * Ignore Contents of this Req-message, because this is for debug purposes only
   */

}

DynamicJsonDocument* Heartbeat::createConf(){
  DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1) + (JSONDATE_LENGTH + 1));
  JsonObject payload = doc->to<JsonObject>();

  char currentTime[JSONDATE_LENGTH + 1] = {'\0'};
  getJsonDateStringFromSystemTime(currentTime, JSONDATE_LENGTH);
  payload["currentTime"] = currentTime;

  return doc;
}
