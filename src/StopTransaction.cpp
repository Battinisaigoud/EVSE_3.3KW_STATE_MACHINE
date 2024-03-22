// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\StopTransaction.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "StopTransaction.h"
#include "OcppEngine.h"
#include "OcppMessage.h"
#include "OcppOperation.h"
#include "SimpleOcppOperationFactory.h"
#include "MeteringService.h"

#include "ChargePointStatusService.h"
#include "StatusNotification.h"
#include "ReserveNow.h"
#include <Preferences.h>

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
extern bool reservation_start_flag;
extern  uint8_t gu8_ongoing_stop ;

extern short int counter_drawingCurrent;

extern int globalmeterstartA;
extern unsigned long st_timeA;
//added by sai
extern uint8_t gu8_powerloss_flag;
extern uint16_t gu8_fault_flag ;
extern uint8_t Fault_occured;
extern uint8_t gu8_remote_stop_flag ;
extern bool wifi_connect;
extern bool gsm_connect;
extern uint8_t gu8_online_flag; 
#if LCD_DISPLAY
#include "LCD_I2C.h"
extern LCD_I2C lcd;
#endif

#if DWIN_ENABLED
#include "dwin.h"

extern unsigned char kwh[8];
extern unsigned char change_page[10];
extern unsigned char HR[8];
extern unsigned char MINS[8];
extern unsigned char SEC[8];
extern unsigned char cid1[8];
extern unsigned char cid2[8];
extern unsigned char cid3[8];
extern unsigned char e1[8];
#endif

extern bool disp_evse_A;
unsigned long stop_time = 0;
/*
* @brief: Feature added by Raja
* This feature will avoid hardcoding of messages. 
*/
//typedef enum resonofstop { EmergencyStop, EVDisconnected , HardReset, Local , Other , PowerLoss, Reboot,Remote, Softreset,UnlockCommand,DeAuthorized};
extern uint8_t reasonForStop;
static const char *resonofstop_str[] = { "EmergencyStop", "EVDisconnected" , "HardReset", "Local" , "Other" , "PowerLoss", "Reboot","Remote", "SoftReset","UnlockCommand","DeAuthorized"};

extern bool flag_GFCI_set_here;

extern bool flag_start_accepted;

extern bool flag_stop_finishing;

extern WebSocketsClient webSocket;
// extern Preferences energymeter;

extern Preferences energymeter;

StopTransaction Stop_Transaction;

StopTransaction::StopTransaction() {

}

const char* StopTransaction::getOcppOperationType(){
    return "StopTransaction";
}

DynamicJsonDocument* StopTransaction::createReq() {

  String idTag = String('\0');

  if (getChargePointStatusService() != NULL) {
    idTag += getChargePointStatusService()->getIdTag();
  }

  DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(5) + (idTag.length() + 1) + (JSONDATE_LENGTH + 1));
  JsonObject payload = doc->to<JsonObject>();
  
  if (!idTag.isEmpty()) { //if there is no idTag present, we shouldn't add a default one
    payload["idTag"] = idTag;
  }
  disp_evse_A = false;
  float meterStop = 0.0f;
  // MeteringService* meteringService = getMeteringService();
  if (getMeteringService() != NULL) {
    
    // if (meteringService != NULL) {
    //   meterStop =(int) meteringService->currentEnergy();
    meterStop =(int) getMeteringService()->currentEnergy();

    if(meterStop > (20000000 - 100000))
        {
         energymeter.begin("MeterData",false); 
          energymeter.putFloat("currEnergy_A", 0);
          energymeter.end();
          Serial.println("[Metering init] Reinitialized currEnergy_A");
        }
    #if DWIN_ENABLED
        uint8_t err = 0;
        stop_time = millis();
        unsigned long seconds = (stop_time - st_timeA) / 1000;
        //unsigned long seconds = (st_timeA - stop_time) / 1000;
        Serial.print("##########################################################################Stop time:");
        Serial.println(stop_time);
        Serial.print("##########################################################################Strt time:");
        Serial.println(st_timeA);
        int hr = seconds / 3600;                   // Number of seconds in an hour
        int mins = (seconds - hr * 3600) / 60;     // Remove the number of hours and calculate the minutes.
        int sec = seconds - hr * 3600 - mins * 60; // Remove the number of hours and minutes, leaving only seconds.

        Serial.print("Hours:");
        Serial.println(hr);
        Serial.print("minutes:");
        Serial.println(mins);
        Serial.print("Seconds:");
        Serial.println(sec);

        err = DWIN_SET(cid1, sizeof(cid1) / sizeof(cid1[0]));
        // Take to page 2.
        change_page[9] = 2;
        err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
        // kwh[7] = float((meterStop-globalmeterstartA)/1000);
        // kwh[7] = int((meterStop-globalmeterstartA)/1000);
        int jaf = meterStop - globalmeterstartA;
        kwh[6] = jaf >> 8;
        kwh[7] = jaf & 0xff;
        // Since Kwh is with 3 decimal points multiply by 1000
        // kwh[7]  = jaf/1000;
        err = DWIN_SET(kwh, sizeof(kwh) / sizeof(kwh[0]));
        HR[7] = hr;
        MINS[7] = mins;
        SEC[7] = sec;

        Serial.print("Hours2...:");
        Serial.println(HR[7]);
        Serial.print("minutes2...:");
        Serial.println(MINS[7]);
        Serial.print("Seconds2...:");
        Serial.println(SEC[7]);

        err = DWIN_SET(HR, sizeof(HR) / sizeof(HR[0]));
        err = DWIN_SET(MINS, sizeof(MINS) / sizeof(MINS[0]));
        err = DWIN_SET(SEC, sizeof(SEC) / sizeof(SEC[0]));

        Serial.print("strop transaction enrgy&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&...........:");
        Serial.println(e1[7],2);
        Serial.print("strop transaction kelo watt power &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&...........:");
        Serial.println(kwh[7],2);

        memset(&e1[7], 0, 1);
        memset(&kwh[7], 0, 1);

        Serial.print("after strop transaction enrgy&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&...........:");
        Serial.println(e1[7],2);
        Serial.print("after strop transaction kelo watt power &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&...........:");
        Serial.println(kwh[7],2);
        

        delay(3000);
        // Take to page 2.
        change_page[9] = 0;
        err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
        delay(50);
    #endif
  }

  payload["meterStop"] = (int) meterStop; //TODO meterStart is required to be in Wh, but measuring unit is probably inconsistent in implementation
  char timestamp[JSONDATE_LENGTH + 1] = {'\0'};
  getJsonDateStringFromSystemTime(timestamp, JSONDATE_LENGTH);
  payload["timestamp"] = timestamp;

  int transactionId = -1;
  if (getChargePointStatusService() != NULL) {
    transactionId = getChargePointStatusService()->getTransactionId();
  }
  payload["transactionId"] = transactionId;
  //added by sai
 if(gu8_powerloss_flag == 1)
 {
  gu8_powerloss_flag =0;
  reasonForStop = 5;
 }
 payload["reason"] = resonofstop_str[reasonForStop];
 


  if (getChargePointStatusService() != NULL) {
    getChargePointStatusService()->stopEnergyOffer();
  }
flag_start_accepted = false;

#if 0
#if LCD_ENABLED


if(Fault_occured == 1)
{
   lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SESSION STOPPED");
  lcd.setCursor(0, 1);
  lcd.print("FAULT : ");
  switch (gu8_fault_flag)
  {
  case 1:
    lcd.setCursor(7, 1);
    lcd.print("OVER VOLTAGE");

    break;
  case 2:
    lcd.setCursor(7, 1);
    lcd.print("UNDER VOLTAGE");
    break;
  case 3:
    lcd.setCursor(7, 1);
    lcd.print("GFCI");
    break;
  case 4:
    lcd.setCursor(7, 1);
    lcd.print("EARTH");
    break;
  case 5:
    lcd.setCursor(7, 1);
    lcd.print("EMERGENCY");
    break;

  default:
    gu8_fault_flag = 0;

    break;
  }
}

else if( gu8_remote_stop_flag == 1)
{

lcd.print("REMOTE STOP");

}
else
{
  lcd.print("RFID STOP");

}
  // lcd.setCursor(0, 3);
  // lcd.print("ENERGY(KWH):       ");
  delay(3000);


#endif
#endif

#if 0
#if LCD_ENABLED
unsigned long stop_time = millis();
lcd.clear();

lcd.setCursor(0, 0);
lcd.print("STATUS : FINISHING");

lcd.setCursor(0, 1);
lcd.print("TRANSACTION DONE");
lcd.setCursor(0, 2);
// lcd.print("KWH");
// lcd.setCursor(4, 1);
lcd.print("ENERGY(kW):      ");
lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
lcd.print(String(float((meterStop - globalmeterstartA) / 1000)));
// lcd.setCursor(0,2);
// lcd.print("WH");
// lcd.setCursor(4,2);
// lcd.print(meterStop-globalmeterstartB);
// lcd.setCursor(0, 2);
// lcd.print("CONNECTOR A");
lcd.setCursor(0, 3);
lcd.print("DURATION  :");
lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
unsigned long seconds = (stop_time - st_timeA) / 1000;
int hr = seconds / 3600;                                                 // Number of seconds in an hour
int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
lcd.print(String(hrMinSec));
delay(5000);
#endif
#endif

//added by sai
#if LCD_DISPLAY
unsigned long stop_time = millis();

if (gu8_online_flag == 1)
{
  if (wifi_connect)
  {
    lcd.clear();
    lcd.setCursor(15, 0);
    lcd.print("Wi-Fi");
  }
  else if (gsm_connect)
  {
    lcd.clear();
    lcd.setCursor(18, 0);
    lcd.print("4G");
  }
}
else if (gu8_online_flag == 0)
{
  lcd.setCursor(12, 0);
  lcd.print("OFFLINE");
}

lcd.setCursor(0, 1);
lcd.print("THANKS FOR CHARGING");
lcd.setCursor(0, 2);
// lcd.print("KWH");
// lcd.setCursor(4, 1);
lcd.print("ENERGY(kW):      ");
lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
lcd.print(String(float((meterStop - globalmeterstartA) / 1000)));
// lcd.setCursor(0,2);
// lcd.print("WH");
// lcd.setCursor(4,2);
// lcd.print(meterStop-globalmeterstartB);
// lcd.setCursor(0, 2);
// lcd.print("CONNECTOR A");
lcd.setCursor(0, 3);
lcd.print("DURATION  :");
lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
unsigned long seconds = (stop_time - st_timeA) / 1000;
int hr = seconds / 3600;                                                 // Number of seconds in an hour
int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
lcd.print(String(hrMinSec));
delay(5000);
#endif


#if 0
			//fire StatusNotification
		//TODO check for online condition: Only inform CS about status change if CP is online
		//TODO check for too short duration condition: Only inform CS about status change if it lasted for longer than MinimumStatusDuration
		OcppOperation *statusNotification = makeOcppOperation(&webSocket,
		new StatusNotification(ChargePointStatus::Finishing));
		initiateOcppOperation(statusNotification);
#endif

//kwh = "";
  return doc;
}

void StopTransaction::processConf(JsonObject payload) {

  //no need to process anything here

  ChargePointStatusService *cpStatusService = getChargePointStatusService();
  if (cpStatusService != NULL){
    //cpStatusService->stopEnergyOffer(); //No. This should remain in createReq
    cpStatusService->stopTransaction();
    cpStatusService->unauthorize();
  }

  SmartChargingService *scService = getSmartChargingService();
  if (scService != NULL){
    scService->endChargingNow();
  }

  if (DEBUG_OUT) Serial.println("[StopTransaction] Request has been accepted!");
  if(gu8_ongoing_stop == 0)
  {
   Serial.print("Available.........6");
   evse_ChargePointStatus = Available;
  }
   
   evse_stop_txn_state = EVSE_STOP_TXN_ACCEPTED;

  //Clearing the reason.
 reasonForStop = 3;
 counter_drawingCurrent = 0;
 // Clearing the flag for gfci as well.
 flag_GFCI_set_here = false;
 flag_stop_finishing = false;

      /*
			 * @brief : Feature added by G. Raja Sumant 29/07/2022
			 * This will take the charge point to reserved state when ever it is available during the reservation loop
			 */
			#if 1
			if (reservation_start_flag)
			{
				getChargePointStatusService()->setReserved(true);
			}
			#endif

}


void StopTransaction::processReq(JsonObject payload) {
  /**
   * Ignore Contents of this Req-message, because this is for debug purposes only
   */
}

DynamicJsonDocument* StopTransaction::createConf(){
  DynamicJsonDocument* doc = new DynamicJsonDocument(2 * JSON_OBJECT_SIZE(1));
  JsonObject payload = doc->to<JsonObject>();

  JsonObject idTagInfo = payload.createNestedObject("idTagInfo");
  idTagInfo["status"] = "Accepted";

  return doc;
}
