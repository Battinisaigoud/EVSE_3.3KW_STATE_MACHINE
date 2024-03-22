// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\RemoteStartTransaction.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "RemoteStartTransaction.h"
#include "OcppEngine.h"
#include "ChargePointStatusService.h"
#include "LCD_I2C.h"

extern bool flag_evseReadIdTag;
extern bool flag_evseAuthenticate;
extern bool flag_evseStartTransaction; // Entry condition for starting transaction.
extern bool flag_evRequestsCharge;
extern bool flag_evseStopTransaction;
extern bool flag_evseUnauthorise;
extern String currentIdTag;
uint8_t remotestart_txn = 0;
extern bool reservation_start_flag;
extern String reserve_currentIdTag;
extern uint8_t reasonForStop_A;
extern bool flag_AuthorizeRemoteTxRequests;
extern enum EvseDevStatuse EvseDevStatus_connector_1; 
extern enum evse_start_txn_stat_t evse_start_txn_state;
//added by sai
extern LCD_I2C lcd;

RemoteStartTransaction Remote_StartTransaction;

RemoteStartTransaction::RemoteStartTransaction()
{
}

const char *RemoteStartTransaction::getOcppOperationType()
{
	return "RemoteStartTransaction";
}

void RemoteStartTransaction::processReq(JsonObject payload)
{
	idTag = String(payload["idTag"].as<String>());
	currentIdTag = idTag;
	connectorId = payload["connectorId"].as<int>();
	// if reserved
	// if strcmp of idTag and reservedidTag is 0
	//  set accepted = true
	// else accepted = false
	// else directly authoizie
	if (getChargePointStatusService()->inferenceStatus() != ChargePointStatus::Available)
	{
		if (getChargePointStatusService()->inferenceStatus() == ChargePointStatus::Reserved)
		{
			Serial.println(F("[RemoteStartTransaction] Trying remote start in reserved state"));
		}
		else
		{
			accepted = false;
			return;
		}
	}

	if (reservation_start_flag)
	{
		if (currentIdTag.equals(reserve_currentIdTag) == true)
		{
			accepted = true;
			getChargePointStatusService()->authorize(idTag, connectorId);
			#if 0
			if (!flag_AuthorizeRemoteTxRequests)
			{
				flag_evseReadIdTag = false;
				flag_evseAuthenticate = false;
				flag_evseStartTransaction = true; // Entry condition for starting transaction.
				flag_evRequestsCharge = false;
				flag_evseStopTransaction = false;
				flag_evseUnauthorise = false;
			}
			#endif
			/*
			 * @brief : Change made by G. Raja Sumant for removing reservation.
			 */
			getChargePointStatusService()->setReserved(false);
		}
		else
		{
			accepted = false;
			Serial.println("[RemoteStartTransaction] Reserved and failed due to mismatch of tag");
		}
	}
	else
	{
        Serial.println("[RemoteStartTransaction] requetsed");
		remotestart_txn = 1;
		accepted = true;
#if 1
#if LCD_DISPLAY
		lcd.clear();
		lcd.setCursor(0, 1);
		lcd.print("AUTHENTICATION ");
		lcd.setCursor(0, 2);
		lcd.print("SUCCESSFUL");
		Serial.println("AUTHENTICATION SUCCESS\n");

#endif
#endif
		// Add condition by checking if available or unavailable
		bool un = false;
		un = getChargePointStatusService()->getUnavailable();
		if (!un)
		{
			getChargePointStatusService()->authorize(idTag, connectorId);
			#if 0
			if (!flag_AuthorizeRemoteTxRequests)
			{
				flag_evseReadIdTag = false;
				flag_evseAuthenticate = false;
				flag_evseStartTransaction = true; // Entry condition for starting transaction.
				flag_evRequestsCharge = false;
				flag_evseStopTransaction = false;
				flag_evseUnauthorise = false;
			}
			#endif
			/*
			 * @brief : Change made by G. Raja Sumant for removing reservation.
			 */
			getChargePointStatusService()->setReserved(false);
		}
		else
		{
			accepted = false;
		}
	}
}
DynamicJsonDocument *RemoteStartTransaction::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

	if (accepted)
	{
		
		  switch (EvseDevStatus_connector_1)
      {
        case flag_EVSE_Read_Id_Tag:
               
            //    gu8_evse_change_state = EVSE_START_TXN;
			//    evse_ChargePointStatus = Preparing;
			   evse_start_txn_state = EVSE_START_TXN_INITIATED;
			   EvseDevStatus_connector_1 = flag_EVSE_Start_Transaction;
		       Serial.println("Remote Start- CMS Accepted");
			   
			   payload["status"] = "Accepted";
          break;    
        default:
                Serial.println("default - Remote Start-");
          break;
      }
		// payload["status"] = "Accepted";
		#if 0
		flag_evseReadIdTag = false;
		flag_evseAuthenticate = false;
		flag_evseStartTransaction = true; // Entry condition for starting transaction.
		flag_evRequestsCharge = false;
		flag_evseStopTransaction = false;
		flag_evseUnauthorise = false;
		#endif
		if (!flag_AuthorizeRemoteTxRequests)
			{
				flag_evseReadIdTag = false;
				flag_evseAuthenticate = false;
				flag_evseStartTransaction = true; // Entry condition for starting transaction.
				flag_evRequestsCharge = false;
				flag_evseStopTransaction = false;
				flag_evseUnauthorise = false;
			}
	}
	else
	{
		payload["status"] = "Rejected";
		flag_evseReadIdTag = true;
		EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
		flag_evseAuthenticate = false;
		flag_evseStartTransaction = false; // Entry condition for starting transaction.
		flag_evRequestsCharge = false;
		flag_evseStopTransaction = false;
		flag_evseUnauthorise = false;
	}
	accepted = false;

	return doc;
}

DynamicJsonDocument *RemoteStartTransaction::createReq()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

	payload["idTag"] = "fefed1d19876";
	payload["connectorId"] = 1;

	return doc;
}

void RemoteStartTransaction::processConf(JsonObject payload)
{
	String status = payload["status"] | "Invalid";

	if (status.equals("Accepted"))
	{
		remotestart_txn = 1; 
		if (DEBUG_OUT)
		Serial.print("[RemoteStartTransaction] Request has been accepted!\n");
#if 0
#if LCD_DISPLAY
		lcd.clear();
		lcd.setCursor(0, 1);
		lcd.print("AUTHENTICATION ");
		lcd.setCursor(0, 2);
		lcd.print("SUCCESSFUL");
		Serial.println("AUTHENTICATION SUCCESS\n");

#endif
#endif
	}
	else
	{
		Serial.print(F("[RemoteStartTransaction] Request has been denied!"));
#if LCD_DISPLAY
		lcd.clear();
		lcd.setCursor(0, 1);
		lcd.print("AUTHENTICATION");
		lcd.setCursor(0, 2);
		lcd.print("DENIED");
		Serial.println("DENIED\n");

#endif
	}
}
