// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\BootNotification.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "BootNotification.h"
#include "EVSE.h"
#include "OcppEngine.h"

#include <string.h>
#include "TimeHelper.h"

extern unsigned int heartbeatInterval;

BootNotification bootNotification;

BootNotification::BootNotification()
{
}

const char *BootNotification::getOcppOperationType()
{
	return "BootNotification";
}

DynamicJsonDocument *BootNotification::createReq()
{
	String cpSerial = String('\0');
	EVSE_getChargePointSerialNumber(cpSerial);
	// DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(4) + strlen(EVSE_getChargePointVendor()) + 1 + cpSerial.length() + 1 + strlen(EVSE_getChargePointModel()) + 25 + 1);
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(4) + CHARGE_POINT_VENDOR_SIZE + 1 + cpSerial.length() + 1 + CHARGE_POINT_MODEL_SIZE + 25 + 1);
	JsonObject payload = doc->to<JsonObject>();
	payload["chargePointVendor"] = CHARGE_POINT_VENDOR; /* EVSE_getChargePointVendor(); */
	payload["chargePointSerialNumber"] = cpSerial;
	payload["chargePointModel"] = CHARGE_POINT_MODEL; /* EVSE_getChargePointModel(); */
	payload["firmwareVersion"] = VERSION;
	return doc;
}

void BootNotification::processConf(JsonObject payload)
{
	const char *currentTime = payload["currentTime"] | "Invalid";
	if (strcmp(currentTime, "Invalid"))
	{
		setTimeFromJsonDateString(currentTime);
	}
	else
	{
		Serial.print("[BootNotification] Error reading time string. Expect format like 2020-02-01T20:53:32.486Z\n");
	}
	/*
	 * @brief interval - this is the the heartbeat interval.
	 * This feature was requested by JioBP. Making this change so that the heartbeat sampling interval would change.
	 */

	heartbeatInterval = payload["interval"] | 30; // requested by JioBP. Setting the default value as 50

	const char *status = payload["status"] | "Invalid";

	if (!strcmp(status, "Accepted"))
	{
		if (DEBUG_OUT)
			Serial.println("[BootNotification] Request has been accepted!\n");
		evse_boot_state = EVSE_BOOT_ACCEPTED;
		if (getChargePointStatusService() != NULL)
		{
			getChargePointStatusService()->boot();
		}
	}
	else
	{
		Serial.print("[BootNotification] Request unsuccessful!\n");
	}
}

void BootNotification::processReq(JsonObject payload)
{
	/*
	 * Ignore Contents of this Req-message, because this is for debug purposes only
	 */
}

DynamicJsonDocument *BootNotification::createConf()
{
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(3) + (JSONDATE_LENGTH + 1));
	JsonObject payload = doc->to<JsonObject>();
	char currentTime[JSONDATE_LENGTH + 1] = {'\0'};
	getJsonDateStringFromSystemTime(currentTime, JSONDATE_LENGTH);
	payload["currentTime"] = currentTime; // currentTime
	payload["interval"] = 86400;		  // heartbeat send interval - not relevant for JSON variant of OCPP so send dummy value that likely won't break
	payload["status"] = "Accepted";
	return doc;
}
