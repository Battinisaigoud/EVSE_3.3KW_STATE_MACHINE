// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\Master.cpp"
#include "Master.h"
#include "Variants.h"
#include "dwin.h"
#include "display_meterValues.h"

SoftwareSerial masterSerial(25,33); //25 Rx, 33 Tx

//#define masterSerial Serial1

#define alprSerial Serial1

// DynamicJsonDocument txM_doc(200);
// DynamicJsonDocument rxM_doc(200);
DynamicJsonDocument txM_doc(600);
DynamicJsonDocument rxM_doc(600);

DynamicJsonDocument txA_doc(200);
DynamicJsonDocument rxA_doc(200);

extern String currentIdTag;
// extern String expiry_time;
String expiry_time;
bool flag_GFCI_set_here = false;

extern bool flag_noVehicle;
bool Offline_idTag_is_valid = false;
bool Offline_idTagStatus_is_valid = false;
uint8_t gu8_lla_updateType_is_valid = false;

uint16_t gu16_locallist_ver = 0;
bool authCache_idTag_is_valid = false;
bool ClearCache_status = false;

extern int8_t button;

extern unsigned char fault_emgy[28];

void customflush(){
  while(masterSerial.available() > 0)
    masterSerial.read();
}

#if 0
/*
  @brief : Read the touch display
*/
#if DWIN_ENABLED
int8_t dwin_input()
{

  button = DWIN_read();
  Serial.printf("Button pressed : %d", button);
  //delay(50);
  return button;

}
#endif
#endif 

int requestConnectorStatus(){
  txM_doc.clear();
  rxM_doc.clear();

  
  const char* type = "";
  int connectorId = 0;
  int startTime = 0;
  bool success = false;
  
  Serial.println("Master: ");
  txM_doc["type"] = "request";
  txM_doc["object"] = "connector";
  
  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(100);
  
  startTime = millis();
  while(millis() - startTime < WAIT_TIMEOUT){ // waiting for response from slave
    if(masterSerial.available()){
      
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.println(" Invalid input! Not a JSON\n");
          break;
        case DeserializationError::NoMemory:
          Serial.println("Error: Not enough memory\n");
          break;
        default:
          Serial.println("Deserialization failed\n");
          break;
     }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(200);
      customflush();
      Serial.println("***Sending request again***");
      delay(100);
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      serializeJson(txM_doc, Serial);
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      connectorId = rxM_doc["connectorId"];
      if(strcmp(type , "response") == 0){
        Serial.println("Received connectorId from slave--->" + String(connectorId));
        return connectorId;
      }
    }
   }
   
 }
  Serial.println("No response from Slave");
  return connectorId;  //returns 0 if connectorId (push button is not pressed)
  
}


bool requestForRelay(int action , int connectorId){
  
  if(connectorId == 0 || connectorId > 3){
    return false;
  }

  // DynamicJsonDocument txM_doc(200);
  // DynamicJsonDocument rxM_doc(200);

  txM_doc.clear();
  txM_doc.clear();

  const char* type = "";
  const char* object = "";
  
  //const char* statusRelay = "";
  int statusRelay=0;
  bool success = false;
  int startTime = 0;
  
  Serial.println("Relay:");
  txM_doc["type"] = "request";
  txM_doc["object"] = "relay";
  
  txM_doc["action"] = action;
  txM_doc["connectorId"] = connectorId;

  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(2000);

  startTime = millis();
  while(millis() - startTime < 6000){ // waiting for response from slave
    if(masterSerial.available()){ 
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.println(" Invalid input! Not a JSON\n");
          break;
        case DeserializationError::NoMemory:
          Serial.println("Error: Not enough memory\n");
          break;
        default:
          Serial.println("Deserialization failed\n");
          break;
      }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(200);
      customflush();
      Serial.println("***Sending request again***");
      delay(100);
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      statusRelay = rxM_doc["status"];
      connectorId = rxM_doc["connectorId"];
      if((strcmp(type , "response") == 0) && (action == statusRelay)){
        Serial.println("Received Status--->" +String(statusRelay)+" for ConnectorId: "+ String(connectorId));
        return true;
      }
    }
   }//end of .available 
  }//end of while loop
  Serial.println("No response from Slave");
  return false;
}

bool requestLed(int colour, int action , int connectorId){
  
 /*  if(connectorId == 0 || connectorId > 4){
    return false;
  } */
  if(connectorId > 4){
    return false;
  }

  // DynamicJsonDocument txM_doc(200);
  // DynamicJsonDocument rxM_doc(200);
  txM_doc.clear();
  rxM_doc.clear();

  const char* type = "";
  const char* object = "";
  
  //const char* statusRelay = "";
  int statusLed=0;
  bool success = false;
  int startTime = 0;
  
  Serial.println("Led:");
  txM_doc["type"] = "request";
  if(connectorId == 4){
    txM_doc["object"] = "rfid";
  }else{
    txM_doc["object"] = "led";
  }
  txM_doc["colour"] = colour;
  txM_doc["action"] = action;
  txM_doc["connectorId"] = connectorId;

  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while(millis() - startTime < WAIT_TIMEOUT){ // waiting for response from slave
    if(masterSerial.available()){ 
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.println(" Invalid input! Not a JSON\n");
          break;
        case DeserializationError::NoMemory:
          Serial.println("Error: Not enough memory\n");
          break;
        default:
          Serial.println("Deserialization failed\n");
          break;
      }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(200);
      customflush();
      Serial.println("***Sending request again***");
      delay(100);
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      connectorId = rxM_doc["connectorId"];
      statusLed = rxM_doc["status"];
      if((strcmp(type , "response") == 0) && (action == statusLed)){
        Serial.println("Received Status--->" +String(statusLed)+" for ConnectorId: "+ String(connectorId));
        return true;
      }
    }
   }//end of .available 
  }//end of while loop
  Serial.println("TimeOut");
  return false;
}

bool requestEmgyStatus(){

  // DynamicJsonDocument txM_doc(100);
  // DynamicJsonDocument rxM_doc(100);
  txM_doc.clear();
  rxM_doc.clear();

  const char* type = "";
  const char* object = "";
  bool success = false;
  int startTime = 0;
  bool statusE = 0;
  Serial.println("EMGY:");
  txM_doc["type"] = "request";
  txM_doc["object"] = "emgy";

  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while(millis() - startTime < WAIT_TIMEOUT){ // waiting for response from slave
    if(masterSerial.available()){ 
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.println(" Invalid input! Not a JSON\n");
          break;
        case DeserializationError::NoMemory:
          Serial.println("Error: Not enough memory\n");
          break;
        default:
          Serial.println("Deserialization failed\n");
          break;
      }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(200);
      customflush();
      Serial.println("***Sending request again***");
      delay(100);
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      object = rxM_doc["object"] | "Invalid";
      statusE = rxM_doc["status"];
      if((strcmp(type , "response") == 0) && (strcmp(object,"emgy") == 0)){
        Serial.println("Received Status--->" +String(statusE));
        if(statusE == true){ 
          return true;
        }else if(statusE ==false){
          return false;
        }
      }
     }
   }//end of .available 
  }//end of while loop
  Serial.println("TimeOut, try again");
  //return true;
  return false;
}



//CP
bool requestforCP_OUT(int action){

  // DynamicJsonDocument txM_doc(200);
  // DynamicJsonDocument rxM_doc(200);
  txM_doc.clear();
  rxM_doc.clear();

  const char* type = "";
  const char* object = "";
  
  bool success = false;
  int startTime = 0;
  bool status = 0;
  
  Serial.println("Control Pilot:");
  txM_doc["type"] = "request";
  txM_doc["object"] = "cpout";
  
  txM_doc["action"] = action;
//  txM_doc["connectorId"] = connectorId;   future Implemnetation

  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while(millis() - startTime < WAIT_TIMEOUT){ // waiting for response from slave
    if(masterSerial.available()){ 
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.println(" Invalid input! Not a JSON\n");
          break;
        case DeserializationError::NoMemory:
          Serial.println("Error: Not enough memory\n");
          break;
        default:
          Serial.println("Deserialization failed\n");
          break;
      }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(200);
      customflush();
      Serial.println("***Sending request again***");
      delay(100);
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      object = rxM_doc["object"] | "Invalid";
      status = rxM_doc["status"];
      if((strcmp(type , "response") == 0) && (strcmp(object,"cpout") == 0)){
        Serial.println("Received Status--->" +String(status));
        return true;
      }
    }
   }//end of .available 
  }//end of while loop
  Serial.println("No response from Slave");
  return false;
}

int requestforCP_IN(){

  // DynamicJsonDocument txM_doc(200);
  // DynamicJsonDocument rxM_doc(200);
  txM_doc.clear();
  rxM_doc.clear();

  const char* type = "";
  const char* object = "";
  
  bool success = false;
  int startTime = 0;
  int value = 0;
  
  Serial.println("Control Pilot:");
  txM_doc["type"] = "request";
  txM_doc["object"] = "cpin";
  
//  txM_doc["connectorId"] = connectorId;   future Implemnetation

  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while(millis() - startTime < WAIT_TIMEOUT){ // waiting for response from slave
    if(masterSerial.available()){ 
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.println(" Invalid input! Not a JSON\n");
          break;
        case DeserializationError::NoMemory:
          Serial.println("Error: Not enough memory\n");
          break;
        default:
          Serial.println("Deserialization failed\n");
          break;
      }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(200);
      customflush();
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      object = rxM_doc["object"] | "Invalid";
      value = rxM_doc["value"];
      if((strcmp(type , "response") == 0) && (strcmp(object,"cpin") == 0)){
        Serial.println("Received value--->" +String(value));
        return value;
      }
    }
   }//end of .available 
  }//end of while loop
  Serial.println("No response from Slave");
  return false;
}

//For ARAI
#if 0
bool requestGfciStatus()
{
return false;
}
#endif
/*
 * @brief: requestGfciStatus - this will request the status from the secondary controller.
 * Added by G. Raja Sumant to fix the bug of POD generic code missign GFCI.
 */
#if 1
bool requestGfciStatus(){

  // DynamicJsonDocument txM_doc(100);
  // DynamicJsonDocument rxM_doc(100);
  txM_doc.clear();
  rxM_doc.clear();

  const char* type = "";
  const char* object = "";
  bool success = false;
  int startTime = 0;
  bool statusE = 0;
  Serial.println("GFCI:");
  txM_doc["type"] = "request";
  txM_doc["object"] = "gfci";

  serializeJson(txM_doc,masterSerial);     //data send to slave   
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while(millis() - startTime < WAIT_TIMEOUT){ // waiting for response from slave
    if(masterSerial.available()){ 
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.println(" Invalid input! Not a JSON\n");
          break;
        case DeserializationError::NoMemory:
          Serial.println("Error: Not enough memory\n");
          break;
        default:
          Serial.println("Deserialization failed\n");
          break;
      }

     if (!success) {
      //propably the payload just wasn't a JSON message
      rxM_doc.clear();
      delay(200);
      customflush();
      Serial.println("***Sending request again***");
      delay(100);
      serializeJson(txM_doc, masterSerial);     // data send to slave again
      //return false;
    }else{
      type = rxM_doc["type"] | "Invalid";
      object = rxM_doc["object"] | "Invalid";
      statusE = rxM_doc["status"];
      if((strcmp(type , "response") == 0) && (strcmp(object,"gfci") == 0)){
        Serial.println("Received Status--->" +String(statusE));
        if(statusE == true){ 
          flag_GFCI_set_here = true;
          return true;
        }else if(statusE ==false){
          return false;
        }
      }
     }
   }//end of .available 
  }//end of while loop
  Serial.println("TimeOut");
  return false;
}
#endif


#if ALPR_ENABLED
void alprRead_loop(){
  rxA_doc.clear();
  txA_doc.clear();

  bool success = false;

  const char* licenseData = "";
  char s[500] = {0};
  int i=0;
  String payload;

if(alprSerial.available()){ 

  // Serial.println(alprSerial.readLine().decode("utf-8").rstrip());
  //while(alprSerial.available()>0){
  payload = alprSerial.readString();
  //  Serial.println(s);
  //}

  Serial.println(payload);
      delay(1000);
     // ReadLoggingStream loggingStream(alprSerial, Serial);
      DeserializationError err = deserializeJson(rxA_doc, payload);
      //  Serial.print(rxM_doc);
      switch (err.code()) {
        case DeserializationError::Ok:
          success = true;
          break;
        case DeserializationError::InvalidInput:
          Serial.print(F(" Invalid input! Not a JSON\n"));
          break;
        case DeserializationError::NoMemory:
          Serial.print(F("Error: Not enough memory\n"));
          break;
        default:
          Serial.print(F("Deserialization failed\n"));
          break;
      }

      if (!success) {
        //propably the payload just wasn't a JSON message
        rxA_doc.clear();
        delay(1000);
      }else{
        //licenseData = rxA_doc["LicensePlate"];
        licenseData = rxA_doc["license"];
        
        String data = String(licenseData);

        if((strcmp(licenseData , "novehicle") == 0)) 
        {
          txA_doc["ESP_Status"] = "OK";
          flag_noVehicle = true;
          serializeJson(txA_doc,alprSerial);     //data send to slave   
          serializeJson(txA_doc, Serial);
          delay(100);
          return;
        }

        if((strcmp(licenseData , "") != 0)) {

          
          Serial.println("Received value--->" +String(licenseData));

          getChargePointStatusService()->authorize(data);
          
          txA_doc["ESP_Status"] = "OK";
          serializeJson(txA_doc,alprSerial);     //data send to slave   
          serializeJson(txA_doc, Serial);
          delay(100);


        }
      }
  }

}


void alprAuthorized() {
    txA_doc.clear();  
    txA_doc["Authorization"] = 1;
    serializeJson(txA_doc, alprSerial);
    serializeJson(txA_doc, Serial);
    Serial.println();
    delay(1000);
}

void alprunAuthorized() {
    txA_doc.clear();  
    txA_doc["Authorization"] = 0;
    serializeJson(txA_doc, alprSerial);
    serializeJson(txA_doc, Serial);
    Serial.println();
    delay(1000);
}


void alprTxnStarted() {
    txA_doc.clear(); 
    txA_doc["Txn_Started"] = 1;
    serializeJson(txA_doc, alprSerial);
    serializeJson(txA_doc, Serial);
    Serial.println();
    delay(1000);
}

void alprTxnStopped() {
    txA_doc.clear();
    txA_doc["Txn_Stopped"] = 0;
    serializeJson(txA_doc, alprSerial);
    serializeJson(txA_doc, Serial);
    Serial.println();
    delay(1000);
}
#endif 


/************************************************* Authentication cache  (SQL DataBase)************************************/
  bool requestSendAuthCache(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;
  time_t expiryDate_epoch = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "SendAuthCache";

  Serial.println("SendAuthCache:");
  
  // Read the idTag from Authentication/Start/Stop confirmation packet
  txM_doc["idTag"] = currentIdTag;

	if (expiry_time.equals("Invalid") == true) 
  {
    txM_doc["expiryTime"] = (19800 + now());
  }
  else
  {
    bool success = getTimeFromJsonDateString(expiry_time.c_str(), &expiryDate_epoch);
		if(success)
		{
      txM_doc["expiryTime"] = expiryDate_epoch;
    }
  }

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.println(" Invalid input! Not a JSON\n");
        break;
      case DeserializationError::NoMemory:
        Serial.println("Error: Not enough memory\n");
        break;
      default:
        Serial.println("Deserialization failed\n");
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        statusLL = rxM_doc["status"];
        object = rxM_doc["object"];
        
        if ((strcmp(type, "response") == 0) && (strcmp(object, "SendAuthCache") == 0))
        {
          Serial.println("Received Status--->" + String(statusLL));
          return true;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}  


#if 1

bool requestGetAuthCache(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "GetAuthCache";
  txM_doc["idTag"] = currentIdTag;
  Serial.println("GetidTag:");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.println(" Invalid input! Not a JSON\n");
        break;
      case DeserializationError::NoMemory:
        Serial.println("Error: Not enough memory\n");
        break;
      default:
        Serial.println("Deserialization failed\n");
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        status = rxM_doc["status"];

        if ((strcmp(type, "response") == 0) && (strcmp(object, "GetAuthCache") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
          if (strcmp(status, "Accepted") == 0)
          {
            authCache_idTag_is_valid = true;
          }
          else
          {
            authCache_idTag_is_valid = false;
          }
          return authCache_idTag_is_valid;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}

#endif

bool requestClearCache(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "ClearCache";
  
  Serial.println("ClearCache");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.println(" Invalid input! Not a JSON\n");
        break;
      case DeserializationError::NoMemory:
        Serial.println("Error: Not enough memory\n");
        break;
      default:
        Serial.println("Deserialization failed\n");
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        status = rxM_doc["status"];

        if ((strcmp(type, "response") == 0) && (strcmp(object, "ClearCache") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
          if (strcmp(status, "Accepted") == 0)
          {
            ClearCache_status = true;
          }
          else
          {
            ClearCache_status = false;
          }
          return authCache_idTag_is_valid;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}
/************************************************* Authentication cache  (SQL DataBase)************************************/
      

/************************************************* Local Authentication List (SQL DataBase) ************************************/

bool requestSendLocalList(JsonObject payload)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "SendLocalList";

  Serial.println("SendLocalList:");

  txM_doc["locallist"] = payload;

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.println(" Invalid input! Not a JSON\n");
        break;
      case DeserializationError::NoMemory:
        Serial.println("Error: Not enough memory\n");
        break;
      default:
        Serial.println("Deserialization failed\n");
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        statusLL = rxM_doc["status"];
        if ((strcmp(type, "response") == 0) && (strcmp(object, "sendLocallist") == 0))
        {
          Serial.println("Received Status--->" + String(statusLL));
          return true;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}

bool requestGetLocalListVersion(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "GetLocalListVersion";

  Serial.println("GetLocalListVersion:");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.println(" Invalid input! Not a JSON\n");
        break;
      case DeserializationError::NoMemory:
        Serial.println("Error: Not enough memory\n");
        break;
      default:
        Serial.println("Deserialization failed\n");
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        statusLL = rxM_doc["listVersion"];
        gu16_locallist_ver = statusLL;
        if ((strcmp(type, "response") == 0) && (strcmp(object, "GetLocalListVersion") == 0))
        {
          Serial.println("Received listVersion--->" + String(statusLL));
          return true;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}

bool requestGetidTag(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "GetidTag";
  txM_doc["idTag"] = currentIdTag;
  Serial.println("GetidTag:");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.println(" Invalid input! Not a JSON\n");
        break;
      case DeserializationError::NoMemory:
        Serial.println("Error: Not enough memory\n");
        break;
      default:
        Serial.println("Deserialization failed\n");
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        status = rxM_doc["status"];

        if ((strcmp(type, "response") == 0) && (strcmp(object, "GetidTag") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
          if (strcmp(status, "Accepted") == 0)
          {
            Offline_idTag_is_valid = true;
          }
          else
          {
            Offline_idTag_is_valid = false;
          }
          return Offline_idTag_is_valid;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}

bool requestGetidTagStatus(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *status = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "GetidTagStatus";
  txM_doc["idTag"] = currentIdTag;
  Serial.println("GetidTagStatus:");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.println(" Invalid input! Not a JSON\n");
        break;
      case DeserializationError::NoMemory:
        Serial.println("Error: Not enough memory\n");
        break;
      default:
        Serial.println("Deserialization failed\n");
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        status = rxM_doc["status"];

        if ((strcmp(type, "response") == 0) && (strcmp(object, "GetidTagStatus") == 0))
        {
          Serial.println("Received GetidTag Status--->" + String(status));
        if(strcmp(status , "Accepted") == 0)
        {
            Offline_idTag_is_valid = true;
        }
        else
        {
            Offline_idTag_is_valid = false;
        }
        return true;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}

bool requestGetupdateType(void)
{

  txM_doc.clear();
  rxM_doc.clear();

  const char *type = "";
  const char *object = "";
  const char *updateType = "";

  int statusLL = 0;
  bool success = false;
  int startTime = 0;

  txM_doc["type"] = "request";
  txM_doc["object"] = "GetupdateType";
  Serial.println("GetupdateType:");

  serializeJson(txM_doc, masterSerial); // data send to slave
  serializeJson(txM_doc, Serial);
  delay(100);

  startTime = millis();
  while (millis() - startTime < WAIT_TIMEOUT)
  { // waiting for response from slave
    if (masterSerial.available())
    {
      ReadLoggingStream loggingStream(masterSerial, Serial);
      DeserializationError err = deserializeJson(rxM_doc, loggingStream);
      //  Serial.print(rxM_doc);
      switch (err.code())
      {
      case DeserializationError::Ok:
        success = true;
        break;
      case DeserializationError::InvalidInput:
        Serial.println(" Invalid input! Not a JSON\n");
        break;
      case DeserializationError::NoMemory:
        Serial.println("Error: Not enough memory\n");
        break;
      default:
        Serial.println("Deserialization failed\n");
        break;
      }

      if (!success)
      {
        // propably the payload just wasn't a JSON message
        rxM_doc.clear();
        Serial.println("***Sending request again***");
        delay(1000);
        serializeJson(txM_doc, masterSerial); // data send to slave again
        // return false;
      }
      else
      {
        type = rxM_doc["type"];
        object = rxM_doc["object"];
        updateType = rxM_doc["updateType"];

        if ((strcmp(type, "response") == 0) && (strcmp(object, "GetupdateType") == 0))
        {
          Serial.println("Received GetidTag updateType--->" + String(updateType));
          if (strcmp(updateType, "Full") == 0)
          {
            gu8_lla_updateType_is_valid = 1;
          }
          else if (strcmp(updateType, "Differential") == 0)
          {
            gu8_lla_updateType_is_valid = 2;
          }
          else
          {
            gu8_lla_updateType_is_valid = 0;
          }
          return true;
        }
      }
    } // end of .available
  }   // end of while loop
  Serial.println("TimeOut");
  return false;
}
/************************************************* Local Authentication List (SQL DataBase) ************************************/



void Master_setup() {
  // put your setup code here, to run once:
  
 // Serial.begin(115200);
  // masterSerial.begin(9600, SWSERIAL_8N1, 25,33, false,256);// Required to fix the deserialization error SERIAL_8N1,26,27
  masterSerial.begin(14400, SWSERIAL_8N1, 25,33, false,256);// Required to fix the deserialization error SERIAL_8N1,26,27
  
  //masterSerial.begin(9600, SERIAL_8N1, 25,33);// Required to fix the deserialization error
  #if ALPR_ENABLED
  alprSerial.begin(115200,SERIAL_8N1,26,27);
  #endif
  Serial.println("***VERSION HISTORY***");
  // Serial.println(F("***Firmware_ver-----EVSE_3.3KW_V1.0.5***"));
  Serial.println("***Firmware_ver-----:");
  Serial.println(String(VERSION) +"***");
  

  #if V_charge_lite1_4
  Serial.println("***Hardware_ver-----VL_1.4***");
  #else
  Serial.println(F("***Hardware_ver-----VL_1.3***"));
  #endif

  #if V_charge_lite1_3
  Serial.println("***Hardware_ver-----VL_1.3***");
  #endif


  #if DWIN_ENABLED
  Serial.println(F("Integrated DWIN 3.5 Inch Display"));
  #endif

  #if LCD_DISPLAY
  Serial.println("Integrated LCD 20x4 Display");
  #endif

  Serial.println("Fixed Session Resume after power recycle with the same transaction ID");
  Serial.println("Low Current count 3min");
  Serial.println("Low Current Threshold 100 milli amps");
  Serial.println("Master serial port baudrate is changed from 9600 to 14400");
  Serial.println("while EVSE device is charging:");
  Serial.println("\t1. Connectivity is changed from Online to offline then, meter values not pushed in the OCCP out-going Message Queues.");
  Serial.println("\t2. if Device restart, it will check onging transaction is exist or not, if yes then resume it, with Relay ON, green and white led blink will indicated.");
  Serial.println("\t3. if Internet off and on, then it will reconnect the internet and update ocpp message to CMS and update valid meter values to CMS.");
  Serial.println("Added GSM based functionality.");
  Serial.println("removed the Wifi on-Event for checking wifi-level Connectivity.");
  Serial.println("Starts device charging even-though the start transaction response is ConcurrentTx.");
  Serial.println("Last update date:06-04-2023");
  Serial.println("Updated by: Krishna Marri");
  //added by sai 
   Serial.println("Implemented of bluetooth configaration parameters and parameters functions");
   Serial.println("Implemented Low current count should be configurable through Bluetooth Application.");
   Serial.println("Implemented Low currnet threshold should be configurable through Bluetooth Application.");
   Serial.println("Implemented Session resume on power recycle should be configurable.");
   Serial.println("Implemented 4G/WiFi OTA configuration ");
   Serial.println("Implemented Online-Offline Functionality configurable");
   Serial.println("URL parser changed:Added compatible code suitable even port number is not given");
   Serial.println("Last update date:04-11-2023");
  Serial.println("Updated by: sai goud");



  Serial.println("Complete core profile, reserve now and ALPR integrated!");
  Serial.print("[MASTER] ESP32 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

}

/*
void loop() {
  // put your main code here, to run repeatedly:
   //requestLed(RED,START,1);

  bool emgcy = requestEmgyStatus();
  Serial.println("Emergency Button Status--> " +String(emgcy));
  if(emgcy == 0){
    for(;;){
      
      Serial.println("EMGY is pressed");
       bool emgcy = requestEmgyStatus();
       if(emgcy ==1) break;
      }
  }
  int connector = requestConnectorStatus();
  Serial.println("Connector: " + String(connector)+ "\n");
  delay(1000);
  if(connector > 0){
   bool statusRelay = requestForRelay(START,connector);
   Serial.println("statusRelay--->" + String(statusRelay));
  
  delay(1000);

  bool ch1 = requestForRelay(STOP,connector);
  Serial.println("ch1 --->" +String(ch1));

  delay(1000);

  bool ch2 = requestLed(RED,START,connector);
  Serial.println("ch2--->" +String(ch2));

  delay(1000);

  requestLed(GREEN,START,connector);

  delay(1000);

    requestLed(BLUE,START,connector);

  delay(1000);
    requestLed(WHITE,START,connector);

  delay(1000);
    requestLed(RED,STOP,connector);

  //  delay(1000);
   // requestLed(RED,START,connector);
  }
}
*/