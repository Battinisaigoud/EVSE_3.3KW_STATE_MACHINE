// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\ReserveNow.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "ReserveNow.h"
#include "OcppEngine.h"
#include "TimeHelper.h"
#include "ChargePointStatusService.h"

#define RESERVE_DURATION  (900)         // 15 Minutes 15 * 1 * 60 = 900 Seconds

extern bool flag_evseReadIdTag ;
extern bool flag_evseAuthenticate ;
extern bool flag_evseStartTransaction ; //Entry condition for starting transaction.
extern bool flag_evRequestsCharge ;
extern bool flag_evseStopTransaction ;
extern bool flag_evseUnauthorise ;
extern bool flag_evseSoftReset;
extern bool flag_rebootRequired;
extern bool flag_evseReserveNow; //added by @mkrishna
extern bool reservation_start_flag;

extern String currentIdTag;
String reserve_currentIdTag;
String expiryDate;
int reservationId;
int reserve_connectorId;
time_t reservation_start_time = 0;
time_t reserveDate = 0;
time_t reservedDuration = RESERVE_DURATION;     // 15 minutes slot Duration 15 * (1 * 60)
bool reserve_state = false;

ReserveNow Reserve_Now;

ReserveNow::ReserveNow() {

	ReserveNowSlotList = LinkedList<reserveNow_slot*>();

}

const char* ReserveNow::getOcppOperationType(){
	return "ReserveNow";
}

void ReserveNow::processReq(JsonObject payload) {

    ReserveNowSlotList = LinkedList<reserveNow_slot*>();


    	/* code */
	if(flag_evseReserveNow)
	{
		accepted = false;
		flag_reserve_faulted = false;
		return;
	}

	if(getChargePointStatusService()->inferenceStatus() == ChargePointStatus::Charging )
	{
		accepted = false;
		flag_reserve_faulted = false;
		return;
	}
	else if(getChargePointStatusService()->inferenceStatus() == ChargePointStatus::Faulted)
	{
		accepted = false;
		flag_reserve_faulted = true;
		return;
	}

	idTag = String(payload["idTag"].as<String>());
	reserve_currentIdTag = idTag;
	connectorId = payload["connectorId"].as<int>();
	reservationId = payload["reservationId"].as<int>();
	reserve_connectorId = connectorId;
	
	Serial.print(F("\r\n***********************************[ReserveNow] ************************************\r\n "));


	const char* expiryTime = payload["expiryDate"] | "Invalid";
	if (strcmp(expiryTime, "Invalid")) {
		bool success = getTimeFromJsonDateString(expiryTime, &reserveDate);
		if(success)
		{
			/*
			* @brief : If you are using steve, uncomment the below
			*/
			//reserveDate = reserveDate - 19800; //  19800 seconds are substrating from Asia/Kolkata (IST) to GMT 
			Serial.print(F("[ReserveNow] expiryDate : "));
			Serial.println(reserveDate);
			
			//reservation_start_time = reserveDate - reservedDuration;
			reservedDuration = reserveDate - now();

			Serial.print(F("[ReserveNow] currentIdTag : "));
			Serial.println(currentIdTag);

			Serial.print(F("[ReserveNow] connectorId : "));
			Serial.println(connectorId);
 
			reserveNowstatus Reservestatus = ReserveNow::ReserveStatus(); 
			if (!flag_evseReserveNow)
			{
				Reservestatus = reserveNowstatus::Accepted;
				ReserveNow::setReserveStatus(Reservestatus);	
				accepted = true;
			}

			if(Reservestatus == reserveNowstatus::Faulted ||
			   Reservestatus == reserveNowstatus::Occupied ||
			   Reservestatus == reserveNowstatus::Rejected ||
			   Reservestatus == reserveNowstatus::Unavailable)
			   {
				if(DEBUG_OUT) Serial.println(F("Current Status::Charging/Preparing/SuspendedEV"));
				
				flag_evseReserveNow = false;	
			   }	
			else if (Reservestatus == reserveNowstatus::Accepted)
			{
		
				if (!flag_evseReserveNow)
				{
					flag_evseReserveNow = true;
					reservation_start_flag = false;
#if 0
					reserveNow_slot *current_reserveslot = new reserveNow_slot();

					current_reserveslot->idTag = currentIdTag;
					current_reserveslot->isAccepted = true;
					current_reserveslot->connectorId = connectorId;
					current_reserveslot->reservationId = reservationId;
					current_reserveslot->Slot_time = RESERVE_DURATION;
					current_reserveslot->start_time = (reserveDate - RESERVE_DURATION);
					current_reserveslot->expiry_time = reserveDate; 
#endif
					if(DEBUG_OUT) Serial.println(F("Current::Available"));

					//https://www.epochconverter.com/timezones?q=1657527293
					//time_t current_time = now() + 19800; // Added the 19800 seconds for Asia/Kolkata (IST)
					time_t current_time = now(); 
					Serial.print(F("[ReserveNow] current time : "));
					Serial.println(current_time);
					time_t delta = reserveDate - now();
					Serial.print(F("[ReserveNow] expiryDate Delta time : "));
					Serial.println(delta);
					//ReserveNowSlotList.add(current_reserveslot);
				}

			}
		}	
		else
		{
			Serial.print(F("[ReserveNow] Error reading time string. Expect format like 2020-02-01T20:53:32.486Z\n"));

		}

	}	 
	else 
	{
		Serial.print(F("[ReserveNow] Error reading time string. Expect format like 2020-02-01T20:53:32.486Z\n"));
	}	
	
	//getChargePointStatusService()->authorize(idTag, connectorId);
}

DynamicJsonDocument* ReserveNow::createConf(){
	DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

    reserveNowstatus Reservestatus = ReserveNow::getReserveStatus();

	switch(Reservestatus)
	{
		case reserveNowstatus::Faulted:
				payload["status"] = "Faulted";
				break;
		case reserveNowstatus::Occupied:
				payload["status"] = "Occupied";
				break;
		case reserveNowstatus::Accepted:
				payload["status"] = "Accepted";
				break;
		case reserveNowstatus::Unavailable:
				payload["status"] = "Unavailable";
				break;
		// case reserveNowstatus::Rejected: // To Do 
		// 		payload["status"] = "Rejected";
		// 		break;
		default:
				payload["status"] = "Faulted";
	}

	if(!accepted)
	{
		if(flag_reserve_faulted)
		payload["status"] = "Faulted";
		else
		payload["status"] = "Occupied";
	}

	Serial.print(F("\r\n***********************************[ReserveNow] ************************************\r\n "));
	Serial.print(F("ReserveNow] \r\n "));

	return doc;
}

DynamicJsonDocument* ReserveNow::createReq() {
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

	payload["idTag"] = "fefed1d19876";
	payload["connectorId"] = 1;

	return doc;
}

void ReserveNow::processConf(JsonObject payload){
	String status = payload["status"] | "Invalid";

	if (status.equals("Accepted")) {
		if (DEBUG_OUT) Serial.println("[ReserveNow] Request has been accepted!\n");
		} else {
			Serial.print(F("[ReserveNow] Request has been denied!"));
	}
}

reserveNowstatus ReserveNow::ReserveStatus() {

    ChargePointStatus inferencedStatus = getChargePointStatusService()->inferenceStatus();
	reserveNowstatus reserve_status;
	switch(inferencedStatus)
	{
		case ChargePointStatus::Preparing:
		case ChargePointStatus::SuspendedEVSE:
		case ChargePointStatus::SuspendedEV:
				reserve_status = reserveNowstatus::Faulted;
				break;
		case ChargePointStatus::Charging:
				reserve_status = reserveNowstatus::Occupied;
				break;
		case ChargePointStatus::Available:
				reserve_status = reserveNowstatus::Accepted;
				break;
		case ChargePointStatus::Unavailable:
				reserve_status = reserveNowstatus::Unavailable;
				break;
		// case ChargePointStatus::Unavailable: // To Do 
		// 		reserve_status = reserveNowstatus::Rejected;
		// 		break;
		default:
				reserve_status = reserveNowstatus::Faulted;
	}

	   ReserveNow::setReserveStatus(reserve_status);	
       return reserve_status;

}	


reserveNowstatus ReserveNow::getReserveStatus(){
	return currentStatus;
}

void ReserveNow::setReserveStatus(reserveNowstatus ReserveStatus){
	this->currentStatus = ReserveStatus;
}

int ReserveNow::compare(time_t a, time_t b) {

   int ret =0;
   
   Serial.print("a :");
   Serial.println(a);
   Serial.print("b :");
   Serial.println(b);
   if(a==b) 
   {
    Serial.print("equal\n");
    ret = 0;
   }
   if(a>b) 
   {
    Serial.print("second date is later\n");
    ret = 1;
   }
   
   if(a<b)  
   {
    printf("seocnd date is earlier\n");
    ret = 2;
   }
  
  return ret;
}