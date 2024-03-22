// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\ChangeConfiguration.h"
//Developed by Wamique, EVRE

#ifndef CHANGECONFIGURATION_H
#define CHANGECONFIGURATION_H

#include "OcppMessage.h"

class ChangeConfiguration : public OcppMessage {
private:
    bool acceptance;
    bool rejected; //added by G. Raja Sumant 08/07/2022 to reject invalid values.
public:
  ChangeConfiguration();

  const char* getOcppOperationType();

  DynamicJsonDocument* createReq();

  void processConf(JsonObject payload);
  
  DynamicJsonDocument* createConf();
  
  void processReq(JsonObject payload);

};

#endif