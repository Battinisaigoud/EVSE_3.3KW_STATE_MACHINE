// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\CustomGsm.h"
//@wamique
#ifndef CUSTOM_GSM
#define CUSTOM_GSM
//#include<SoftwareSerial.h>
#include <ArduinoJson.h>
#include "libraries/arduinoWebSockets-master/src/WebSocketsClient.h"
#include "OcppEngine.h"
#include "EVSE.h"
#include "Variants.h"
/*
* @brief : OTA requirements.
*/
#include "FS.h"
#include "SPIFFS.h"
#include <Update.h>
#include "FFat.h"
#include <CRC32.h>

/*
#define WS_FIN            0x80
#define WS_FIN            0x80
#define WS_OPCODE_TEXT    0x01
#define WS_OPCODE_BINARY  0x02
#define WS_OPCODE_CLOSE   0x08
#define WS_OPCODE_PING    0x09
#define WS_OPCODE_PONG    0x0a
// Second byte
#define WS_MASK           0x80
#define WS_SIZE16         126
#define WS_SIZE64         127
*/
/**********************************/
//#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_MODEM_SIM7600
#define SerialAT Serial2
#define SerialMon Serial

#include <SoftwareSerial.h>


#if !defined(TINY_GSM_RX_BUFFER)
// #define TINY_GSM_RX_BUFFER 1650
#define TINY_GSM_RX_BUFFER 1024//CHANGED BY ABHIGNA

#endif

#define TINY_GSM_USE_GPRS true

// Range to attempt to autobaud
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

const char apn[]  = "m2misafe";
const char gprsUser[] = "";
const char gprsPass[] = "";

//const char server[] = "13.233.136.157";
//const char resource[] = "/steve/websocket/CentralSystemService/dummyCP002";

#include <TinyGsmClient.h>

//const int  port = 8080;
/***********************************/

#define WEBSOCKETS_MAX_HEADER_SIZE (14)
#define WEBSOCKETS_MAX_DATA_SIZE (15 * 1024)
#define WEBSOCKETS_USE_BIG_MEM
#define GET_FREE_HEAP ESP.getFreeHeap()

/*********************************************************/
bool sendTXTGsmStr(String & payload);
bool sendTXTGsm(uint8_t * payload, size_t length = 0, bool headerToPayload = false);
bool sendFrame(WSopcode_t opcode, uint8_t * payload, size_t length, bool fin, bool headerToPayload);
#if GSM_PING 
bool sendPingGsmStr(String &payload);
bool sendPingGsm(uint8_t *payload, size_t length, bool headerToPayload);
#endif
/*************************************************************/

void SetupGsm();
void printSerialData();
//void SendData(String comm,int tim, int sp);
void ConnectToServer();
void gsmOnEvent();
void gsm_Loop();

/*
* @brief : OTA functions
*/

// void printPercent(uint32_t, uint32_t);
// void setup_4G_OTA();
// void pu(fs::FS & fs, const char * path);
#endif
