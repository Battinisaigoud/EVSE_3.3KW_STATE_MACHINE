// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\CustomGsm.cpp"
#include "CustomGsm.h"
#include "bluetoothConfig.h"
#include "EVSE.h"
#include <Preferences.h>
#include "dwin.h"
extern String ws_url_prefix_m;
extern String host_m;
extern int port_m;
extern String path_m;
extern String protocol_m;
extern Preferences preferences;
extern String uri;

extern bool flag_ping_sent;
extern bool notFaulty_A;

extern int client_reconnect_flag;

extern bool internet;
extern uint8_t gu8_check_online_count2;
extern bool ongoingTxn_m;



TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

//added by sai
extern bool webSocketConncted;

// uint32_t   knownCRC32    = 0x6f50d767;
// uint32_t   knownFileSize = 1148544;  // In case server does not send it

#if LCD_DISPLAY
#include "LCD_I2C.h"
extern LCD_I2C lcd;
#endif


#if SIMCOM_A7672S
#define RXD2 16
#define TXD2 17

// Added for SIMCOM A7670C
#elif SIMCOM_A7670C
#define RXD2 17
#define TXD2 16
#endif

#if DWIN_ENABLED
extern unsigned char clun[22];
extern unsigned char not_avail[22];
extern unsigned char avail[22];
extern unsigned char g[22];
extern unsigned char clear_avail[28];
#endif

extern uint8_t gu8_online_flag; 

void SetupGsm()
{

 // Added for SIMCOM A7670C
    SerialAT.begin(115200, SERIAL_8N1, RXD2, TXD2);
  if (DEBUG_OUT)
    Serial.println("[CustomSIM7672] Starting 4G Setup");
  //TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);      // Commented for SIMCOM A7670C
  delay(200);
  // getSimStatus();
  if (DEBUG_OUT)
    Serial.println("[CustomSIM7672] Initializing modem...");
  modem.restart();                                                   

  String modemInfo = modem.getModemInfo();
  if (DEBUG_OUT)
    Serial.print("[CustomSIM7672] Modem Info: ");
  Serial.println(modemInfo);

#if TINY_GSM_USE_GPRS && defined TINY_GSM_MODEM_XBEE
  // The XBee must run the gprsConnect function BEFORE waiting for network!
  modem.gprsConnect(apn, gprsUser, gprsPass);
#endif

  if (DEBUG_OUT)
    SerialMon.println("[CustomSIM7672] Waiting for network...");
   
  // if (!modem.waitForNetwork()) {
  // if (!modem.waitForNetwork(120000))
  //added by sai

  
  if (!modem.waitForNetwork(20000))
  {
    if (DEBUG_OUT)
      SerialMon.println("[CustomSIM7672] fail");
      
    delay(200);
    return ;
  }
  if (DEBUG_OUT)
    SerialMon.println("[CustomSIM7672] success");

  if (modem.isNetworkConnected())
  {
    if (DEBUG_OUT)
      SerialMon.println("[CustomSIM7672] Network connected");
  }

#if TINY_GSM_USE_GPRS
  // GPRS connection parameters are usually set after network registration
  if (DEBUG_OUT)
    SerialMon.print(F("[CustomSIM7672] Connecting to "));
  preferences.begin("credentials", false);
  String apn_1 = preferences.getString("apn", "m2misafe");
  preferences.end();
  SerialMon.print(apn_1);
  if (!modem.gprsConnect(apn_1.c_str(), gprsUser, gprsPass))
  {
    if (DEBUG_OUT)
      SerialMon.println("[CustomSIM7672] fail");
    delay(200);
    return;
  }
  if (DEBUG_OUT)
    SerialMon.println("[CustomSIM7672] success");

  if (modem.isGprsConnected())
  {
    if (DEBUG_OUT)
      SerialMon.println("[CustomSIM7672]GPRS connected");
  }

  int csq = modem.getSignalQuality();
  Serial.println("Signal quality: " + String(csq));
  delay(1);

#endif
}

void ConnectToServer()
{
  host_m.trim();
  const char *host = (host_m).c_str();
  Serial.println(host);

  if (DEBUG_OUT)
    SerialMon.println(F("[CustomSIM7672] Connecting to "));
  SerialMon.println(String(host_m) + "\n" + String(port_m));
  if (!client.connect(host, port_m))
  {
    if (DEBUG_OUT)
      SerialMon.println(F("[CustomSIM7672] Connection request failed"));
    delay(1000);
    return;
  }
  if (DEBUG_OUT)
    SerialMon.println(F("[CustomSIM7672] Connected"));

  // // Make a HTTP GET request:
  //   Serial.println(ws_url_prefix_m);
  //   String url = String(ws_url_prefix_m);
  //   url = url.substring(6);
  //   int a = url.indexOf('/');
  //   url = url.substring(a);
  //   String cpSerial = String("");
  //   preferences.begin("credentials",false);
  //   EVSE_A_getChargePointSerialNumber(cpSerial);
  //   Serial.println(cpSerial);
  //   url += cpSerial;

  //   preferences.end();

  Serial.println("[Custom SIM7672: ]" + String(path_m));
  SerialMon.println("Performing HTTP GET request...");
  client.print(String("GET ") + path_m + " HTTP/1.1\r\n");
  client.print(String("Host: ") + host_m + "\r\n");
  client.print("Upgrade: websocket\r\n");  // delay(300);
  client.print("Connection: Upgrade\r\n"); // delay(300);
  //  client.print("Origin: http://13.233.136.157:8080\r\n");//delay(300);
  client.print("Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n"); // delay(300);
  //client.print("Sec-WebSocket-Protocol: ocpp2.0, ocpp1.6\r\n");    // delay(300);
  client.print("Sec-WebSocket-Protocol: ocpp1.6\r\n");    // delay(300);
  //  client.print("Connection: keep-alive\r\n");
  client.print("Sec-WebSocket-Version: 13\r\n\r\n"); // delay(300);

  delay(100);

  // printSerialData();
  gsm_Loop();
}

#if 0
void ConnectToServer(){
    const char* host = host_m.c_str(); 

   if(DEBUG_OUT) SerialMon.print("[CustomSIM7672] Connecting to ");
    SerialMon.println(String(host_m) + "\n" + String(port_m));
    if (!client.connect(host, port_m)) {
       if(DEBUG_OUT) SerialMon.println("[CustomSIM7672] Connection request failed");
        delay(1000);
        return;
    }
   if(DEBUG_OUT) SerialMon.println("[CustomSIM7672] Connected");


  // Make a HTTP GET request:
    Serial.println(ws_url_prefix_m);
    String url = String(ws_url_prefix_m);
    url = url.substring(6);
    int a = url.indexOf('/');
    url = url.substring(a);
    String cpSerial = String("");
    preferences.begin("credentials",false);
    EVSE_getChargePointSerialNumber(cpSerial);
    Serial.println(cpSerial);
    Serial.println(url);
    Serial.println(host_m);
    url += cpSerial;

    preferences.end();

    if(DEBUG_OUT) Serial.println("[Custom SIM7672: ]"+ String(url));
    if(DEBUG_OUT) SerialMon.println("Performing HTTP GET request...");
    client.print(String("GET ") + url + " HTTP/1.1\r\n");
    client.print(String("Host: ") + host_m + "\r\n");
    client.print("Upgrade: websocket\r\n");//delay(300);
    client.print("Connection: Upgrade\r\n");//delay(300);
  //  client.print("Origin: http://13.233.136.157:8080\r\n");//delay(300);
    client.print("Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n");//delay(300);
    //client.print("Sec-WebSocket-Protocol: ocpp2.0, ocpp1.6\r\n");//delay(300);
    client.print("Sec-WebSocket-Protocol: ocpp1.6\r\n");
   //  client.print("Connection: keep-alive\r\n");
    client.print("Sec-WebSocket-Version: 13\r\n\r\n");//delay(300);


    delay(100);

    //printSerialData();
    gsm_Loop();

}
#endif

void printSerialData()
{
  while (client.available() != 0)
    Serial.println(client.readString());
}

int counter_clientConnection = 0;

void gsm_Loop()
{
uint8_t err = 0;
  if (!client.connected())
  {

    Serial.println("[CustomSIM7672] Client is disconnected. Trying to connect");
    
    //added by sai
   webSocketConncted = false;
    gu8_online_flag = 0;
    internet = false;
    gu8_check_online_count2 = 10;
#if 0
#if LCD_ENABLED
    lcd.clear();
    lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
    lcd.print("CHARGER  UNAVAILABLE");
    // lcd.setCursor(0, 1);
    // lcd.print("TAP RFID/SCAN QR");
    // lcd.setCursor(0, 2);
    // lcd.print("CONNECTION");
    lcd.setCursor(0, 2);
    lcd.print("OFFLINE");
// lcd.print("CLOUD: offline");
#endif
#endif

#if DWIN_ENABLED

    err = DWIN_SET(not_avail, sizeof(not_avail) / sizeof(not_avail[0]));
    avail[4] = 0x51;
    err = DWIN_SET(avail, sizeof(clear_avail) / sizeof(clear_avail[0]));
    err = DWIN_SET(clun, sizeof(clun) / sizeof(clun[0]));
#endif
    delay(100); // do not flood the network (have to create millis)
    if (counter_clientConnection++ >= 1)
    {
      SetupGsm();
      counter_clientConnection = 0;
    }
    ConnectToServer();
    
    //added by sai
     #if 0
  
    if(gu8_online_flag == 0)
    {
    	if (getChargePointStatusService()->getEmergencyRelayClose() == true)
				{
					// Serial.println(" FAULTY CONDITION DETECTED.");
					// Serial.println(F("****EVSE_StopSession*****"));
					
								EVSE_StopSession();
				}
    }

    #endif


  }
  else
  {
    #if DWIN_ENABLED
    if(client_reconnect_flag == 2)
    {
    avail[4] = 0x55;
    err = DWIN_SET(avail, sizeof(avail) / sizeof(avail[0]));
    avail[4] = 0x51;
    err = DWIN_SET(avail, sizeof(clear_avail) / sizeof(clear_avail[0]));
    err = DWIN_SET(g, sizeof(g) / sizeof(g[0]));
    }
    #endif
    Serial.println("[Custom SIM7672S] Client Connected");
     gu8_online_flag = 1;
    counter_clientConnection = 0;
  }

  gsmOnEvent();
}

void processJson(String &IncomingDataString)
{
  int index1 = -1;
  int index2 = -1;
  int len;
  uint8_t *payload;
  size_t DataLen;
  String DataString;

  int csq = modem.getSignalQuality();
  Serial.println("Signal quality: " + String(csq));

  len = IncomingDataString.length();
  if (DEBUG_EXTRA)
    Serial.println("\nLen-->" + String(len));
  IncomingDataString.trim();
  len = IncomingDataString.length();
  if (DEBUG_EXTRA)
    Serial.println("\nAfter trim Len-->" + String(len));

  index1 = IncomingDataString.indexOf('[');
  index2 = IncomingDataString.lastIndexOf(']');
  delay(250);
  if (DEBUG_EXTRA)
    Serial.println(IncomingDataString);
  //  delay(1000);
  // index3 = IncomingDataString.indexOf(']');
  if (DEBUG_EXTRA)
    Serial.println("");
  if (DEBUG_EXTRA)
    Serial.println("Index 1--->" + String(index1));
  if (DEBUG_EXTRA)
    Serial.println("Index 2--->" + String(index2));
  // Serial.println("Index 2--->"+String(index3));
  if (index1 != -1 && index2 != -1)
  {

    DataString = IncomingDataString.substring(index1, index2 + 1);
    DataLen = DataString.length();

    if (DEBUG_EXTRA)
      Serial.println(DataString + "\n Length:" + String(DataLen));
    Serial.println("[CustomSIM7672] Wsc_Txt: " + DataString);

    payload = (uint8_t *)DataString.c_str();

    if (!processWebSocketEvent(payload, DataLen))
    { // forward message to OcppEngine
      if (DEBUG_OUT)
        Serial.println("[CustomSIM7672] Processing WebSocket input event failed!\n");
    }

    IncomingDataString = ""; // Clearing the local buffer
  }
  // if(DEBUG_EXTRA) Serial.println("It's not Json packet");
}

void printingUint(String IncomingDataString)
{
  int length = IncomingDataString.length();

  uint8_t *payload = (uint8_t *)IncomingDataString.c_str();

  Serial.println("");
  Serial.print("Payload Size-->" + String(sizeof(payload)));
  for (int i = 0; i < length; i++)
  {
    Serial.print(" ");
    Serial.print(payload[i], DEC);
  }
  Serial.println("");
}

void gsmOnEvent()
{
  bool flagWebSktHdrTxt = 0;
  bool flagCounter = 0;
  bool flagJsonPkt = 1;
  char c;
  char header;
  int CounterOpenBracket = 0;
  int CounterCloseBracket = 0;
  String IncomingDataString = "";
  const char *pongResp = "rockybhai"; 
  uint8_t len1 = strlen(pongResp);
  uint8_t sum1 = 0;
  while (client.connected() && client.available())
  {
    c = client.read();
   #if 0
    if(c == 0X0A && flag_ping_sent)   
    {
      flag_ping_sent = false;
      Serial.println("pong received2$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    break;
    }
    #endif
    sum1 = (c == pongResp[sum1]) ? sum1 + 1 : 0;
    if (sum1 == len1) 
    {
      flag_ping_sent = false;
      Serial.println("pong received!@@@@@@@@@@@@");
      //added by sai
      

        webSocketConncted = true;

        if (evse_boot_state == EVSE_BOOT_ACCEPTED && ongoingTxn_m == 1)
        {
          Serial.print("NOT SET ");
          evse_ChargePointStatus = NOT_SET;
        }

      break;
    }

    if (c == 129 && flagJsonPkt == 1)
    { // Reading Headers
      flagJsonPkt = 0;
      flagWebSktHdrTxt = 1;
    }

    if (flagWebSktHdrTxt == 1)
    {
      flagWebSktHdrTxt = 0;
      header = client.read(); // reading payload length
      if (header >= 126)
      {
        client.read(); // flushing Extra headers
        client.read();
      }
    }
    else
    { // End of Headers
      IncomingDataString += c;
      if (c == '[')
      {
        CounterOpenBracket++;
        flagCounter = 1;
      }
      if (c == ']')
        CounterCloseBracket++;

      if (CounterOpenBracket == CounterCloseBracket && flagCounter == 1)
      {

        if (DEBUG_EXTRA)
          Serial.print(c);
        if (DEBUG_EXTRA)
          printingUint(IncomingDataString);
        processJson(IncomingDataString);
        CounterOpenBracket = 0;
        flagCounter = 0;
        CounterCloseBracket = 0;
        flagJsonPkt = 1;
        flagWebSktHdrTxt = 0;
      }
      else
      {

        if (1)
          Serial.print(c);
      }
    }
  }
}
/*void gsmOnEvent(){

   // String DataString;
    String IncomingDataString;
    int CounterOpenBracket=0;
    int CounterCloseBracket=0;
    char c;
    int flag=0;
   while(client.connected() && client.available()) {
    delay(2);

  //  if(DEBUG_OUT) Serial.println("[CustomSIM7672]Data receieved in GSMBuffer: ");
    c = client.read();

    IncomingDataString += c;

    if(c=='[') {CounterOpenBracket++; flag=1;}
    if(c==']') CounterCloseBracket++;

    if(CounterOpenBracket == CounterCloseBracket && flag == 1){

        Serial.print(c);
        printinUint(IncomingDataString);
        processJson(IncomingDataString);
        CounterOpenBracket=0;
        flag=0;
        CounterCloseBracket=0;
    }else{

        Serial.print(c);
    }

 //Commented this section
    IncomingDataString = client.readString();
    Serial.println(IncomingDataString);
    // int startByte = IncomingDataString.indexOf('[');
    int startByte = 2;
    int endByte = IncomingDataString.indexOf(']');
   if(DEBUG_OUT) Serial.println("[CustomSIM7672]Indexes are: ");
    Serial.println(startByte);
    Serial.println(endByte);
    if(startByte != -1 && endByte !=-1){
      DataString = IncomingDataString.substring(startByte,endByte+1);
      // IncomingDataString.substring(2);
      DataLen = DataString.length();
      Serial.println(DataString);
      Serial.println(DataLen);

      payload = (uint8_t *)DataString.c_str();

    if (!processWebSocketEvent(payload, DataLen)) { //forward message to OcppEngine
       if(DEBUG_OUT) Serial.print(F("[CustomSIM7672] Processing WebSocket input event failed!\n"));
      }
   }

  }
}*/

/***********************************************************************************************/
bool sendTXTGsmStr(String &payload)
{

  return sendTXTGsm((uint8_t *)payload.c_str(), payload.length());
}

bool sendTXTGsm(uint8_t *payload, size_t length, bool headerToPayload)
{

  if (length == 0)
  {
    length = strlen((const char *)payload);
  }
  if (true)
  {
    return sendFrame(WSop_text, payload, length, true, headerToPayload);
  }
  return false;
}
//added for gsm ping

#if GSM_PING   
bool sendPingGsmStr(String &payload)
{

  return sendPingGsm((uint8_t *)payload.c_str(), payload.length(),false);
}

bool sendPingGsm(uint8_t *payload, size_t length, bool headerToPayload)
{

  if (length == 0)
  {
    length = strlen((const char *)payload);
  }
  if (true)
  {
    return sendFrame(WSop_ping, payload, length, true, headerToPayload);
  }
  return false;
}
#endif

uint8_t createHeader(uint8_t *headerPtr, WSopcode_t opcode, size_t length, bool mask, uint8_t maskKey[4], bool fin)
{
  uint8_t headerSize;
  // calculate header Size
  if (length < 126)
  {
    headerSize = 2;
  }
  else if (length < 0xFFFF)
  {
    headerSize = 4;
  }
  else
  {
    headerSize = 10;
  }

  if (mask)
  {
    headerSize += 4;
  }

  // create header

  // byte 0
  *headerPtr = 0x00;
  if (fin)
  {
    *headerPtr |= bit(7); ///< set Fin
  }
  *headerPtr |= opcode; ///< set opcode
  headerPtr++;

  // byte 1
  *headerPtr = 0x00;
  if (mask)
  {
    *headerPtr |= bit(7); ///< set mask
  }

  if (length < 126)
  {
    *headerPtr |= length;
    headerPtr++;
  }
  else if (length < 0xFFFF)
  {
    *headerPtr |= 126;
    headerPtr++;
    *headerPtr = ((length >> 8) & 0xFF);
    headerPtr++;
    *headerPtr = (length & 0xFF);
    headerPtr++;
  }
  else
  {
    // Normally we never get here (to less memory)
    *headerPtr |= 127;
    headerPtr++;
    *headerPtr = 0x00;
    headerPtr++;
    *headerPtr = 0x00;
    headerPtr++;
    *headerPtr = 0x00;
    headerPtr++;
    *headerPtr = 0x00;
    headerPtr++;
    *headerPtr = ((length >> 24) & 0xFF);
    headerPtr++;
    *headerPtr = ((length >> 16) & 0xFF);
    headerPtr++;
    *headerPtr = ((length >> 8) & 0xFF);
    headerPtr++;
    *headerPtr = (length & 0xFF);
    headerPtr++;
  }

  if (mask)
  {
    *headerPtr = maskKey[0];
    headerPtr++;
    *headerPtr = maskKey[1];
    headerPtr++;
    *headerPtr = maskKey[2];
    headerPtr++;
    *headerPtr = maskKey[3];
    headerPtr++;
  }
  return headerSize;
}

bool sendFrame(WSopcode_t opcode, uint8_t *payload, size_t length, bool fin, bool headerToPayload)
{

  if (!client.connected())
  {

    if (DEBUG_OUT)
      Serial.println("[CustomSIM7672] Client is not connected");
    return false;
  }

  /*
    if(client->tcp && !client->tcp->connected()) {
        DEBUG_WEBSOCKETS("[WS][%d][sendFrame] not Connected!?\n", client->num);
        return false;
    }

    if(client->status != WSC_CONNECTED) {
        DEBUG_WEBSOCKETS("[WS][%d][sendFrame] not in WSC_CONNECTED state!?\n", client->num);
        return false;
    }
*/
  //   DEBUG_WEBSOCKETS("[WS][%d][sendFrame] ------- send message frame -------\n", client->num);
  //   DEBUG_WEBSOCKETS("[WS][%d][sendFrame] fin: %u opCode: %u mask: %u length: %u headerToPayload: %u\n", client->num, fin, opcode, true, length, headerToPayload);

  //   if(opcode == WSop_text) {
  //       DEBUG_WEBSOCKETS("[WS][%d][sendFrame] text: %s\n", client->num, (payload + (headerToPayload ? 14 : 0)));
  //   }

  uint8_t maskKey[4] = {0x00, 0x00, 0x00, 0x00};
  uint8_t buffer[WEBSOCKETS_MAX_HEADER_SIZE] = {0};

  uint8_t headerSize;
  uint8_t *headerPtr;
  uint8_t *payloadPtr = payload;
  bool useInternBuffer = false;
  bool ret = true;

  // calculate header Size
  if (length < 126)
  {
    headerSize = 2;
  }
  else if (length < 0xFFFF)
  {
    headerSize = 4;
  }
  else
  {
    headerSize = 10;
  }

  if (true)
  {
    headerSize += 4;
  }

#ifdef WEBSOCKETS_USE_BIG_MEM

  // only for ESP since AVR has less HEAP
  // try to send data in one TCP package (only if some free Heap is there)
  if (!headerToPayload && ((length > 0) && (length < 1400)) && (GET_FREE_HEAP > 6000))
  {
    //  DEBUG_WEBSOCKETS("[WS][%d][sendFrame] pack to one TCP package...\n", client->num);
    uint8_t *dataPtr = (uint8_t *)malloc(length + WEBSOCKETS_MAX_HEADER_SIZE);
    if (dataPtr)
    {
      memcpy((dataPtr + WEBSOCKETS_MAX_HEADER_SIZE), payload, length);
      headerToPayload = true;
      useInternBuffer = true;
      payloadPtr = dataPtr;
    }
  }
#endif

  // set Header Pointer
  if (headerToPayload)
  {
    // calculate offset in payload
    headerPtr = (payloadPtr + (WEBSOCKETS_MAX_HEADER_SIZE - headerSize));
  }
  else
  {
    headerPtr = &buffer[0];
  }

  if (true && useInternBuffer)
  {
    // if we use a Intern Buffer we can modify the data
    // by this fact its possible the do the masking
    for (uint8_t x = 0; x < sizeof(maskKey); x++)
    {
      maskKey[x] = random(0xFF);
    }
  }

  createHeader(headerPtr, opcode, length, true, maskKey, fin);

  if (true && useInternBuffer)
  {
    uint8_t *dataMaskPtr;

    if (headerToPayload)
    {
      dataMaskPtr = (payloadPtr + WEBSOCKETS_MAX_HEADER_SIZE);
    }
    else
    {
      dataMaskPtr = payloadPtr;
    }

    for (size_t x = 0; x < length; x++)
    {
      dataMaskPtr[x] = (dataMaskPtr[x] ^ maskKey[x % 4]);
    }
  }

#ifndef NODEBUG_WEBSOCKETS
  unsigned long start = micros();
#endif

  if (headerToPayload)
  {
    // header has be added to payload
    // payload is forced to reserved 14 Byte but we may not need all based on the length and mask settings
    // offset in payload is calculatetd 14 - headerSize
    if (client.write(&payloadPtr[(WEBSOCKETS_MAX_HEADER_SIZE - headerSize)], (length + headerSize)) != (length + headerSize))
    {
      ret = false;
    }
  }
  else
  {
    // send header
    if (client.write(&buffer[0], headerSize) != headerSize)
    {
      ret = false;
    }

    if (payloadPtr && length > 0)
    {
      // send payload
      if (client.write(&payloadPtr[0], length) != length)
      {
        ret = false;
      }
    }
  }

  //  DEBUG_WEBSOCKETS("[WS][%d][sendFrame] sending Frame Done (%luus).\n", client->num, (micros() - start));
  // mySerial.write("\r\n");
  // mySerial.write(0x1a);
  // mySerial.flush();
  delay(100);
// printSerialData();
#ifdef WEBSOCKETS_USE_BIG_MEM
  if (useInternBuffer && payloadPtr)
  {
    free(payloadPtr);
  }
#endif
  if (DEBUG_OUT)
    Serial.println("[CustomGsm-frame] Return : " + String(ret));
  return ret;
}

//------------------------------------------------------------------ 4G OTA ------------------------------------------------------------------------------------
#if 0


const char server[] = "34.93.75.210";
const int port = 80;
// const char resource[] = "/display_TestUART.ino.esp32.bin"; //here de bin file
const char resource[] = "/fota2.php"; // here de bin file


void printPercent(uint32_t readLength, uint32_t contentLength)
{
  // If we know the total length
  if (contentLength != (uint32_t)-1)
  {
    SerialMon.print("\r ");
    SerialMon.print((100.0 * readLength) / contentLength);
    SerialMon.print('%');
  }
  else
  {
    SerialMon.println(readLength);
  }
}

void setup_4G_OTA()
{
  /*
  * @brief : The following may not be necessary as modem is already connected.
  */
  #if 0
  modem.gprsConnect(apn, gprsUser, gprsPass);
  // ledOn(YELLOW);
  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork())
  {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected())
  {
    SerialMon.println("Network connected");
  }
  // ledOn(YELLOW);
#if TINY_GSM_USE_GPRS
  // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("Connecting to "));
  preferences.begin("credentials", false);
  String apn_1 = preferences.getString("apn", "m2misafe");
  preferences.end();
  SerialMon.print(apn_1);
  if (!modem.gprsConnect(apn_1.c_str(), gprsUser, gprsPass))
  {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected())
  {
    SerialMon.println("GPRS connected");
  }
#endif


#endif
  // ledOn(YELLOW);
  SetupGsm();
  SerialMon.print(F("Connecting to "));
  SerialMon.print(server);
  if (!client.connect(server, port))
  {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  // Make a HTTP GET request:
  // ledOn(YELLOW);

  client.print(String("POST ") + resource + " HTTP/1.1\r\n");
  client.print(String("Host: ") + server + "\r\n");
  client.print(String("x-ESP32-STA-MAC: ") + String(WiFi.macAddress()) + "\r\n");
  client.print(String("x-ESP32-AP-MAC: ") + String(WiFi.softAPmacAddress()) + "\r\n");
  client.print(String("x-ESP32-sketch-md5: ") + String(ESP.getSketchMD5()) + "\r\n");
  client.print(String("x-ESP32-STA-MAC: ") + String(ESP.getSdkVersion()) + "\r\n");
  client.print(String("x-ESP32-STA-MAC: ") + String(ESP.getFreeSketchSpace()) + "\r\n");
  client.print("Connection: close\r\n\r\n");

  // Let's see what the entire elapsed time is, from after we send the request.
  uint32_t timeElapsed = millis();
  // ledOn(YELLOW);
  SerialMon.println(F("Waiting for response header"));

  // While we are still looking for the end of the header (i.e. empty line
  // FOLLOWED by a newline), continue to read data into the buffer, parsing each
  // line (data FOLLOWED by a newline). If it takes too long to get data from
  // the client, we need to exit.

  const uint32_t clientReadTimeout = 600000;
  uint32_t clientReadStartTime = millis();
  String headerBuffer;
  bool finishedHeader = false;
  uint32_t contentLength = 0;

  while (!finishedHeader)
  {
    int nlPos;

    if (client.available())
    {
      clientReadStartTime = millis();
      while (client.available())
      {
        char c = client.read();
        headerBuffer += c;

        // Uncomment the lines below to see the data coming into the buffer
        if (c < 16)
          SerialMon.print('0');
        SerialMon.print(c, HEX);
        SerialMon.print(' ');
        /*if (isprint(c))
          SerialMon.print(reinterpret_cast<char> c);
          else
          SerialMon.print('*');*/
        SerialMon.print(' ');

        // Let's exit and process if we find a new line
        if (headerBuffer.indexOf(F("\r\n")) >= 0)
          break;
      }
    }
    else
    {
      if (millis() - clientReadStartTime > clientReadTimeout)
      {
        // Time-out waiting for data from client
        SerialMon.println(F(">>> Client Timeout !"));
        break;
      }
    }
    // ledOn(YELLOW);
    //  See if we have a new line.
    nlPos = headerBuffer.indexOf(F("\r\n"));

    if (nlPos > 0)
    {
      headerBuffer.toLowerCase();
      // Check if line contains content-length
      if (headerBuffer.startsWith(F("content-length:")))
      {
        contentLength =
            headerBuffer.substring(headerBuffer.indexOf(':') + 1).toInt();
        // SerialMon.print(F("Got Content Length: "));  // uncomment for
        // SerialMon.println(contentLength);            // confirmation
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
  // 1. The client doesn't provide data quickly enough to keep up with this
  // loop.
  // 2. If the client data is segmented in the middle of the 'Content-Length: '
  // header,
  //    then that header may be missed/damaged.
  //

  uint32_t readLength = 0;
  CRC32 crc;
  File file = FFat.open("/update.bin", FILE_APPEND);
  // ledOn(YELLOW);
  // if (finishedHeader && contentLength == knownFileSize) {
  if (finishedHeader)
  {

    SerialMon.println(F("Reading response data"));
    clientReadStartTime = millis();

    printPercent(readLength, contentLength);
    while (readLength < contentLength && client.connected() &&
           millis() - clientReadStartTime < clientReadTimeout)
    {
      while (client.available())
      {
        uint8_t c = client.read();
        // char c = client.read();
        // SerialMon.print(c);
        if (!file.write(c))
        {
          Serial.println("Appending file");
        }

        // SerialMon.print(reinterpret_cast<char>c);  // Uncomment this to show
        // data
        crc.update(c);
        readLength++;
        if (readLength % (contentLength / 13) == 0)
        {
          printPercent(readLength, contentLength);
        }
        clientReadStartTime = millis();
      }
    }
    printPercent(readLength, contentLength);
  }

  timeElapsed = millis() - timeElapsed;
  SerialMon.println();
  file.close();

  // Shutdown

  client.stop();
  SerialMon.println(F("Server disconnected"));

#if TINY_GSM_USE_WIFI
  modem.networkDisconnect();
  SerialMon.println(F("WiFi disconnected"));
#endif
#if TINY_GSM_USE_GPRS
  modem.gprsDisconnect();
  SerialMon.println(F("GPRS disconnected"));
#endif

  float duration = float(timeElapsed) / 1000;

  SerialMon.println();
  SerialMon.print("Content-Length: ");
  SerialMon.println(contentLength);
  SerialMon.print("Actually read:  ");
  SerialMon.println(readLength);
  SerialMon.print("Calc. CRC32:    0x");
  SerialMon.println(crc.finalize(), HEX);
  // SerialMon.print("Known CRC32:    0x");
  // SerialMon.println(knownCRC32, HEX);
  SerialMon.print("Duration:       ");
  SerialMon.print(duration);
  SerialMon.println("s");
  // ledOn(YELLOW);
  //  Do nothing forevermore
  // pu();
  pu(FFat, "/update.bin");
}

// void pu()
void pu(fs::FS &fs, const char *path)
{
  // File updateBin = SPIFFS.open("/update.bin");
  SerialMon.println(F("***Starting OTA update***"));
  // File updateBin = FFat.open("/update.bin");
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