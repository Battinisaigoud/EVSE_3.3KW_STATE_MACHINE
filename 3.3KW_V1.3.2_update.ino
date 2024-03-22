/*
   POD_EVRE_GENERIC

   With Added GFCI support - 24/03/2022
   With 4G support - 20/04/2022

   Copyright 2022 raja <raja@raja-IdeaPad-Gaming-3-15IMH05>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.


  <POD without Control Pilot Support>
  The following code is developed by Pulkit Agrawal & Wamique.
  Added Master-Slave files
  Added EnergyMeter Fix
  Added 2G+Wifi
  Added OTA

  POD_Generic
  With no "-1" as transaction ID

  POD_G_OFFLINE
  Start transaction in offline mode
*/

// ESP32 Libraries
#include <Arduino.h>
// #include "src/libraries/arduinoWebSockets-master/src/WebSocketsClient.h"
// #include <ArduinoJson.h>
#include "src/Peripherals.h"
#include "src/OTA.h"
#include <nvs_flash.h>

#include "FFat.h"

#if WIFI_ENABLED
#include <WiFi.h>
// #define SSIDW   "Amplify Mobility_PD"
// #define PSSWD   "Amplify5"
#endif

/*wifi driver*/
#include "src/evse_wifi.h"

extern uint8_t evse_wifiConnected;

// OCPP Message Generation Class
#include "src/OcppEngine.h"
#include "src/SmartChargingService.h"
#include "src/ChargePointStatusService.h"
#include "src/MeteringService.h"
#include "src/GetConfiguration.h"
#include "src/TimeHelper.h"
#include "src/SimpleOcppOperationFactory.h"
#include "src/EVSE.h"

// OCPP message classes
#include "src/OcppOperation.h"
#include "src/OcppMessage.h"
#include "src/Authorize.h"
#include "src/BootNotification.h"
#include "src/Heartbeat.h"
#include "src/StartTransaction.h"
#include "src/StopTransaction.h"
#include "src/DataTransfer.h"
#include "src/Variants.h"

#include "src/fota_handler.h"
#include "src/FirmwareStatus_OCPP.h"

#include "src/ChargePointStatusService.h"
#include "src/StatusNotification.h"

// Master Class
#include "src/Master.h"

// Power Cycle
#include "src/PowerCycle.h"

#if CP_ACTIVE
// Control Pilot files
#include "src/ControlPilot.h"
#endif

#include "src/internet.h"
#include "src/urlparse.h"

#include "esp32-hal-cpu.h"
#include "ESP32Time.h"
#include "Esp.h"

#include "core_version.h"
#include <FastLED.h>
// 4G

#include <TinyGsmClient.h>
#include <CRC32.h>

#include "FFat.h"
#include "FS.h"
#include "SPIFFS.h"

#include "esp32-hal-cpu.h"
#include "ESP32Time.h"
#include "Esp.h"

#include "core_version.h"

// void OTA_4G_setup4G(void);
// void OTA_4G_setup_4G_OTA_get(void);
// void OTA_4G_setup_4G_OTA(void);

// void setup_4G_OTA();
// void setup_4G_OTA_get();
// void setup4G();
// 4G OTA
void OTA_4G_setup4G();

void OTA_4G_setup_4G_OTA_get();

void OTA_4G_setup_4G_OTA();

#define TINY_GSM_MODEM_SIM7600
// TinyGsm ota_modem(Serial2);
// TinyGsmClient ota_client(ota_modem);
TinyGsm ota_modem(SerialAT);
TinyGsmClient ota_client(ota_modem);

// CHANGED BY AV
uint8_t waitForResp(uint8_t timeout);
void printPercent(uint32_t readLength, uint32_t contentLength);
void pu(fs::FS &fs, const char *path);

void listDir(fs::FS &fs, const char *dirname, uint8_t levels); // added by AV

bool gsm_net = false; // ADDED BY AV
extern ChargePointStatus evse_ChargePointStatus;
uint8_t wifi_connection_available = 0;
uint8_t gu8_websocket_begin_once = 0xFF;

extern uint8_t gu8_rfidflag;
extern int8_t fault_code_A;
uint8_t timerMv_count = 0;

extern uint8_t gu8_ongoing_stop;

// Gsm Files
// #if GSM_ENABLED
#include "src/CustomGsm.h"
extern TinyGsmClient client;
// #endif
size_t E_reason;

WebSocketsClient webSocket;

// SmartChargingService *smartChargingService;
ChargePointStatusService *chargePointStatusService;

// Mertering Service declarations
MeteringService *meteringService;
ATM90E36 eic(5);
#define SS_EIC 5 // GPIO 5 chip_select pin
SPIClass *hspi = NULL;

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
bool webSocketConncted = false;

bool flag_ping_sent = false;

extern bool wifi_reconnected_flag;

uint8_t gu8_OTA_update_flag = 0;

extern CRGB leds;

enum evse_connection_sta evse_connection_status;

// RFID declarations
#define MFRC_RST 22
#define MFRC_SS 15
MFRC522 mfrc522(MFRC_SS, MFRC_RST); // Create MFRC522 instance
SPIClass *hspiRfid = NULL;

extern uint8_t Fault_occured; // added by sai

uint8_t readConnectorVal = 0;
extern uint8_t remotestart_txn;

#if DWIN_ENABLED
#include "src/dwin.h"
int8_t button = 0;
extern unsigned char ct[22];        // connected
extern unsigned char nct[22];       // not connected
extern unsigned char et[22];        // ethernet
extern unsigned char wi[22];        // wifi
extern unsigned char tr[22];        // tap rfid
extern unsigned char utr[22];       // rfid unavailable
extern unsigned char g[22];         // 4g
extern unsigned char clu[22];       // connected
extern unsigned char clun[22];      // not connected
extern unsigned char avail[22];     // available
extern unsigned char not_avail[22]; // not available
extern unsigned char change_page[10];
extern unsigned char tap_rfid[30];
extern unsigned char clear_tap_rfid[30];
extern unsigned char CONN_UNAVAIL[30];
extern unsigned char clear_avail[28];
extern unsigned char select_connector[30];

extern bool flag_faultOccured_A;
extern bool flag_faultOccured_B;
extern bool flag_faultOccured_C;

extern unsigned char v1[8]; // 2];//not connected
extern unsigned char v2[8];
extern unsigned char v3[8];
extern unsigned char i1[8];
extern unsigned char i2[8];
extern unsigned char i3[8];
extern unsigned char e1[8];
extern unsigned char e2[8];
extern unsigned char e3[8];
extern unsigned char charging[28];
extern unsigned char cid1[8];
extern unsigned char cid2[8];
extern unsigned char cid3[8];
extern unsigned char unavail[30];
void display_avail();
int8_t dwin_input();
#endif

// Flags used to control the section. They are defined in EVSE.h/EVSE.cpp
extern bool flag_evseIsBooted;
extern bool flag_evseReadIdTag;
extern bool flag_evseAuthenticate;
extern bool flag_evseStartTransaction;
extern bool flag_evRequestsCharge;
extern bool flag_evseStopTransaction;
extern bool flag_evseUnauthorise;
extern bool flag_evseSoftReset;
extern bool flag_rebootRequired;
extern bool flag_controlPAuthorise;
extern bool flag_evseReserveNow;    // added by @mkrishna
extern bool reservation_start_flag; // added by @mkrishna

extern bool flag_AuthorizeRemoteTxRequests;

extern int transactionId;

extern bool notFaulty_A;

extern int globalmeterstartA;

/*
 * @brief : Feature added by G. Raja Sumant
 * 09/08/2022 For checking FOTA from OCPP class.
 */
extern bool fota_available;

// ESP32 RTC Timer functionality
ESP32Time esp32_rtc(0); // offset in seconds GMT+1
uint64_t esp32_rtc_epoch = 0;

// global variable for currentIdTag used by various functions.
String currentIdTag;
String currentIdTag_Off;
extern String reserve_currentIdTag; // Reserve Now ID Tag

unsigned long timer_green_offline = 0;

// Bluetooth
#include "src/bluetoothConfig.h"
#define TIMEOUT_BLE 60000

#include "src/display_meterValues.h"
#include "src/evse_addr_led.h"

// 20x4 lcd display

#if LCD_DISPLAY
#include "src/LCD_I2C.h"

LCD_I2C lcd(0x27, 20, 4); // Default address of most PCF8574 modules, change according
#endif
String URI = "evse_bm_h_3_3kw_ota";

extern BluetoothSerial SerialBT;
bool isInternetConnected = true;

uint8_t gu8_BLE_off_flag = 0;
bool flagswitchoffBLE = false;
int startBLETime = 0;
String ws_url_prefix_m = "";
String host_m = "";
int port_m = 0;
String protocol_m = "";
String key_m = "";
String ssid_m = "";
// Added for url_parser.cpp
String path_m;
String protocol = "ocpp1.6";

bool wifi_enable = false;
bool gsm_enable = false;
uint8_t offline_stop_store = 0;

bool wifi_connect = false;
bool gsm_connect = false;

bool ethernet_connect = false;
bool offline_connect = false;
extern Preferences preferences;

bool flag_noVehicle = false;
bool evse_A_unavail = false;

int prevTxnId = -2;

String url_m = "";
Preferences resumeTxn;
Preferences change_config;
String idTagData_m = "";
int txnidData_m = -1;
bool ongoingTxn_m = false;

// metering Flag
extern bool flag_MeteringIsInitialised;

extern unsigned int meterSampleInterval;
extern unsigned int heartbeatInterval;

extern bool flag_stop_finishing;
extern bool flag_start_accepted;

extern bool flag_nopower;
// Led timers
ulong timercloudconnect = 0;

uint8_t gu8_poweronrest_online_flag = 0;

uint8_t gu8_online_flag = 0;

// uint8_t gu8_check_online_count = 90; // 52;
uint8_t gu8_check_online_count = 50; // 52;
uint8_t gu8_check_online_count2 = 0;

bool internet = false;
int counter_wifiNotConnected = 0;
int counter_gsmNotConnected = 0;

extern String gs32lowcurcnt;
extern uint16_t currentCounterThreshold;
extern String strgf32mincurr;
extern float minCurr;
bool ongoingtxpor_1 = false;
bool otaenable_1 = false;
bool onlineoffline_1 = false;
extern uint8_t reasonForStop;
extern String CP_Id_m;
extern Preferences FOTA_preference;
extern float discurrEnergy_A;
float LastPresentEnergy_A = 0;

uint8_t gu8_start_accepted = 0;

String Exiting_currentIdTag;
String Stored_Tag_ID_A = "";

/* EVSE to OCPP message update state machine */

EvseDevStatuse EvseDevStatus_connector_1;
uint8_t gu8_evse_change_state = EVSE_BOOT;
enum evsetransactionstatus EVSE_transaction_status;

enum evse_boot_stat_t evse_boot_state;           /* Boot Notification State machine */
enum evse_authorize_stat_t evse_authorize_state; /* Authorize State machine */
enum evse_start_txn_stat_t evse_start_txn_state; /* Start Transaction State machine */
enum evse_stop_txn_stat_t evse_stop_txn_state;   /* Stop Transaction State machine */

uint8_t offline_fault = 0;

uint8_t gu8_powerloss_flag = 0;
uint8_t gu8_stoponlineoffline_flag = 0;

void wifi_Loop();
void connectToWebsocket();
// void WiFiEvent(WiFiEvent_t event);
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
// void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);

#if 0
void check_connectivity(void);
#endif
// wifi_OTA
void setup_WIFI_OTA_get_1(void);
void performUpdate_WiFi_1(WiFiClient &updateSource, size_t updateSize);
void setup_WIFI_OTA_1(void);
void printPercent_1(uint32_t readLength, uint32_t contentLength);
void setup_WIFI_OTA_getconfig_1(void);

void EVSE_ReadInput(MFRC522 *mfrc522);
bool assignEvseToConnector(String readIdTag, int readConnectorVal);
// bool assignEvseToConnector_Offl(String readIdTag, int readConnectorVal);

// #define EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION String("0.0.0\r\n")
// #define DEVICE_ID String("evse_001")

/*
  const char ENDPOINT_CA_CERT[] PROGMEM = \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
  "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
  "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
  "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
  "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
  "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
  "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
  "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
  "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
  "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
  "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
  "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
  "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
  "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
  "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
  "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
  "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
  "rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
  "-----END CERTIFICATE-----\n";
*/
void setup()
{
  // Test LED
  //   pinMode(16,OUTPUT);
  // deleteFlash();
  Serial.begin(115200);
#if V_charge_lite1_4
  pinMode(GFCI_PIN, INPUT);
#endif
  Master_setup();
#if LCD_DISPLAY
  lcd.begin(true, 26, 27); // If you are using more I2C devices using the Wire library use lcd.begin(false)
  // this stop the library(LCD_I2C) from calling Wire.begin()
  lcd.backlight();
#endif
  // https://arduino-esp8266.readthedocs.io/en/latest/Troubleshooting/debugging.html
  Serial.setDebugOutput(true);

  if (DEBUG_OUT)
    Serial.println();
  if (DEBUG_OUT)
    Serial.println();
  if (DEBUG_OUT)
    Serial.println();

#if 0
#if LCD_ENABLED
  lcd.setCursor(4, 1);      // Or setting the cursor in the desired position.
  lcd.print("**BOOTING**"); // You can make spaces using well... spaces
  // lcd.setCursor(0, 1);      // Or setting the cursor in the desired position.
  // lcd.print("####################");
#endif
#endif

#if LCD_DISPLAY
  lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
  lcd.print("CHARGER INITIALIZING"); // You can make spaces using well... spaces
  // lcd.setCursor(0, 1);      // Or setting the cursor in the desired position.
  // lcd.print("####################");
#endif

#if DWIN_ENABLED
  uint8_t err = 0;
  dwin_setup();
  change_page[9] = 0;
  err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0])); // page 0
  delay(10);
  err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0])); // status not available
  delay(10);
  err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0])); // status not available
  delay(10);
  err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0])); // cloud: not connected
  delay(10);
  err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0])); // cloud: not connected
  delay(10);
  CONN_UNAVAIL[4] = 0X66;
  err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
  CONN_UNAVAIL[4] = 0X71;
  err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
  CONN_UNAVAIL[4] = 0X7B;
  err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
  // err = DWIN_SET(clear_tap_rfid, sizeof(clear_tap_rfid) / sizeof(clear_tap_rfid[0]));
  // delay(50);
#endif

  for (uint8_t t = 4; t > 0; t--)
  {
    if (DEBUG_OUT)
      Serial.printf("[SETUP][HALO Generic] ESP32 BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(500);
  }
#if 0
  SerialMon.printf("[SETUP][HALO Generic] ESP32 portTICK_PERIOD_MS %u Milli second \r\n", portTICK_PERIOD_MS);
  SerialMon.printf("[SETUP][HALO Generic] ESP32 Freertos tick rate %d Hz\r\n", configTICK_RATE_HZ);
  SerialMon.printf("[SETUP][HALO Generic] ESP32 Cpu Frequency  %d Mhz \r\n", getCpuFrequencyMhz());  // In MHz
  SerialMon.printf("[SETUP][HALO Generic] ESP32 Xtal Frequency %d Mhz \r\n", getXtalFrequencyMhz()); // In MHz
  SerialMon.printf("[SETUP][HALO Generic] ESP32 Apb Frequency  %d Hz \r\n", getApbFrequency());      // In Hz
  SerialMon.printf("[SETUP][HALO Generic] ESP32 total heap size  %d \r\n", ESP.getHeapSize());
  SerialMon.printf("[SETUP][HALO Generic] ESP32 available heap  %d \r\n", ESP.getFreeHeap());
  SerialMon.printf("[SETUP][HALO Generic] ESP32 lowest level of free heap since boot  %d \r\n", ESP.getMinFreeHeap());
  SerialMon.printf("[SETUP][HALO Generic] ESP32 largest block of heap that can be allocated at once  %d \r\n", ESP.getMaxAllocPsram());
  SerialMon.printf("[SETUP][HALO Generic] ESP32 chip Reversion  %d \r\n", ESP.getChipRevision());
  SerialMon.printf("[SETUP][HALO Generic] ESP32 chip Model  %s \r\n", ESP.getChipModel());
  SerialMon.printf("[SETUP][HALO Generic] ESP32 chip Cores  %d \r\n", ESP.getChipCores());
  SerialMon.printf("[SETUP][HALO Generic] ESP32 SDK Version  %s \r\n", ESP.getSdkVersion());
  SerialMon.printf("[SETUP][HALO Generic] ESP32 Flash Chip Size  %d \r\n", ESP.getFlashChipSize());
  SerialMon.printf("[SETUP][HALO Generic] ESP32 Flash Chip Speed  %d \r\n", ESP.getFlashChipSpeed());
  SerialMon.printf("[SETUP][HALO Generic] ESP32 Sketch Size  %d \r\n", ESP.getSketchSize());
  SerialMon.printf("[SETUP][HALO Generic] ESP32 Sketch MD5  %x \r\n", ESP.getSketchMD5());
  SerialMon.printf("[SETUP][HALO Generic] ESP32 Free Sketch Space  %d \r\n", ESP.getFreeSketchSpace());

  SerialMon.println("[SETUP][HALO Generic] DEVICE VERSION :" + String(VERSION));
  SerialMon.println("[SETUP][HALO Generic] DEVICE VERSION :" + String(URI));
  SerialMon.println("  ****************HUB****************** ");
   
  SerialMon.println("[SETUP][HALO Generic] CHARGE POINT VENDOR : " + String(CHARGE_POINT_VENDOR));
  SerialMon.println("[SETUP][HALO Generic] CHARGE POINT MODEL : " + String(CHARGE_POINT_MODEL));
  // SerialMon.println("[SETUP][HALO Generic] ARDUINO ESP32 GIT DESC RELEASE VERSION : " +String(ARDUINO_ESP32_GIT_DESC));
  SerialMon.println("[SETUP][HALO Generic] ARDUINO ESP32 RELEASE VERSION :" + String(ARDUINO_ESP32_RELEASE));
  SerialMon.println("[SETUP][HALO Generic] ARDUINO ESP32 GIT VERSION : " + String(ARDUINO_ESP32_GIT_VER));

  SerialMon.println("[SETUP][AC001] Compiled: " __DATE__ ", " __TIME__ ", " __VERSION__);

  // SerialMon.println( F("[SETUP][AC001] FILE NAME : "__FILE__));
  SerialMon.print(F("[SETUP][AC001] Arduino IDE version: "));
  SerialMon.println(ARDUINO, DEC);
#endif

  // added by sai

  Serial.println("*** EVSE Booting ......!***");
  Serial.print("EVSE FIRMWARE VERSION:  ");
  Serial.print("[SETUP] ESP32 Freertos tick rate ");
  Serial.println(configTICK_RATE_HZ);
  Serial.print("[SETUP] ESP32 Freertos portTICK_PERIOD_MS 0");
  Serial.println(portTICK_PERIOD_MS);
  Serial.print("[SETUP] ESP32 Cpu Frequency  ");
  Serial.println(getCpuFrequencyMhz());
  Serial.print("[SETUP] ESP32 Xtal Frequency ");
  Serial.println(getXtalFrequencyMhz());
  Serial.print("[SETUP] ESP32 Apb Frequency ");
  Serial.println(getApbFrequency());
  Serial.print("[SETUP] ESP32 total heap size ");
  Serial.println(ESP.getHeapSize());
  Serial.print("[SETUP] ESP32 available heap  ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("[SETUP] ESP32 lowest level of free heap since boot ");
  Serial.println(ESP.getMinFreeHeap());
  Serial.print("[SETUP] ESP32 largest block of heap that can be allocated at once  ");
  Serial.println(ESP.getMaxAllocPsram());
  Serial.print("[SETUP] ESP32 chip Reversion  ");
  Serial.println(ESP.getChipRevision());
  Serial.print("[SETUP] ESP32 chip Model ");
  Serial.println(ESP.getChipModel());
  Serial.print("[SETUP] ESP32 chip Cores ");
  Serial.println(ESP.getChipCores());
  Serial.print("[SETUP] ESP32 SDK Version  ");
  Serial.println(ESP.getSdkVersion());
  Serial.print("[SETUP] ESP32 Flash Chip Size  ");
  Serial.println(ESP.getFlashChipSize());
  Serial.print("[SETUP] ESP32 Flash Chip Speed  ");
  Serial.println(ESP.getFlashChipSpeed());
  Serial.print("[SETUP] ESP32 Sketch Size  ");
  Serial.println(ESP.getSketchSize());
  Serial.print("[SETUP] ESP32 Sketch MD5");
  Serial.println(ESP.getSketchMD5());
  Serial.print("[SETUP] ESP32 Free Sketch Space  ");
  Serial.println(ESP.getFreeSketchSpace());
  // ble_mac_add = WiFi.macAddress();
  // Serial.print(F("[SETUP] ESP32 WIFI MAC "));
  // Serial.println(ble_mac_add);
  Serial.print("[SETUP]EVSE DEVICE VERSION :");
  Serial.println(VERSION);
  Serial.print("[SETUP]EVSE CHARGE POINT VENDOR : ");
  Serial.println(CHARGE_POINT_VENDOR);
  Serial.print("[SETUP]EVSE CHARGE POINT MODEL : ");
  Serial.println(CHARGE_POINT_MODEL);
  // Serial.println("[SETUP]EVSE ARDUINO ESP32 GIT DESC RELEASE VERSION : " +String(ARDUINO_ESP32_GIT_DESC));
  Serial.print("[SETUP]EVSE ARDUINO ESP32 RELEASE VERSION :");
  Serial.println(ARDUINO_ESP32_RELEASE);
  Serial.print("[SETUP]EVSE ARDUINO ESP32 GIT VERSION : ");
  Serial.println(ARDUINO_ESP32_GIT_VER);
  Serial.print("[SETUP]EVSE Compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);
  // Serial.println(/* __VERSION__ */);
  Serial.print("[SETUP] FILE NAME : ");
  Serial.println(__FILE__);
  Serial.print("[SETUP] Arduino IDE version: ");
  Serial.print(ARDUINO, DEC);

  // Serial.print(F(__FILE__));

  //  if(!FFat.begin(true)){
  //       Serial.println("Mount Failed");
  //   }
  //   else
  //   {
  //     Serial.println("File system mounted");
  //   }
#if 0
#if LCD_ENABLED
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("PREPARING");
  lcd.setCursor(5, 3);
  lcd.print("PLEASE WAIT");
#endif
#endif
#if 0
//added by sai
#if LCD_ENABLED
  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("WAITING FOR NETWORK");
  // lcd.setCursor(5, 3);
  // lcd.print("PLEASE WAIT");
#endif
#endif

  if (ARAI_BOARD)
    requestLed(BLINKYWHITE_EINS, START, 1); // 1 means 1st connector

  /*****************************************/
  requestForRelay(STOP, 1);
  requestforCP_OUT(STOP);

// #if BLE_ENABLE
//   startingBTConfig();
// #endif
/************************Preferences***********************************************/
#if 0
  preferences.begin("credentials", false);

  ws_url_prefix_m = preferences.getString("ws_url_prefix", ""); //characters
  if (ws_url_prefix_m.length() > 0) {
    Serial.println("Fetched WS URL success: " + String(ws_url_prefix_m));
  } else {
    Serial.println("Unable to Fetch WS URL / Empty");
  }
  delay(100);

  host_m = preferences.getString("host", "");
  if (host_m.length() > 0) {
    Serial.println("Fetched host data success: " + String(host_m));
  } else {
    Serial.println("Unable to Fetch host data / Empty");
  }
  delay(100);

  port_m = preferences.getInt("port", 0);
  if (port_m > 0) {
    Serial.println("Fetched port data success: " + String(port_m));
  } else {
    Serial.println("Unable to Fetch port Or port is 0000");
  }
  delay(100);

  protocol_m = preferences.getString("protocol", "");
  if (protocol_m.length() > 0) {
    Serial.println("Fetched protocol data success: " + String(protocol_m));
  } else {
    Serial.println("Unable to Fetch protocol");
  }
#endif

  urlparser();
  preferences.begin("credentials", false);
  ssid_m = preferences.getString("ssid", "");
  if (ssid_m.length() > 0)
  {
    Serial.println("Fetched SSID: " + String(ssid_m));
  }
  else
  {
    Serial.println("Unable to Fetch SSID");
  }

  key_m = preferences.getString("key", "");
  if (key_m.length() > 0)
  {
    Serial.println("Fetched Key: " + String(key_m));
  }
  else
  {
    Serial.println("Unable to Fetch key");
  }

  wifi_enable = preferences.getBool("wifi", 0);
  Serial.println("Fetched protocol data: " + String(wifi_enable));

  gsm_enable = preferences.getBool("gsm", 0);
  // gsm_enable = 1;
  Serial.println("Fetched protocol data: " + String(gsm_enable));

  Serial.println("closing preferences");

  // added by sai
  gs32lowcurcnt = preferences.getString("lowcurcnt", "");

  Serial.println("****************gs32lowcurcnt**************** " + String(gs32lowcurcnt));

  currentCounterThreshold = gs32lowcurcnt.toInt();

  Serial.println("********************currentCounterThreshold*******************: " + String(currentCounterThreshold));

  strgf32mincurr = preferences.getString("mincurr", "");
  Serial.println("*******************strgf32mincurr**********************: " + String(strgf32mincurr));

  minCurr = strgf32mincurr.toFloat();
  Serial.println("*********************minCurr*******************: " + String(minCurr));

  ongoingtxpor_1 = preferences.getBool("ongoingtxpor", 0);
  Serial.println("*********************ongoingtxpor_1*********************: " + String(ongoingtxpor_1));

  otaenable_1 = preferences.getBool("otaenable", 0);
  Serial.println("*************************otaenable**************************: " + String(otaenable_1));

  onlineoffline_1 = preferences.getBool("onlineoffline", 0);
  Serial.println("***********************onlineoffline***********************: " + String(onlineoffline_1));
  preferences.end();
  // added by sai
  FOTA_preference.begin("fota_url", false);
  String evse_fota_uri = FOTA_preference.getString("fota_uri", "");
  uint32_t evse_fota_retries = FOTA_preference.getUInt("fota_retries", 0);
  String evse_fota_date = FOTA_preference.getString("fota_date", "");
  bool evse_fota_avail = FOTA_preference.getBool("fota_avial", false);
  FOTA_preference.end();

  Serial.println("FOTA UPDATE states ");
  Serial.println("evse_fota_uri :  " + String(evse_fota_uri));
  Serial.println("evse_fota_retries :  " + String(evse_fota_retries));
  Serial.println("evse_fota_date :  " + String(evse_fota_date));
  Serial.println("evse_fota_avail :  " + String(evse_fota_avail));

  /*
   * @brief change_config: Feature added by G. Raja Sumant
   * 09/07/2022
   * The values stored using change configuration must
   * be restored after a reboot as well.
   */

  change_config.begin("configurations", false);
  meterSampleInterval = change_config.getInt("meterSampleInterval", 0);
  heartbeatInterval = change_config.getInt("heartbeatInterval", 50);
  flag_AuthorizeRemoteTxRequests = change_config.getBool("authRemoteStart", false); // skip auth by default
  // change_config.getBool("")
  change_config.end();

  // WiFi
  wifi_connect = wifi_enable;
  gsm_connect = gsm_enable;
  // bool internet = false;
  // int counter_wifiNotConnected = 0;
  // int counter_gsmNotConnected = 0;

  startBLETime = millis();
  Serial.println("startBLETime : " + String(startBLETime));
  // #if BLE_ENABLE
  //   while(millis() - startBLETime < TIMEOUT_BLE){currentEnergy
  //     bluetooth_Loop();
  //     flagswitchoffBLE = true;
  //     Serial.print(".");
  //     delay(1000);

  //   }
  //   if(flagswitchoffBLE == true){
  //       flagswitchoffBLE = false;
  //       Serial.println("Disconnecting BT");
  //       //SerialBT.println("Wifi Connected");
  //       SerialBT.println("Disconnecting BT");
  //       delay(100);
  //       SerialBT.flush();
  //       SerialBT.disconnect();
  //       SerialBT.end();
  //       Serial.println(ESP.getFreeHeap());
  //     }
  // #endif

  // added by sai
  //  if (otaenable_1 == 1)
  // evse_fota_avail = true;
  if ((otaenable_1 == 1) || (evse_fota_avail == true))
  {
    otaenable_1 = 0;
    preferences.begin("credentials", false);
    preferences.putBool("otaenable", false);
    preferences.end();
    if (gsm_enable == true)
    {

      if (!FFat.begin(true))
      {
        Serial.println("Mount Failed");
      }
      else
      {
        Serial.println("File system mounted");
#if EVSE_FOTA_ENABLE_4G

        OTA_4G_setup4G();
        Serial.println("******checking for OTA******");
        requestLed(ORANGE, START, 1);
#if LCD_DISPLAY
        lcd.clear();
        lcd.setCursor(0, 2);
        lcd.print("CHARGER UPDATING");
#endif
        OTA_4G_setup_4G_OTA_get();
        Serial.println("******OTA check done******");
        FOTA_preference.begin("fota_url", false);
        FOTA_preference.putString("fota_uri", "");
        FOTA_preference.putUInt("fota_retries", 0);
        FOTA_preference.putString("fota_date", "");
        FOTA_preference.putBool("fota_avial", false);
        FOTA_preference.end();
        delay(1000);

        OTA_4G_setup_4G_OTA();
        Serial.println("******OTA update done******");
        requestLed(BLINKYWHITE_EINS, START, 1);

#endif
      }
    }

    else if (wifi_enable == true)
    {

      gu8_check_online_count = 52;
      gu8_check_online_count2 = 10;
      // delete old config
      WiFi.disconnect(true);
      WiFi.begin(ssid_m.c_str(), key_m.c_str());

#if EVSE_FOTA_ENABLE_WIFI

      // ptr_leds->Yellow_led();
      // requestLed(ORANGE, START, 1);
      setup_WIFI_OTA_get_1();
      FOTA_preference.begin("fota_url", false);
      FOTA_preference.putString("fota_uri", "");
      FOTA_preference.putUInt("fota_retries", 0);
      FOTA_preference.putString("fota_date", "");
      FOTA_preference.putBool("fota_avial", false);
      FOTA_preference.end();

      switch (gu8_OTA_update_flag)
      {
      case 2:
        Serial.println("OTA update available, In Switch ...!");
        requestLed(ORANGE, START, 1);
        //  requestLed(BLINKYORANGE, START, 1);
        setup_WIFI_OTA_1();
        break;
      case 3:
        Serial.println("No OTA update available, In Switch ...!");
        break;
      default:
        Serial.println("default case OTA update, In Switch ...! \r\n gu8_OTA_update_flag :" + String(gu8_OTA_update_flag));
        break;
      }
#endif

#if 0
    WiFi.onEvent(WiFiEvent);
    WiFi.onEvent(WiFiGotIP, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
#endif
    }
  }

  else if (gsm_enable)
  {
    if (wifi_enable == false)
    {
      gu8_check_online_count = 2;
      gu8_check_online_count2 = 2;
    }
  }

#if 0
  while (internet == false)
  {
    Serial.println("Internet loop");
#if DWIN_ENABLED
      err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
      delay(50);
      err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0]));
      delay(50);
      err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
      delay(50);
      err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0]));
      delay(50);
#endif
    bluetooth_Loop();
    if (wifi_enable == true && wifi_connect == true)
    {
      Serial.println("Waiting for WiFi Connction...");

      if (WiFi.status() == WL_CONNECTED)
      {
        wifi_reconnected_flag = true;
        internet = true;
        gsm_connect = false;
        Serial.println("Connected via WiFi");
#if LCD_ENABLED
        lcd.clear();
        lcd.setCursor(3, 2);
        lcd.print("STATUS: WIFI");
#endif

#if DWIN_ENABLED
          // Cloud : WiFi
          err = DWIN_SET(wi, sizeof(wi) / sizeof(wi[0]));
          delay(50);
          err = DWIN_SET(wi, sizeof(wi) / sizeof(wi[0]));
          delay(50);
          // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
          // delay(50);
          // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
          // delay(50);
#endif
        delay(100);
        connectToWebsocket();
      }
      else if (WiFi.status() != WL_CONNECTED)
      {
        Serial.print(".");
        delay(1000);
        //  bluetooth_Loop();
        wifi_Loop();
        Serial.println("Wifi Not Connected: " + String(counter_wifiNotConnected));
#if DWIN_ENABLED
          err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
          delay(50);
          err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0]));
          delay(50);
#endif
        if (counter_wifiNotConnected++ > 50)
        {
          counter_wifiNotConnected = 0;
          if (gsm_enable == true)
          {
            wifi_connect = false;
            gsm_connect = true;
          }
        }
      }
    }
    else if (gsm_enable == true && gsm_connect == true)
    {
      Serial.println("gsm_enable == true && gsm_connect == true");
      SetupGsm(); // redundant @optimise
      ConnectToServer();
      if (!client.connected())
      {
        gsm_Loop();
        //   bluetooth_Loop();

        Serial.println("GSM not Connected: " + String(counter_gsmNotConnected));
#if DWIN_ENABLED
          err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
          delay(50);
          err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0]));
          delay(50);
#endif
        if (counter_gsmNotConnected++ > 2)
        { // 2 == 5min
          counter_gsmNotConnected = 0;

          if (wifi_enable == true)
          {
            wifi_connect = true;
            gsm_connect = false;
          }
        }
      }
      else if (client.connected())
      {
        internet = true;
        wifi_connect = false;
        Serial.println("connected via 4G");
#if LCD_ENABLED
        lcd.clear();
        lcd.setCursor(0, 2);
        lcd.print("CONNECTED VIA 4G");
#endif
#if DWIN_ENABLED
          // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
          // delay(50);
          // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
          // delay(50);
          err = DWIN_SET(g, sizeof(g) / sizeof(g[0]));
          delay(50);
          err = DWIN_SET(g, sizeof(g) / sizeof(g[0]));
          delay(50);
#endif
      }
    }
  }
#endif
#if BLE_ENABLE
  startingBTConfig();
#endif
  // ADDED BY SAI WIFI CREDENDIAL
  preferences.begin("credentials", false);

  ssid_m = preferences.getString("ssid", "");
  if (ssid_m.length() > 0)
  {
    Serial.println("Fetched SSID: " + String(ssid_m));
  }
  else
  {
    Serial.println("Unable to Fetch SSID");
  }

  key_m = preferences.getString("key", "");
  if (key_m.length() > 0)
  {
    Serial.println("Fetched Key: " + String(key_m));
  }
  else
  {
    Serial.println("Unable to Fetch key");
  }

  wifi_enable = preferences.getBool("wifi", 0);
  Serial.println("Fetched protocol data: " + String(wifi_enable));

  gsm_enable = preferences.getBool("gsm", 0);
  // gsm_enable = 1;
  Serial.println("Fetched protocol data: " + String(gsm_enable));

  Serial.println("closing preferences");
  preferences.end();

#if LCD_DISPLAY
  lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
  lcd.print("CHARGER INITIALIZING"); // You can make spaces using well... spaces
  // lcd.setCursor(0, 1);      // Or setting the cursor in the desired position.
  // lcd.print("####################");
#endif

  // added by sai
  Serial.println("conneting to the internet");
  for (uint8_t i = 0; i < 30; i++)
  {
    Serial.print(".");
    delay(1000);
  }

  /* wifi driver*/
  if (wifi_connect)
  {
    Serial.println("wifi_init...........");
    wifi_init();
    delay(2000);
    connectToWebsocket();
    delay(1000);
  }

#if 1

  while ((internet == false) && (gu8_check_online_count))
  {
    Serial.println("Internet loop");
    // requestLed(WHITE, START, 1);
    // added by sai
    //  requestLed(BLINKYWHITE_EINS, START, 1);

    Serial.println("gu8_check_online_count" + String(gu8_check_online_count));
    gu8_check_online_count--;

#if DWIN_ENABLED
    err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
    delay(50);
    err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0]));
    delay(50);
    err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
    delay(50);
    err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0]));
    delay(50);
#endif
    bluetooth_Loop();
#if 0

    if (wifi_enable == true && wifi_connect == true)
    {
      Serial.println("Waiting for WiFi Connction...");

      Serial.print("[WiFi_Loop]: ");
      if (WiFi.status() != WL_CONNECTED)
      {
        // delay(200);
        Serial.println("Disconnected!!!");
        // if (wifi_counter++ > 5 && (WiFi.status() != WL_CONNECTED))
        if (WiFi.status() != WL_CONNECTED)
        {
          Serial.print(".");
          WiFi.disconnect();
          delay(500);
          Serial.println("[WIFI] Trying to reconnect again");
                    // added by sai
#if LCD_DISPLAY
          lcd.clear();
          lcd.setCursor(1, 1);
          lcd.print("CONNECTING TO WI-FI");

#endif
          // WiFi.reconnect();
          // WiFi.begin(ssid_m.c_str(),key_m.c_str());
          WiFi.begin(ssid_m.c_str(), key_m.c_str());
          wifi_connection_available = 1;
          delay(1500);
        }
      }

// added by sai
#if 1
      else
      {
        if (WiFi.status() == WL_CONNECTED)
        {

          wifi_connection_available = 1;
          // online_to_offline_flag = false;
          if (wifi_connection_available == 1)
          {
            wifi_reconnected_flag = true;
            internet = true;
            gsm_connect = false;
            delay(100);
            wifi_connection_available = 0;
            connectToWebsocket();
          }
          webSocket.loop();
        }
#if DISPLAY_ENABLED
        while (millis() - cloud_refresh > 5000)
        {
          // cloud offline
          cloud_refresh = millis();
          cloudConnect_Disp(3);
          checkForResponse_Disp();
        }
#endif
      }
#endif

#if 0
      if (WiFi.status() == WL_CONNECTED)
      {
        wifi_reconnected_flag = true;
        internet = true;
        gsm_connect = false;
        Serial.println("Connected via WiFi");
        Serial.print(F("WiFi Signal Strength is: "));
        Serial.print(WiFi.RSSI());
        Serial.println(F("dBm"));
#if 0
#if LCD_ENABLED
        lcd.clear();
        lcd.setCursor(3, 2);
        lcd.print("STATUS: WIFI");
#endif
#endif

#if DWIN_ENABLED
        // Cloud : WiFi
        err = DWIN_SET(wi, sizeof(wi) / sizeof(wi[0]));
        delay(50);
        err = DWIN_SET(wi, sizeof(wi) / sizeof(wi[0]));
        delay(50);
        // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
        // delay(50);
        // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
        // delay(50);
#endif
        delay(100);
        connectToWebsocket();

        gu8_poweronrest_online_flag = 0;
      }
      else if (WiFi.status() != WL_CONNECTED)
      {
        Serial.print(".");
        delay(1000);
        //  bluetooth_Loop();
        wifi_Loop();
        Serial.println("Wifi Not Connected: " + String(counter_wifiNotConnected));
#if DWIN_ENABLED
        err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
        delay(50);
        err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0]));
        delay(50);
#endif
        if (counter_wifiNotConnected++ > 50)
        {
          counter_wifiNotConnected = 0;
          if (gsm_enable == true)
          {
            wifi_connect = false;
            gsm_connect = true;
            gu8_poweronrest_online_flag = 1;
          }
        }
      }
#endif
    }
#endif
    /*wifi driver*/
    if (wifi_enable)
    {
#if LCD_DISPLAY
      lcd.clear();
      lcd.setCursor(1, 1);
      lcd.print("CONNECTING TO WI-FI");

#endif
      if (evse_wifiConnected)
      {
        wifi_connection_available = 1;
        // online_to_offline_flag = false;
        if (wifi_connection_available == 1)
        {
          wifi_reconnected_flag = true;
          internet = true;
          gsm_connect = false;
          delay(100);
          wifi_connection_available = 0;
          // connectToWebsocket();
        }
        webSocket.loop();
      }
      else
      {
      }
    }

    else if (gsm_enable == true && gsm_connect == true)
    {
      Serial.println("gsm_enable == true && gsm_connect == true");
      SetupGsm(); // redundant @optimise

      ConnectToServer();
      if (!client.connected())
      {
        gsm_Loop();
        //   bluetooth_Loop();

        Serial.println("GSM not Connected: " + String(counter_gsmNotConnected));
#if DWIN_ENABLED
        err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
        delay(50);
        err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0]));
        delay(50);
#endif
        if (counter_gsmNotConnected++ > 2)
        { // 2 == 5min
          counter_gsmNotConnected = 0;
          gu8_poweronrest_online_flag = 1;
          if (wifi_enable == true)
          {
            wifi_connect = true;
            gsm_connect = false;
          }
        }
      }
      else if (client.connected())
      {
        internet = true;
        wifi_connect = false;
        Serial.println("connected via 4G");
#if 0
#if LCD_ENABLED
        lcd.clear();
        lcd.setCursor(0, 2);
        lcd.print("CONNECTED VIA 4G");
#endif
#endif
#if DWIN_ENABLED
        // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
        // delay(50);
        // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
        // delay(50);
        err = DWIN_SET(g, sizeof(g) / sizeof(g[0]));
        delay(50);
        err = DWIN_SET(g, sizeof(g) / sizeof(g[0]));
        delay(50);
#endif
      }
    }
    else if ((wifi_connect == false) && (gsm_connect = false))
    {
    }
  }

#endif

  //  #if WIFI_ENABLED
  // Update the WiFi credentials in EVSE.h
  //  if(wifi_enable == true){
  //  if (DEBUG_OUT) Serial.println("Waiting for WiFi Connction...");
  //  WiFi.begin(ssid_m.c_str(), key_m.c_str());

  //  while (WiFi.status() != WL_CONNECTED) {
  //      Serial.print(".");
  //  delay(1000);
  //    bluetooth_Loop();
  //    wifi_Loop();
  //  }
  // //#endif
  //  }
  /*
    #if GSM_ENABLED
    SetupGsm();
    delay(1000);
    ConnectToServer();
    delay(5000);

    while(!client.connected()){
      delay(1000);
      gsm_Loop();
    }

    Serial.println("Connected via GSM");
    #endif
  */

  // SPI Enable for Energy Meter Read
  hspi = new SPIClass(HSPI); // Init SPI bus
  hspi->begin();
  pinMode(SS_EIC, OUTPUT); // HSPI SS Pin

  // SPI Enable for RFID
  hspiRfid = new SPIClass(HSPI);
  hspiRfid->begin();
  mfrc522.PCD_Init(); // Init MFRC522

  // ocppEngine_initialize(&webSocket, 4096); //default JSON document size = 4096

  ocppEngine_initialize(&webSocket, 4096); // default JSON document size = 2048

  /*********Preferences Block For Restarting previously running Txn [Power Cycle]***********/
  resumeTxn.begin("resume", false); // opening preferences in R/W mode
  idTagData_m = resumeTxn.getString("idTagData", "");
  ongoingTxn_m = resumeTxn.getBool("ongoingTxn", false);
  transactionId = resumeTxn.getInt("TxnIdData", -1);
  globalmeterstartA = resumeTxn.getFloat("meterStart", false);
  // resumeTxn.end();
  Serial.println("Stored ID:" + String(idTagData_m));
  Serial.println("Ongoing Txn: " + String(ongoingTxn_m));
  Serial.println("Txn id is: " + String(transactionId));
  Serial.println("meterstart is: " + String(globalmeterstartA));
  /*offline stop get*/
  // resumeTxn.begin("resume", false);
  reasonForStop = resumeTxn.getInt("reasonForStop", 3);
  fault_code_A = resumeTxn.getInt("fault_code_A", -1);
  offline_stop_store = resumeTxn.getInt("offline_stop", false);

  resumeTxn.end();
  /****************************************************************************************/

  /*//Example for SmartCharging Service usage
    smartChargingService = new SmartChargingService(16.0f); //default charging limit: 16A
    smartChargingService->setOnLimitChange([](float limit) {
    if (DEBUG_OUT) Serial.print(F("setOnLimitChange Callback: Limit Change arrived at Callback function: new limit = "));
    if (DEBUG_OUT) Serial.print(limit);
    if (DEBUG_OUT) Serial.print(F(" A\n"));
    EVSE_setChargingLimit(limit);
    });*/

  chargePointStatusService = new ChargePointStatusService(&webSocket); // adds itself to ocppEngine in constructor
  meteringService = new MeteringService(&webSocket);

  // set system time to default value; will be without effect as soon as the BootNotification conf arrives
  setTimeFromJsonDateString("2021-22-12T11:59:55.123Z"); // use if needed for debugging

  flag_evseIsBooted = false;
  flag_evseSoftReset = false;
  flag_rebootRequired = false;

/*
 * @brief : Feature added by G. Raja Sumant to fix the session resume operation.
 */
// This depends if the power fail scenario is evaluated as true.
#if 0
  if (ongoingTxn_m)
  {
     if (ongoingtxpor_1 == 1)
    {
    flag_evRequestsCharge = true;
    }
    if (DEBUG_OUT)
      Serial.println(F("****on going transaction is true*****"));
  }
  else
  {
    if (DEBUG_OUT)
      Serial.println(F("****on going transaction is false****"));
  }
#endif

#if 0

  EVSE_setOnBoot([]()
                 {
                  // Commented for Boot Notification is not send to  CMS, when Power On Reset (POR) is with Offline to online
    // if (gu8_online_flag == 1)
    // {
      //this is not in loop, that is why we need not update the flag immediately to avoid multiple copies of bootNotification.
    OcppOperation *bootNotification = makeOcppOperation(&webSocket, new BootNotification());
    initiateOcppOperation(bootNotification);

    if (gu8_online_flag == 1)
    {
      bootNotification->setOnReceiveConfListener([](JsonObject payload) {

      if ( flag_MeteringIsInitialised == false) {
        Serial.println("[SetOnBooT] Initializing metering services");
        meteringService->init(meteringService);
      }

      if (DEBUG_OUT) Serial.print(F("EVSE_setOnBoot Callback: Metering Services Initialization finished.\n"));


      flag_evseIsBooted = true; //Exit condition for booting.
      flag_evseReadIdTag = true; //Entry condition for reading ID Tag.
      flag_evseAuthenticate = false;
      flag_evseStartTransaction = false;
       evse_ChargePointStatus = Available;
        Serial.println(".......................1");
     
      //flag_evRequestsCharge = false;
       if (ongoingTxn_m)
       {

#if 1
         // added by sai
         if (ongoingtxpor_1 == 1)
         {

          //added by sai
          Serial.println("Charging**********1");
           evse_ChargePointStatus = Charging; 
          flag_evRequestsCharge = true;
           EvseDevStatus_connector_1 = flag_EVSE_Request_Charge;
          if (DEBUG_OUT)
           Serial.println(F("****[EVSE_setOnBoot] on going transaction is true*****"));

           flag_evseStopTransaction = false;
         }
         else
         {

           Serial.println("Txn id is:$$$$$$$$$$$$$$$$$$$$$$$$$$$@@@@@@@@ " + String(transactionId));
           getChargePointStatusService()->startTransaction(transactionId);
           Serial.println("Txn id is:***********######## " + String(transactionId));
           gu8_powerloss_flag = 1;
           reasonForStop = 5;

           // flag_evseStopTransaction = true;

           Serial.println(F("****EVSE_StopSession_1*****"));

           EVSE_StopSession();
         }
#endif
       }
       else
       {
         // added by sai
         Serial.println("Available**********1");
         evse_ChargePointStatus = Available;
         Serial.println(".......................2");
#if 0
#if LCD_ENABLED
         if (wifi_connect)
         {

           lcd.clear();
           lcd.setCursor(15, 0);
           lcd.print("WI-FI");
           lcd.setCursor(0, 1);
           lcd.print("CHARGER AVAILABLE");
           lcd.setCursor(0, 2);
           lcd.print("TAP RFID/SCAN QR");
           lcd.setCursor(0, 3);
           lcd.print("TO START");
         }
         else if (gsm_connect)
         {

           lcd.clear();
           lcd.setCursor(15, 0);
           lcd.print("4G");
           lcd.setCursor(0, 1);
           lcd.print("CHARGER AVAILABLE");
           lcd.setCursor(0, 2);
           lcd.print("TAP RFID/SCAN QR");
           lcd.setCursor(0, 3);
           lcd.print("TO START");
         }
#endif
#endif
         flag_evRequestsCharge = false;
         if (DEBUG_OUT)
           Serial.println(F("****[EVSE_setOnBoot] on going transaction is false*****"));
       }
      // flag_evseStopTransaction = false;
      flag_evseUnauthorise = false;
      if (DEBUG_OUT) Serial.print(F("EVSE_setOnBoot Callback: Closing Relays.\n"));

      if (DEBUG_OUT) Serial.print(F("EVSE_setOnBoot Callback: Boot successful. Calling Read User ID Block.\n"));
    });

    }
    else
    {
      if ( flag_MeteringIsInitialised == false) {
        Serial.println("[SetOnBooT] Initializing metering services");
        meteringService->init(meteringService);
      }

      if (DEBUG_OUT) Serial.print(F("EVSE_setOnBoot Callback: Metering Services Initialization finished.\n"));
 
      flag_evseIsBooted = true; //Exit condition for booting.
      flag_evseReadIdTag = true; //Entry condition for reading ID Tag.
      flag_evseAuthenticate = false;
      flag_evseStartTransaction = false;
      //added by sai
       Serial.println("Available**********2");
      //  evse_ChargePointStatus = Available;
       Serial.println(".......................3");
      //flag_evRequestsCharge = false;
       if (ongoingTxn_m)
       {

#if 1
         // added by sai
         if (ongoingtxpor_1 == 1)
         {

          //added by sai
          Serial.println("Charging**********1");
           evse_ChargePointStatus = Charging; 
          flag_evRequestsCharge = true;
          EvseDevStatus_connector_1 = flag_EVSE_Request_Charge;
          if (DEBUG_OUT)
           Serial.println(F("****[EVSE_setOnBoot] on going transaction is true*****"));

           flag_evseStopTransaction = false;
         }
         else
         {

           Serial.println("Txn id is:$$$$$$$$$$$$$$$$$$$$$$$$$$$@@@@@@@@ " + String(transactionId));
           getChargePointStatusService()->startTransaction(transactionId);
           Serial.println("Txn id is:***********######## " + String(transactionId));
           gu8_powerloss_flag = 1;
           reasonForStop = 5;

           // flag_evseStopTransaction = true;

           Serial.println(F("****EVSE_StopSession_1*****"));

           EVSE_StopSession();
         }
#endif
       }
       else
       {
         // added by sai
         Serial.println("Available**********1");
         evse_ChargePointStatus = Available;
         Serial.println(".......................4");
#if 0
#if LCD_ENABLED
         if (wifi_connect)
         {

           lcd.clear();
           lcd.setCursor(15, 0);
           lcd.print("WI-FI");
           lcd.setCursor(0, 1);
           lcd.print("CHARGER AVAILABLE");
           lcd.setCursor(0, 2);
           lcd.print("TAP RFID/SCAN QR");
           lcd.setCursor(0, 3);
           lcd.print("TO START");
         }
         else if (gsm_connect)
         {

           lcd.clear();
           lcd.setCursor(15, 0);
           lcd.print("4G");
           lcd.setCursor(0, 1);
           lcd.print("CHARGER AVAILABLE");
           lcd.setCursor(0, 2);
           lcd.print("TAP RFID/SCAN QR");
           lcd.setCursor(0, 3);
           lcd.print("TO START");
         }
#endif
#endif
         flag_evRequestsCharge = false;
         if (DEBUG_OUT)
           Serial.println(F("****[EVSE_setOnBoot] on going transaction is false*****"));
       }
      // flag_evseStopTransaction = false;
      flag_evseUnauthorise = false;

    } });

#endif

  EVSE_setOnReadUserId([]()
                       {
    if (DEBUG_OUT) Serial.print("EVSE_setOnReadUserId Callback: EVSE waiting for User ID read...\n");
    Serial.println("gu8_online_flag" + String(gu8_online_flag));

    static ulong timerForRfid = millis();
    currentIdTag = "";
    currentIdTag_Off = "";
    resumeTxn.begin("resume", false); //opening preferences in R/W mode
    idTagData_m = resumeTxn.getString("idTagData", "");
    ongoingTxn_m = resumeTxn.getBool("ongoingTxn", false);

    if (offline_stop_store)
    {
      offline_stop_store = 0;
      OcppOperation *stopTransaction = makeOcppOperation(&webSocket, new StopTransaction());
      initiateOcppOperation(stopTransaction);
      evse_ChargePointStatus = Available;
      Serial.print("EVSE_setOnStopTransaction  before Callback: Closing Relays.\n");
      if (webSocketConncted)
      {
        resumeTxn.begin("resume", false);

        resumeTxn.putBool("ongoingTxn", false);
        resumeTxn.putString("idTagData", "");
        resumeTxn.putInt("TxnIdData", -1);
        Serial.println("------Is the session running? ----- ");
        Serial.println(resumeTxn.getBool("ongoingTxn", false));
        Serial.println(resumeTxn.getInt("TxnIdData", -2));
        resumeTxn.putInt("reasonForStop", -1);
        resumeTxn.putInt("fault_code_A", -1);

        resumeTxn.putInt("offline_stop", 0);

        // delay(10000);
        resumeTxn.end();
      }
    }
    // flag_evRequestsCharge = false;   // flag_evRequestsCharge is placed here for ongoingTxn_m Blinky Green LED indication purpose
    // added for resume

#if 0

    if (ongoingTxn_m)
    {

#if 1
      // added by sai
      if (ongoingtxpor_1 == 1)
      {

        Serial.println("ongoingTxn_m**********entry");
        // added by sai
        Serial.println("Charging**********r");
        evse_ChargePointStatus = Charging;
          delay(500);
        flag_evRequestsCharge = true;
        EvseDevStatus_connector_1 = flag_EVSE_Request_Charge;
        Serial.println("4G_resumetest2");
        if (DEBUG_OUT)
        Serial.println(F("****[EVSE_setOnBoot] on going transaction is true*****"));
        flag_evseStopTransaction = false;
      }
      else if(ongoingtxpor_1 == 0)
      {

       

        Serial.println("Txn id is:$$$$$$$$$$$$$$$$$$$$$$$$$$$@@@@@@@@ " + String(transactionId));
        getChargePointStatusService()->startTransaction(transactionId);
        Serial.println("Txn id is:***********######## " + String(transactionId));
        gu8_powerloss_flag = 1;
        reasonForStop = 5;

        // flag_evseStopTransaction = true;

        Serial.println(F("****EVSE_StopSession_2*****"));

        EVSE_StopSession();
       


      }
#endif
    }

    else
    {
      // added by sai
      // Serial.println("Available**********2");
      // evse_ChargePointStatus = Available;
#if 0
#if LCD_ENABLED
      if (wifi_connect)
      {

        lcd.clear();
        lcd.setCursor(15, 0);
        lcd.print("WI-FI");
        lcd.setCursor(0, 1);
        lcd.print("CHARGER AVAILABLE");
        lcd.setCursor(0, 2);
        lcd.print("TAP RFID/SCAN QR");
        lcd.setCursor(0, 3);
        lcd.print("TO START");
      }
      else if (gsm_connect)
      {

        lcd.clear();
        lcd.setCursor(15, 0);
        lcd.print("4G");
        lcd.setCursor(0, 1);
        lcd.print("CHARGER AVAILABLE");
        lcd.setCursor(0, 2);
        lcd.print("TAP RFID/SCAN QR");
        lcd.setCursor(0, 3);
        lcd.print("TO START");
      }
#endif
#endif
      flag_evRequestsCharge = false;
      if (DEBUG_OUT)
        Serial.println(F("****[EVSE_setOnBoot] on going transaction is false*****"));
    }
#endif

#if 1
    if(gu8_online_flag == 0) 
    {


      //added by sai 
      if(onlineoffline_1 == 1)
      {

        // onlineoffline_flag = 1;
       
       if (DEBUG_OUT) Serial.print("EVSE_setOnReadUserId OFFLINE Callback: EVSE waiting for User ID read...\n"); 
       if ((ongoingTxn_m == 1) && (idTagData_m != "") &&
          (getChargePointStatusService()->getEmergencyRelayClose() == false)) {  //giving priority to stored data
        currentIdTag = resumeTxn.getString("idTagData", "");
        
        Serial.println("[EVSE_setOnReadUserId] OFFLINE Resuming Session");
        requestLed(BLUE, START, 1);
        delay(500);
        requestLed(BLINKYGREEN_EINS, START, 1);
      } else if ((getChargePointStatusService()->getEmergencyRelayClose() == false) &&
                 getChargePointStatusService()->getUnavailable() == false) {
#if LED_ENABLED
        if (millis() - timerForRfid > 5000) { //timer for sending led request
           
           requestLed(BLINKYWHITE_EINS, START, 1); 
           delay(1000);
           requestLed(GREEN, START, 1);
           
          

          timerForRfid = millis();
        }
#endif

        currentIdTag = EVSE_getCurrnetIdTag(&mfrc522);
        
        Serial.println("[OFFLINE]********RFID**********");
      
      }
      }
      
    }
#endif

    
    else if (wifi_connect == true) {
      if ((ongoingTxn_m == 1) && (idTagData_m != "") &&
          (getChargePointStatusService()->getEmergencyRelayClose() == false) &&
          (WiFi.status() == WL_CONNECTED) &&
          (webSocketConncted == true) &&
          (isInternetConnected == true)) {  //giving priority to stored data

        currentIdTag = resumeTxn.getString("idTagData", "");
        Serial.println("[EVSE_setOnReadUserId] Resuming Session");
       
      

        requestLed(BLUE, START, 1);
        delay(500);
        requestLed(BLINKYGREEN_EINS, START, 1);
      } else if ((getChargePointStatusService()->getEmergencyRelayClose() == false) &&
                 (WiFi.status() == WL_CONNECTED) &&
                 (webSocketConncted == true) &&
                 (isInternetConnected == true) && getChargePointStatusService()->getUnavailable() == false) {
#if LED_ENABLED
        if (millis() - timerForRfid > 5000) { //timer for sending led request
          requestLed(GREEN, START, 1);
            
          wifi_reconnected_flag = true;
          timerForRfid = millis();
        }
#endif

        currentIdTag = EVSE_getCurrnetIdTag(&mfrc522);
        


        Serial.println("[WiFi]********RFID**********");
      }/*else if((getChargePointStatusService()->getEmergencyRelayClose() == false)&&
              WiFi.status() != WL_CONNECTED &&
              isInternetConnected == false){

            currentIdTag_Off  = EVSE_getCurrnetIdTag(&mfrc522);
            Serial.println("[OFFL-WIFI]********RFID**********");

            if(currentIdTag_Off.equals("") != true){
              startOfflineTxn();
              currentIdTag = "";
            }

    }*/

    } else if (gsm_connect) {
      if ((ongoingTxn_m == 1) && (idTagData_m != "") &&
          (getChargePointStatusService()->getEmergencyRelayClose() == false) &&
          (client.connected() == true)) {  //giving priority to stored data
        currentIdTag = resumeTxn.getString("idTagData", "");
        
        Serial.println("[EVSE_setOnReadUserId] Resuming Session");
        requestLed(BLUE, START, 1);
        delay(500);
        requestLed(BLINKYGREEN_EINS, START, 1);
      } else if ((getChargePointStatusService()->getEmergencyRelayClose() == false) &&
                 (client.connected() == true) && getChargePointStatusService()->getUnavailable() == false) {
#if LED_ENABLED
        if (millis() - timerForRfid > 5000) { //timer for sending led request
          requestLed(GREEN, START, 1);
          timerForRfid = millis();
        }
#endif

        currentIdTag = EVSE_getCurrnetIdTag(&mfrc522);
        //added by sai
        	// requestLed(BLINKYBLUE, START, 1);
        Serial.println("[GSM]********RFID**********");
      }/*else if((getChargePointStatusService()->getEmergencyRelayClose() == false)&&
      (client.connected() == false)){

          currentIdTag_Off  = EVSE_getCurrnetIdTag(&mfrc522);
          Serial.println("[OFFL-GSM]********RFID**********");

          if(currentIdTag_Off.equals("") != true){
            startOfflineTxn();
            currentIdTag = "";
          }

    }*/
    }
#if 0
    else if(gu8_online_flag == 0) 
    {
      if (DEBUG_OUT) Serial.print(F("EVSE_setOnReadUserId OFFLINE Callback: EVSE waiting for User ID read...\n")); 
      if ((ongoingTxn_m == 1) && (idTagData_m != "") &&
          (getChargePointStatusService()->getEmergencyRelayClose() == false) &&
          (client.connected() == true)) {  //giving priority to stored data
        currentIdTag = resumeTxn.getString("idTagData", "");
        
        Serial.println("[EVSE_setOnReadUserId] OFFLINE Resuming Session");
        requestLed(BLUE, START, 1);
        delay(500);
        requestLed(BLINKYGREEN_EINS, START, 1);
      } else if ((getChargePointStatusService()->getEmergencyRelayClose() == false) &&
                 (client.connected() == true) && getChargePointStatusService()->getUnavailable() == false) {
#if LED_ENABLED
        if (millis() - timerForRfid > 5000) { //timer for sending led request
          requestLed(GREEN, START, 1);
          timerForRfid = millis();
        }
#endif

        currentIdTag = EVSE_getCurrnetIdTag(&mfrc522);
        Serial.println("[OFFLINE]********RFID**********");
      
    }
    }
#endif

    resumeTxn.end();
    if (currentIdTag.equals("") == true) {
      flag_evseReadIdTag = true; //Looping back read block as no ID tag present.
      EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
      flag_evseAuthenticate = false;
      flag_evseStartTransaction = false;
      flag_evRequestsCharge = false;
      flag_evseStopTransaction = false;
      flag_evseUnauthorise = false;
    } else {
      flag_evseReadIdTag = false;
      //Bug : It should not authenticate. Instead, it should start the txn with on going txn id.
      //flag_evseAuthenticate = true; //Entry condition for authentication block.
      //flag_evseAuthenticate = true; //Do not re authenticate ?
      //This change is disabling the stop condition via rfid condition.
      //Make this code 
      if(ongoingTxn_m)
      {
      flag_evseAuthenticate = false;
      if(DEBUG_OUT) Serial.println("****[EVSE_setOnReadUserId] on going transaction is true*****");
      }
      else
      {
        //flag_evseAuthenticate = true;
        //if(DEBUG_OUT) Serial.println(F("****[EVSE_setOnReadUserId] on going transaction is false*****"));
        /*
        * @brief : Reservation implementation
        * by Krishna.M & Sumant 14/07/2022
        */
        //if (flag_evseReserveNow)
        if (reservation_start_flag)
        {
          if (currentIdTag.equals(reserve_currentIdTag) == true)
          {
            evse_ChargePointStatus = Preparing;
            flag_evseAuthenticate = true;
             EvseDevStatus_connector_1 = flag_EVSE_Authentication;
             evse_authorize_state = EVSE_AUTHORIZE_INITIATED;


           // flag_evseReadIdTag = false; // Looping back read block as no ID tag present.
            
            if (DEBUG_OUT)
              Serial.println("****[EVSE_setOnReadUserId] on going, Successful User ID Read : Authentication ID TAG is true*****");
          }
          else
          {
            flag_evseReadIdTag = true; // Looping back read block as no ID tag present.
            EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
            flag_evseAuthenticate = false;
            if (DEBUG_OUT) Serial.print("EVSE_setOnReadUserId Callback: User ID Read : UnAuthentication ID TAG.\n");
          }
        }
        else
        {
          if( remotestart_txn == 0)
          {
          evse_ChargePointStatus = Preparing;
          }
          flag_evseAuthenticate = true;
          EvseDevStatus_connector_1 = flag_EVSE_Authentication;
          evse_authorize_state = EVSE_AUTHORIZE_INITIATED;
          Serial.println("****[flag_EVSE_Authentication]*****");

        }
        if (DEBUG_OUT)
          Serial.println("****[EVSE_setOnReadUserId] on going transaction is false*****");
      }
      flag_evseStartTransaction = false;
      // flag_evRequestsCharge = false;
      flag_evseStopTransaction = false;
      flag_evseUnauthorise = false;
      if (DEBUG_OUT) Serial.print("EVSE_setOnReadUserId Callback: Successful User ID Read. Calling Authentication Block.\n");
    } });

  EVSE_setOnsendHeartbeat([]()
                          {
    if (DEBUG_OUT) Serial.print("EVSE_setOnsendHeartbeat Callback: Sending heartbeat signal...\n");

    if (gu8_online_flag == 1)
    {
    OcppOperation *heartbeat = makeOcppOperation(&webSocket, new Heartbeat());
    initiateOcppOperation(heartbeat);
    heartbeat->setOnReceiveConfListener([](JsonObject payload) {
      const char* currentTime = payload["currentTime"] | "Invalid";
      if (strcmp(currentTime, "Invalid")) {
        if (setTimeFromJsonDateString(currentTime)) {
          if (DEBUG_OUT) Serial.print("EVSE_setOnsendHeartbeat Callback: Request has been accepted!\n");
        } else {
          Serial.print("EVSE_setOnsendHeartbeat Callback: Request accepted. But Error reading time string. Expect format like 2020-02-01T20:53:32.486Z\n");
        }
      } else {
        Serial.print("EVSE_setOnsendHeartbeat Callback: Request denied. Missing field currentTime. Expect format like 2020-02-01T20:53:32.486Z\n");
      }
    }); 
    } });

  EVSE_setOnAuthentication([]()
                           {
    if (DEBUG_OUT) Serial.print("EVSE_setOnAuthentication Callback: Authenticating...\n");
    flag_evseAuthenticate = false;

    OcppOperation *authorize = makeOcppOperation(&webSocket, new Authorize(currentIdTag));
    initiateOcppOperation(authorize);
    chargePointStatusService->authorize(currentIdTag);
    if (gu8_online_flag == 1)
    {
    authorize->setOnReceiveConfListener([](JsonObject payload) {
      const char* status = payload["idTagInfo"]["status"] | "Invalid";
      if (!strcmp(status, "Accepted")) {
        flag_evseReadIdTag = false;
        flag_evseAuthenticate = false;
        flag_evseStartTransaction = true; //Entry condition for starting transaction.
        EvseDevStatus_connector_1 = flag_EVSE_Start_Transaction;
        // evse_start_txn_state = EVSE_START_TXN_INITIATED;
        flag_evRequestsCharge = false;
        flag_evseStopTransaction = false;
        flag_evseUnauthorise = false;

        if (DEBUG_OUT) Serial.print("EVSE_setOnAuthentication Callback: Authorize request has been accepted! Calling StartTransaction Block.\n");
       
        requestLed(BLUE, START, 1);
#if ALPR_ENABLED
         alprAuthorized();
#endif
#if CP_ACTIVE
        flag_controlPAuthorise = true;
#endif
       requestSendAuthCache();
      } else {
#if ALPR_ENABLED
        //alprTxnStopped();
        alprunAuthorized();
#endif
        flag_evseReadIdTag = false;
        flag_evseAuthenticate = false;
        flag_evseStartTransaction = false;
        flag_evRequestsCharge = false;
        flag_evseStopTransaction = false;
        flag_evseUnauthorise = true; //wrong ID tag received, so clearing the global current ID tag variable and setting up to read again.
        EvseDevStatus_connector_1 = flag_EVSE_UnAutharized;
        if (DEBUG_OUT) Serial.print("EVSE_setOnAuthentication Callback: Authorize request has been denied! Read new User ID. \n");
#if LCD_DISPLAY
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("AUTHENTICATION");
        lcd.setCursor(0, 2);
        lcd.print("DENIED");
        Serial.println("DENIED\n");
        delay(100);

#endif
      }
    });
 }
 else if(gu8_online_flag== 0)
 {
  currentIdTag = getChargePointStatusService()->getIdTag();

  getChargePointStatusService()->authorize(currentIdTag);
  flag_evseReadIdTag = false;
  flag_evseAuthenticate = false;
  flag_evseStartTransaction = true; // Entry condition for starting transaction.
  EvseDevStatus_connector_1 = flag_EVSE_Start_Transaction;
  Serial.println("offline flag_EVSE_Start_Transaction ");
  evse_start_txn_state = EVSE_START_TXN_INITIATED;
  flag_evRequestsCharge = false;
  flag_evseStopTransaction = false;
  flag_evseUnauthorise = false;

  if (DEBUG_OUT)
    Serial.print("EVSE_setOnAuthentication Callback: Authorize request has been accepted! Calling StartTransaction Block.\n");

  requestLed(BLUE, START, 1);
#if ALPR_ENABLED
  alprAuthorized();
#endif
#if CP_ACTIVE
  flag_controlPAuthorise = true;
#endif
 } });

#if 0

  EVSE_setOnStartTransaction([]()
                             {
                               flag_evseStartTransaction = false;
                               resumeTxn.begin("resume", false); // opening preferences in R/W mode
                               OcppOperation *startTransaction = makeOcppOperation(&webSocket, new StartTransaction());
                               initiateOcppOperation(startTransaction);
                               if (gu8_online_flag == 1)
                               {
                                 startTransaction->setOnReceiveConfListener([](JsonObject payload)
                                                                            {
      const char* status = payload["idTagInfo"]["status"] | "Invalid";
      if (!strcmp(status, "Accepted") || !strcmp(status, "ConcurrentTx")) {

        flag_evseReadIdTag = false;
        flag_evseAuthenticate = false;
        flag_evseStartTransaction = false;
        flag_evRequestsCharge = true;
        flag_evseStopTransaction = false;
        flag_evseUnauthorise = false;
        if (DEBUG_OUT) Serial.print(F("EVSE_setOnStartTransaction Callback: StartTransaction was successful\n"));
        //added by sai
          evse_ChargePointStatus = Charging;
#if ALPR_ENABLED
    alprTxnStarted();
#endif
        //*****Storing tag data in EEPROM****//
        /*
        * bug identified. Here the currentIdTag is a variable reading rfid. 
        * Ideally when remote start occurs this variable is null! 
        * Needs fixing to be done
        */
       if (currentIdTag.equals("") == true)
       {
        resumeTxn.putString("idTagData", currentIdTag);
        resumeTxn.putBool("ongoingTxn", true);
        resumeTxn.end();
       }
       else
       {
        resumeTxn.putString("idTagData", currentIdTag); 
        resumeTxn.putBool("ongoingTxn", true);
        resumeTxn.putFloat("meterStart", globalmeterstartA);
        resumeTxn.end();
       }
        //***********************************//

      } else {
        flag_evseReadIdTag = false;
        flag_evseAuthenticate = false;
        flag_evseStartTransaction = false;
        flag_evRequestsCharge = false;
        flag_evseStopTransaction = false;
        flag_evseUnauthorise = true; //wrong ID tag received, so clearing the global current ID tag variable and setting up to read again.
        if (DEBUG_OUT) Serial.print(F("EVSE_setOnStartTransaction Callback: StartTransaction was unsuccessful\n"));

#if ALPR_ENABLED
        alprTxnStopped();
#endif
#if CP_ACTIVE
        requestforCP_OUT(STOP);  //stop pwm
#endif
        //resume namespace
        resumeTxn.putBool("ongoingTxn", false);
        resumeTxn.putString("idTagData", "");
        resumeTxn.end();

      } });
                               }
                               else
                               {
                                 flag_evseReadIdTag = false;
                                 flag_evseAuthenticate = false;
                                 flag_evseStartTransaction = false;
                                 flag_evRequestsCharge = true;
                                 flag_evseStopTransaction = false;
                                 flag_evseUnauthorise = false;
                                 if (DEBUG_OUT)
                                   Serial.print(F("EVSE_setOnStartTransaction Callback: StartTransaction was successful\n"));
                                   //added by sai
                                     evse_ChargePointStatus = Charging;
#if ALPR_ENABLED
                                 alprTxnStarted();
#endif
                                 //*****Storing tag data in EEPROM****//
                                 /*
                                  * bug identified. Here the currentIdTag is a variable reading rfid.
                                  * Ideally when remote start occurs this variable is null!
                                  * Needs fixing to be done
                                  */
                                 if (currentIdTag.equals("") == true)
                                 {
                                   resumeTxn.putString("idTagData", currentIdTag);
                                   resumeTxn.putBool("ongoingTxn", true);
                                   resumeTxn.end();
                                 }
                                 else
                                 {
                                   resumeTxn.putString("idTagData", currentIdTag);
                                   resumeTxn.putBool("ongoingTxn", true);
                                   resumeTxn.putFloat("meterStart", globalmeterstartA);
                                   resumeTxn.end();
                                 }
                                 //***********************************//
                               } });

#endif

#if 0
  EVSE_setOnStopTransaction([]()
                            {
                              flag_evseStopTransaction = false;
                              flag_stop_finishing = true;
                              //added by sai
                              // evse_ChargePointStatus = Finishing;
  
                              

                              int txnId = getChargePointStatusService()->getTransactionId();
                              Serial.println("txnId : " + String(txnId));
                              Serial.println("prevTxnId :" + String(prevTxnId));
                              if (txnId != prevTxnId)
                              {
#if 1
// fire StatusNotification
// TODO check for online condition: Only inform CS about status change if CP is online
// TODO check for too short duration condition: Only inform CS about status change if it lasted for longer than MinimumStatusDuration
#if 0
                                OcppOperation *statusNotification = makeOcppOperation(&webSocket,
                                                                                      new StatusNotification(ChargePointStatus::Finishing));
                                initiateOcppOperation(statusNotification);
#endif
#endif

                                OcppOperation *stopTransaction = makeOcppOperation(&webSocket, new StopTransaction());
                                initiateOcppOperation(stopTransaction);
                                if (DEBUG_OUT)

                                Serial.print(F("EVSE_setOnStopTransaction  before Callback: Closing Relays.\n"));
                               LastPresentEnergy_A = (discurrEnergy_A)*1000 + LastPresentEnergy_A;
                               Serial.println("discurrEnergy_A:"+String(discurrEnergy_A));
                               Serial.println("LastPresentEnergy_A:"+String(LastPresentEnergy_A));

                                /**********************Until Offline functionality is implemented***********/
                                // Resume namespace(Preferences)
                                resumeTxn.begin("resume", false);
                                resumeTxn.putBool("ongoingTxn", false);
                                resumeTxn.putString("idTagData", "");
                                resumeTxn.putInt("TxnIdData", -1);
                                Serial.println(F("------Is the session running? ----- "));
                                Serial.println(resumeTxn.getBool("ongoingTxn", false));
                                Serial.println(resumeTxn.getInt("TxnIdData", -2));
                                // delay(10000);
                                resumeTxn.end();
#if 0
                                  //added by sai
                                energymeter.begin("MeterData", false);
                                energymeter.putFloat("currEnergy", 0);
                                Serial.println(" currEnergy: " + String(current_energy));
                                energymeter.end();
#endif

                                // Clear the flag for on going transaction
                                ongoingTxn_m = false;

#if ALPR_ENABLED
                                alprTxnStopped();
#endif

                                if (wifi_connect == true)
                                {
                                  if (!webSocketConncted || WiFi.status() != WL_CONNECTED || isInternetConnected == false)
                                  {
                                    if(onlineoffline_1 == 0)  //added by sai
                                    {
                                      Serial.println("***********onlyonline************");
                                      if(gu8_stoponlineoffline_flag == 1 && (gu8_online_flag == 1))
                                    {
                                    //chargePointStatusService->unauthorize(); // can be buggy
                                    //chargePointStatusService->stopTransaction();
                                    flag_evseReadIdTag = true;
                                    EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
                                    flag_evseAuthenticate = false;
                                    flag_evseStartTransaction = false;
                                    flag_evRequestsCharge = false;
                                    flag_evseStopTransaction = false;
                                    flag_evseUnauthorise = false;
                                    Serial.println("[Wifi] Clearing Stored ID tag in StopTransaction()");
                                     gu8_stoponlineoffline_flag = 0;
                                    }
                                    }

#if 1
                                    //added by sai
                                    else if(onlineoffline_1 == 1)
                                    {

                                      
                                    Serial.println("***********onlineoffline************");
                                    if(gu8_stoponlineoffline_flag == 1 && (gu8_online_flag == 1))
                                    {
                                    chargePointStatusService->unauthorize(); // can be buggy
                                    chargePointStatusService->stopTransaction();
                                    
                                    flag_evseReadIdTag = true;
                                    EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
                                    flag_evseAuthenticate = false;
                                    flag_evseStartTransaction = false;
                                    flag_evRequestsCharge = false;
                                    flag_evseStopTransaction = false;
                                    flag_evseUnauthorise = false;
                                    Serial.println("[Wifi] Clearing Stored ID tag in StopTransaction()");
                                    gu8_stoponlineoffline_flag = 0;

                                    }  


                                    }
#endif

                                  }
                                }
                                else if (gsm_connect == true)
                                {
                                  if (client.connected() == false)
                                  {
                                    if(onlineoffline_1 == 0) //added by sai
                                    {
                                    Serial.println("***********onlineofflinestop************");
                                    
                                    // chargePointStatusService->unauthorize(); // can be buggy
                                    // chargePointStatusService->stopTransaction();
                                    flag_evseReadIdTag = true;
                                    EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
                                    flag_evseAuthenticate = false;
                                    flag_evseStartTransaction = false;
                                    flag_evRequestsCharge = false;
                                    flag_evseStopTransaction = false;
                                    flag_evseUnauthorise = false;
                                    Serial.println("[GSM] Clearing Stored ID tag in StopTransaction()");
                                    }
#if 1
                                    //added by sai
                                    else if(onlineoffline_1 == 1) 
                                    {

                                    Serial.println("***********offlinestop************");
                                      if(gu8_stoponlineoffline_flag == 1 && (gu8_online_flag == 1))
                                    {
                                    chargePointStatusService->unauthorize(); // can be buggy
                                    chargePointStatusService->stopTransaction();
                                    flag_evseReadIdTag = true;
                                    EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
                                    flag_evseAuthenticate = false;
                                    flag_evseStartTransaction = false;
                                    flag_evRequestsCharge = false;
                                    flag_evseStopTransaction = false;
                                    flag_evseUnauthorise = false;
                                    Serial.println("[GSM] Clearing Stored ID tag in StopTransaction()");
                                     gu8_stoponlineoffline_flag = 0;
                                    }
                                    }
#endif
                                  }
                                }
                                requestForRelay(STOP, 1);
                                //  requestLed(VOILET,START,1);
                                delay(500);
                                /***************************************************************************/
                                if (gu8_online_flag == 1)
    {
                                stopTransaction->setOnReceiveConfListener([](JsonObject payload)
                                                                          {
      flag_evseReadIdTag = false;
      flag_evseAuthenticate = false;
      flag_evseStartTransaction = false;
      flag_evRequestsCharge = false;
      flag_evseStopTransaction = false;
      flag_evseUnauthorise = true;

      if (DEBUG_OUT) Serial.print(F("EVSE_setOnStopTransaction Callback: Closing Relays.\n"));
      //added by sai
      // evse_ChargePointStatus = Finishing;
      if (DEBUG_OUT) Serial.print(F("EVSE_setOnStopTransaction Callback: StopTransaction was successful\n"));
      if (DEBUG_OUT) Serial.print(F("EVSE_setOnStopTransaction Callback: Reinitializing for new transaction. \n"));
       evse_ChargePointStatus = Available;
       });
    }
    else
    {
      flag_evseReadIdTag = false;
      flag_evseAuthenticate = false;
      flag_evseStartTransaction = false;
      flag_evRequestsCharge = false;
      flag_evseStopTransaction = false;
      flag_evseUnauthorise = true;
      if (DEBUG_OUT) Serial.print(F("EVSE_setOnStopTransaction Callback: Closing Relays.\n"));
      //added by sai
      // evse_ChargePointStatus = Finishing;
      if (DEBUG_OUT) Serial.print(F("EVSE_setOnStopTransaction Callback: StopTransaction was successful\n"));
      if (DEBUG_OUT) Serial.print(F("EVSE_setOnStopTransaction Callback: Reinitializing for new transaction. \n"));
    }

                              }
                              else
                              {
                                Serial.println(F("[EVSE] EVSE_setOnStopTransaction already called. Skipping make OCPP operation to avoid duplication"));
                              } });

#endif

  EVSE_setOnUnauthorizeUser([]()
                            {
    if (flag_evseSoftReset == true) {
      //This 'if' block is developed by @Wamique.
      flag_evseReadIdTag = false;
      flag_evseAuthenticate = false;
      flag_evseStartTransaction = false;
      flag_evRequestsCharge = false;
      flag_evseStopTransaction = false;
      flag_evseUnauthorise = false;
      flag_rebootRequired = true;
      if (DEBUG_OUT) Serial.println("EVSE_setOnUnauthorizeUser Callback: Initiating Soft reset");
    } else if (flag_evseSoftReset == false) {
      //added by sai
      
    
       Serial.println("Available**********3");
       if(notFaulty_A)
       {
      
        Serial.print("Available.........2");
        
        evse_ChargePointStatus = Available;
        
        
       }
      
      flag_evseReadIdTag = true;
      EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
      flag_evseAuthenticate = false;
      flag_evseStartTransaction = false;
      flag_evRequestsCharge = false;
      flag_evseStopTransaction = false;
      flag_evseUnauthorise = false;
      if (DEBUG_OUT) Serial.print("EVSE_setOnUnauthorizeUser Callback: Unauthorizing user and setting up for new user ID read.\n");
      chargePointStatusService->unauthorize();

    } });

  if (DEBUG_OUT)
    Serial.println("Waiting for Web Socket Connction...");
#if 0
  while (!webSocketConncted && wifi_connect == true)
  {
    Serial.print("*");
    delay(50); // bit**
    webSocket.loop();
    bluetooth_Loop();
  } // can add break, when Offline functionality is implemented
#endif
#if 1
  while ((!webSocketConncted && wifi_connect == true) && gu8_check_online_count2)
  {
    Serial.print("*");
    delay(50); // bit**
    webSocket.loop();
    bluetooth_Loop();
    Serial.println("gu8_check_online_count2:" + String(gu8_check_online_count2));
    gu8_check_online_count2--;
  }
#endif

  gu8_check_online_count2 = 10;

  if (gsm_enable)
  {
    if (wifi_enable == false)
    {
      gu8_check_online_count = 2;
      gu8_check_online_count2 = 2;
    }
  }

  EVSE_initialize();

  Serial.println("Stored ID:" + String(idTagData_m));
  Serial.println("Ongoing Txn: " + String(ongoingTxn_m));

  // requestLed(GREEN,START,1);
  PowerCycleInit();
  flagswitchoffBLE = false;
  gu8_BLE_off_flag = 0;
  Serial.println("End of Setup");
#if DWIN_ENABLED
  err = DWIN_SET(clear_tap_rfid, sizeof(clear_tap_rfid) / sizeof(clear_tap_rfid[0]));
  delay(50);
  err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
  delay(50);

  // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
  // delay(50);
#endif

} // end of setup()

void deleteFlash()
{
  nvs_flash_erase(); // erase the NVS partition and...
  nvs_flash_init();  // initialize the NVS partition.
}

void loop()
{

  Serial.println("****************************************************************************S***********************************************");
#if BLE_ENABLE

  if (flagswitchoffBLE == true)
  {
    flagswitchoffBLE = false;
    Serial.println("Disconnecting BT");
    // SerialBT.println("Wifi Connected");
    SerialBT.println("Disconnecting BT");
    delay(100);
    SerialBT.flush();
    SerialBT.disconnect();
    SerialBT.end();
    Serial.println(ESP.getFreeHeap());
  }
  else
  {
    if (gu8_BLE_off_flag == 0)
    {
      bluetooth_Loop();
      Serial.println("millis() : " + String(millis()));
      Serial.println("TIME DIFF : " + String(millis() - startBLETime));
      if (millis() - startBLETime >= TIMEOUT_BLE)
      {
        gu8_BLE_off_flag = 1;
        flagswitchoffBLE = true;
        Serial.println("millis() : " + String(millis()));
      }
    }
  }

#if 0
  if (millis() - startBLETime < TIMEOUT_BLE)
  {
    bluetooth_Loop();
    flagswitchoffBLE = true;
    Serial.println("millis() : " +String(millis()));
  }
  else
  {
    if (flagswitchoffBLE == true)
    {
      flagswitchoffBLE = false;
      Serial.println("Disconnecting BT");
      // SerialBT.println("Wifi Connected");
      SerialBT.println("Disconnecting BT");
      delay(100);
      SerialBT.flush();
      SerialBT.disconnect();
      SerialBT.end();
      Serial.println(ESP.getFreeHeap());
    }
  }
#endif

  /*
  #if DWIN_ENABLED
    display_avail();
  #endif
  */

#endif

//  Serial.println("flagCP: "+String(flag_controlPAuthorise));
#if 0
  if (gu8_online_flag == 1)
  {
    ocppEngine_loop();
  }
#endif

  webSocket.loop();

  if (evse_ChargePointStatus == NOT_SET && flag_evseIsBooted)
  {

    chargePointStatusService->loop();
  }

  // if (webSocketConncted == 1)
  // {
  //   ocppEngine_loop();
  // }

  EVSE_ReadInput(&mfrc522);

  EVSE_loop();
  //  digitalWrite(16,HIGH);
  // emergencyRelayClose_Loop();
  /*
   * @brief : Reservation will be enabled
   */
  EVSE_Reservation_loop();

  // EVSE_loop();

  if (flag_evseIsBooted)
  {
    Serial.println(".............");
    chargePointStatusService->loop();
  }

  emergencyRelayClose_Loop();
// meteringService->loop();
// added by sai
#if 1
  switch (evse_ChargePointStatus)
  {
  case Charging:
    if (millis() - timerMv_count >= (meterSampleInterval))
    {
      timerMv_count = millis();
      uint32_t lu32_start = millis();
      Serial.println("start meter time:" + String(lu32_start));
      meteringService->loop();
      uint32_t lu32_end = millis();
      Serial.println("end meter time:" + String(lu32_end));
    }
    break;
  default:
    // switch (evse_ChargePointStatus)
    // {
    // case EVSE_Available:
    // case EVSE_Preparing:
    // case EVSE_SuspendedEVSE:
    // case EVSE_SuspendedEV:
    // case EVSE_Finishing:
    // case EVSE_Reserved:
    // case EVSE_Unavailable:
    // case EVSE_Faulted:
#if 0
      if (millis() - timerHb > ((heartbeatInterval * 1000) - 200)) //
      {
        timerHb = millis();
        if (gu8_online_flag == 1)
        {
          if (DEBUG_OUT)
            Serial.print("EVSE_Sending heartbeat signal...\n");
          OcppOperation *heartbeat = makeOcppOperation(&webSocket, new Heartbeat());
          initiateOcppOperation(heartbeat);
          heartbeat->setOnReceiveConfListener([](JsonObject payload)
                                              {
      const char* currentTime = payload["currentTime"] | "Invalid";
      if (strcmp(currentTime, "Invalid")) {
          if (setTimeFromJsonDateString(currentTime)) {
            if (DEBUG_OUT) Serial.print("EVSE_setOnsendHeartbeat Callback: Request has been accepted!\n");
          } else {
            Serial.print("EVSE Expect format like 2020-02-01T20:53:32.486Z\n");
          }
        } else {
          Serial.print("EVSE Missing field currentTime. Expect format like 2020-02-01T20:53:32.486Z\n");
        } });
        }
      }
#endif
    //   break;
    // }
    break;
  }
#endif

  if (webSocketConncted == 1)
  {
    ocppEngine_loop();
  }

#if ALPR_ENABLED
  alprRead_loop();
  if (flag_noVehicle)
  {
    EVSE_StopSession();
    flag_noVehicle = false;
  }
#endif

  // check_connectivity();
  emergencyRelayClose_Loop();

  // webSocket.loop();
  internetLoop();
  cloudConnectivityLed_Loop();
  webSocket.loop();
// added by sai
#if 0
  // commenting for testing wifi_loop switch logic
  // if (session_ongoing_flag == false && webSocketConncted == 0)
  if (webSocketConncted == 0)
  {
    // Serial.println("session_ongoing_flag Internet loop 1");
    if (gu8_websocket_begin_once == 1)
    {
      Serial.println("gu8_websocket_begin_once 1");
      gu8_websocket_begin_once = 0;
      Serial.println("host_m Name:" + String(host_m));
      Serial.println("port_m Name:" + String(port_m));
      Serial.println("path_m Name:" + String(path_m));
      Serial.println("protocol Name:" + String(protocol));
      // webSocket.begin(host_m, port_m, path_m, protocol);
      connectToWebsocket();
    }
    internetLoop();
    webSocket.loop();
  }
#endif

// if(gu8_online_flag == 0)
// {
//   check_connectivity();
// }

// #if WIFI_ENABLED                                //Not using this for Surya's model
//    wifi_Loop();
// #endif
// webSocket.loop();
// cloudConnectivityLed_Loop();
// #if WIFI_ENABLED || ETHERNET_ENABLED
// webSocket.loop();
//    #if ETHERNET_ENABLED
//    //  Ethernet.maintain();
//    //  ethernetLoop();
//    //  webSocket.loop();
//    #endif
// #endif
/*
  #if GSM_ENABLED
  gsm_Loop();
  #endif
*/
// LED_Loop check internet status every 30 sec

// smartChargingService->loop();
/*
 * @brief : Change made by G. Raja Sumant
 * 19/07/2022
 * Status notification should be sent only when boot notification gets accepted
 */
#if 0
  if (flag_evseIsBooted)
  {
    chargePointStatusService->loop();
  }
  meteringService->loop();
#endif

#if LCD_DISPLAY
  stateTimer();
  disp_lcd_meter();
#endif
// commented by sai for 12hours
//  PowerCycle_Loop();
#if DWIN_ENABLED
  stateTimer();
  disp_dwin_meter();
#endif

#if CP_ACTIVE
  ControlP_loop();
#endif
#if 0
  // ota_Loop();
  if (fota_available)
  {
    if (wifi_connect)
    {
      FOTA_WiFi();
    }
    else if (gsm_connect)
    {
      setup_4G_OTA();
    }
  }

#endif

  // added by sai

  Serial.println("Websocket: " + String(webSocketConncted));
  if (webSocketConncted == 1)
  {

    Serial.println("***************web_socketconnected***********");
  }
  if (wifi_connect)
  {
    Serial.printf("%4d - SIGNAL STRENGTH \n", WiFi.RSSI());
  }
  Serial.println("FREE HEAP");
  Serial.println(ESP.getFreeHeap());
  Serial.println("\n*********************************************************E**************************************");
  // delay(700);
}

#if DWIN_ENABLED
#if 1
/***************************************EVSE_READINPUT BLOCK*********************************************************/
String readIdTag = "";
void EVSE_ReadInput(MFRC522 *mfrc522)
{ // this funtion should be called only if there is Internet
  readIdTag = "";
  bool result = false;
  unsigned long tout = millis();
  int8_t readConnectorVal = 0;
  readIdTag = readRfidTag(true, mfrc522);
  if (readIdTag.equals("") == false)
  {
    // EVSE_StopTxnRfid(readIdTag);
    // readConnectorVal = requestConnectorStatus();
#if DWIN_ENABLED
    change_page[9] = 3; // change to page 3 and wait for input
    uint8_t err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);
    err = DWIN_SET(select_connector, sizeof(select_connector) / sizeof(select_connector[0]));
    delay(10);
    flush_dwin();
    requestLed(BLUE, START, 1);
    requestLed(BLUE, START, 2);
    requestLed(BLUE, START, 3);
    while (millis() - tout < 15000)
    {

      readConnectorVal = dwin_input();
      /*
        @brief: If we want to use both the physical switches/display
      */
      // readConnectorVal = requestConnectorStatus();
      if (readConnectorVal > 0)
      {
        // change_page[9] = 6; // change to page 3 and wait for input
        // err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
        /*switch (readConnectorVal)
        {
          case 1: err = DWIN_SET(cid1, sizeof(cid1) / sizeof(cid1[0]));
            break;
          case 2: err = DWIN_SET(cid2, sizeof(cid2) / sizeof(cid2[0]));
            break;
          case 3: err = DWIN_SET(cid3, sizeof(cid3) / sizeof(cid3[0]));
            break;
        }*/
        // bool result = assignEvseToConnector(readIdTag, readConnectorVal);

        if (offline_connect)
        {
          currentIdTag = readIdTag;
          Serial.println(F("******selecting via offline*****"));
          // result = assignEvseToConnector_Offl(readIdTag, readConnectorVal);
        }
        else
        {
          uint8_t result_checker = 0;
          Serial.println(F("******selecting via online*****"));
          // result = assignEvseToConnector(readIdTag, readConnectorVal);
          /*
           * @brief ReserveNow for A,B,C condition checker.
           */

          if (reservation_start_flag)
          {
            if (readIdTag.equals(reserve_currentIdTag))
            {
              // result = assignEvseToConnector(readIdTag, readConnectorVal);
              if (result)
              {
                result_checker++;
              }
            }
            else
            {
              result = false;
            }
          }

          // if (result_checker == 0)
          // result = assignEvseToConnector(readIdTag, readConnectorVal);
        }
        if (result == true)
        {
          Serial.println(F("Attached/Detached EVSE to the requested connector"));
          break;
        }
        else
        {
          Serial.println(F("Unable To attach/detach EVSE to the requested connector"));
          // err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
          // delay(10);

          // break;
        }
      }
      else
      {
        Serial.println(F("Invalid Connector Id Received"));
        /*err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
          delay(10);
          change_page[9] = 0; // change to page 3 and wait for input
          uint8_t err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
          delay(10);*/
        // break;
        // delay(2000);
      }
    }
    change_page[9] = 0; // Now it should come back to home page
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);
#endif
  }
  // delay(100);
  /*  uint8_t err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
    delay(10);
    change_page[9] = 0; // change to page 0 and wait for input
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);

    change_page[9] = 0; // change to page 0 and wait for input
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);

    change_page[9] = 0; // change to page 0 and wait for input
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);*/
}
#endif

#if 0
/***************************************EVSE_READINPUT BLOCK*********************************************************/
String readIdTag = "";
void EVSE_ReadInput(MFRC522* mfrc522) {    // this funtion should be called only if there is Internet
  readIdTag = "";
  unsigned long tout = millis();
  int8_t readConnectorVal = 0;
  readIdTag = readRfidTag(true, mfrc522);
  if (readIdTag.equals("") == false) {
    //EVSE_StopTxnRfid(readIdTag);
    //readConnectorVal = requestConnectorStatus();
#if DWIN_ENABLED
    change_page[9] = 2; // change to page 3 and wait for input
    uint8_t err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);
    flush_dwin();
    
      //readConnectorVal = dwin_input();
      /*
        @brief: If we want to use both the physical switches/display
      */
      readConnectorVal = requestConnectorStatus();

      if (readConnectorVal > 0) {
        change_page[9] = 6; // change to page 3 and wait for input
        err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
        switch (readConnectorVal)
        {
          case 1: err = DWIN_SET(cid1, sizeof(cid1) / sizeof(cid1[0]));
            break;
          case 2: err = DWIN_SET(cid2, sizeof(cid2) / sizeof(cid2[0]));
            break;
          case 3: err = DWIN_SET(cid3, sizeof(cid3) / sizeof(cid3[0]));
            break;
        }
        bool result = assignEvseToConnector(readIdTag, readConnectorVal);
        if (result == true) {
          Serial.println(F("Attached/Detached EVSE to the requested connector"));
          //break;
        } else {
          Serial.println(F("Unable To attach/detach EVSE to the requested connector"));
          //err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
          //delay(10);
           change_page[9] = 0; // change to page 3 and wait for input
     err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));

          //break;
        }
      } else {
        Serial.println(F("Invalid Connector Id Received"));
          change_page[9] = 0; // change to page 3 and wait for input
     err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
        /*err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
          delay(10);
          change_page[9] = 0; // change to page 3 and wait for input
          uint8_t err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
          delay(10);*/
        //break;
        //delay(2000);
      }

#endif
  }
  //delay(100);
  /*  uint8_t err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
    delay(10);
    change_page[9] = 0; // change to page 0 and wait for input
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);

    change_page[9] = 0; // change to page 0 and wait for input
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);

    change_page[9] = 0; // change to page 0 and wait for input
    err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
    delay(10);*/

}
#endif
#endif

/*
   Called by Websocket library on incoming message on the internet link
*/
// #if WIFI_ENABLED || ETHERNET_ENABLED
extern OnSendHeartbeat onSendHeartbeat;
int wscDis_counter = 0;
int wscConn_counter = 0;
#if 0
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    webSocketConncted = false;
    // gu8_online_flag = 0;
    Serial.println("Counter:" + String(wscDis_counter));
    if (DEBUG_OUT)
      Serial.print("[WSc] Disconnected!!!\n");
    if (wscDis_counter++ > 2)
    {
      delay(200);
      Serial.println("Trying to reconnect to WSC endpoint");
      wscDis_counter = 0;
      offline_connect = true;
      gu8_online_flag = 0;
      gu8_check_online_count = 10;
      gu8_check_online_count2 = 10;
      // Serial.println("URL:" + String(url_m));
      Serial.println("URL:" + String(path_m));
      //  webSocket.beginSSL(host_m.c_str(), port_m, url_m.c_str(), protocol_m);
      // webSocket.begin(host_m.c_str(), port_m, url_m.c_str(), protocol_m);
      webSocket.begin(host_m, port_m, path_m, protocol);
      while (!webSocketConncted)
      { // how to take care if while loop fails
        Serial.print("..***..");
        delay(100);       // bit**
        webSocket.loop(); // after certain time stop relays and set fault state
        if (wscConn_counter++ > 30)
        {
          wscConn_counter = 0;
          Serial.println("[Wsc] Unable To Connect");
          break;
        }
      }
    }
    // have to add websocket.begin if websocket is unable to connect //Static variable
    break;
  case WStype_CONNECTED:
    webSocketConncted = true;
    offline_connect = false;
  if(gu8_online_flag == 0)
  {
      evse_ChargePointStatus = NOT_SET;
  }
        // Serial.println("Web socket NOT_SET ");
    
    gu8_online_flag = 1;
    // evse_ChargePointStatus = NOT_SET;
    // Serial.println("Web socket NOT_SET ");


    if (DEBUG_OUT)
      Serial.printf("[WSc] Connected to url: %s\n", payload);
    break;
  case WStype_TEXT:
    if (DEBUG_OUT)
      if (DEBUG_OUT)
        Serial.printf("[WSc] get text: %s\n", payload);

    if (!processWebSocketEvent(payload, length))
    { // forward message to OcppEngine
      if (DEBUG_OUT)
        Serial.print("[WSc] Processing WebSocket input event failed!\n");
    }
    break;
  case WStype_BIN:
    if (DEBUG_OUT)
      Serial.print("[WSc] Incoming binary data stream not supported");
    break;
  case WStype_PING:
    // pong will be send automatically
    if (DEBUG_OUT)
      Serial.print("[WSc] get ping\n");
    break;
  case WStype_PONG:
    // answer to a ping we send
    if (DEBUG_OUT)
      Serial.print("[WSc] get pong\n");
    break;
  }
}
#endif
/*wifi dirver*/

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    webSocketConncted = false;
    gu8_online_flag = 0;
    Serial.print("[WSc] Disconnected!!!\n");

    Serial.println("Trying to reconnect to WSC endpoint");

//  Serial.println("URL:" + String(path_m));
//  webSocket.begin(host_m, port_m, path_m, protocol);

// webSocket.loop();

// gu8_online_flag = 0;
#if 1
    Serial.println("Counter:" + String(wscDis_counter));
    if (DEBUG_OUT)
      Serial.print("[WSc] Disconnected!!!\n");
    if (wscDis_counter++ > 2)
    {
      delay(200);
      Serial.println("Trying to reconnect to WSC endpoint");
      wscDis_counter = 0;
      offline_connect = true;
      gu8_online_flag = 0;
      gu8_check_online_count = 10;
      gu8_check_online_count2 = 10;
      // Serial.println("URL:" + String(url_m));
      Serial.println("URL:" + String(path_m));
      //  webSocket.beginSSL(host_m.c_str(), port_m, url_m.c_str(), protocol_m);
      // webSocket.begin(host_m.c_str(), port_m, url_m.c_str(), protocol_m);
      webSocket.begin(host_m, port_m, path_m, protocol);
      while (!webSocketConncted)
      { // how to take care if while loop fails
        Serial.print("..***..");
        delay(100);       // bit**
        webSocket.loop(); // after certain time stop relays and set fault state
        if (wscConn_counter++ > 30)
        {
          wscConn_counter = 0;
          Serial.println("[Wsc] Unable To Connect");
          break;
        }
      }
    }
#endif
    // have to add websocket.begin if websocket is unable to connect //Static variable
    break;
  case WStype_CONNECTED:
    webSocketConncted = true;
    offline_connect = false;
    if (gu8_online_flag == 0)
    {
      Serial.print("NOT SET ");
      evse_ChargePointStatus = NOT_SET;
    }
    // Serial.println("Web socket NOT_SET ");

    gu8_online_flag = 1;
    // evse_ChargePointStatus = NOT_SET;
    // Serial.println("Web socket NOT_SET ");
    if (DEBUG_OUT)
      Serial.printf("[WSc] Connected to url: %s\n", payload);
    break;
  case WStype_TEXT:
    if (DEBUG_OUT)
      if (DEBUG_OUT)
        Serial.printf("[WSc] get text: %s\n", payload);

    if (!processWebSocketEvent(payload, length))
    { // forward message to OcppEngine
      if (DEBUG_OUT)
        Serial.print("[WSc] Processing WebSocket input event failed!\n");
    }
    break;
  case WStype_BIN:
    if (DEBUG_OUT)
      Serial.print("[WSc] Incoming binary data stream not supported");
    break;
  case WStype_PING:
    // pong will be send automatically
    if (DEBUG_OUT)
      Serial.print("[WSc] get ping\n");
    break;
  case WStype_PONG:
    // answer to a ping we send
    if (DEBUG_OUT)
      Serial.print("[WSc] get pong\n");
    break;
  }
}
// #endif

#if DWIN_ENABLED
int8_t dwin_input()
{

  button = DWIN_read();
  Serial.printf("Button pressed : %d", button);
  // delay(50);
  return button;
}
#endif

// #if WIFI_ENABLED
#if 0
int wifi_counter = 0;
void wifi_Loop()
{
  Serial.print("[WiFi_Loop]: ");
  if (WiFi.status() != WL_CONNECTED)
  {
    // delay(200);
    Serial.println("Disconnected!!!");
    if (wifi_counter++ > 5 && (WiFi.status() != WL_CONNECTED))
    {
      wifi_counter = 0;
      Serial.print(".");
      WiFi.disconnect();
      delay(500);
      Serial.println("[WIFI] Trying to reconnect again");
      // WiFi.reconnect();
      // WiFi.begin(ssid_m.c_str(),key_m.c_str());
      WiFi.begin(ssid_m.c_str(), key_m.c_str());
      wifi_connection_available = 1;
      delay(1500);
    }
  }

// added by sai
#if 1
  else
  {
    if (WiFi.status() == WL_CONNECTED)
    {

      // online_to_offline_flag = false;
      if (wifi_connection_available == 1)
      {
        delay(100);
        wifi_connection_available = 0;
        connectToWebsocket();
      }
      webSocket.loop();
    }
#if DISPLAY_ENABLED
    while (millis() - cloud_refresh > 5000)
    {
      // cloud offline
      cloud_refresh = millis();
      cloudConnect_Disp(3);
      checkForResponse_Disp();
    }
#endif
  }
#endif
}

#endif
// #endif

#if DWIN_ENABLED
extern bool disp_evse_A;
extern bool EMGCY_FaultOccured;
void display_avail()
{
  // Serial.println(F("[DWIN MAIN] Trying to update status"));
  // if (millis() - timer_dwin_avail > 5000)
  //{
  // timer_dwin_avail = millis();
  uint8_t faulty_count = 0;
  uint8_t err = 0;
  if (isInternetConnected)
  {
    if (notFaulty_A && !EMGCY_FaultOccured && !disp_evse_A && !evse_A_unavail && !flag_nopower)
    {
      // avail[4] = 0X55;
      // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
      // delay(10);
    }
    else
    {
      // Serial.println(F("****Display A faulty or charging****"));
      faulty_count++;
      // err = DWIN_SET(unavail, sizeof(unavail) / sizeof(unavail[0]));
      // delay(10);
    }
  }
  else

  {
    Serial.println(F("Internet not available : Hence not updating status."));
    /*
     * @brief: Here all 3 must go to unavailable! By G. Raja Sumant 02/09/2022
     */
#if DWIN_ENABLED
    if (!flag_nopower)
    {
      CONN_UNAVAIL[4] = 0X66;
      err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
      CONN_UNAVAIL[4] = 0X71;
      err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
      CONN_UNAVAIL[4] = 0X7B;
      err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
    }
    // When all 3 are not available!
    faulty_count = 3;
#endif
  }
  /*
   * @brief : This is kept outside as it has some issues inside
   */

  if (faulty_count >= 3)
  {
// When all 3 are not available!
#if DWIN_ENABLED
    err = DWIN_SET(unavail, sizeof(unavail) / sizeof(unavail[0]));
    delay(10);
#endif
  }
  else
  {
#if DWIN_ENABLED
    // err = DWIN_SET(clear_tap_rfid, sizeof(clear_tap_rfid) / sizeof(clear_tap_rfid[0]));
    // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
#endif
    // Serial.println(F("[TAP RFID]"));
  }
  //}
}
#endif

short int counterPing = 0;
void cloudConnectivityLed_Loop()
{
#if GSM_PING
  if (gsm_connect == true)
  {
    if (counterPing++ >= 3 && !flag_ping_sent)
    {
      gu8_online_flag = 1;
      // sending ping after every 30 sec [if internet is not there sending ping packet itself consumes 10sec]
      Serial.println("sending ping..........!!!!!!@@@@@@#########################>>>>>>>>.......");
      // char c = 0x09;
      // client.write(0X09); // send a ping
      // client.write("[9,\"heartbeat\"]"); // send a ping
      /*String p = "9";
      sendPingGsmStr(p);*/
      String p = "rockybhai";
      sendPingGsmStr(p);
      // sendFrame(WSop_ping,"HB",size_t(2),true,true);
      // flag_ping_sent = true;
      counterPing = 0;
      // check for pong inside gsmOnEvent
    }
  }
#endif
  // Serial.println("Inside cloudConnectivityLed_Loop........................!!!!!!!!!!!!11");
#if DWIN_ENABLED
  uint8_t err = 0;
#endif

  //  Serial.println("*wifi_connect******************************************************* " + String(wifi_connect));
  if (wifi_connect == true)
  {

#if 0
    if (counterPing++ >= 3)
    { // sending ping after every 30 sec [if internet is not there sending ping packet itself consumes 10sec]
      isInternetConnected = webSocket.sendPing();
      Serial.println("*Sending Ping To Server: #@@@@@@@@@@@@@" + String(isInternetConnected));
      // added by sai
      if (isInternetConnected == 1)
      {
        Serial.println("***********pong_Received*************");
      }
      // added by sai
      if (isInternetConnected == 0)
      {
        if (webSocketConncted == true)
        {
          WiFi.disconnect(true);
          gu8_websocket_begin_once = 1;
          webSocketConncted = false;
          // online_to_offline_flag = 1;
        }
      }

      counterPing = 0;
    }

#endif
    // if ((WiFi.status() != WL_CONNECTED || webSocketConncted == false || isInternetConnected == false) && getChargePointStatusService()->getEmergencyRelayClose() == false)

    // added by sai
    //   Serial.println("*isInternetConnected: " + String(isInternetConnected));
    //  Serial.println("*webSocketConncted: " + String(webSocketConncted));

    if ((WiFi.status() != WL_CONNECTED || webSocketConncted == false || isInternetConnected == false))
    { // priority is on fault
      gu8_check_online_count2 = 10;
      if (millis() - timercloudconnect > 15000) /* changed the Timeout for disconnect from 10000 to 15000 @ 24032023*/
      {                                         // updates in 5sec

#if LED_ENABLED
        if (ARAI_BOARD)
        {
          // Serial.println("*notFaulty_A " + String(notFaulty_A));
          if (notFaulty_A)
          {
            wifi_reconnected_flag = false;
            Serial.println("*notFaulty_A ");
            // added by sai
            //  Serial.println("*test1 ");
            // requestLed(BLINKYWHITE_EINS, START, 1); // No internet
          }
        }
#endif

#if DWIN_ENABLED
        err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0]));
        avail[4] = 0x51;
        err = DWIN_SET(avail, sizeof(clear_avail) / sizeof(clear_avail[0]));
        err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0]));
#endif

        if (getChargePointStatusService()->getEmergencyRelayClose() == false)
        {
#if 0
#if LCD_ENABLED
          lcd.clear();
          lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
          lcd.print("STATUS: UNAVAILABLE");
          // lcd.setCursor(0, 1);
          // lcd.print("TAP RFID/SCAN QR");
          lcd.setCursor(0, 2);
          lcd.print("CONNECTION");
          lcd.setCursor(0, 3);
          lcd.print("CLOUD: OFFLINE");
          // lcd.print("CLOUD: offline");
#endif
#endif
          // added by sai
          if (onlineoffline_1 == 1)
          {
#if 1
#if LCD_ENABLED
            lcd.clear();
            lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
            lcd.print("CHARGER AVAILABLE");
            lcd.setCursor(0, 1);
            lcd.print("TAP RFID/SCAN QR");
            lcd.setCursor(0, 3);
            lcd.print("TO START");
            lcd.setCursor(0, 4);
            lcd.print("CLOUD: OFFLINE");

#endif
#endif
          }
          if (gu8_online_flag == 0)
          {
            if (onlineoffline_1 == 0)
            {
              requestLed(BLINKYWHITE_EINS, START, 1);

              uint8_t lud_count = 0;
              lud_count++;
              if (lud_count >= 1)
              {
                lud_count = 0;

#if LCD_ENABLED
                lcd.clear();
                lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
                lcd.print("CHARGER  UNAVAILABLE");

                lcd.setCursor(0, 2);
                lcd.print("OFFLINE");
#endif
              }
            }
          }
          timercloudconnect = millis();
        }
      }
#if 0
    err = DWIN_SET(wi, sizeof(wi) / sizeof(wi[0]));
#endif
    }
    // else if (gsm_connect == true && client.connected() == false && getChargePointStatusService()->getEmergencyRelayClose() == false)
    else if (gsm_connect == true && client.connected() == false)
    {

      if (millis() - timercloudconnect > 15000) /* changed the Timeout for disconnect from 10000 to 15000 @ 24032023*/
      {                                         // updates in 5sec
#if LED_ENABLED
        if (ARAI_BOARD)
        {
          if (notFaulty_A)
          {
            Serial.println("*notFaulty_A ");
            // Serial.println("*test2 ");
            requestLed(BLINKYWHITE_EINS, START, 1); // No internet
          }
        }
#endif
#if 0
      err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0]));
#endif
        timercloudconnect = millis();
        if (getChargePointStatusService()->getEmergencyRelayClose() == false)
        {
#if 0
#if LCD_ENABLED
        lcd.clear();
        lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
        lcd.print("CHARGER  UNAVAILABLE");
        // lcd.setCursor(0, 2);
        // lcd.print("CONNECTION");
        lcd.setCursor(0, 3);
        lcd.print("OFFLINE");

#endif
#endif
        }
      }
    }
  }
}

void connectToWebsocket()
{

  // url_m = String(ws_url_prefix_m);
  //  String cpSerial = String("");
  //  EVSE_A_getChargePointSerialNumber(cpSerial);
  //  url_m += cpSerial; //most OCPP-Server require URLs like this. Since we're testing with an echo server here, this is obsolete

  // #if WIFI_ENABLED || ETHERNET_ENABLED
  Serial.print("Connecting to: ");
  Serial.println(path_m);
  // webSocket.begin(host_m, port_m, url_m, protocol_m);

  Serial.println(String(host_m.c_str()) + "::" + String(port_m) + "::" + String(path_m.c_str()) + "::" + String(protocol));

  webSocket.begin(host_m, port_m, path_m, protocol);
  // event handler

  webSocket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);

  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocket.enableHeartbeat(25000, 3000, 2);
  // webSocket.enableHeartbeat(60000, 9000, 3);
  // server address, port and URL
  Serial.print("Connected to WebSockets Server @ IP address: ");
  // Serial.println(WS_SERVER);
  Serial.println(String(host_m.c_str()));
  // #endif
}

/*
 * @brief : This is the old way of doing it.
 * G. Raja Sumant
 * Migrating it to url parser like AC001 to maintain uniformity
 */
#if 0
void connectToWebsocket() {

  url_m = String(ws_url_prefix_m);
  String cpSerial = String("");
  EVSE_getChargePointSerialNumber(cpSerial);
  url_m += cpSerial; //most OCPP-Server require URLs like this. Since we're testing with an echo server here, this is obsolete

  //#if WIFI_ENABLED || ETHERNET_ENABLED
  Serial.println(url_m);
  webSocket.begin(host_m.c_str(), port_m, url_m.c_str(), protocol_m);

  Serial.println(String(host_m.c_str()) + "::" + String(port_m) + "::" + String(url_m.c_str()) + "::" + String(protocol_m));
  //webSocket.beginSSL(host_m.c_str(), port_m ,url_m.c_str(), protocol_m);
  // event handler

  webSocket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
  //#endif
}
#endif

#if 1

void check_connectivity(void)
{
  while ((internet == false) && (gu8_check_online_count2))
  {
    Serial.println("In Super loop");

    Serial.println("gu8_check_online_count2 = " + String(gu8_check_online_count2));
    gu8_check_online_count2--;

    if (wifi_enable == true && wifi_connect == true)
    {
      Serial.println("Waiting for WiFi Connction...");

      if (WiFi.status() == WL_CONNECTED)
      {
        wifi_reconnected_flag = true;
        internet = true;
        gsm_connect = false;
        Serial.println("Connected via WiFi");
        delay(100);
        connectToWebsocket();
      }
      else if (WiFi.status() != WL_CONNECTED)
      {
        Serial.print(".");
        delay(10);

        // added by sai
#if LCD_DISPLAY
        lcd.clear();
        lcd.setCursor(1, 1);
        lcd.print("CONNECTING TO WI-FI");

#endif
        // wifi_Loop();
        Serial.println("Wifi Not Connected: " + String(counter_wifiNotConnected));
#if DWIN_ENABLED
        err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
        delay(50);
        err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0]));
        delay(50);
#endif
        // if (counter_wifiNotConnected++ > 50)
        // {
        //   counter_wifiNotConnected = 0;
        //   if (gsm_enable == true)
        //   {
        //     wifi_connect = false;
        //     gsm_connect = true;
        //   }
        // }
      }
    }
    else if (gsm_enable == true && gsm_connect == true)
    {
      Serial.println("gsm_enable == true && gsm_connect == true");
      // added by sai

      SetupGsm(); // redundant @optimise
      // to do
      // add global flag for sim 4g module to connected with internet

      ConnectToServer();
      if (!client.connected())
      {

        // added by sai
#if LCD_DISPLAY
        lcd.clear();
        lcd.setCursor(1, 1);
        lcd.print("CONNECTING TO 4G");

#endif
        gsm_Loop();
        //   bluetooth_Loop();

        Serial.println("GSM not Connected: " + String(counter_gsmNotConnected));
#if DWIN_ENABLED
        err = DWIN_SET(nct, sizeof(nct) / sizeof(nct[0]));
        delay(50);
        err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0]));
        delay(50);
#endif
        if (counter_gsmNotConnected++ > 2)
        { // 2 == 5min
          counter_gsmNotConnected = 0;

          if (wifi_enable == true)
          {
            wifi_connect = true;
            gsm_connect = false;
          }
        }
      }
      else if (client.connected())
      {
        internet = true;
        wifi_connect = false;
        Serial.println("connected via 4G");
#if 0
#if LCD_ENABLED
        lcd.clear();
        lcd.setCursor(0, 2);
        lcd.print("CONNECTED VIA 4G");
#endif
#endif
#if DWIN_ENABLED
        // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
        // delay(50);
        // err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
        // delay(50);
        err = DWIN_SET(g, sizeof(g) / sizeof(g[0]));
        delay(50);
        err = DWIN_SET(g, sizeof(g) / sizeof(g[0]));
        delay(50);
#endif
      }
    }
  }
}
#endif

#if 0

void WiFiEvent(WiFiEvent_t event)
{
  Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event)
  {
  case SYSTEM_EVENT_WIFI_READY:
    Serial.println("WiFi interface ready");
    break;
  case SYSTEM_EVENT_SCAN_DONE:
    Serial.println("Completed scan for access points");
    break;
  case SYSTEM_EVENT_STA_START:
    Serial.println("WiFi client started");
    break;
  case SYSTEM_EVENT_STA_STOP:
    Serial.println("WiFi clients stopped");
    break;
  case SYSTEM_EVENT_STA_CONNECTED:
    Serial.println("Connected to access point");
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    gu8_check_online_count2 = 10;
    Serial.println("Disconnected from WiFi access point");
    break;
  case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
    Serial.println("Authentication mode of access point has changed");
    break;
  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.print("Obtained IP address: ");
    Serial.println(WiFi.localIP());
    break;
  case SYSTEM_EVENT_STA_LOST_IP:
    Serial.println("Lost IP address and IP address is reset to 0");
    break;
  case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
    Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
    break;
  case SYSTEM_EVENT_STA_WPS_ER_FAILED:
    Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
    break;
  case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
    Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
    break;
  case SYSTEM_EVENT_STA_WPS_ER_PIN:
    Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
    break;
  case SYSTEM_EVENT_AP_START:
    Serial.println("WiFi access point started");
    break;
  case SYSTEM_EVENT_AP_STOP:
    Serial.println("WiFi access point  stopped");
    break;
  case SYSTEM_EVENT_AP_STACONNECTED:
    Serial.println("Client connected");
    break;
  case SYSTEM_EVENT_AP_STADISCONNECTED:
    Serial.println("Client disconnected");
    break;
  case SYSTEM_EVENT_AP_STAIPASSIGNED:
    Serial.println("Assigned IP address to client");
    break;
  case SYSTEM_EVENT_AP_PROBEREQRECVED:
    Serial.println("Received probe request");
    break;
  case SYSTEM_EVENT_GOT_IP6:
    Serial.println("IPv6 is preferred");
    break;
  case SYSTEM_EVENT_ETH_START:
    Serial.println("Ethernet started");
    break;
  case SYSTEM_EVENT_ETH_STOP:
    Serial.println("Ethernet stopped");
    break;
  case SYSTEM_EVENT_ETH_CONNECTED:
    Serial.println("Ethernet connected");
    break;
  case SYSTEM_EVENT_ETH_DISCONNECTED:
    Serial.println("Ethernet disconnected");
    break;
  case SYSTEM_EVENT_ETH_GOT_IP:
    Serial.println("Obtained IP address");
    break;
  default:
    break;
  }
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  gu8_check_online_count2 = 4;
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  // Serial.println(info.disconnected.reason);
}

#endif

// wifi_OTA
#if 1

void setup_WIFI_OTA_get_1(void)
{
  HTTPClient http;

  uint8_t gu8_wifi_count = 50;
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_STA); /*wifi station mode*/
  // WiFi.begin(ssid_, password);
  WiFi.begin(ssid_m.c_str(), key_m.c_str());
  Serial.println("\nConnecting");

  while ((WiFi.status() != WL_CONNECTED) && (gu8_wifi_count))
  {
    Serial.print(".");
    delay(100);
    gu8_wifi_count--;
    if (gu8_wifi_count <= 0)
    {
      gu8_wifi_count = 0;
    }
    // vTaskDelay(10000 / portTICK_PERIOD_MS);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
  delay(50);
  // vTaskDelay(5000 / portTICK_PERIOD_MS);

  Serial.setDebugOutput(true);

  // WiFiMulti.addAP("EVRE", "Amplify5");

  // Client address
  Serial.print("WebSockets Client started @ IP address: ");
  Serial.println(WiFi.localIP());

  //  String URI = String("http://34.100.138.28/fota2.php");

#if TEST_OTA
  // String URI = String("http://34.100.138.28/evse_test_ota.php");

  String URI = String("http://34.100.138.28/evse_bm_3_3kw_ota.php");

  Serial.println("[OTA] Test OTA Begin...");

#else
  String URI = String("http://34.100.138.28/evse_ota.php");
  Serial.println("[OTA] OTA Begin...");
#endif

  Serial.println("[HTTP] begin...");

  Serial.print("The URL given is:");
  //   Serial.println(uri);
  Serial.println(URI);

#if 1
  int updateSize = 0;

  // configure server and url
  // String post_data = "{\"version\":\"CP001/hello.ino.esp32\", \"deviceId\":\"CP001\"}";
  // String post_data = "{\"version\":\"display_TestUART.ino.esp32\",\"deviceId\":\"CP001\"}";
  /*http.begin("https://us-central1-evre-iot-308216.cloudfunctions.net/otaUpdate");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Connection", "keep-alive");*/

  // http.begin("http://34.93.75.210/fota2.php");
  http.begin(URI);

  http.setUserAgent(F("ESP32-http-Update"));
  http.addHeader(F("x-ESP32-STA-MAC"), WiFi.macAddress());
  http.addHeader(F("x-ESP32-AP-MAC"), WiFi.softAPmacAddress());
  // http.addHeader(F("x-ESP32-sketch-md5"), String(ESP.getSketchMD5()));
  http.addHeader(F("x-ESP32-STA-MAC"), ESP.getSdkVersion());
  http.addHeader(F("x-ESP32-STA-MAC"), String(ESP.getFreeSketchSpace()));
  // http.addHeader(F("x-ESP32-sketch-size"), String(ESP.getSketchSize()));
  // http.addHeader(F("x-ESP32-device-id: "), DEVICE_ID);
  http.addHeader(F("x-ESP32-device-id"), CP_Id_m);
  http.addHeader(F("x-ESP32-firmware-version"), EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION /* + "\r\n" */);
  http.addHeader(F("x-ESP32-sketch-md5"), String(ESP.getSketchMD5()) /*  + String("\r\n")) */);
  // http.addHeader(F("x-ESP32-sketch-md5"), 425b2e2a27e2308338f7c8ede108ee9f);

  // int httpCode = http.POST(post_data);
  // int httpCode = http.GET(post_data);
  int httpCode = http.GET();
  // int httpCode = http.POST();

  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.println("Checking for new firmware updates...");

    // If file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      // get lenght of document (is -1 when Server sends no Content-Length header)
      int len = http.getSize();
      updateSize = len;
      Serial.printf("[OTA] Update found, File size(bytes) : %d\n", len);
      String get_response = http.getString();
      Serial.printf("[OTA] http response : %s\n", get_response);
      Serial.println("[HTTP] connection closed or file end.\n");

      if (get_response.equals("true") == true)
      {
        Serial.print("OTA update available");
        gu8_OTA_update_flag = 2;
      }
      else if (get_response.equals("false") == false)
      {
        gu8_OTA_update_flag = 3;
        Serial.print("no OTA update");
      }
    }
    // If there is no file at server
    if (httpCode == HTTP_CODE_INTERNAL_SERVER_ERROR)
    {
      Serial.println("[HTTP] No Updates");
      Serial.println();
      // ESP.restart();
    }
    Serial.println("[HTTP] Other response code");
    Serial.println(httpCode);
    Serial.println();
  }
  http.end();

#endif
}

void setup_WIFI_OTA_1(void)
{
  HTTPClient http;
  uint8_t err_cnt = 0;

  uint8_t gu8_wifi_count = 50;
#if 1
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA); /*wifi station mode*/
  // WiFi.begin(ssid_, password);
  WiFi.begin(ssid_m.c_str(), key_m.c_str());
  Serial.println("\nConnecting");

  while ((WiFi.status() != WL_CONNECTED) && (gu8_wifi_count))
  {
    Serial.print(".");
    delay(100);
    gu8_wifi_count--;
    if (gu8_wifi_count <= 0)
    {
      gu8_wifi_count = 0;
    }
    // vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
#endif
  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());

  delay(50);
  // vTaskDelay(5000 / portTICK_PERIOD_MS);

  Serial.setDebugOutput(true);

  // WiFiMulti.addAP("EVRE", "Amplify5");

  // Client address
  Serial.print("WebSockets Client started @ IP address: ");
  Serial.println(WiFi.localIP());

  //  String URI = String("http://34.100.138.28/fota2.php");

#if TEST_OTA
  String URI = String("http://34.100.138.28/evse_bm_3_3kw_ota.php");
  Serial.println("[OTA] Test OTA Begin...");
#else
  String URI = String("http://34.100.138.28/evse_ota.php");
  Serial.println("[OTA] OTA Begin...");
#endif

  Serial.println("[HTTP] begin...");

  Serial.print("The URL given is:");
  //   Serial.println(uri);
  Serial.println(URI);

#if 1
  int updateSize = 0;

  // configure server and url
  // String post_data = "{\"version\":\"CP001/hello.ino.esp32\", \"deviceId\":\"CP001\"}";
  // String post_data = "{\"version\":\"display_TestUART.ino.esp32\",\"deviceId\":\"CP001\"}";
  /*http.begin("https://us-central1-evre-iot-308216.cloudfunctions.net/otaUpdate");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Connection", "keep-alive");*/

  // http.begin("http://34.93.75.210/fota2.php");
  http.begin(URI);

  http.setUserAgent(F("ESP32-http-Update"));
  http.addHeader(F("x-ESP32-STA-MAC"), WiFi.macAddress());
  http.addHeader(F("x-ESP32-AP-MAC"), WiFi.softAPmacAddress());
  http.addHeader(F("x-ESP32-sketch-md5"), String(ESP.getSketchMD5()));
  http.addHeader(F("x-ESP32-STA-MAC"), ESP.getSdkVersion());
  http.addHeader(F("x-ESP32-STA-MAC"), String(ESP.getFreeSketchSpace()));
  // http.addHeader(F("x-ESP32-sketch-size"), String(ESP.getSketchSize()));
  // http.addHeader(F("x-ESP32-device-id"), DEVICE_ID);
  http.addHeader(F("x-ESP32-device-id"), CP_Id_m);
  // http.addHeader(F("x-ESP32-device-test-id: "), DEVICE_ID);
  http.addHeader(F("x-ESP32-firmware-version"), EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION /* + "\r\n" */);

  //   int httpCode = http.POST(post_data);
  // int httpCode = http.GET(post_data);
  // int httpCode = http.GET();
  // int httpCode = http.POST(DEVICE_ID);
  int httpCode = http.POST(CP_Id_m);

  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.println("Checking for new firmware updates...");

    // If file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      // get lenght of document (is -1 when Server sends no Content-Length header)
      int len = http.getSize();
      updateSize = len;
      Serial.printf("[OTA] Update found, File size(bytes) : %d\n", len);

#if 1
      // get tcp stream
      WiFiClient *client = http.getStreamPtr();
      // Serial.println();
      performUpdate_WiFi_1(*client, (size_t)updateSize);
      while (err_cnt < 2)
      {
        if (E_reason == 8)
        {
          performUpdate_WiFi_1(*client, (size_t)updateSize);
        }
        err_cnt++;
      }
      Serial.println("[HTTP] connection closed or file end.\n");
#endif
      Serial.println("[HTTP] connection closed or file end.\n");
    }
    // If there is no file at server
    if (httpCode == HTTP_CODE_INTERNAL_SERVER_ERROR)
    {
      Serial.println("[HTTP] No Updates");
      Serial.println();
      // ESP.restart();
    }
    Serial.println("[HTTP] Other response code");
    Serial.println(httpCode);
    Serial.println();
  }
  http.end();
#endif
}

#endif

#if 1

// perform the actual update from a given stream
void performUpdate_WiFi_1(WiFiClient &updateSource, size_t updateSize)
{
  if (Update.begin(updateSize))
  {
    Serial.println("...Downloading File...");
#if LCD_DISPLAY
    lcd.clear();
    lcd.setCursor(0, 2);
    lcd.print("***CHARGER UPDATING***");
#endif
    Serial.println();

    // Writing Update
    size_t written = Update.writeStream(updateSource);

    printPercent_1(written, updateSize);

    if (written == updateSize)
    {
      Serial.println("Written : " + String(written) + "bytes successfully");
    }
    else
    {
      Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
      // ptr_leds->red_led();
      for (uint8_t idx = 0; idx < NUM_LEDS; idx++)
      {
        leds[idx] = CRGB::Red;
        FastLED.show(COLOR_BRIGHTNESS);
      }
    }
    if (Update.end())
    {
      Serial.println("OTA done!");
      if (Update.isFinished())
      {
        Serial.println("Update successfully completed. Rebooting...");
        Serial.println();
        ESP.restart();
      }
      else
      {
        Serial.println("Update not finished? Something went wrong!");
      }
    }
    else
    {
      E_reason = Update.getError();
      Serial.println("Error Occurred. Error #: " + String(Update.getError()));
    }
  }
  else
  {
    Serial.println("Not enough space to begin OTA");
  }
}

void printPercent_1(uint32_t readLength, uint32_t contentLength)
{
  // If we know the total length
  if (contentLength != (uint32_t)-1)
  {
    Serial.print("\r ");
    Serial.print((100.0 * readLength) / contentLength);
    Serial.print('%');
  }
  else
  {
    Serial.println(readLength);
  }
}

void setup_WIFI_OTA_getconfig_1(void)
{
  HTTPClient http;

  uint8_t gu8_wifi_count = 50;
#if 1
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA); /*wifi station mode*/
  // WiFi.begin(ssid_, password);
  WiFi.begin(ssid_m.c_str(), key_m.c_str());
  Serial.println("\nConnecting");

  while ((WiFi.status() != WL_CONNECTED) && (gu8_wifi_count))
  {
    Serial.print(".");
    delay(100);
    gu8_wifi_count--;
    if (gu8_wifi_count <= 0)
    {
      gu8_wifi_count = 0;
    }
    // vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
#endif

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
  delay(50);
  // vTaskDelay(5000 / portTICK_PERIOD_MS);

  Serial.setDebugOutput(true);

  // WiFiMulti.addAP("EVRE", "Amplify5");

  // Client address
  Serial.print("WebSockets Client started @ IP address: ");
  Serial.println(WiFi.localIP());

  //  String URI = String("http://34.100.138.28/fota2.php");

#if TEST_OTA
  // String URI = String("http://34.100.138.28/evse_test_ota.php");
  String URI_GET_CONFIG = String("http://34.100.138.28/evse-config-update.php");

  Serial.println("[OTA]  Get config Begin...");
#else
  String URI = String("http://34.100.138.28/evse_ota.php");
  Serial.println("[OTA] OTA Begin...");
#endif

  Serial.println("[HTTP] begin...");

  Serial.print("The URL given is:");
  //   Serial.println(uri);
  Serial.println(URI_GET_CONFIG);

#if 1
  int updateSize = 0;

  // configure server and url
  // String post_data = "{\"version\":\"CP001/hello.ino.esp32\", \"deviceId\":\"CP001\"}";
  // String post_data = "{\"version\":\"display_TestUART.ino.esp32\",\"deviceId\":\"CP001\"}";
  /*http.begin("https://us-central1-evre-iot-308216.cloudfunctions.net/otaUpdate");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Connection", "keep-alive");*/

  // http.begin("http://34.93.75.210/fota2.php");
  http.begin(URI_GET_CONFIG);

  http.setUserAgent(F("ESP32-http-Update"));
  http.addHeader(F("x-ESP32-STA-MAC"), WiFi.macAddress());
  http.addHeader(F("x-ESP32-AP-MAC"), WiFi.softAPmacAddress());
  http.addHeader(F("x-ESP32-sketch-md5"), String(ESP.getSketchMD5()));
  http.addHeader(F("x-ESP32-STA-MAC: "), ESP.getSdkVersion());
  http.addHeader(F("x-ESP32-STA-MAC: "), String(ESP.getFreeSketchSpace()));
  // http.addHeader(F("x-ESP32-sketch-size"), String(ESP.getSketchSize()));
  // http.addHeader(F("x-ESP32-device-id: "), DEVICE_ID);
  http.addHeader(F("x-ESP32-device-id: "), CP_Id_m);
  // http.addHeader(F("x-ESP32-device-test-id: "), DEVICE_ID);
  http.addHeader(F("x-ESP32-firmware-version: "), EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION /* + "\r\n" */);

  // int httpCode = http.POST(post_data);
  // int httpCode = http.GET(post_data);
  int httpCode = http.GET();
  // int httpCode = http.POST();

  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.println("Checking for new configs...");

    // If file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      // get lenght of document (is -1 when Server sends no Content-Length header)
      int len = http.getSize();
      // updateSize = len;
      Serial.printf("[OTA] get config ,  : %d\n", len);
      String get_response = http.getString();
      Serial.printf("[OTA] http response : %s\n", get_response);
      Serial.println("[HTTP] connection closed or file end.\n");

#if 0 
        DeserializationError error = deserializeJson(server_config, get_response);

        //{"wifi":"EVRE","port":"80","otaupdatetime":"86400"}

        if (error)
        {
          Serial.print(F("DeserializeJson() failed: "));
          Serial.println(error.f_str());
          // return connectedToWifi;
        }
        if (server_config.containsKey("wifi"))
        {
          wifi_server = server_config["wifi"];
        }
        if (server_config.containsKey("port"))
        {
          port_server = server_config["port"];
        }

        if (server_config.containsKey("otaupdatetime"))
        {
          ota_update_time = server_config["otaupdatetime"];
        }

          get_response="";
        // put_server_config(); 
        Serial.println("\r\nclient disconnected....");
#endif
    }
    // If there is no file at server
    if (httpCode == HTTP_CODE_INTERNAL_SERVER_ERROR)
    {
      Serial.println("[HTTP] No Updates");
      Serial.println();
      // ESP.restart();
    }
    Serial.println("[HTTP] Other response code");
    Serial.println(httpCode);
    Serial.println();
  }
  http.end();

#endif
}

#endif
// 4G_OTA

/************************************************************4G_OTA***************************************************************/
#if 1

#define SerialAT Serial2

#if 1
// const char resource[] = "/evse_test_ota.php";
// http://34.100.138.28/evse_romel_3_3kw_ota.php
// const char resource[] = "/evse_adani_7_ota_test.php"; // for 3s charger

const char resource[] = "/evse_bm_3_3kw_ota.php"; // for 3s charger
const char server[] = "34.100.138.28";
const int port = 80;

#endif

// const char gprsUser[] = "";
// const char gprsPass[] = "";

uint32_t knownCRC32 = 0x6f50d767;
uint32_t knownFileSize = 1148544;

// extern TinyGsm modem;
/// extern TinyGsmClient client;

// extern TinyGsm modem(Serial2);
// extern TinyGsmClient client(modem);

String APN = "m2misafe";
char g_apn[8] = {'m', '2', 'm', 'i', 's', 'a', 'f', 'e'};
// char g_apn[14] = {'a', 'i', 'r', 't', 'e', 'l', 'g', 'p', 'r', 's', '.', 'c', 'o', 'm'};

void OTA_4G_setup_4G_OTA_get(void)
{
  // modem.gprsConnect(apn, gprsUser, gprsPass);
  // APN = getAPN();

  // strcpy(g_apn,APN.c_str());
  // setup4G();
  OTA_4G_setup4G();
  Serial.print("Waiting for network...");
  if (!ota_modem.waitForNetwork())
  {
    Serial.println(" fail");

#if FREE_RTOS_THREAD
    vTaskDelay(10000 / portTICK_PERIOD_MS);
#else
    delay(10000);
#endif
    return;
  }
  Serial.println(" success");

  if (ota_modem.isNetworkConnected())
  {
    Serial.println("Network connected");
  }
#if TINY_GSM_USE_GPRS | 1
  // GPRS connection parameters are usually set after network registration
  Serial.print("Connecting to ");
  Serial.print(g_apn);
  if (!ota_modem.gprsConnect(g_apn, gprsUser, gprsPass))
  {
    Serial.println(" fail");
#if FREE_RTOS_THREAD
    vTaskDelay(10000 / portTICK_PERIOD_MS);
#else
    delay(10000);
#endif
    return;
  }
  Serial.println(" success");

  if (ota_modem.isGprsConnected())
  {
    Serial.println("GPRS connected");
  }
#endif
  Serial.print("Connecting to ");

  Serial.print(server);

  // strcpy(&fota_host[0], host_fota.c_str());
  // strcpy(&fota_path[0], path_fota.c_str());
  // fota_port = port_fota;

  // Serial.print(fota_host);

  if (!ota_client.connect(server, port))
  // if (!ota_client.connect(fota_host, fota_port))
  {
    Serial.println(" fail");
#if FREE_RTOS_THREAD
    vTaskDelay(10000 / portTICK_PERIOD_MS);
#else
    delay(10000);
#endif
    return;
  }
  Serial.println(" success");

  // ota_client.print(String("POST ") + resource + " HTTP/1.0\r\n");
  ota_client.print(String("GET ") + resource + " HTTP/1.1\r\n");
  ota_client.print(String("Host: ") + server + "\r\n");

  // dynamic update
  //  ota_client.print(String("GET ") + fota_path + " HTTP/1.1\r\n");
  //  ota_client.print(String("Host: ") + fota_host + "\r\n");
  ota_client.print(String("x-ESP32-STA-MAC: ") + String(WiFi.macAddress()) + "\r\n");
  ota_client.print(String("x-ESP32-AP-MAC: ") + String(WiFi.softAPmacAddress()) + "\r\n");
  ota_client.print(String("x-ESP32-sketch-md5: ") + String(ESP.getSketchMD5()) + "\r\n");
  ota_client.print(String("x-ESP32-STA-MAC: ") + String(ESP.getSdkVersion()) + "\r\n");
  ota_client.print(String("x-ESP32-STA-MAC: ") + String(ESP.getFreeSketchSpace()) + "\r\n");

  // ota_client.print(String("x-ESP32-firmware-version: ") + VERSION + "\r\n");
  // ota_client.print(String("x-ESP32-device-id: ") + "evse_001" + "\r\n");

  ota_client.print(String("x-ESP32-device-id: ") + CP_Id_m + "\r\n");
  ota_client.print(String("x-ESP32-firmware-version: ") + EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION + "\r\n");

  ota_client.print("Connection: close\r\n\r\n");

  // Let's see what the entire elapsed time is, from after we send the request.
  uint32_t timeElapsed = millis();
  Serial.println("Waiting for response header");

  // While we are still looking for the end of the header (i.e. empty line
  // FOLLOWED by a newline), continue to read data into the buffer, parsing each
  // line (data FOLLOWED by a newline). If it takes too long to get data from
  // the ota_client, we need to exit.

  const uint32_t ota_clientReadTimeout = 600000;
  uint32_t ota_clientReadStartTime = millis();
  String headerBuffer;
  bool finishedHeader = false;
  uint32_t contentLength = 0;

  while (!finishedHeader)
  {
    int nlPos;

    if (ota_client.available())
    {
      ota_clientReadStartTime = millis();
      while (ota_client.available())
      {
        char c = ota_client.read();
        headerBuffer += c;

        // Uncomment the lines below to see the data coming into the buffer
        if (c < 16)
          Serial.print('0');
        Serial.print(c, HEX);
        Serial.print(' ');
        /*if (isprint(c))
          Serial.print(reinterpret_cast<char> c);
          else
          Serial.print('*');*/
        Serial.print(' ');

        // Let's exit and process if we find a new line
        if (headerBuffer.indexOf(F("\r\n")) >= 0)
          break;
      }
    }
    else
    {
      if (millis() - ota_clientReadStartTime > ota_clientReadTimeout)
      {
        // Time-out waiting for data from ota_client
        Serial.println(">>> ota_client Timeout !");
        break;
      }
    }
    // See if we have a new line.
    nlPos = headerBuffer.indexOf(F("\r\n"));

    if (nlPos > 0)
    {
      headerBuffer.toLowerCase();
      // Check if line contains content-length
      if (headerBuffer.startsWith(F("content-length:")))
      {
        contentLength = headerBuffer.substring(headerBuffer.indexOf(':') + 1).toInt();
        // Serial.print(F("Got Content Length: "));  // uncomment for
        // Serial.println(contentLength);            // confirmation
      }

      headerBuffer.remove(0, nlPos + 2); // remove the line
    }
    else if (nlPos == 0)
    {
      // if the new line is empty (i.e. "\r\n" is at the beginning of the line),
      // we are done with the header.
      finishedHeader = true;
    }
  }

  // The two cases which are not managed properly are as follows:
  // 1. The ota_client doesn't provide data quickly enough to keep up with this
  // loop.
  // 2. If the ota_client data is segmented in the middle of the 'Content-Length: '
  // header,
  //    then that header may be missed/damaged.
  //

  uint32_t readLength = 0;
  CRC32 crc;
  // File file = FFat.open("/update.bin", FILE_APPEND);
  // if (finishedHeader && contentLength == knownFileSize) {
  if (finishedHeader)
  {

    Serial.println("Reading response data");
    ota_clientReadStartTime = millis();

    String get_response = "";
    // printPercent(readLength, contentLength);
    while (readLength < contentLength && ota_client.connected() &&
           millis() - ota_clientReadStartTime < ota_clientReadTimeout)
    {
      ota_clientReadStartTime = millis();
      while (ota_client.available())
      {
        // uint8_t c = ota_client.read();
        char c = ota_client.read();
        if (c == 'f')
        {
          Serial.print("no OTA update");
          gu8_OTA_update_flag = 3;
        }
        if (c == 't')
        {
          Serial.print("OTA update available");
          gu8_OTA_update_flag = 2;
        }
        // Serial.print(c);
        get_response += c;
        // Serial.print(reinterpret_cast<char>c);  // Uncomment this to show
        // data
        // crc.update(c);
        readLength++;
      }
      Serial.println("\r\n" + get_response);

      if (get_response.equals("true") == true)
      {
        // gu8_OTA_update_flag = 2;
        // Serial.print("OTA update available");
      }
      else if (get_response.equals("false") == false)
      {
        // gu8_OTA_update_flag = 3;
        // Serial.print("no OTA update");
      }
      get_response = "";
      Serial.println("\r\nota_client disconnected....");
    }
    // printPercent(readLength, contentLength);
  }

  timeElapsed = millis() - timeElapsed;
  Serial.println();

  ota_client.stop();
  Serial.println("Server disconnected");

#if TINY_GSM_USE_GPRS
  ota_modem.gprsDisconnect();
  Serial.println("GPRS disconnected");
#endif

  float duration = float(timeElapsed) / 1000;

  Serial.println();
  Serial.print("Content-Length: ");
  Serial.println(contentLength);
  Serial.print("Actually read:  ");
  Serial.println(readLength);
  Serial.print("Calc. CRC32:    0x");
  Serial.println(crc.finalize(), HEX);
  Serial.print("Known CRC32:    0x");
  Serial.println(knownCRC32, HEX);
  Serial.print("Duration:       ");
  Serial.print(duration);
  Serial.println("s");
}
///////////////////////////get_response///////////////////

//------------------------------------------ 4G OTA ----------------------------------------//
void OTA_4G_setup_4G_OTA(void)
{
  // strcpy(g_apn,APN.c_str());
  OTA_4G_setup4G();
  Serial.print("Waiting for network...");
  if (!ota_modem.waitForNetwork())
  {
    Serial.println(" fail");
#if FREE_RTOS_THREAD
    vTaskDelay(10000 / portTICK_PERIOD_MS);
#else
    delay(10000);
#endif
    return;
  }
  Serial.println(" success");

  if (ota_modem.isNetworkConnected())
  {
    Serial.println("Network connected");
  }
#if TINY_GSM_USE_GPRS
  // GPRS connection parameters are usually set after network registration
  Serial.print("Connecting to ");
  Serial.print(g_apn);
  if (!ota_modem.gprsConnect(g_apn, gprsUser, gprsPass))
  {
    Serial.println(" fail");
#if FREE_RTOS_THREAD
    vTaskDelay(10000 / portTICK_PERIOD_MS);
#else
    delay(10000);
#endif
    return;
  }
  Serial.println(" success");

  if (ota_modem.isGprsConnected())
  {
    Serial.println("GPRS connected");
  }
#endif
  Serial.print("Connecting to ");
  Serial.print(server);
  // Serial.print(fota_host);

  if (!ota_client.connect(server, port))
  // if (!ota_client.connect(fota_host, fota_port))
  {
    Serial.println(" fail");
#if FREE_RTOS_THREAD
    vTaskDelay(10000 / portTICK_PERIOD_MS);
#else
    delay(10000);
#endif
    return;
  }
  Serial.println(" success");
  // int sketch_md5=ESP.getSketchMD5();
  // Serial.println("sketch_md5 =" + String(sketch_md5));
  //  Make a HTTP GET request:

  ota_client.print(String("POST ") + resource + " HTTP/1.1\r\n");
  ota_client.print(String("Host: ") + server + "\r\n");
  // ota_client.print(String("POST ") + fota_path + " HTTP/1.1\r\n");
  // ota_client.print(String("Host: ") + fota_host + "\r\n");
  ota_client.print(String("x-ESP32-STA-MAC: ") + String(WiFi.macAddress()) + "\r\n");
  ota_client.print(String("x-ESP32-AP-MAC: ") + String(WiFi.softAPmacAddress()) + "\r\n");
  ota_client.print(String("x-ESP32-sketch-md5: ") + String(ESP.getSketchMD5()) + "\r\n");
  ota_client.print(String("x-ESP32-STA-MAC: ") + String(ESP.getSdkVersion()) + "\r\n");
  ota_client.print(String("x-ESP32-STA-MAC: ") + String(ESP.getFreeSketchSpace()) + "\r\n");

  // ota_client.print(String("x-ESP32-firmware-version: ") + VERSION + "\r\n");
  // ota_client.print(String("x-ESP32-device-id: ") + "evse_001" + "\r\n");
  ota_client.print(String("x-ESP32-device-id: ") + CP_Id_m + "\r\n");
  ota_client.print(String("x-ESP32-firmware-version: ") + EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION + "\r\n");

  ota_client.print("Connection: close\r\n\r\n");

  // Let's see what the entire elapsed time is, from after we send the request.
  uint32_t timeElapsed = millis();
  Serial.println("Waiting for response header");

  // While we are still looking for the end of the header (i.e. empty line
  // FOLLOWED by a newline), continue to read data into the buffer, parsing each
  // line (data FOLLOWED by a newline). If it takes too long to get data from
  // the ota_client, we need to exit.

  const uint32_t ota_clientReadTimeout = 600000;
  uint32_t ota_clientReadStartTime = millis();
  String headerBuffer;
  bool finishedHeader = false;
  uint32_t contentLength = 0;

  while (!finishedHeader)
  {
    int nlPos;

    if (ota_client.available())
    {
      ota_clientReadStartTime = millis();
      while (ota_client.available())
      {
        char c = ota_client.read();
        headerBuffer += c;

        // Uncomment the lines below to see the data coming into the buffer
        if (c < 16)
          Serial.print('0');
        Serial.print(c, HEX);
        Serial.print(' ');
        /*if (isprint(c))
          Serial.print(reinterpret_cast<char> c);
          else
          Serial.print('*');*/
        Serial.print(' ');

        // Let's exit and process if we find a new line
        if (headerBuffer.indexOf(F("\r\n")) >= 0)
          break;
      }
    }
    else
    {
      if (millis() - ota_clientReadStartTime > ota_clientReadTimeout)
      {
        // Time-out waiting for data from ota_client
        Serial.println(">>> ota_client Timeout !");
        break;
      }
    }
    // See if we have a new line.
    nlPos = headerBuffer.indexOf(F("\r\n"));

    if (nlPos > 0)
    {
      headerBuffer.toLowerCase();
      // Check if line contains content-length
      if (headerBuffer.startsWith(F("content-length:")))
      {
        contentLength = headerBuffer.substring(headerBuffer.indexOf(':') + 1).toInt();
        Serial.print("Got Content Length: "); // uncomment for
        Serial.println(contentLength);        // confirmation
        // if(contentLength <= 0)
        // {
        //   // Serial.print(F("Got Content Length: "));  // comment at 06092023_170900
        //   // Serial.println(contentLength);
        //   contentLength = 1;
        // }
      }

      headerBuffer.remove(0, nlPos + 2); // remove the line
    }
    else if (nlPos == 0)
    {
      // if the new line is empty (i.e. "\r\n" is at the beginning of the line),
      // we are done with the header.
      finishedHeader = true;
    }
  }

  // The two cases which are not managed properly are as follows:
  // 1. The ota_client doesn't provide data quickly enough to keep up with this
  // loop.
  // 2. If the ota_client data is segmented in the middle of the 'Content-Length: '
  // header,
  //    then that header may be missed/damaged.
  //

  uint32_t readLength = 0;
  CRC32 crc;

#if 1
  File fs = FFat.open("/update.bin", FILE_APPEND);
  if (FFat.remove("/update.bin"))
  {
    Serial.println("- file deleted");
  }
  else
  {
    Serial.println("- delete failed");
  }
#endif

  // pinMode(WATCH_DOG_PIN, OUTPUT);
  // gu32reset_watch_dog_timer_count = 0;

  File file = FFat.open("/update.bin", FILE_APPEND);
  // if (finishedHeader && contentLength == knownFileSize) {
  if (finishedHeader)
  {

    Serial.println("Reading response data");
    ota_clientReadStartTime = millis();

    OTA_4G_printPercent(readLength, contentLength);
    while (readLength < contentLength && ota_client.connected() &&
           millis() - ota_clientReadStartTime < ota_clientReadTimeout)
    {
      ota_clientReadStartTime = millis();
      while (ota_client.available())
      {
        uint8_t c = ota_client.read();
        // char c = ota_client.read();
        // Serial.print(c);
        if (!file.write(c))
        {
          Serial.println("not Appending file");
        }

        // Serial.print(reinterpret_cast<char>c);  // Uncomment this to show
        // data
        crc.update(c);
        readLength++;
        if (readLength % (contentLength / 100) == 0)
        {
          OTA_4G_printPercent(readLength, contentLength);
          Serial.print("  ");
          Serial.printf("[OTA] ESP32 heap size  %d \r\n", ESP.getHeapSize());
          Serial.println("  ");
        }

        // gu32reset_watch_dog_timer_count++;
        // if (gu32reset_watch_dog_timer_count > WATCH_DOG_RESET_COUNT + WATCH_DOG_RESET_INTERVAL)
        // {
        //   gu32reset_watch_dog_timer_count = 0;
        //   // ptr_watch_dog->evse_watch_dog_off();
        //   digitalWrite(WATCH_DOG_PIN, LOW);
        // }
        // else if (gu32reset_watch_dog_timer_count > WATCH_DOG_RESET_COUNT)
        // {
        //   // gu32reset_watch_dog_timer_count=0;
        //   // ptr_watch_dog->evse_watch_dog_on();
        //   digitalWrite(WATCH_DOG_PIN, HIGH);
        // }
      }
      Serial.println("ota_client disconnected....");
    }
    OTA_4G_printPercent(readLength, contentLength);
  }

  timeElapsed = millis() - timeElapsed;
  Serial.println();
  file.close();

  // Shutdown

  ota_client.stop();
  Serial.println("Server disconnected");

#if TINY_GSM_USE_WIFI
  ota_modem.networkDisconnect();
  Serial.println(F("WiFi disconnected"));
#endif
#if TINY_GSM_USE_GPRS
  ota_modem.gprsDisconnect();
  Serial.println("GPRS disconnected");
#endif

  float duration = float(timeElapsed) / 1000;

  Serial.println();
  Serial.print("Content-Length: ");
  Serial.println(contentLength);
  Serial.print("Actually read:  ");
  Serial.println(readLength);
  Serial.print("Calc. CRC32:    0x");
  Serial.println(crc.finalize(), HEX);
  Serial.print("Known CRC32:    0x");
  Serial.println(knownCRC32, HEX);
  Serial.print("Duration:       ");
  Serial.print(duration);
  Serial.println("s");
  // Do nothing forevermore
  // pu();
  // OTA_4G_pu(FFat, "/update.bin");
  OTA_4G_pu("/update.bin");
}

/////////////////////SETUP 4G////////////////////////////////////////////

#define GSM_RXD2 16
#define GSM_TXD2 17

#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

void OTA_4G_setup4G(void)
{
  SerialAT.begin(115200, SERIAL_8N1, GSM_RXD2, GSM_TXD2);
  Serial.println(F("[CustomSIM7672] Starting 4G Setup"));
  TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
  SerialAT.println("AT+CMGD=1,4");
  Serial.println("AT+CMGD=1,4");
  OTA_4G_waitForResp(20);
  Serial.println(F("[CustomSIM7672] ota_modem restart..."));
  ota_modem.restart();
  delay(200);
  Serial.println(F("[CustomSIM7672] Initializing ota_modem..."));

  String ota_modemInfo = ota_modem.getModemInfo();
  Serial.print("[CustomSIM7672] ota_modem Info: ");
  Serial.println(ota_modemInfo);
  ota_modem.gprsConnect(APN.c_str(), gprsUser, gprsPass);
  Serial.println(F("[CustomSIM7672] Waiting for network..."));
  if (!ota_modem.waitForNetwork())
  {
    Serial.println(F("[CustomSIM7672] fail"));
    // delay(200);
#if FREE_RTOS_THREAD
    vTaskDelay(200 / portTICK_PERIOD_MS);
#else
    delay(200);
#endif
    return;
  }
  Serial.println("[CustomSIM7672] success");
  if (ota_modem.isNetworkConnected())
  {
    Serial.println(F("[CustomSIM7672] Network connected"));
  }
  Serial.print("[CustomSIM7672] Connecting to APN :  ");
  Serial.print(APN.c_str());
  if (!ota_modem.gprsConnect(APN.c_str(), gprsUser, gprsPass))
  {
    Serial.println(F("[CustomSIM7672] fail"));
    // delay(200);
#if FREE_RTOS_THREAD
    vTaskDelay(200 / portTICK_PERIOD_MS);
#else
    delay(200);
#endif
    // gsm_net = false;
    return;
  }
  Serial.println(F("[CustomSIM7672] success"));
  if (ota_modem.isGprsConnected())
  {
    Serial.println(F("[CustomSIM7672] 4G connected"));
    // gsm_net = true;
  }
  int csq = ota_modem.getSignalQuality();
  Serial.println("Signal quality: " + String(csq));
  // delay(1);
#if FREE_RTOS_THREAD
  vTaskDelay(1 / portTICK_PERIOD_MS);
#else
  delay(1);
#endif
}
/////////////////////SETUP 4G////////////////////////////////////////////

/*************************fatfs***********/

/*waitForResp*/

uint8_t OTA_4G_waitForResp(uint8_t timeout)
{
  const char *crtResp = "+HTTPACTION: 1,200"; // Success
  const char *Resp400 = "+HTTPACTION: 1,4";   // Conf Error
  const char *Resp500 = "+HTTPACTION: 1,5";   // Internal Server Error
  const char *Resp700 = "+HTTPACTION: 1,7";   // Internal Server Error
  const char *errResp = "+HTTP_NONET_EVENT";
  const char *errResp1 = "ERROR";
  const char *okResp = "OK";
  const char *DOWNResp = "DOWNLOAD";
  const char *clkResp = "+CCLK: ";      // Success
  const char *pdndeactResp = "+CGEV: "; // pdn deact
  const char *mqttconnectResp = "+CMQTTCONNECT: 0,0";
  const char *mqttcheckconnResp = "+CMQTTCONNECT: 0,\"tcp://34.134.133.145:1883\",60,1";
  const char *mqttdisconnResp = "+CMQTTDISC: 0,0";

  uint8_t len1 = strlen(crtResp);
  uint8_t len2 = strlen(Resp400);
  uint8_t len3 = strlen(errResp);
  uint8_t len4 = strlen(Resp700);
  uint8_t len5 = strlen(errResp1);
  uint8_t len6 = strlen(okResp);
  uint8_t len7 = strlen(DOWNResp);
  uint8_t len8 = strlen(clkResp);
  uint8_t len11 = strlen(pdndeactResp);
  uint8_t len12 = strlen(mqttconnectResp);
  uint8_t len13 = strlen(mqttcheckconnResp);
  uint8_t len14 = strlen(mqttdisconnResp);

  uint8_t sum1 = 0;
  uint8_t sum2 = 0;
  uint8_t sum3 = 0;
  uint8_t sum4 = 0;
  uint8_t sum5 = 0;
  uint8_t sum6 = 0;
  uint8_t sum7 = 0;
  uint8_t sum8 = 0;
  uint8_t sum11 = 0;
  uint8_t sum12 = 0;
  uint8_t sum13 = 0;
  uint8_t sum14 = 0;
  unsigned long timerStart, timerEnd;
  timerStart = millis();
  while (1)
  {
    timerEnd = millis();
    if (timerEnd - timerStart > 1000 * timeout)
    {
      // gsm_net = false;
      return -1; // Timed out
    }
    if (SerialAT.available())
    {
      char c = SerialAT.read();
      Serial.print(c);
      sum1 = (c == crtResp[sum1]) ? sum1 + 1 : 0;
      sum2 = (c == Resp400[sum2]) ? sum2 + 1 : 0;
      sum3 = (c == errResp[sum3]) ? sum3 + 1 : 0;
      sum4 = (c == Resp700[sum4]) ? sum4 + 1 : 0;
      sum5 = (c == errResp1[sum5]) ? sum5 + 1 : 0;
      sum6 = (c == okResp[sum6]) ? sum6 + 1 : 0;
      sum7 = (c == DOWNResp[sum7]) ? sum7 + 1 : 0;
      sum8 = (c == clkResp[sum8]) ? sum8 + 1 : 0;
      sum11 = (c == pdndeactResp[sum11]) ? sum11 + 1 : 0;
      sum12 = (c == mqttconnectResp[sum12]) ? sum12 + 1 : 0;
      sum13 = (c == mqttcheckconnResp[sum13]) ? sum13 + 1 : 0;
      sum14 = (c == mqttdisconnResp[sum14]) ? sum14 + 1 : 0;

      if (sum1 == len1)
      {
        // gsm_net = true;
        Serial.println(F("Success!"));
        return 1;
      }
      else if (sum2 == len2)
      {
        // gsm_net = true;
        Serial.println("400 Error!");
        return 0;
      }
      else if (sum3 == len3)
      {
        Serial.println(F("NONET Error!"));
        // gsm_net = false;
        return 0;
      }
      else if (sum4 == len4)
      {
        Serial.println(F("700 No internet!"));
        // gsm_net = false;
        return 0;
      }
      else if (sum5 == len5)
      {
        return 0;
      }
      else if (sum6 == len6)
      {
        Serial.println(F("AT_OK"));
        return 0;
      }
      else if (sum7 == len7)
      {
        return 0;
      }
      else if (sum8 == len8)
      {
        Serial.println(F("OK"));
#if 0
        Serial.print(F("Timestamp : "));
        //Serial.println(timestamp);
        for (int i = 0; i < strlen(timestamp); i++)
        {
          Serial.print(i);
          Serial.print(":");
          Serial.println(timestamp[i]);
        }
        //timeEpoch();
#endif
        return 0;
      }
      else if (sum11 == len11)
      {
        /*
          A7600 Series_ AT Command Manual

          +CGEV: ME PDN DEACT 1
           The mobile termination has deactivated a context.
           The context represents a PDN connection in LTE or a Primary PDP context in GSM/UMTS.
           The <cid> for this context is provided to the TE.
           The format of the parameter <cid> NOT E is found in command +CGDCONT.
        */
        Serial.println(F("PDN DEACTED ...!"));
        // gsm_net = false;
        return 0;
      }
      else if (sum12 == len12)
      {
        Serial.println(F("Device connected to server using mqtt"));
        return 0;
      }
      else if (sum13 == len13)
      {
        Serial.println(F("Already connected"));
        return 0;
      }
      else if (sum14 == len14)
      {
        Serial.println(F("mqtt disconnected"));
        return 0;
      }
    }
  }
  while (SerialAT.available())
  {
    SerialAT.read();
  }
  return 0;
}
/*waitForResp*/

/*print percentage */

void OTA_4G_printPercent(uint32_t readLength, uint32_t contentLength)
{
  // If we know the total length
  if (contentLength != (uint32_t)-1)
  {
    Serial.print("\r ");
    Serial.print((100.0 * readLength) / contentLength);
    Serial.print('%');
  }
  else
  {
    Serial.println(readLength);
  }
}
/*print percentage */

void OTA_4G_pu(const char *path)
{
  // FS fs;
  Serial.println(F("***Starting OTA update***"));
  File updateBin = FFat.open(path);

  if (updateBin)
  {
    if (updateBin.isDirectory())
    {
      Serial.println("Directory error");
      updateBin.close();
      return;
    }

    size_t updateSize = updateBin.size();

    if (Update.begin(updateSize))
    {
      size_t written = Update.writeStream(updateBin);
      if (written == updateSize)
      {
        Serial.println("Writes : " + String(written) + " successfully");
      }
      else
      {
        Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
      }
      if (Update.end())
      {
        Serial.println("OTA finished!");
        if (Update.isFinished())
        {
          Serial.println("Restart ESP device!");
          // esp_deep_sleep(1000 * 1000);

          /* Watch dog timer is added */
          // gu32reset_watch_dog_timer_count = 0;
          // gu32reset_watch_dog_timer_count++;
          // digitalWrite(WATCH_DOG_PIN, LOW);
          // while (gu32reset_watch_dog_timer_count <= WATCH_DOG_RESET_COUNT)
          // {
          //   gu32reset_watch_dog_timer_count++;
          // }
          // digitalWrite(WATCH_DOG_PIN, HIGH);

          // evse_preferences_ble.begin("fota_url", false);
          // evse_preferences_ble.putString("fota_uri", "");
          // evse_preferences_ble.putUInt("fota_retries", 0);
          // evse_preferences_ble.putString("fota_date", "");
          // evse_preferences_ble.putBool("fota_avial", false);
          // evse_preferences_ble.end();

          ESP.restart();
        }
        else
        {
          Serial.println("OTA not finished yet");
        }
      }
      else
      {
        Serial.println("Error occured #: " + String(Update.getError()));
        // esp_deep_sleep(1000 * 1000);

        /* Watch dog timer is added */
        // gu32reset_watch_dog_timer_count = 0;
        // gu32reset_watch_dog_timer_count++;
        // digitalWrite(WATCH_DOG_PIN, LOW);
        // while (gu32reset_watch_dog_timer_count <= WATCH_DOG_RESET_COUNT)
        // {
        //   gu32reset_watch_dog_timer_count++;
        // }
        // digitalWrite(WATCH_DOG_PIN, HIGH);

        ESP.restart();
      }
    }
    else
    {
      Serial.println("Cannot begin update");
    }
  }
  updateBin.close();
}

#endif

#if 1

/***************************************EVSE_READINPUT BLOCK*********************************************************/
String readIdTag = "";
void EVSE_ReadInput(MFRC522 *mfrc522)
{ // this funtion should be called only if there is Internet

  readIdTag = "";

  readIdTag = readRfidTag(true, mfrc522);
  if (readIdTag.equals("") == false)
  {
    // EVSE_StopTxnRfid(readIdTag);

    readConnectorVal = 1;

    if (readConnectorVal > 0)
    {
      bool result = false;
// if (offline_connect || online_to_offline_flag)
#if 0
      if (webSocketConncted == 0)
      {
        switch (readConnectorVal)
        {
        case 1:
          if (EvseDevStatus_connector_1 == flag_EVSE_Request_Charge)
          {
            // idTagData_A = resumeTxn_A.getString("idTagData_A", "");
            // Serial.println("currentidtag_AAAAAAA"+String(idTagData_A));
            Exiting_currentIdTag = readIdTag;
            Serial.println("Current id_tag:" + String(Exiting_currentIdTag));
            Serial.println("******selecting via offline*****,...........1");
            if (currentIdTag ==  Exiting_currentIdTag )
            {
              // Online_offline_stop_A = 1;
              Serial.println("******sTOPPING IN OFFLINe*****,...........1");
              // result = assignEvseToConnector_Offl(readIdTag, readConnectorVal);
            }
          }
          break;

        default:

          break;
        }
      }

#endif

      if (gu8_online_flag == 1)
      {
        // gu8_online_trans_initiated = 1;
        Serial.println("******selecting via online*****");
        result = assignEvseToConnector(readIdTag, readConnectorVal);
        switch (readConnectorVal)
        {
        case 1:
          Stored_Tag_ID_A = readIdTag;
          break;
        default:

          break;
        }
      }
      else if (gu8_online_flag == 0)
      {
        Serial.println(F("******selecting via offline*****"));

        // evse_connection_status = EVSE_OFFLINE;

        result = assignEvseToConnector(readIdTag, readConnectorVal);
        switch (readConnectorVal)
        {
        case 1:
          Stored_Tag_ID_A = readIdTag;
          break;
        default:

          break;
        }
      }

      if (result == true)
      {
        Serial.println("Attached/Detached EVSE to the requested connector");
      }
      else
      {
        Serial.println("Unable To attach/detach EVSE to the requested connector");
      }
    }
    else
    {
      Serial.println("Invalid Connector Id Received");

      // delay(2000);
    }
  }
  // delay(100);
}

bool assignEvseToConnector(String readIdTag, int readConnectorVal)
{
  bool status = false;
  unsigned long tout = millis();
  if (readConnectorVal == 1)
  {
    if (getChargePointStatusService()->getIdTag() == readIdTag && getChargePointStatusService()->getTransactionId() != -1)
    {
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("RFID TAPPED");
      lcd.setCursor(0, 2);
      lcd.print("AUTHENTICATING");
      // stop session
      // gu8_online_trans_initiated = 0;
      Serial.println("[EVSE_A] Stopping Transaction with RFID TAP");
      EVSE_StopSession();
      // session_ongoing_flag = false;
      // session_on_offline_txn_completed = false;
      status = true;
    }

#if 0
else  if (evse_ChargePointStatus == Available)
      {
        if (DEBUG_OUT)
          Serial.print("assignEvseToConnector : ChargePointStatus::Available .\n");
        getChargePointStatusService()->authorize(readIdTag, readConnectorVal);
        // gu8_online_trans_initiated = 1;
        status = true;
      }

#endif

#if 1

    if (onlineoffline_1 == 1)
    {
      if (evse_ChargePointStatus == Available)
      {
        if (DEBUG_OUT)
          Serial.print("assignEvseToConnector : ChargePointStatus::Available .\n");
        getChargePointStatusService()->authorize(readIdTag, readConnectorVal);
        // gu8_online_trans_initiated = 1;
        status = true;
      }
    }
#if 1
    if (getChargePointStatusService()->getIdTag() == readIdTag && getChargePointStatusService()->getEvDrawsEnergy() == true)
    {
      Serial.println("[EVSE_A] Stopping Transaction with RFID TAP OFFLINE");
      EVSE_StopSession();
      // session_ongoing_flag = false;
      // session_on_offline_txn_completed = false;
      status = true;
    }
#endif
    else if (onlineoffline_1 == 0 && gu8_online_flag == 1)
    {

      if (evse_ChargePointStatus == Available)
      {
        if (DEBUG_OUT)
          Serial.print("assignEvseToConnector : ChargePointStatus::Available .\n");
        getChargePointStatusService()->authorize(readIdTag, readConnectorVal);
        // gu8_online_trans_initiated = 1;
        status = true;
      }
    }

#endif

    else if (evse_ChargePointStatus == Reserved)
    {
      if (flag_evseReserveNow)
      {

        if (DEBUG_OUT)
          Serial.print("EVSE_setOnAuthentication_A Callback: ReserveIdTag_A \n");

        getChargePointStatusService()->authorize(readIdTag, readConnectorVal); // authorizing twice needed to be improvise
        status = true;
      }
    }
  }

  else
  {
    Serial.println("Connector Unavailable");
    status = false;
  }

  return status;
}

#endif
