// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\GetConfiguration.h"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef GETCONFIGURATION_H
#define GETCONFIGURATION_H

#include "OcppMessage.h"

class GetConfiguration : public OcppMessage {
private:

public:
  GetConfiguration();

  const char* getOcppOperationType();

  DynamicJsonDocument* createReq();

  void processConf(JsonObject payload);
  
  DynamicJsonDocument* createConf();

  void processReq(JsonObject payload);


};

#endif
