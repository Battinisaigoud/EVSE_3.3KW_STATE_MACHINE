// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\OTA.cpp"
#include "OTA.h"
#include "ChargePointStatusService.h"
#include "OcppEngine.h"
#include "Master.h"

extern bool wifi_connect;
unsigned long int startTimer = 0;

void setupOTA() {
    Serial.println(F("[HTTP] begin..."));
    
    int updateSize = 0;
    HTTPClient http;
  
    // configure server and url
    String post_data = "{\"version\":\"POD_OFFLINE/POD_OFFLINE.ino.esp32\", \"deviceId\":\"POD\"}";
    //String post_data = "{\"version\":\"POD_OFFLINE/POD_TECELL.ino.esp32\", \"deviceId\":\"POD\"}"; //This is Vishakha URL.
    http.begin("https://us-central1-evre-iot-308216.cloudfunctions.net/otaUpdate");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Connection", "keep-alive");

    int httpCode = http.POST(post_data);
  
    if(httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.println(F("Checking for new firmware updates..." ));
    
        // If file found at server
        if(httpCode == HTTP_CODE_OK) {
            // get lenght of document (is -1 when Server sends no Content-Length header)
            int len = http.getSize();
            updateSize = len;
            Serial.printf("[OTA] Update found, File size(bytes) : %d\n", len);
    
            // get tcp stream
            WiFiClient* client = http.getStreamPtr();

            Serial.println();
            performUpdate(*client, (size_t)updateSize);   
            Serial.println(F("[HTTP] connection closed or file end.\n"));
        }
        // If there is no file at server
        if(httpCode == HTTP_CODE_INTERNAL_SERVER_ERROR) {
            Serial.println(F("[HTTP] No Updates"));
            Serial.println();
            //ESP.restart();
        }
    }
    http.end();
}


// perform the actual update from a given stream
void performUpdate(WiFiClient& updateSource, size_t updateSize) {
    if (Update.begin(updateSize)) {
        Serial.println("...Downloading File...");
        Serial.println();
        
        // Writing Update
        size_t written = Update.writeStream(updateSource);
        if (written == updateSize) {
            Serial.println("Written : " + String(written) + "bytes successfully");
        }
        else {
            Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
        }
        if (Update.end()) {
            Serial.println(F("OTA done!"));
            if (Update.isFinished()) {
                Serial.println(F("Update successfully completed. Rebooting..."));
                Serial.println();
                ESP.restart();
            }
            else {
                Serial.println(F("Update not finished? Something went wrong!"));
            }
        }
        else {
            Serial.println("Error Occurred. Error #: " + String(Update.getError()));
        }
    }
    else{
        Serial.println(F("Not enough space to begin OTA"));
    }
}


void ota_Loop(){

	if(millis() - startTimer > 43200000){

		if((getChargePointStatusService()->inferenceStatus() == ChargePointStatus::Available  || getChargePointStatusService()->inferenceStatus() == ChargePointStatus::Faulted) && wifi_connect == true){
			requestLed(BLUE,START,1); 
			setupOTA();
			startTimer = millis();
		} 
	}
}