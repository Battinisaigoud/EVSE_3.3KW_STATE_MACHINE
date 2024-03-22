// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\SetChargingProfile.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "SetChargingProfile.h"

// SetChargingProfile Set_ChargingProfile;

SetChargingProfile::SetChargingProfile(SmartChargingService *smartChargingService) 
  : smartChargingService(smartChargingService) {

}

const char* SetChargingProfile::getOcppOperationType(){
    return "SetChargingProfile";
}

void SetChargingProfile::processReq(JsonObject payload) {

  int connectorID = payload["connectorId"]; //<-- not used in this implementation

  // JsonObject csChargingProfiles = payload["csChargingProfiles"];

  // smartChargingService->updateChargingProfile(&csChargingProfiles);
}

DynamicJsonDocument* SetChargingProfile::createConf(){ //TODO review
  DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
  JsonObject payload = doc->to<JsonObject>();

   payload["status"] = "Notsupported";
  // payload["status"] = "Accepted";
  return doc;
}
