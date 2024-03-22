// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\bluetoothConfig.cpp"
#include "bluetoothConfig.h"

BluetoothSerial SerialBT;
//********Websocket Data********//
const char *ws_url_prefix = "";
const char *chargepoint = "";
//Ignoring the below for implementing url parser
#if 0
const char *host = "";
int port;
const char *protocol = "";
#endif

const char *apn_1 = "";

const char *ssid = "";
const char *key = "";

bool wifi = false;
bool gsm = false;

String message = "";

char *adminBT = "ESP_BT";
char *psswdBT = "54321";

char incomingChar;

StaticJsonDocument<1000> docBT;

const char *admin = "";
const char *psswd = "";
// const char* ssid = "";
// const char* key = "";

bool verified = false;
//added by sai
const char *lowcurcnt = "";
const char *mincurr = "";
bool ongoingtxpor = false;
bool otaenable = false;
bool onlineoffline = false;

/*
void wifiConnectWithStoredCredential(){

  String esid="";
  String epass="";
  int counterBT = 0;
  Serial.println("Trying to connect with wifi with stored credential");
  SerialBT.println("Trying to connect with wifi with stored credential");

  delay(1000);
  EEPROM.begin(512); //Initialasing EEPROM

  //---------------------------------------- Read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);

  SerialBT.print("SSID: ");
  SerialBT.println(esid);

  Serial.println("Reading EEPROM pass");
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass.c_str());

  SerialBT.println("PASS:*******");

  WiFi.begin(esid.c_str(), epass.c_str());
  delay(1000);
  SerialBT.println("Waiting for connection");
  while(WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);

    Serial.print('.');
    if(counterBT++ > 60){
      Serial.println("Invalid WiFi Credential");
      SerialBT.println("\nInvalid WiFi Credential, Bluetooth endpoint is opened");
      WiFi.disconnect();
    //  WiFi.end();
      break;
    }
  }
    if(WiFi.status() == WL_CONNECTED){
        Serial.print('\n');
        Serial.println("Connection established!");
        Serial.print("IP address:\t");
        Serial.println(WiFi.localIP());

        SerialBT.print('\n');
        SerialBT.println("Connection established!");
        SerialBT.print("IP address:\t");
        SerialBT.println(WiFi.localIP());
    }
}
*/

/*
preferences structure in EEPROM

credentials{
  "ws_url_prefix" : ""
  "chargepoint"   : ""
  "host"          :
  "port"          : ""
  "protocol"      : ""
  "wifiEnabled"   : ""
  "gsmEnabled"    : ""
}
*/

Preferences preferences;

void eepromStoreCred()
{

  preferences.begin("credentials", false); // creating namespace "credentials" in EEPROM, false --> R/W mode
  preferences.clear();

  // preferences.remove("ssid");
  // preferences.remove("key");
  // preferences.remove("ws_url_prefix");
  // preferences.remove("chargepoint");
  // preferences.remove("host");
  // preferences.remove("port");
  // preferences.remove("protocol");

  // delay(1000);

  preferences.putString("ssid", ssid);
  preferences.putString("key", key);
  preferences.putString("ws_url_prefix", ws_url_prefix);
  preferences.putString("chargepoint", chargepoint);
  /*
  * @brief G. Raja Sumant
  * Ignoring the below due to urlparser implementation
  */
  #if 0
  preferences.putString("host", host);
  preferences.putInt("port", port);
  preferences.putString("protocol", protocol);
  #endif 
  preferences.putBool("wifi", wifi);
  preferences.putBool("gsm", gsm);
  preferences.putString("apn", apn_1);

  Serial.println("****************EEPROM*****************");
  Serial.println(String(preferences.getString("ssid", "")));
  Serial.println(String(preferences.getString("key", "")));
  Serial.println(String(preferences.getString("ws_url_prefix", "")));
  Serial.println(String(preferences.getString("chargepoint", "")));
  Serial.println(String(preferences.getString("host", "")));
  Serial.println(String(preferences.getInt("port", 0)));
  Serial.println(String(preferences.getString("protocol", "")));
  Serial.println(String(preferences.getString("apn", "m2misafe")));

  Serial.println("WiFi: " + String(preferences.getBool("wifi", 0)));
  Serial.println("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
  Serial.println("GSM: " + String(preferences.getBool("gsm", 0)));
  delay(1000);

  // preferences.putString("wifiEnabled",wifiEnabled);
  // preferences.putString("gsmEnabled",gsmEnabled);

  Serial.println("Credential:");
  Serial.println(ws_url_prefix);
  Serial.println(chargepoint);
  #if 0
  Serial.println(host);
  Serial.println(port);
  Serial.println(protocol);
  #endif 
  Serial.println("Stored SSID: " + String(ssid));
  Serial.println("Stored Key: " + String(key));

  delay(2000);

  SerialBT.println("Credential:");
  SerialBT.println(ws_url_prefix);
  SerialBT.println(chargepoint);
  #if 0
  SerialBT.println(host);
  SerialBT.println(port);
  SerialBT.println(protocol);
  #endif
  SerialBT.println(ssid);
  SerialBT.println(key);
  SerialBT.println("Wifi: " + String(wifi));
  SerialBT.println("Gsm: " + String(gsm));
  SerialBT.println("Restarting Device");

  Serial.println("Credentials Saved using Preferences");
  SerialBT.println("Credentials Saved using Preferences");
  
  //added by sai
  preferences.putString("lowcurcnt", lowcurcnt);
  preferences.putString("mincurr", mincurr);
  preferences.putBool("ongoingtxpor", ongoingtxpor);
  preferences.putBool("otaenable", otaenable);
  preferences.putBool("onlineoffline", onlineoffline);
 
  preferences.end();
  delay(2000);
  Serial.println("Restarting ESP");
  ESP.restart();
  // String ssid_l = String(ssid).c_str();
  // String key_l = String(key).c_str();
}

bool VerifyCred(String message)
{

  bool status = false;
  Serial.println("Verifying Credential");
  SerialBT.println("Verifying Credential");
  DeserializationError error = deserializeJson(docBT, message);
  if (error)
  {
    Serial.println("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return status;
  }

  admin = docBT["admin"];
  psswd = docBT["psswd"];

  ws_url_prefix = docBT["ws_url"];
  chargepoint = docBT["cp"];
  #if 0
  host = docBT["host"];
  port = docBT["port"];
  protocol = docBT["protocol"];
  #endif

  if (docBT.containsKey("apn"))
  {
    apn_1 = docBT["apn"];
  }
  else
  {
    Serial.println("no apn was sent, using defaults");
    SerialBT.println("no apn was sent, using defaults");
  }

  ssid = docBT["ssid"];
  key = docBT["key"];
  wifi = docBT["wifi"];
  gsm = docBT["gsm"];
  //added by sai
lowcurcnt = docBT["lowcurcnt"];
mincurr  =  docBT["mincurr"];
ongoingtxpor =  docBT["ongoingtxpor"];
otaenable = docBT["otaenable"];
onlineoffline = docBT["onlineoffline"];


  // wifiEnabled = docBT["wifi"];
  // gsmEnabled = docBT["gsm"];
  Serial.println(String("SSID + PASSWORD: ") + String(ssid) + String(key));
  delay(1000);

  if ((strcmp(adminBT, admin) == 0) && (strcmp(psswdBT, psswd) == 0))
  {
    Serial.println("\nLogin success");
    SerialBT.println("Login successfully....storing data in EEPROM");
    status = true;
    SerialBT.println("Wait...device will reboot");

    return status;
  }

  return false;
}

void startingBTConfig()
{ //

  // if(!SerialBT.begin("ESP_AC001")){
  if (!SerialBT.begin("EVRE_POD_" + String(WiFi.macAddress())))
  {
    Serial.println("An error occurred initializing Bluetooth");
  }
  else
  {
    SerialBT.println("Bluetooth initialized");
    Serial.println("Bluetooth initialized as:");
    Serial.println("EVRE_POD_" + String(WiFi.macAddress()));
  }
  delay(2000);
  SerialBT.println("Configuration mode is ON");
  Serial.println(ESP.getFreeHeap());
  bluetooth_Loop();
}

void bluetooth_Loop()
{

  while (SerialBT.available())
  {
    char incomingChar = SerialBT.read();
    if (incomingChar != '\r' && incomingChar != '\n')
    {
      message += String(incomingChar);
      //    Serial.print(incomingChar,HEX);
    }
    else
    {
      Serial.print(message);
      if (!message.equals(""))
      {
        verified = VerifyCred(message);
        if (verified)
          eepromStoreCred();
      }
      break;
    }
  }
  message = "";
}
