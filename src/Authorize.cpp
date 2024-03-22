// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\Authorize.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Authorize.h"
#include "OcppEngine.h"
#include "Variants.h"
extern uint8_t gu8_state_change;
extern uint8_t gu8_evse_change_state;

extern enum evse_authorize_stat_t evse_authorize_state; /* Authorize State machine */

Authorize authorize;

Authorize::Authorize()
{
	idTag = String("defaultCPIDTag"); // Use a default payload. In the typical use case of this library, you probably you don't even need Authorization at all
}

Authorize::Authorize(String &idTag)
{
	this->idTag = String(idTag);
	//	EEPROM.begin(sizeof(EEPROM_Data));
	//	EEPROM.put(28,idTag);
	//	EEPROM.commit();
	//	EEPROM.end();
}

const char *Authorize::getOcppOperationType()
{
	return "Authorize";
}

DynamicJsonDocument *Authorize::createReq()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1) + (idTag.length() + 1));
	// send to Authorize only when Available.
	JsonObject payload = doc->to<JsonObject>();
	payload["idTag"] = idTag;
	return doc;
	/*
	ChargePointStatus inferencedStatus = getChargePointStatusService()->inferenceStatus();
	if(inferencedStatus == ChargePointStatus::Available)
	{
	JsonObject payload = doc->to<JsonObject>();
	payload["idTag"] = idTag;
	return doc;
	}
	*/
}

void Authorize::processConf(JsonObject payload)
{
	String idTagInfo = payload["idTagInfo"]["status"] | "Invalid";

	if (idTagInfo.equals("Accepted"))
	{
		if (DEBUG_OUT)
			Serial.print("[Authorize] Request has been accepted!\n");
		evse_authorize_state = EVSE_AUTHORIZE_ACCEPTED;
		//  EvseDevStatus_connector_1 = flag_EVSE_Start_Transaction;
		//  evse_start_txn_state = EVSE_START_TXN_INITIATED;
		gu8_evse_change_state = EVSE_START_TXN;
		/*
		 * @brief : Change made by G. Raja Sumant for removing reservation.
		 */
		getChargePointStatusService()->setReserved(false);
		ChargePointStatusService *cpStatusService = getChargePointStatusService();
		if (cpStatusService != NULL)
		{
			cpStatusService->authorize();
		}
	}
	else
	{
		evse_authorize_state = EVSE_AUTHORIZE_REJECTED;
		Serial.print("[Authorize] Request has been denied!");
	}
}

void Authorize::processReq(JsonObject payload)
{
	/*
	 * Ignore Contents of this Req-message, because this is for debug purposes only
	 */
}

DynamicJsonDocument *Authorize::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(2 * JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	JsonObject idTagInfo = payload.createNestedObject("idTagInfo");
	idTagInfo["status"] = "Accepted";
	return doc;
}
