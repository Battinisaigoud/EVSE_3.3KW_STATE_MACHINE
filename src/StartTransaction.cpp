// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\StartTransaction.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "StartTransaction.h"
#include "TimeHelper.h"
#include "OcppEngine.h"
#include "MeteringService.h"
#include "LCD_I2C.h"

bool flag_start_accepted = false;
//added by sai
extern LCD_I2C lcd;

int globalmeterstartA = 0;
unsigned long st_timeA = 0;

StartTransaction Start_Transaction;

StartTransaction::StartTransaction() {
	if (getChargePointStatusService() != NULL) {
		if (!getChargePointStatusService()->getIdTag().isEmpty()) {
			idTag = String(getChargePointStatusService()->getIdTag());
			connectorId = getChargePointStatusService()->getConnectorId();
		}
	}
	//if (idTag.isEmpty()) idTag = String("wrongIDTag"); //Use a default payload. In the typical use case of this library, you probably you don't even need Authorization at all
}

StartTransaction::StartTransaction(String &idTag) {
	this->idTag = String(idTag);
}

StartTransaction::StartTransaction(String &idTag, int &connectorId) {
	this->idTag = String(idTag);
	this->connectorId = connectorId;
}


const char* StartTransaction::getOcppOperationType(){
	return "StartTransaction";
}

DynamicJsonDocument* StartTransaction::createReq() {

	/*if(getChargePointStatusService()->inferenceStatus() != ChargePointStatus::Available )
	{
		accepted = false;
		return;
	}*/

	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(5) + (JSONDATE_LENGTH + 1) + (idTag.length() + 1));
	JsonObject payload = doc->to<JsonObject>();

	payload["connectorId"] = connectorId;
	MeteringService* meteringService = getMeteringService();
	if (meteringService != NULL) {
		payload["meterStart"] =(int) meteringService->currentEnergy();
	}
	globalmeterstartA = payload["meterStart"];
	st_timeA = millis();
	char timestamp[JSONDATE_LENGTH + 1] = {'\0'};
	getJsonDateStringFromGivenTime(timestamp, JSONDATE_LENGTH + 1, now());
	payload["timestamp"] = timestamp;
	payload["idTag"] = idTag;

	return doc;
}

void StartTransaction::processConf(JsonObject payload) {

	const char* idTagInfoStatus = payload["idTagInfo"]["status"] | "Invalid";
	int transactionId = payload["transactionId"] | -1;
	//EEPROM.begin(sizeof(EEPROM_Data));
	//EEPROM.put(68, transactionId);
	//EEPROM.commit();
	//EEPROM.end();

	if (!strcmp(idTagInfoStatus, "Accepted")|| !strcmp(idTagInfoStatus, "ConcurrentTx")) {
		if (DEBUG_OUT) Serial.println("[StartTransaction] Request has been accepted!\n");
		evse_start_txn_state = EVSE_START_TXN_ACCEPTED;

#if LCD_DISPLAY
		lcd.clear();
		lcd.setCursor(0, 1);
		lcd.print("AUTHENTICATION ");
		lcd.setCursor(0, 2);
		lcd.print("SUCCESSFUL");
		Serial.println("AUTHENTICATION SUCCESS\n");
		// delay(1500);
#endif
		flag_start_accepted = true;
		//added by sai
		//  evse_ChargePointStatus = Preparing;
		ChargePointStatusService *cpStatusService = getChargePointStatusService();
		if (cpStatusService != NULL){
			cpStatusService->startTransaction(transactionId);
			cpStatusService->startEnergyOffer();
		}

		SmartChargingService *scService = getSmartChargingService();
		if (scService != NULL)
		{
			scService->beginChargingNow();
		}
		

	} else {
		Serial.print(F("[StartTransaction] Request has been denied!\n"));
			evse_start_txn_state = EVSE_START_TXN_REJECTED;

#if LCD_DISPLAY
		lcd.clear();
		lcd.setCursor(0, 1);
		lcd.print("AUTHENTICATION ");
		lcd.setCursor(0, 2);
		lcd.print("DENIED");

#endif
	}
}


void StartTransaction::processReq(JsonObject payload) {

	/**
	* Ignore Contents of this Req-message, because this is for debug purposes only
	*/

}

DynamicJsonDocument* StartTransaction::createConf(){
	DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2));
	JsonObject payload = doc->to<JsonObject>();

	JsonObject idTagInfo = payload.createNestedObject("idTagInfo");
	idTagInfo["status"] = "Accepted";
	payload["transactionId"] = 123456; //sample data for debug purpose

	return doc;
}


