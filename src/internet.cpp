// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\internet.cpp"
#include "internet.h"

#if DWIN_ENABLED
#include "dwin.h"
extern unsigned char unavail[22]; // not available
extern unsigned char CONN_UNAVAIL[30];
extern unsigned char clear_avail[28];
#endif

void wifi_Loop();
void cloudConnectivityLed_Loop();

extern bool isInternetConnected;
extern TinyGsmClient client;
extern WebSocketsClient webSocket;

extern bool wifi_connect;
extern bool wifi_enable;

extern bool gsm_connect;
extern bool gsm_enable;

extern uint8_t gu8_poweronrest_online_flag;

int counter_gsmconnect = 0;
int counter_wifiConnect = 0;

extern void connectToWebsocket();

void internetLoop(){

	if(wifi_enable == true && gsm_enable == true){
		//
		wifi_gsm_connect();

	}else if(wifi_enable == true){
		//
		wifi_connect = true;
		gsm_connect  = false;
		// wifi_Loop();
		// connectToWebsocket();
		webSocket.loop();
		// cloudConnectivityLed_Loop();

	}else if(gsm_enable == true){

		gsm_connect = true;
		wifi_connect = false;  //redundant
		gsm_Loop();
	}

}



void wifi_gsm_connect(){

	if(wifi_connect == true){
		gsm_connect = false;
		Serial.println("[wifi_gsm_connect] WiFi");
		// wifi_Loop();
		webSocket.loop();
		// cloudConnectivityLed_Loop();
		if(WiFi.status() != WL_CONNECTED || isInternetConnected == false){
			Serial.println("[wifi] counter_wifiConnect"+ String(counter_wifiConnect));
			counter_wifiConnect++;
			if(counter_wifiConnect > 50){
				Serial.println("Switching To gsm");
				WiFi.disconnect();
				counter_wifiConnect = 0;
				wifi_connect = false;
				gsm_connect = true;
			}

		}else{
			counter_wifiConnect = 0;
			Serial.println("default Counter_wifiConnect");
		}
	}else if(gsm_connect == true){
		wifi_connect  = false;
		Serial.println("[wifi_gsm_connect] GSM ");
		gsm_Loop();

		if(!client.connected()){
			Serial.println("[gsm] counter_gsmconnect:"+ String(counter_gsmconnect));
			//if(counter_gsmconnect++ > 3){   //almost 5 min
			if(counter_gsmconnect++ > 2){   //faster approach
				counter_gsmconnect = 0;
				Serial.println("Switching to WIFI");
				#if DWIN_ENABLED
				uint8_t err = 0;
    CONN_UNAVAIL[4] = 0X66;
    err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
    CONN_UNAVAIL[4] = 0X71;
    err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
    CONN_UNAVAIL[4] = 0X7B;
    err = DWIN_SET(CONN_UNAVAIL, sizeof(CONN_UNAVAIL) / sizeof(CONN_UNAVAIL[0]));
	err = DWIN_SET(unavail, sizeof(unavail) / sizeof(unavail[0]));
    delay(10);
	#endif
				wifi_connect = true;
				gsm_connect = false;
			}

		}



	}



}