// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\GetConfiguration.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "GetConfiguration.h"
#include "OcppEngine.h"
#include "Variants.h"

extern bool wifi_connect;
extern unsigned int heartbeatInterval;
extern unsigned int meterSampleInterval;

GetConfiguration Get_Configuration;

GetConfiguration::GetConfiguration() {
    
}

const char* GetConfiguration::getOcppOperationType(){
    return "GetConfiguration";
}

DynamicJsonDocument* GetConfiguration::createReq() {
	DynamicJsonDocument* doc = new DynamicJsonDocument(0);
  JsonObject payload = doc->to<JsonObject>();
   /*
    * empty payload
    */
  return doc;
}

  void GetConfiguration::processReq(JsonObject payload){
  	Serial.println(F("*******Received GetConfiguration Request*******"));
  }

void GetConfiguration::processConf(JsonObject payload){
    String status = payload["status"] | "Invalid";

    if (status.equals("Accepted")) {
        if (DEBUG_OUT) Serial.println("[GetConfiguration] Request has been accepted!\n");
    } else {
        Serial.println("[GetConfiguration] Request has been denied!");
    }
}

DynamicJsonDocument* GetConfiguration::createConf(){
  	DynamicJsonDocument *doc = new DynamicJsonDocument(3072);
	JsonObject payload = doc->to<JsonObject>();

	JsonArray configurationKey = payload.createNestedArray("configurationKey");

//	if(wifi_connect == true){    // as this is not working with gsm
	JsonObject configurationKey_0 = configurationKey.createNestedObject();
	configurationKey_0["key"] = "AuthorizationCacheEnabled";
	configurationKey_0["readonly"] = true;
	configurationKey_0["value"] = "False";

	JsonObject configurationKey_1 = configurationKey.createNestedObject();
	configurationKey_1["key"] = "AuthorizeRemoteTxRequests";
	configurationKey_1["readonly"] = true;
	configurationKey_1["value"] = "False";

	JsonObject configurationKey_2 = configurationKey.createNestedObject();
	configurationKey_2["key"] = "ClockAlignedDataInterval";
	configurationKey_2["readonly"] = true;
	//configurationKey_2["value"] = "120"; 
	/*
	* @brief : Feature modified by G. Raja Sumant
	* 08/07/2022
	* Since we do not have a clocked aligned event, we are ignoring it.
	* We give periodic meter values only.
	*/
	configurationKey_2["value"] = "0"; //A value of "0" (numeric zero), by convention, is to be interpreted to mean that no clock-aligned data should be transmitted.

	JsonObject configurationKey_3 = configurationKey.createNestedObject();
	configurationKey_3["key"] = "ConnectionTimeOut";
	configurationKey_3["readonly"] = true;
	configurationKey_3["value"] = "120";

	JsonObject configurationKey_4 = configurationKey.createNestedObject();
	configurationKey_4["key"] = "GetConfigurationMaxKeys";
	configurationKey_4["readonly"] = true;
	configurationKey_4["value"] = "1";

	JsonObject configurationKey_5 = configurationKey.createNestedObject();
	configurationKey_5["key"] = "HeartbeatInterval";
	configurationKey_5["readonly"] = false;
	configurationKey_5["value"] = /*"60";*/String(heartbeatInterval);

	JsonObject configurationKey_6 = configurationKey.createNestedObject();
	configurationKey_6["key"] = "LocalPreAuthorize";
	configurationKey_6["readonly"] = true;
	configurationKey_6["value"] = "False";

	JsonObject configurationKey_7 = configurationKey.createNestedObject();
	configurationKey_7["key"] = "MeterValuesAlignedData";
	configurationKey_7["readonly"] = true;
	configurationKey_7["value"] = "Power.Active.Import, Energy.Active.Import.Register, Voltage, Current.Import, Temperature";

	JsonObject configurationKey_8 = configurationKey.createNestedObject();
	configurationKey_8["key"] = "MeterValuesSampledData";
	configurationKey_8["readonly"] = true;
	configurationKey_8["value"] = "Energy.Active.Import.Register";

	JsonObject configurationKey_9 = configurationKey.createNestedObject();
	configurationKey_9["key"] = "MeterValueSampleInterval";
	configurationKey_9["readonly"] = false;
	configurationKey_9["value"] = String(meterSampleInterval);/*"60";*/

	JsonObject configurationKey_10 = configurationKey.createNestedObject();
	configurationKey_10["key"] = "MinimumStatusDuration";
	configurationKey_10["readonly"] = true;
	configurationKey_10["value"] = "1";

	JsonObject configurationKey_11 = configurationKey.createNestedObject();
	configurationKey_11["key"] = "NumberOfConnectors";
	configurationKey_11["readonly"] = true;
	configurationKey_11["value"] = "1";   // for POD and Infinity

	JsonObject configurationKey_12 = configurationKey.createNestedObject();
	configurationKey_12["key"] = "ResetRetries";
	configurationKey_12["readonly"] = true;
	configurationKey_12["value"] = "5";

	JsonObject configurationKey_13 = configurationKey.createNestedObject();
	configurationKey_13["key"] = "StopTransactionOnEVSideDisconnect";
	configurationKey_13["readonly"] = true;
	configurationKey_13["value"] = "True";

	JsonObject configurationKey_14 = configurationKey.createNestedObject();
	configurationKey_14["key"] = "StopTransactionOnInvalidId";
	configurationKey_14["readonly"] = true;
	configurationKey_14["value"] = "True";

	JsonObject configurationKey_15 = configurationKey.createNestedObject();
	configurationKey_15["key"] = "SupportedFeatureProfiles";
	configurationKey_15["readonly"] = true;
	configurationKey_15["value"] = "core,RemoteTrigger,Reservation";/*,RemoteTrigger";*/

	// JsonObject configurationKey_16 = configurationKey.createNestedObject();
	// configurationKey_16["key"] = "SupportedFeatureProfilesMaxLength";
	// configurationKey_16["readonly"] = true;
	// configurationKey_16["value"] = "10"; //It should be 1 ideally

	// JsonObject configurationKey_17 = configurationKey.createNestedObject();
	// configurationKey_17["key"] = "LocalAuthListEnabled";
	// configurationKey_17["readonly"] = true;
	// configurationKey_17["value"] = "False";

	// JsonObject configurationKey_18 = configurationKey.createNestedObject();
	// configurationKey_18["key"] = "LocalAuthListMaxLength";
	// configurationKey_18["readonly"] = true;
	// configurationKey_18["value"] = "10";

	// JsonObject configurationKey_19 = configurationKey.createNestedObject();
	// configurationKey_19["key"] = "SendLocalListMaxLength";
	// configurationKey_19["readonly"] = true;
	// configurationKey_19["value"] = "10";

	// JsonObject configurationKey_20 = configurationKey.createNestedObject();
	// configurationKey_20["key"] = "ReserveConnectorZeroSupported";
	// configurationKey_20["readonly"] = true;
	// configurationKey_20["value"] = "False";
//}
	JsonArray unknownKey = payload.createNestedArray("unknownKey");

	return doc;

}




