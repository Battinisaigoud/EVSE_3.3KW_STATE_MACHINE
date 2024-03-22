// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\RemoteStopTransaction.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "RemoteStopTransaction.h"
#include "OcppEngine.h"
#include "OcppMessage.h"
#include "OcppOperation.h"
#include "SimpleOcppOperationFactory.h"

#include "ChargePointStatusService.h"
#include "StatusNotification.h"

extern WebSocketsClient webSocket;
extern bool flag_evseIsBooted;
extern bool flag_evseReadIdTag;
extern bool flag_evseAuthenticate;
extern bool flag_evseStartTransaction;
extern bool flag_evRequestsCharge;
extern bool flag_evseStopTransaction;
extern bool flag_evseUnauthorise;
extern uint8_t remotestart_txn;

extern uint8_t reasonForStop;

extern bool disp_evse_A;
//added by sai
uint8_t gu8_remote_stop_flag = 0;
extern enum EvseDevStatuse EvseDevStatus_connector_1; 
extern uint8_t gu8_evse_change_state;

RemoteStopTransaction Remote_StopTransaction;

RemoteStopTransaction::RemoteStopTransaction() {

}

const char* RemoteStopTransaction::getOcppOperationType(){
	return "RemoteStopTransaction";
}


// sample message: [2,"9f639cdf-8a81-406c-a77e-60dff3cb93eb","RemoteStopTransaction",{"transactionId":2042}]
void RemoteStopTransaction::processReq(JsonObject payload) {
	transactionId = payload["transactionId"];
	
	if (transactionId == getChargePointStatusService()->getTransactionId()){

     switch (EvseDevStatus_connector_1)
      {
        case flag_EVSE_Request_Charge:
               evse_stop_txn_state= EVSE_STOP_TXN_INITIATED;
            //    gu8_evse_change_state = EVSE_STOP_TXN;
			   EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;
		       Serial.println("Remote Stop- CMS Accepted");
			   remotestart_txn = 0;
			   payload["status"] = "Accepted";
          break;    
        default:
                Serial.println("default - Remote Stop-");
          break;
      }
      reasonForStop = 7; //Remote
	  accepted = true;


		flag_evseReadIdTag = false;
		flag_evseAuthenticate = false;
		flag_evseStartTransaction = false;
		flag_evRequestsCharge = false;
		flag_evseStopTransaction = true;
		// EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;
		flag_evseUnauthorise = false;
		reasonForStop = 7; //Remote
		gu8_remote_stop_flag = 1;
		//getChargePointStatusService()->stopTransaction();
		accepted = true;
		disp_evse_A = false;
		#if 0
			//fire StatusNotification
		//TODO check for online condition: Only inform CS about status change if CP is online
		//TODO check for too short duration condition: Only inform CS about status change if it lasted for longer than MinimumStatusDuration
		OcppOperation *statusNotification = makeOcppOperation(&webSocket,
		new StatusNotification(ChargePointStatus::Finishing));
		initiateOcppOperation(statusNotification);
#endif
	}
	else
	{
		if (DEBUG_OUT) Serial.println(F("[RemoteStopTransaction] TXN id does not match!\n"));
		if (DEBUG_OUT) Serial.println(transactionId);
		if (DEBUG_OUT) Serial.println(getChargePointStatusService()->getTransactionId());
		accepted = false;

	}
	
}

DynamicJsonDocument* RemoteStopTransaction::createConf(){
	DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();
	if(accepted)
	payload["status"] = "Accepted";
	else
	payload["status"] = "Rejected";
	return doc;
}

DynamicJsonDocument* RemoteStopTransaction::createReq() {
	DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1));
	JsonObject payload = doc->to<JsonObject>();

	payload["idTag"] = "fefed1d19876";

	return doc;
}

void RemoteStopTransaction::processConf(JsonObject payload){
	String status = payload["status"] | "Invalid";

	if (status.equals("Accepted")) {
		if (DEBUG_OUT) Serial.println("[RemoteStopTransaction] Request has been accepted!\n");
		} else {
			Serial.print(F("[RemoteStopTransaction] Request has been denied!"));
	}
}
