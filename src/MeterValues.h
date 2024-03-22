// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\MeterValues.h"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef METERVALUES_H
#define METERVALUES_H

#include "OcppOperation.h"
#include <LinkedList.h>
#include "TimeHelper.h"

class MeterValues : public OcppMessage {
private:

  LinkedList<time_t> sampleTime;
  LinkedList<float> power;
  LinkedList<float> energy;
  LinkedList<float> voltage;
  LinkedList<float> current;
  LinkedList<float> temperature;

public:
  //MeterValues(LinkedList<time_t> *sampleTime, LinkedList<float> *energy);

  //MeterValues(LinkedList<time_t> *sampleTime, LinkedList<float> *energy, LinkedList<float> *power);
  
  MeterValues(LinkedList<time_t> *sampleTimeReg, LinkedList<float> *powerReg, LinkedList<float> *energyReg, LinkedList<float> *voltageReg, LinkedList<float> *currentReg, LinkedList<float> *temperatureReg);

  MeterValues(); //for debugging only. Make this for the server pendant

  ~MeterValues();

  const char* getOcppOperationType();

  DynamicJsonDocument* createReq();

  void processConf(JsonObject payload);

  void processReq(JsonObject payload);

  DynamicJsonDocument* createConf();
};

#endif
