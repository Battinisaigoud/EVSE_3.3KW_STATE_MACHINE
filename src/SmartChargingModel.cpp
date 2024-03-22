// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\SmartChargingModel.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "SmartChargingModel.h"

#include <string.h>

#include <Arduino.h>

// ChargingSchedulePeriod Charging_SchedulePeriod;

ChargingSchedulePeriod::ChargingSchedulePeriod(JsonObject *json){
  startPeriod = (*json)["startPeriod"];
  limit = (*json)["limit"]; //one fractural digit at most
  numberPhases = (*json)["numberPhases"] | -1;
}
int ChargingSchedulePeriod::getStartPeriod(){
  return this->startPeriod;
}
float ChargingSchedulePeriod::getLimit(){
  return this->limit;
}
int ChargingSchedulePeriod::getNumberPhases(){
  Serial.println("[SmartChargingModel] Unsupported operation: ChargingSchedulePeriod::getNumberPhases(); No phase control implemented");
  return this->numberPhases;
}

void ChargingSchedulePeriod::printPeriod(){
  Serial.println("      startPeriod: ");
  Serial.print(startPeriod);
  // Serial.print(F("\n"));

  Serial.println("      limit: ");
  Serial.print(limit);
  // Serial.print(F("\n"));

  Serial.println("      numberPhases: ");
  Serial.print(numberPhases);
  // Serial.print(F("\n"));
}

ChargingSchedule::ChargingSchedule(JsonObject *json, ChargingProfileKindType chargingProfileKind, RecurrencyKindType recurrencyKind)
      : chargingProfileKind(chargingProfileKind)
      , recurrencyKind(recurrencyKind) {  
  
  duration = (*json)["duration"] | -1;
  if (!getTimeFromJsonDateString((*json)["startSchedule"] | "1970-01-01T00:00:00.000Z", &startSchedule)){
    //non-success
  }
  schedulingUnit = (*json)["scheduleUnit"]; //either 'A' or 'W'
  
  chargingSchedulePeriod = LinkedList<ChargingSchedulePeriod*>();
  JsonArray periodJsonArray = (*json)["chargingSchedulePeriod"];
  for (JsonObject periodJson : periodJsonArray) {
    ChargingSchedulePeriod *period = new ChargingSchedulePeriod(&periodJson);
    chargingSchedulePeriod.add(period);
  }

  //Expecting sorted list of periods but specification doesn't garantuee it
  chargingSchedulePeriod.sort([] (ChargingSchedulePeriod *&p1, ChargingSchedulePeriod *&p2) {
    return p1->getStartPeriod() - p2->getStartPeriod();
  });
  
  minChargingRate = (*json)["minChargingRate"] | -1.0f;
}

ChargingSchedule::~ChargingSchedule(){
  for (int i = 0; i < chargingSchedulePeriod.size(); i++) {
    delete chargingSchedulePeriod.get(i);
  }
  chargingSchedulePeriod.clear();
}

float maximum(float f1, float f2){
  if (f1 < f2) {
    return f2;
  } else {
    return f1;
  }
}

bool ChargingSchedule::inferenceLimit(time_t t, time_t startOfCharging, float *limit, time_t *nextChange) {
  time_t basis; //point in time to which schedule-related times are relative
  *nextChange = INFINITY_THLD; //defaulted to Infinity
  switch (chargingProfileKind) {
    case (ChargingProfileKindType::Absolute):
      //check if schedule is not valid yet but begins in future
      if (startSchedule > t) {
        //not valid YET
        *nextChange = startSchedule;
        return false;
      }
      //If charging profile is absolute, prefer startSchedule as basis. If absent, use chargingStart instead. If absent, no
      //behaviour is defined
      if (startSchedule > 0) {
        basis = startSchedule;
      } else if (startOfCharging > 0 && startOfCharging < t) {
        basis = startOfCharging;
      } else {
        Serial.println("[SmartChargingModel] Undefined behaviour: Inferencing limit from absolute profile, but neither startSchedule, nor start of charging are set! Abort\n");
        return false;
      }
      break;
    case (ChargingProfileKindType::Recurring):
      if (recurrencyKind == RecurrencyKindType::Daily) {
        basis = t - ((t - startSchedule) % SECS_PER_DAY);
        *nextChange = basis + SECS_PER_DAY; //constrain nextChange to basis + one day
      } else if (recurrencyKind == RecurrencyKindType::Weekly) {
        basis = t - ((t - startSchedule) % SECS_PER_WEEK);
        *nextChange = basis + SECS_PER_WEEK;
      } else {
        Serial.println("[SmartChargingModel] Undefined behaviour: Recurring ChargingProfile but no RecurrencyKindType set! Assume \"Daily\"");
        basis = t - ((t - startSchedule) % SECS_PER_DAY);
        *nextChange = basis + SECS_PER_DAY;
      }
      break;
    case (ChargingProfileKindType::Relative):
      //assumed, that it is relative to start of charging
      //start of charging must be before t or equal to t
      if (startOfCharging > t) {
        //Relative charging profiles only work with a currently active charging session which is not the case here
        return false;
      }
      basis = startOfCharging;
      break;
  }

  if (t < basis) { //check for error
    Serial.println("[SmartChargingModel] Error in SchmartChargingModel::inferenceLimit: time basis is smaller than t, but t must be >= basis\n");
    return false;
  }
  
  time_t t_toBasis = t - basis;

  if (duration > 0){
    //duration is set

    //check if duration is exceeded and if yes, abort inferencing limit
    //if no, the duration is an upper limit for the validity of the schedule
    if (t_toBasis >= duration) { //"duration" is given relative to basis
      return false;
    } else {
      *nextChange = minimum(*nextChange, basis + duration);
    }
  }

  /*
   * Work through the ChargingProfilePeriods here. If the right period was found, assign the limit parameter from it
   * and make nextChange equal the beginning of the following period. If the right period is the last one, nextChange
   * will remain the time determined before.
   */
  float limit_res = -1.0f; //If limit_res is still -1 after the loop, the inference process failed
  for (int i = 0; i < chargingSchedulePeriod.size(); i++){
    if (chargingSchedulePeriod.get(i)->getStartPeriod() > t_toBasis) {
      // found the first period that comes after t_toBasis.
      *nextChange = basis + chargingSchedulePeriod.get(i)->getStartPeriod();
      break; //The currently valid limit was set the iteration before
    }
    limit_res = chargingSchedulePeriod.get(i)->getLimit();
  }
  
  if (limit_res >= 0.0f) {
    *limit = maximum(limit_res, minChargingRate);
    return true;
  } else {
    return false; //No limit was found. Either there is no ChargingProfilePeriod, or each period begins after t_toBasis
  }
}


void ChargingSchedule::printSchedule(){
  Serial.println("    duration: ");
  Serial.print(duration);
  // Serial.print(F("\n"));
  
  Serial.println("    startSchedule: ");
  Serial.print(startSchedule);
  // Serial.print(F("\n"));

  Serial.println("    schedulingUnit: ");
  Serial.print(schedulingUnit);
  // Serial.print(F("\n"));

  for (int i = 0; i < chargingSchedulePeriod.size(); i++){
    chargingSchedulePeriod.get(i)->printPeriod();
  }

  Serial.println("    minChargingRate: ");
  Serial.print(minChargingRate);
  // Serial.print(F("\n"));
}

ChargingProfile::ChargingProfile(JsonObject *json){
  
  chargingProfileId = (*json)["chargingProfileId"];
  transactionId = (*json)["transactionId"] | -1;
  stackLevel = (*json)["stackLevel"];

  if (DEBUG_OUT) {
    Serial.println("[SmartChargingModel] ChargingProfile created with chargingProfileId = ");
    Serial.print(chargingProfileId);
    // Serial.print(F("\n"));
  }
  
  const char *chargingProfilePurposeStr = (*json)["chargingProfilePurpose"] | "Invalid";
  if (DEBUG_OUT) {
    Serial.println("[SmartChargingModel] chargingProfilePurposeStr=");
    Serial.print(chargingProfilePurposeStr);
    // Serial.print(F("\n"));
  }
  if (!strcmp(chargingProfilePurposeStr, "ChargePointMaxProfile")) {
    chargingProfilePurpose = ChargingProfilePurposeType::ChargePointMaxProfile;
  } else if (!strcmp(chargingProfilePurposeStr, "TxDefaultProfile")) {
    chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
  //} else if (!strcmp(chargingProfilePurposeStr, "TxProfile")) {
  } else {
    chargingProfilePurpose = ChargingProfilePurposeType::TxProfile;
  }
  const char *chargingProfileKindStr = (*json)["chargingProfileKind"] | "Invalid";
  if (!strcmp(chargingProfileKindStr, "Absolute")) {
    chargingProfileKind = ChargingProfileKindType::Absolute;
  } else if (!strcmp(chargingProfileKindStr, "Recurring")) {
    chargingProfileKind = ChargingProfileKindType::Recurring;
  //} else if (!strcmp(chargingProfileKindStr, "Relative")) {
  } else {
    chargingProfileKind = ChargingProfileKindType::Relative;
  }
  const char *recurrencyKindStr = (*json)["recurrencyKind"] | "Invalid";
  if (DEBUG_OUT) {
    Serial.println("[SmartChargingModel] recurrencyKindStr=");
    Serial.print(recurrencyKindStr);
    Serial.print('\n');
  }
  if (!strcmp(recurrencyKindStr, "Daily")) {
    recurrencyKind = RecurrencyKindType::Daily;
  } else if (!strcmp(recurrencyKindStr, "Weekly")) {
    recurrencyKind = RecurrencyKindType::Weekly;
  } else {
    recurrencyKind = RecurrencyKindType::NOT_SET; //not part of OCPP 1.6
  }

  const char *validFromStr = (*json)["validFrom"] | "1970-01-01T00:00:00.000Z";
  if (!getTimeFromJsonDateString(validFromStr, &validFrom)){
    //non-success
    Serial.println("[SmartChargingModel] Error reading validFrom. Expect format like 2020-02-01T20:53:32.486Z. Assume year 1970 for now\n");
  }

  const char *validToStr = (*json)["validTo"] | INFINITY_STRING;
  if (!getTimeFromJsonDateString(validToStr, &validTo)){
    //non-success
    Serial.println("[SmartChargingModel] Error reading validTo. Expect format like 2020-02-01T20:53:32.486Z. Assume year 2037 for now\n");
  }
  
  JsonObject schedule = (*json)["chargingSchedule"]; 
  chargingSchedule = new ChargingSchedule(&schedule, chargingProfileKind, recurrencyKind);
}

ChargingProfile::~ChargingProfile(){
  delete chargingSchedule;
}

/**
 * Modulo, the mathematical way
 * 
 * dividend:    -4  -3  -2  -1   0   1   2   3   4 
 * % divisor:    3   3   3   3   3   3   3   3   3
 * = remainder:  2   0   1   2   0   1   2   0   1
 */
int modulo(int dividend, int divisor){
  int remainder = dividend;
  while (remainder < 0) {
    remainder += divisor;
  }
  while (remainder >= divisor) {
    remainder -= divisor;
  }
  return remainder;
}

bool ChargingProfile::inferenceLimit(time_t t, time_t startOfCharging, float *limit, time_t *nextChange){
  if (t > validTo && validTo > 0) {
    *nextChange = INFINITY_THLD;
    return false; //no limit defined
  }
  if (t < validFrom) {
    *nextChange = validFrom;
    return false; //no limit defined
  }

  return chargingSchedule->inferenceLimit(t, startOfCharging, limit, nextChange);
}

bool ChargingProfile::inferenceLimit(time_t t, float *limit, time_t *nextChange){
  return inferenceLimit(t, INFINITY_THLD, limit, nextChange);
}

bool ChargingProfile::checkTransactionId(int chargingSessionTransactionID) {
  if (transactionId >= 0 && chargingSessionTransactionID >= 0){
    //Transaction IDs are valid
    if (chargingProfilePurpose == ChargingProfilePurposeType::TxProfile //only then a transactionId can restrict the limits
          && transactionId != chargingSessionTransactionID) {
      return false;
    }
  }
  return true;
}

int ChargingProfile::getStackLevel(){
  return stackLevel;
}
  
ChargingProfilePurposeType ChargingProfile::getChargingProfilePurpose(){
  return chargingProfilePurpose;
}

void ChargingProfile::printProfile(){
  Serial.println("  chargingProfileId: ");
  Serial.print(chargingProfileId);
  // Serial.print(F("\n"));

  Serial.println("  transactionId: ");
  Serial.print(transactionId);
  // Serial.print(F("\n"));

  Serial.println("  stackLevel: ");
  Serial.print(stackLevel);
  // Serial.print(F("\n"));

  Serial.println("  chargingProfilePurpose: ");
  switch (chargingProfilePurpose) {
    case (ChargingProfilePurposeType::ChargePointMaxProfile):
      Serial.println("ChargePointMaxProfile");
      break;
    case (ChargingProfilePurposeType::TxDefaultProfile):
      Serial.println("TxDefaultProfile");
      break;
    case (ChargingProfilePurposeType::TxProfile):
      Serial.println("TxProfile");
      break;    
  }
  Serial.print(F("\n"));

  Serial.println("  chargingProfileKind: ");
  switch (chargingProfileKind) {
    case (ChargingProfileKindType::Absolute):
      Serial.println("Absolute");
      break;
    case (ChargingProfileKindType::Recurring):
      Serial.println("Recurring");
      break;
    case (ChargingProfileKindType::Relative):
      Serial.println("Relative");
      break;    
  }
  Serial.print(F("\n"));

  Serial.println("  recurrencyKind: ");
  switch (recurrencyKind) {
    case (RecurrencyKindType::Daily):
      Serial.println("Daily");
      break;
    case (RecurrencyKindType::Weekly):
      Serial.println("Weekly");
      break;
    case (RecurrencyKindType::NOT_SET):
      Serial.println("NOT_SET");
      break;    
  }
  Serial.print(F("\n"));

  Serial.println("  validFrom: ");
  printTime(validFrom);
  Serial.print(F("\n"));

  Serial.println("  validTo: ");
  printTime(validTo);
  Serial.print(F("\n"));
  chargingSchedule->printSchedule();
}
