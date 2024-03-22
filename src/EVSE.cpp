// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\EVSE.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

/**
Edited by Pulkit Agrawal.
G. Raja Sumant and Krishna for reservation.
*/

#include "EVSE.h"
#include "Master.h"
#include "ControlPilot.h"
#include "CustomGsm.h"
#include "ReserveNow.h"
#include "display_meterValues.h"
#include "LCD_I2C.h"
#include "BootNotification.h"
#include "MeteringService.h"
#include "ChargePointStatusService.h"
#include "OcppOperation.h"
#include "OcppEngine.h"
#include "OcppMessage.h"
#include "Authorize.h"
#include "Heartbeat.h"
#include "StartTransaction.h"
#include "StopTransaction.h"
#include "DataTransfer.h"
#include "SimpleOcppOperationFactory.h"

#include "CustomGsm.h"

#if DWIN_ENABLED
#include "dwin.h"

extern unsigned char v1[8];
extern unsigned char v2[8];
extern unsigned char v3[8];
extern unsigned char i1[8];
extern unsigned char i2[8];
extern unsigned char i3[8];
extern unsigned char e1[8];
extern unsigned char e2[8];
extern unsigned char e3[8];
extern unsigned char change_page[10];
extern unsigned char avail[22];
extern unsigned char charging[28];
extern unsigned char cid1[8];
extern unsigned char fault_emgy[28];
extern unsigned char GFCI_55[20];
extern unsigned char GFCI_66[20];
extern unsigned char clear_avail[28];

#endif
extern int client_reconnect_flag;
// timeout for heartbeat signal.
ulong T_SENDHEARTBEAT = 60000;
bool timeout_active = false;
bool timer_initialize = false;
ulong timeout_start = 0;
ulong timerDisplay;
extern ATM90E36 eic;
extern bool wifi_connect;
extern bool gsm_connect;
extern LCD_I2C lcd;
extern uint8_t reasonForStop_A;
time_t current_reserveDate;
extern time_t reserveDate;
extern time_t reservedDuration;
extern String reserve_currentIdTag;
uint8_t Fault_occured = 0;

uint8_t offline_c = 0;

extern int8_t fault_code_A;

uint8_t gu8_ongoing_stop = 0;

// float minCurr = 0.10;           /* Change the min current from 0.05 to 0.1 Ampere*/
// float minCurr = 0.25;

// new variable names defined by @Pulkit. might break the build.
OnBoot onBoot;
OnReadUserId onReadUserId;
OnSendHeartbeat onSendHeartbeat;
OnAuthentication onAuthentication;
OnStartTransaction onStartTransaction;
OnStopTransaction onStopTransaction;
OnUnauthorizeUser onUnauthorizeUser;

LinkedList<reserveNow_slot *> ReserveNowSlotList;

// new flag names. replace them with old names.
bool evIsPlugged;
bool flag_evseIsBooted;
bool flag_evseReadIdTag;
bool flag_evseAuthenticate;
bool flag_evseStartTransaction;
bool flag_evRequestsCharge;
bool flag_evseStopTransaction;
bool flag_evseUnauthorise;
bool flag_rebootRequired;
bool flag_evseSoftReset;				 // added by @Wamique
bool flag_evseReserveNow = false;		 // added by @mkrishna
bool flag_evseCancelReservation = false; // added by @mkrishna

bool lu8_send_status_flag = false;

bool disp_evse_A = false;

bool notFaulty_A = false;

extern uint8_t gu8_online_flag;

extern uint16_t gu8_fault_flag;

#if LCD_DISPLAY
#include "LCD_I2C.h"
extern LCD_I2C lcd;
#endif
// not used. part of Smart Charging System.
float chargingLimit = 32.0f;
String Ext_currentIdTag = "";
extern MFRC522 mfrc522;
extern String currentIdTag;
extern String currentIdTag_Off;

long int blinckCounter = 0;
int counter1 = 0;

// uint16_t currentCounterThreshold = 650;
String gs32lowcurcnt = "";
uint16_t currentCounterThreshold = 120;
String strgf32mincurr = "";

float minCurr = 0.25;

// added by sai
extern uint8_t gu8_stoponlineoffline_flag;
extern bool fota_available;
uint8_t gu8_state_change = 0;
extern bool flag_evseStopTransaction;

extern bool flag_stop_finishing;
extern int prevTxnId;
extern float LastPresentEnergy_A;
extern float discurrEnergy_A;
extern bool onlineoffline_1;
extern Preferences resumeTxn;
extern int globalmeterstartA;
extern uint8_t gu8_start_accepted;

extern bool ongoingTxn_m;
extern bool ongoingtxpor_1;
extern int transactionId;
extern uint8_t gu8_powerloss_flag;
extern WebSocketsClient webSocket;
extern MeteringService *meteringService;

extern ChargePointStatusService *chargePointStatusService;
extern bool flag_MeteringIsInitialised;

/* EVSE to OCPP message update state machine */
extern enum EvseDevStatuse EvseDevStatus_connector_1;

extern enum evsetransactionstatus EVSE_transaction_status;

extern enum evse_boot_stat_t evse_boot_state;			/* Boot Notification State machine */
extern enum evse_authorize_stat_t evse_authorize_state; /* Authorize State machine */
extern enum evse_start_txn_stat_t evse_start_txn_state; /* Start Transaction State machine */
extern enum evse_stop_txn_stat_t evse_stop_txn_state;	/* Stop Transaction State machine */
extern uint8_t gu8_evse_change_state;

// Reason for stop
extern uint8_t reasonForStop;

ulong t;
ulong relay_timer = 0;
int connectorDis_counter = 0;

extern EVSE_states_enum EVSE_state;
extern Preferences preferences;
extern TinyGsmClient client;
short int fault_counter = 0;
bool flag_faultOccured = false;

short int counter_drawingCurrent = 0;
float drawing_current = 0;

extern bool webSocketConncted;
extern bool isInternetConnected;
short counter_faultstate = 0;

extern time_t reservation_start_time;
time_t reservation_expiry_time = 0;
bool prepare_reserve_slot = false;
bool reservation_start_flag = false;

ulong timerHb = 0;
unsigned int heartbeatInterval = 30;

extern bool flag_GFCI_set_here;

extern String idTagData_m;
extern int txnidData_m;
extern bool ongoingTxn_m;
extern uint8_t remotestart_txn;

// initialize function. called when EVSE is booting.
// NOTE: It should be also called when there is a reset or reboot required. create flag to control that. @Pulkit
/**********************************************************/
void EVSE_StopSession()
{

	if (getChargePointStatusService()->getEvDrawsEnergy() == true)
	{
		getChargePointStatusService()->stopEvDrawsEnergy();
	}

	resumeTxn.begin("resume", false);
	resumeTxn.putInt("reasonForStop", reasonForStop);
	resumeTxn.putInt("fault_code_A", fault_code_A);
	if (webSocketConncted == 0)
	{
		resumeTxn.putInt("offline_stop", 1);
	}

	resumeTxn.end();

	disp_evse_A = false;
	remotestart_txn = 0;
	// digitalWrite(32, LOW);
	requestForRelay(STOP, 1);
	delay(1000);
	flag_evseReadIdTag = false;
	flag_evseAuthenticate = false;
	flag_evseStartTransaction = false;
	flag_evRequestsCharge = false;
	flag_evseStopTransaction = true;

	EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;
	evse_stop_txn_state = EVSE_STOP_TXN_INITIATED;
	flag_evseUnauthorise = false;
	Serial.println("[EVSE] Stopping Session : " + String(EVSE_state));
}
/**************************************************************************/

void EVSE_initialize()
{
	EvseDevStatus_connector_1 = flag_EVSE_is_Booted;

	evse_boot_state = EVSE_BOOT_INITIATED;
	// EVSE_transaction_status = EVSE_NO_TRANSACTION;

	if (gu8_online_flag == 1)
	{
		if (DEBUG_OUT)
		{
			Serial.print("[EVSE] EVSE is powered on or reset. Starting Boot.\n");
		}
		// onBoot();
	}
	else
	{
		if (DEBUG_OUT)
		{
			Serial.print("[EVSE] EVSE is powered on or reset.\n");
		}
	}
}

// This is the main loop function which is controlling the whole charfing process. All the flags created are used to control the flow of the program.

void EVSE_loop()
{

#if 0

	if (flag_evseIsBooted == false)
	{
		if (DEBUG_OUT)
			Serial.println("[EVSE] Booting...");

		// added by sai
		if (wifi_connect)
		{
#if LCD_ENABLED
			lcd.clear();
			lcd.setCursor(1, 1);
			lcd.print("CONNECTING TO Wi-Fi");

#endif
		}
		else if (gsm_connect)
		{
#if LCD_ENABLED
			lcd.clear();
			lcd.setCursor(1, 1);
			lcd.print("CONNECTING TO 4G");

#endif
		}

		delay(1000);
		// onBoot();
		t = millis();
		if(onBoot != NULL)
		{
			if (millis() - timerHb > (heartbeatInterval * 1000))
				{
					timerHb = millis();
					onBoot();
				}
		}
		return;
	}
	else if (flag_evseIsBooted == true)
	{
		

		if (flag_evseReadIdTag == true)
		{
			
			if (fota_available == true)
			{
				Serial.print(F("fota_available trigerred"));
				fota_available = false;
				ESP.restart();
			}
			if (onReadUserId != NULL)
			{
				onReadUserId();
				if (millis() - timerHb > (heartbeatInterval * 1000))
				{
					timerHb = millis();
					onSendHeartbeat();
				}
			}
			return;
		}
		else if (flag_evseAuthenticate == true)
		{
			if (onAuthentication != NULL)
			{
				// Add condition by checking if available or unavailable
				bool un = false;
				un = getChargePointStatusService()->getUnavailable();
				//currentIdTag = getChargePointStatusService()->getIdTag();
				//currentIdTag = EVSE_getCurrnetIdTag(&mfrc522);
				if (!un)
				{
					if (flag_evseReserveNow)
					{
						Serial.print(F(" reserve_currentIdTag :"));
						Serial.println(reserve_currentIdTag);
						
						Serial.print(F(" currentIdTag :"));
						Serial.println(currentIdTag);
						
						if (reserve_currentIdTag == currentIdTag)
						{
							if (DEBUG_OUT)
								Serial.print(F("EVSE_OnAuthentication Callback: ReserveIdTag is same with currentIdTag.\n"));

							bool auth_checker = false;
							auth_checker =  requestGetAuthCache();

							if (auth_checker)
							{
								getChargePointStatusService()->authorize(currentIdTag);
								flag_evseReadIdTag = false;
								flag_evseAuthenticate = false;
								flag_evseStartTransaction = true; // Entry condition for starting transaction.
								flag_evRequestsCharge = false;
								flag_evseStopTransaction = false;
								flag_evseUnauthorise = false;
#if LCD_ENABLED
								// lcd.clear();

								// lcd.setCursor(0, 1);
								// lcd.print("SCAN DONE.");
#endif

								if (DEBUG_OUT)
									Serial.print(F("EVSE_setOnAuthentication Callback: Authorize request has been accepted! Calling StartTransaction Block.\n"));

								requestLed(BLINKYBLUE, START, 1);
								// Serial.print(F("B1\n"));
							}
							else
							{
// 								#if LCD_ENABLED
//     lcd.clear();

//     lcd.setCursor(0, 1);
//     lcd.print("SCAN DONE.");
//   #endif
								onAuthentication();
							}
						}
						else
						{
							
							if (DEBUG_OUT)
								Serial.print(F("EVSE_OnAuthentication_A Callback: ReserveIdTag_A is not same with currentIdTag_A .\n"));
							return ;
						}
					}	
				else
				{
					if (requestGetAuthCache())
						{
							getChargePointStatusService()->authorize(currentIdTag);
							flag_evseReadIdTag = false;
							flag_evseAuthenticate = false;
							flag_evseStartTransaction = true; // Entry condition for starting transaction.
							flag_evRequestsCharge = false;
							flag_evseStopTransaction = false;
							flag_evseUnauthorise = false;

							if (DEBUG_OUT)
								Serial.print(F("EVSE_setOnAuthentication Callback: Authorize request has been accepted! Calling StartTransaction Block.\n"));
								//commented by sai
								//  Serial.print(F("test 3\n"));
							// requestLed(BLUE, START, 1);
// #if LCD_ENABLED
// 							lcd.clear();

// 							lcd.setCursor(0, 1);
// 							lcd.print("SCAN DONE.");
// #endif
						}
						else
						{
// #if LCD_ENABLED
// 							lcd.clear();

// 							lcd.setCursor(0, 1);
// 							lcd.print("SCAN DONE.");
// #endif
							onAuthentication();
						}
				}

			}
			else
			{
				Serial.print(F("EVSE_setOnAuthentication Callback: Authorize denied as un available! Calling StartTransaction Block.\n"));
			}
			}
			return;
		}
		else if (flag_evseStartTransaction == true)
		{
			if (onStartTransaction != NULL)
			{
#if CP_ACTIVE
				if ((EVSE_state == STATE_C || EVSE_state == STATE_D) && getChargePointStatusService()->getEmergencyRelayClose() == false)
				{
					onStartTransaction();
				}
				else
				{
					Serial.println("Connect the Connector to EV / Or fault exist"); // here have to add timeout of 30 sec
					connectorDis_counter++;
					// EVSE_stopTransactionByRfid();
					if (connectorDis_counter > 25)
					{
						connectorDis_counter = 0;

						EVSE_StopSession();
					}
				}
#endif

#if !CP_ACTIVE
				// Serial.println(F("****I was called****"));
				onStartTransaction();
#endif
			}
		}
		else if (flag_evRequestsCharge == true)
		{

#if CP_ACTIVE
			// flag_evRequestsCharge = false;
			if (getChargePointStatusService() != NULL && getChargePointStatusService()->getEvDrawsEnergy() == false)
			{

				/***********************Control Pilot @Wamique******************/
				if (EVSE_state == STATE_C || EVSE_state == STATE_D)
				{
					if (getChargePointStatusService()->getEmergencyRelayClose() == false)
					{
						EVSE_StartCharging();
					}
					else if (getChargePointStatusService()->getEmergencyRelayClose() == true)
					{
						Serial.println("The voltage / current / Temp is out or range. FAULTY CONDITION DETECTED.");
					}
				}
				else if (EVSE_state == STATE_SUS)
				{
					EVSE_Suspended();
					Serial.println(counter1);
					if (counter1++ > 25)
					{ // Have to implement proper timeout
						counter1 = 0;
						EVSE_StopSession();
					}
				}
				else if (EVSE_state == STATE_DIS || EVSE_state == STATE_E || EVSE_state == STATE_B || EVSE_state == STATE_A)
				{

					//	EVSE_StopSession();     // for the very first time cable can be in disconnected state

					// if(txn == true){           // can implement counter > 10 just to remove noise
					EVSE_StopSession();
					//	}
				}
				else
				{

					Serial.println("[EVSE] STATE Error" + String(EVSE_state));
					delay(2000);

					//	requestLed(RED,START,1);
				}
			}
			if (getChargePointStatusService()->getEvDrawsEnergy() == true)
			{

				//	txn = true;

				if (EVSE_state == STATE_C || EVSE_state == STATE_D)
				{

					if (DEBUG_OUT)
						Serial.println("[EVSE_CP] Drawing Energy");

					if (millis() - t > 10000)
					{
						if (getChargePointStatusService()->getEmergencyRelayClose() == false)
						{
							requestLed(BLINKYGREEN_EINS, START, 1);
							t = millis();
						}
					}
					/*
					if(blinckCounter++ % 2 == 0){
						requestLed(GREEN,START,1);
					}else{
						requestLed(GREEN,STOP,1);
					}*/
				}
				else if (EVSE_state == STATE_A || EVSE_state == STATE_E || EVSE_state == STATE_B)
				{ // Although CP Inp will never go to A,B state
					if (counter_faultstate++ > 5)
					{
						EVSE_StopSession();
						counter_faultstate = 0;
					}
				}
				else if (EVSE_state == STATE_SUS)
				{
					EVSE_Suspended(); // pause transaction :update suspended state is considered in charging state
				}
				else if (EVSE_state == STATE_DIS)
				{

					Serial.println("[EVSE] Connect the Connector with EV and Try again");
					EVSE_StopSession();
				}
			}

			/***Implemented Exit Feature with RFID @Wamique****/
			EVSE_stopTransactionByRfid();
#endif

#if !CP_ACTIVE
			if (getChargePointStatusService() != NULL && getChargePointStatusService()->getEvDrawsEnergy() == false)
			{
				if (getChargePointStatusService()->getEmergencyRelayClose() == false)
				{
					getChargePointStatusService()->startEvDrawsEnergy();

					if (DEBUG_OUT)
						Serial.print(F("[EVSE] Opening Relays.\n"));
					reasonForStop = 3; // Local
					requestForRelay(START, 1);
					//added by sai
#if lednotuse
					if (ARAI_BOARD)
						requestLed(ORANGE, START, 1);
					delay(1200);
					if (ARAI_BOARD)
						requestLed(WHITE, START, 1);
					delay(1200);
					requestLed(GREEN, START, 1);
					delay(1000);
#endif

					if (DEBUG_OUT)
						Serial.println("[EVSE] Started Drawing Energy");
				}
				else if (getChargePointStatusService()->getEmergencyRelayClose() == true)
				{
					Serial.println("The voltage or current is out or range. FAULTY CONDITION DETECTED.");
					
				}
			}
            //added by sai
			if (getChargePointStatusService()->getEmergencyRelayClose() == true)
				{
					Serial.println(" FAULTY CONDITION DETECTED.");
					Serial.println(F("****EVSE_StopSession*****"));
					
								EVSE_StopSession();
				}

			if (getChargePointStatusService()->getEvDrawsEnergy() == true)
			{
				// delay(250);
				disp_evse_A = true;
				if (DEBUG_OUT)
					Serial.println("[EVSE] Drawing Energy");

				// blinking green Led
				if (millis() - t > 5000)
				{
					// if((WiFi.status() == WL_CONNECTED) && (webSocketConncted == true) && (isInternetConnected == true)&& getChargePointStatusService()->getEmergencyRelayClose() == false){
					// 	requestLed(BLINKYGREEN_EINS,START,1);
					// 	t = millis();
					// }

					if (getChargePointStatusService()->getEmergencyRelayClose() == false)
					{
						requestLed(BLINKYGREEN_EINS, START, 1);
						t = millis();
					}

					if (millis() - relay_timer > 15000)
					{
						if (getChargePointStatusService()->getEmergencyRelayClose() == false)
						requestForRelay(START, 1);
						relay_timer = millis();
					}
				}
				// Current check
				drawing_current = eic.GetLineCurrentA();
				if (drawing_current <= minCurr)
				{
					counter_drawingCurrent++;
					Serial.printf("[EVSE_A] low current count = %d / %d\n",counter_drawingCurrent, currentCounterThreshold);
					if (counter_drawingCurrent > currentCounterThreshold) // 150 earlier
					{
						counter_drawingCurrent = 0;
						// Check for the case where reasonForStop is not Local , Other
						if (reasonForStop != 3 || reasonForStop != 4)
						// if (reasonForStop < 5 && reasonForStop >= 3 )
						{
							if (reasonForStop != 5)
							{
								Serial.printf("reason for stop %d", reasonForStop);
								reasonForStop = 1; // EV disconnected
								Serial.println(F("Stopping Session Becoz of no Current"));
								/*
								 * @brief : According to ARAI standard for power loss
								 * If reasonForStop == 5 do not stop a session in case of power loss. Restart it once the power is back.
								 */
								Serial.println(F("****EVSE_StopSession_2*****"));
								EVSE_StopSession();
							}
						}
					}
				}
				else
				{
					counter_drawingCurrent = 0;
					// currentCounterThreshold = 650; // ARAI standard is 2.
					// currentCounterThreshold = 120;//commented by sai not required
					Serial.println("counter_drawingCurrent Reset");
				}
			}
			// Implemented Exit Feature with RFID @Wamique//
			// Bug fixed by G. Raja Sumant.
			EVSE_stopTransactionByRfid();
#endif
			// this is the only 'else if' block which is calling next else if block. the control is from this file itself. the control is not changed from any other file. but the variables are required to be present as extern in other file to decide calling of other functions.
			return;
		}
		else if (flag_evseStopTransaction == true)
		{
			if (getChargePointStatusService() != NULL)
			{
				getChargePointStatusService()->stopEvDrawsEnergy();
			}
			if (onStopTransaction != NULL)
			{
				onStopTransaction();
#if CP_ACTIVE
				requestforCP_OUT(STOP); // stop pwm
#endif
			}
			return;
		}
		else if (flag_evseUnauthorise == true)
		{
			if (onUnauthorizeUser != NULL)
			{
				onUnauthorizeUser();
			}
			return;
		}
		else if (flag_rebootRequired == true)
		{
			// soft reset execution.
			flag_evseIsBooted = false;
			flag_rebootRequired = false;
			flag_evseSoftReset = false;
			if (DEBUG_OUT)
				Serial.print(F("[EVSE] rebooting in 5 seconds...\n"));
			delay(5000);
			ESP.restart();
		}
		else
		{
			if (DEBUG_OUT)
				Serial.print(F("[EVSE] waiting for response...\n"));
			// Added by G. Raja Sumant 21/05/2022 for checking rfid stop during power off state.
			EVSE_stopTransactionByRfid();
			delay(500);
		}
	}
#endif

	/*********************************************Switch for evse dev status****************************************/

	switch (EvseDevStatus_connector_1)
	{
	case flag_EVSE_is_Booted:
	{

		switch (evse_boot_state)
		{
		case EVSE_BOOT_INITIATED:
		{
			Serial.println("EVSE boot notification ");
			OcppOperation *bootNotification = makeOcppOperation(&webSocket, new BootNotification());
			initiateOcppOperation(bootNotification);
			meteringService->init(meteringService);
			evse_boot_state = EVSE_BOOT_SENT;
			Serial.print("\r\nevse_boot_state  :EVSE_BOOT_INITIATED\r\n");

			if (ongoingTxn_m)
			{

				gu8_ongoing_stop = 1;
			}

			break;
		}

		case EVSE_BOOT_SENT:
		{
			if (gu8_state_change == 0)
			{
				gu8_state_change = 1;
				Serial.print("\r\nevse_boot_state  :EVSE_BOOT_SENT\r\n");
				if (gu8_online_flag == 0)
				{
					evse_boot_state = EVSE_BOOT_ACCEPTED;
				}
			}
			break;
		}

		case EVSE_BOOT_ACCEPTED:
		{
			Serial.print("EVSE_BOOT_ACCEPTED");

			if (gu8_online_flag == 1)
			{
				if (ongoingTxn_m != true)
				{
					// EVSE_transaction_status = EVSE_NO_TRANSACTION;
					if (gu8_ongoing_stop == 0)
					{
						Serial.print("Available.........1");
						evse_ChargePointStatus = Available;
					}
				}

				evse_boot_state = EVSE_BOOT_DEFAULT;
				flag_evseIsBooted = true;  // Exit condition for booting.
				flag_evseReadIdTag = true; // Entry condition for reading ID Tag.
				EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
				flag_evseAuthenticate = false;
				flag_evseStartTransaction = false;

				//  if ((evse_ChargePointStatus != Preparing))
				//  {
				// 	 evse_ChargePointStatus = Available;
				//  }

				if (ongoingTxn_m)
				{

					// EVSE_transaction_status = EVSE_START_TRANSACTION;

					if (ongoingtxpor_1 == 1)
					{

						evse_ChargePointStatus = Charging;
						EvseDevStatus_connector_1 = flag_EVSE_Request_Charge;
						getChargePointStatusService()->authorize(idTagData_m);
						getChargePointStatusService()->startTransaction(transactionId);

						if (DEBUG_OUT)
							Serial.println("****[EVSE_setOnBoot] on going transaction is true*****");
						flag_evseStopTransaction = false;
					}
					else if (ongoingtxpor_1 == 0)
					{

						Serial.println("Txn id is:$$$$$$$$$$$$$$$$$$$$$$$$$$$@@@@@@@@ " + String(transactionId));
						getChargePointStatusService()->authorize(idTagData_m);
						getChargePointStatusService()->startTransaction(transactionId);
						Serial.println("Txn id is:***********######## " + String(transactionId));
						gu8_powerloss_flag = 1;
						reasonForStop = 5;
						Serial.println("****EVSE_StopSession_1*****");

						EVSE_StopSession();
					}
				}
				else if (ongoingTxn_m == 0)
				{
					// Serial.println("Available**********7");
					//   evse_ChargePointStatus = Available;
					flag_evRequestsCharge = false;
					// resume_stop_start_txn_A = 0 ;
					if (DEBUG_OUT)
						Serial.println("****[EVSE_setOnBoot] on going transaction is false*****");
				}
				flag_evseUnauthorise = false;
				gu8_state_change = 0;
				Serial.print("\r\nevse_boot_state  :EVSE_BOOT_ACCEPTED\r\n");
			}
			else
			{
				if (flag_MeteringIsInitialised == false)
				{
					Serial.println("[SetOnBooT] Initializing metering services");
					meteringService->init(meteringService);
				}

				if (DEBUG_OUT)
					Serial.print("EVSE_setOnBoot Callback: Metering Services Initialization finished.\n");

				flag_evseIsBooted = true;  // Exit condition for booting.
				flag_evseReadIdTag = true; // Entry condition for reading ID Tag.
				EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
				evse_boot_state = EVSE_BOOT_DEFAULT;
				flag_evseAuthenticate = false;
				flag_evseStartTransaction = false;
				// added by sai
				//   Serial.println("Available**********2");
				//   evse_ChargePointStatus = Available;
				// flag_evRequestsCharge = false;
				if (ongoingTxn_m)
				{

#if 1
					// added by sai
					if (ongoingtxpor_1 == 1)
					{

						// added by sai
						Serial.println("Charging**********1");
						evse_ChargePointStatus = Charging;
						flag_evRequestsCharge = true;
						EvseDevStatus_connector_1 = flag_EVSE_Request_Charge;
						if (DEBUG_OUT)
							Serial.println("****[EVSE_setOnBoot] on going transaction is true*****");

						flag_evseStopTransaction = false;
					}
					else if (ongoingtxpor_1 == 0)
					{

						Serial.println("Txn id is:$$$$$$$$$$$$$$$$$$$$$$$$$$$@@@@@@@@ " + String(transactionId));
						getChargePointStatusService()->startTransaction(transactionId);
						Serial.println("Txn id is:***********######## " + String(transactionId));
						gu8_powerloss_flag = 1;
						reasonForStop = 5;

						// flag_evseStopTransaction = true;

						Serial.println("****EVSE_StopSession_1*****");

						EVSE_StopSession();
					}
#endif
				}
				else
				{
					//  evse_ChargePointStatus = Available;
					flag_evRequestsCharge = false;
					if (DEBUG_OUT)
						Serial.println("****[EVSE_setOnBoot] on going transaction is false*****");
				}
				// flag_evseStopTransaction = false;
				flag_evseUnauthorise = false;
			}
			break;
		}

		case EVSE_BOOT_REJECTED:

		{
			evse_boot_state = EVSE_BOOT_INITIATED;
			Serial.print("\r\nevse_boot_state  :EVSE_BOOT_REJECTED\r\n");
			break;
		}

		case EVSE_BOOT_DEFAULT:

		{
			break;
		}

		default:

			break;
		}

		break;
	}

	case flag_EVSE_Read_Id_Tag:
	{
		Serial.print(" flag_EVSE_Read_Id_Tag :");
		if (fota_available == true)
		{
			fota_available = false;

			// serial.println("fota triggered esp restarting");

			ESP.restart();
		}
		if (onReadUserId != NULL)
		{
			onReadUserId();
			if (millis() - timerHb > (heartbeatInterval * 1000))
			{
				timerHb = millis();
				onSendHeartbeat();
			}
		}
		break;
	}

	case flag_EVSE_Authentication:
	{
		Serial.print(" flag_EVSE_Authentication :");
		if (onAuthentication != NULL)
		{
			// Add condition by checking if available or unavailable
			bool un = false;
			un = getChargePointStatusService()->getUnavailable();
			currentIdTag = getChargePointStatusService()->getIdTag();
			// currentIdTag = EVSE_getCurrnetIdTag(&mfrc522);
			if (!un)
			{
				if (flag_evseReserveNow)
				{
					Serial.print(" reserve_currentIdTag :");
					Serial.println(reserve_currentIdTag);

					Serial.print(" currentIdTag :");
					Serial.println(currentIdTag);

					if (reserve_currentIdTag == currentIdTag)
					{
						if (DEBUG_OUT)
							Serial.print("EVSE_OnAuthentication Callback: ReserveIdTag is same with currentIdTag.\n");

						bool auth_checker = false;
						auth_checker = requestGetAuthCache();

						if (auth_checker)
						{
							getChargePointStatusService()->authorize(currentIdTag);
							flag_evseReadIdTag = false;
							flag_evseAuthenticate = false;
							flag_evseStartTransaction = true; // Entry condition for starting transaction.
							EvseDevStatus_connector_1 = flag_EVSE_Start_Transaction;
							evse_start_txn_state = EVSE_START_TXN_INITIATED;
							Serial.println("flag_EVSE_Start_Transaction4");
							flag_evRequestsCharge = false;
							flag_evseStopTransaction = false;
							flag_evseUnauthorise = false;

							if (DEBUG_OUT)
								Serial.print("EVSE_setOnAuthentication Callback: Authorize request has been accepted! Calling StartTransaction Block.\n");

							requestLed(BLINKYBLUE, START, 1);
							// Serial.print(F("B1\n"));
						}
						else
						{

							onAuthentication();
						}
					}
					else
					{

						if (DEBUG_OUT)
							Serial.print("EVSE_OnAuthentication_A Callback: ReserveIdTag_A is not same with currentIdTag_A .\n");
						return;
					}
				}
				else
				{

					if (requestGetAuthCache())

					{
						getChargePointStatusService()->authorize(currentIdTag);
						flag_evseReadIdTag = false;
						flag_evseAuthenticate = false;
						flag_evseStartTransaction = true; // Entry condition for starting transaction.
						EvseDevStatus_connector_1 = flag_EVSE_Start_Transaction;
						evse_start_txn_state = EVSE_START_TXN_INITIATED;
						Serial.println("flag_EVSE_Start_Transaction5");
						flag_evRequestsCharge = false;
						flag_evseStopTransaction = false;
						flag_evseUnauthorise = false;

						if (DEBUG_OUT)
							Serial.print("EVSE_setOnAuthentication Callback: Authorize request has been accepted! Calling StartTransaction Block.\n");
						// commented by sai
						//   Serial.print(F("test 3\n"));
						// requestLed(BLUE, START, 1);
					}
					else
					{
						// Serial.println("onAuthentication calling");
						// onAuthentication();
						switch (evse_authorize_state)
						{
						case EVSE_AUTHORIZE_INITIATED:
						{
							if (DEBUG_OUT)
								Serial.print("EVSE_setOnAuthentication Callback: Authenticating...\n");
							flag_evseAuthenticate = false;

							OcppOperation *authorize = makeOcppOperation(&webSocket, new Authorize(currentIdTag));
							initiateOcppOperation(authorize);
							evse_authorize_state = EVSE_AUTHORIZE_SENT;
							chargePointStatusService->authorize(currentIdTag);

							break;
						}

						case EVSE_AUTHORIZE_SENT:
						{

							Serial.print("\r\evse_authorize_state  :EVSE_AUTHORIZE_SENT\r\n");

							if (gu8_online_flag == 0)
							{
								evse_authorize_state = EVSE_AUTHORIZE_ACCEPTED;
							}

							break;
						}

						case EVSE_AUTHORIZE_ACCEPTED:
						{

							if (gu8_online_flag == 1)
							{
								// authorize->setOnReceiveConfListener([](JsonObject payload)
								// 									{
								//   const char* status = payload["idTagInfo"]["status"] | "Invalid";
								if (evse_authorize_state == EVSE_AUTHORIZE_ACCEPTED)
								{
									flag_evseReadIdTag = false;
									flag_evseAuthenticate = false;
									flag_evseStartTransaction = true; // Entry condition for starting transaction.
									EvseDevStatus_connector_1 = flag_EVSE_Start_Transaction;
									evse_start_txn_state = EVSE_START_TXN_INITIATED;
									flag_evRequestsCharge = false;
									flag_evseStopTransaction = false;
									flag_evseUnauthorise = false;

									if (DEBUG_OUT)
										Serial.print("EVSE_setOnAuthentication Callback: Authorize request has been accepted! Calling StartTransaction Block.\n");

									requestLed(BLUE, START, 1);

									requestSendAuthCache();
								}
								else
								{
#if ALPR_ENABLED
									// alprTxnStopped();
									alprunAuthorized();
#endif
									flag_evseReadIdTag = false;
									flag_evseAuthenticate = false;
									flag_evseStartTransaction = false;
									flag_evRequestsCharge = false;
									flag_evseStopTransaction = false;
									flag_evseUnauthorise = true; // wrong ID tag received, so clearing the global current ID tag variable and setting up to read again.
									EvseDevStatus_connector_1 = flag_EVSE_UnAutharized;
									if (DEBUG_OUT)
										Serial.print("EVSE_setOnAuthentication Callback: Authorize request has been denied! Read new User ID. \n");
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
								//   });
							}
							else if (gu8_online_flag == 0)
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
							}

							break;
						}

						case EVSE_AUTHORIZE_REJECTED:
						{

							evse_authorize_state = EVSE_AUTHORIZE_INITIATED;

							break;
						}

						case EVSE_AUTHORIZE_DEFAULT:
						{

							break;
						}

						default:
						{
							break;
						}
						}
					}
				}
			}
			else
			{
				Serial.print("EVSE_setOnAuthentication Callback: Authorize denied as un available! Calling StartTransaction Block.\n");
			}
		}

		break;
	}

	case flag_EVSE_Start_Transaction:
	{
		Serial.println("flag_EVSE_Start_Transaction ");

		switch (evse_start_txn_state)
		{
		case EVSE_START_TXN_INITIATED:
		{

			Serial.println("EVSE_START_TXN_INITIATED ");

			//  if (gu8_evse_change_state == EVSE_START_TXN)
			//  {
			flag_evseStartTransaction = false;
			resumeTxn.begin("resume", false); // opening preferences in R/W mode

			OcppOperation *startTransaction = makeOcppOperation(&webSocket, new StartTransaction());
			initiateOcppOperation(startTransaction);

			evse_start_txn_state = EVSE_START_TXN_SENT;
			Serial.println("EVSE_START_TXN_SENT ");
			//  }

			break;
		}

		case EVSE_START_TXN_SENT:
		{
			Serial.println("EVSE_START_TXN_SENTED ");
			if (gu8_online_flag == 0)
			{

				evse_start_txn_state = EVSE_START_TXN_ACCEPTED;
				evse_connection_status = EVSE_OFFLINE;
			}

			break;
		}

		case EVSE_START_TXN_ACCEPTED:
		{
			Serial.println("EVSE_START_TXN_ACCEPTED ");
			//  EvseDevStatus_connector_1 = flag_EVSE_Request_Charge;
			gu8_evse_change_state = EVSE_CHARGING;
			gu8_start_accepted = 0;
			offline_c = 1;

			if (gu8_online_flag == 1)
			{
				flag_evseReadIdTag = false;
				flag_evseAuthenticate = false;
				flag_evseStartTransaction = false;
				flag_evRequestsCharge = true;
				EvseDevStatus_connector_1 = flag_EVSE_Request_Charge;

				flag_evseStopTransaction = false;
				flag_evseUnauthorise = false;
				if (DEBUG_OUT)
					Serial.print("EVSE_setOnStartTransaction Callback: StartTransaction was successful\n");

				// added by sai
				evse_ChargePointStatus = Charging;
#if ALPR_ENABLED
				alprTxnStarted();
#endif

				if (currentIdTag.equals("") == true)
				{
					resumeTxn.begin("resume_A", false);
					resumeTxn.putString("idTagData_A", getChargePointStatusService()->getIdTag());
					resumeTxn.putString("idTagData", currentIdTag);
					resumeTxn.putBool("ongoingTxn", true);
					resumeTxn.putFloat("meterStart", globalmeterstartA);
					resumeTxn.end();
				}
				else
				{
					resumeTxn.begin("resume_A", false);
					resumeTxn.putString("idTagData_A", getChargePointStatusService()->getIdTag());
					resumeTxn.putString("idTagData", currentIdTag);
					resumeTxn.putBool("ongoingTxn", true);
					resumeTxn.putFloat("meterStart", globalmeterstartA);
					resumeTxn.end();
				}
			}
			else if (gu8_online_flag == 0)
			{
				flag_evseReadIdTag = false;
				flag_evseAuthenticate = false;
				flag_evseStartTransaction = false;
				flag_evRequestsCharge = true;
				EvseDevStatus_connector_1 = flag_EVSE_Request_Charge;

				flag_evseStopTransaction = false;
				flag_evseUnauthorise = false;
				if (DEBUG_OUT)
					Serial.print("EVSE_setOnStartTransaction Callback: StartTransaction was successful\n");
				// added by sai
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
					resumeTxn.begin("resume_A", false);
					resumeTxn.putString("idTagData_A", getChargePointStatusService()->getIdTag());
					resumeTxn.putString("idTagData", currentIdTag);
					resumeTxn.putBool("ongoingTxn", true);
					resumeTxn.putFloat("meterStart", globalmeterstartA);
					resumeTxn.end();
				}
				else
				{
					resumeTxn.begin("resume_A", false);
					resumeTxn.putString("idTagData_A", getChargePointStatusService()->getIdTag());
					resumeTxn.putString("idTagData", currentIdTag);
					resumeTxn.putBool("ongoingTxn", true);
					resumeTxn.putFloat("meterStart", globalmeterstartA);
					resumeTxn.end();
				}
				//***********************************//
			}

			break;
		}

		case EVSE_START_TXN_REJECTED:
		{

			flag_evseReadIdTag = false;
			flag_evseAuthenticate = false;
			flag_evseStartTransaction = false;
			flag_evRequestsCharge = false;
			flag_evseStopTransaction = false;
			flag_evseUnauthorise = true; // wrong ID tag received, so clearing the global current ID tag variable and setting up to read again.
			EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
			if (DEBUG_OUT)
				Serial.print("EVSE_setOnStartTransaction Callback: StartTransaction was unsuccessful\n");

#if ALPR_ENABLED
			alprTxnStopped();
#endif
#if CP_ACTIVE
			requestforCP_OUT(STOP); // stop pwm
#endif
			// resume namespace
			resumeTxn.putBool("ongoingTxn", false);
			resumeTxn.putString("idTagData", "");
			resumeTxn.end();
			break;
		}

		case EVSE_START_TXN_DEFAULT:
		{

			break;
		}

		default:
			break;
		}

		break;
	}

	case flag_EVSE_Request_Charge:

	{
		Serial.println("flag_EVSE_Request_Charge ");
#if !CP_ACTIVE
		if (getChargePointStatusService() != NULL && getChargePointStatusService()->getEvDrawsEnergy() == false)
		{
			if (getChargePointStatusService()->getEmergencyRelayClose() == false)
			{
				getChargePointStatusService()->startEvDrawsEnergy();

				if (DEBUG_OUT)
					Serial.print("[EVSE] Opening Relays.\n");
				reasonForStop = 3; // Local
				Serial.println("relay 2");
				requestForRelay(START, 1);

				if (DEBUG_OUT)
					Serial.println("[EVSE] Started Drawing Energy");
			}
			else if (getChargePointStatusService()->getEmergencyRelayClose() == true)
			{
				Serial.println("The voltage or current is out or range. FAULTY CONDITION DETECTED.");
			}
		}
		// added by sai
		if (getChargePointStatusService()->getEmergencyRelayClose() == true)
		{
			Serial.println(" FAULTY CONDITION DETECTED.");
			Serial.println("****EVSE_StopSession*****");

			EVSE_StopSession();
		}

		if (getChargePointStatusService()->getEvDrawsEnergy() == true)
		{
			// delay(250);
			disp_evse_A = true;
			if (DEBUG_OUT)
				Serial.println("[EVSE] Drawing Energy");

			// blinking green Led
			if (millis() - t > 5000)
			{
				// if((WiFi.status() == WL_CONNECTED) && (webSocketConncted == true) && (isInternetConnected == true)&& getChargePointStatusService()->getEmergencyRelayClose() == false){
				// 	requestLed(BLINKYGREEN_EINS,START,1);
				// 	t = millis();
				// }

				if (getChargePointStatusService()->getEmergencyRelayClose() == false)
				{
					requestLed(BLINKYGREEN_EINS, START, 1);
					t = millis();
				}

				if (millis() - relay_timer > 15000)
				{
					if (getChargePointStatusService()->getEmergencyRelayClose() == false)
						Serial.println("relay 3");
					requestForRelay(START, 1);
					relay_timer = millis();
				}
			}
			// Current check
			drawing_current = eic.GetLineCurrentA();
			if (drawing_current <= minCurr)
			{
				counter_drawingCurrent++;
				Serial.printf("[EVSE_A] low current count = %d / %d\n", counter_drawingCurrent, currentCounterThreshold);
				if (counter_drawingCurrent > currentCounterThreshold) // 150 earlier
				{
					counter_drawingCurrent = 0;
					// Check for the case where reasonForStop is not Local , Other
					if (reasonForStop != 3 || reasonForStop != 4)
					// if (reasonForStop < 5 && reasonForStop >= 3 )
					{
						if (reasonForStop != 5)
						{
							Serial.printf("reason for stop %d", reasonForStop);
							reasonForStop = 1; // EV disconnected
							Serial.println("Stopping Session Becoz of no Current");
							/*
							 * @brief : According to ARAI standard for power loss
							 * If reasonForStop == 5 do not stop a session in case of power loss. Restart it once the power is back.
							 */
							Serial.println(F("****EVSE_StopSession_2*****"));
							EVSE_StopSession();
						}
					}
				}
			}
			else
			{
				counter_drawingCurrent = 0;
				// currentCounterThreshold = 650; // ARAI standard is 2.
				// currentCounterThreshold = 120;//commented by sai not required
				Serial.println("counter_drawingCurrent Reset");
			}
		}
		// Implemented Exit Feature with RFID @Wamique//
		// Bug fixed by G. Raja Sumant.
		// EVSE_stopTransactionByRfid();
#endif

		break;
	}

	case flag_EVSE_Stop_Transaction:
	{
		Serial.println("flag_EVSE_Stop_Transaction ");

		switch (evse_stop_txn_state)
		{
		case EVSE_STOP_TXN_INITIATED:
		{

			flag_evseStopTransaction = false;
			flag_stop_finishing = true;
			// added by sai
			//  evse_ChargePointStatus = Finishing;

			int txnId = getChargePointStatusService()->getTransactionId();
			Serial.println("txnId : " + String(txnId));
			Serial.println("prevTxnId :" + String(prevTxnId));
			offline_c = 0;
			if (txnId != prevTxnId)
			{

				OcppOperation *stopTransaction = makeOcppOperation(&webSocket, new StopTransaction());
				initiateOcppOperation(stopTransaction);
				evse_stop_txn_state = EVSE_STOP_TXN_SENT;
				if (DEBUG_OUT)

					Serial.print("EVSE_setOnStopTransaction  before Callback: Closing Relays.\n");

				LastPresentEnergy_A = (discurrEnergy_A) * 1000 + LastPresentEnergy_A;
				Serial.println("discurrEnergy_A:" + String(discurrEnergy_A));
				Serial.println("LastPresentEnergy_A:" + String(LastPresentEnergy_A));

				/**********************Until Offline functionality is implemented***********/
				// Resume namespace(Preferences)
				resumeTxn.begin("resume", false);

				resumeTxn.putBool("ongoingTxn", false);
				if (webSocketConncted)
				{
					resumeTxn.putString("idTagData", "");
					resumeTxn.putInt("TxnIdData", -1);
					resumeTxn.putInt("reasonForStop", -1);
					resumeTxn.putInt("fault_code_A", -1);
					resumeTxn.putInt("offline_stop", 0);
				}

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
						if (onlineoffline_1 == 0) // added by sai
						{
							Serial.println("***********onlyonline************");
							if (gu8_stoponlineoffline_flag == 1 && (gu8_online_flag == 1))
							{
								// chargePointStatusService->unauthorize(); // can be buggy
								// chargePointStatusService->stopTransaction();
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
						// added by sai
						else if (onlineoffline_1 == 1)
						{

							Serial.println("***********onlineoffline************");
							if (gu8_stoponlineoffline_flag == 1 && (gu8_online_flag == 1))
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
						if (onlineoffline_1 == 0) // added by sai
						{
							Serial.println("***********onlineofflinestop************");

							// chargePointStatusService->unauthorize(); // can be buggy   // uncommented by shiva
							// chargePointStatusService->stopTransaction();    // uncommented by shiva
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
						// added by sai
						else if (onlineoffline_1 == 1)
						{

							Serial.println("***********offlinestop************");
							if (gu8_stoponlineoffline_flag == 1 && (gu8_online_flag == 1))
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
			}

			break;
		}

		case EVSE_STOP_TXN_SENT:
		{
			Serial.print("EVSE_STOP_TXN_SENT\n");
			if (gu8_online_flag == 0)
			{

				evse_stop_txn_state = EVSE_STOP_TXN_ACCEPTED;
			}

			break;
		}

		case EVSE_STOP_TXN_ACCEPTED:
		{
			Serial.print("EVSE_STOP_TXN_ACCEPTED\n");
			int txnId = getChargePointStatusService()->getTransactionId();
			Serial.println("txnId : " + String(txnId));
			Serial.println("prevTxnId :" + String(prevTxnId));

			if (txnId != prevTxnId)
			{
				if (gu8_online_flag == 1)
				{
					flag_evseReadIdTag = false;
					flag_evseAuthenticate = false;
					flag_evseStartTransaction = false;
					flag_evRequestsCharge = false;
					flag_evseStopTransaction = false;
					flag_evseUnauthorise = true;

					EvseDevStatus_connector_1 = flag_EVSE_UnAutharized;

					if (DEBUG_OUT)
						Serial.print("EVSE_setOnStopTransaction Callback: Closing Relays.\n");
					// added by sai
					//  evse_ChargePointStatus = Finishing;
					if (DEBUG_OUT)
						Serial.print("EVSE_setOnStopTransaction Callback: StopTransaction was successful\n");
					if (DEBUG_OUT)
						Serial.print("EVSE_setOnStopTransaction Callback: Reinitializing for new transaction. \n");
					// Serial.println("Available**********8");
					if (gu8_ongoing_stop == 0)
					{
						Serial.print("Available.........3");
						evse_ChargePointStatus = Available;
					}
				}
				else if (gu8_online_flag == 0)
				{
					flag_evseReadIdTag = false;
					flag_evseAuthenticate = false;
					flag_evseStartTransaction = false;
					flag_evRequestsCharge = false;
					flag_evseStopTransaction = false;
					flag_evseUnauthorise = true;
					EvseDevStatus_connector_1 = flag_EVSE_UnAutharized;
					if (DEBUG_OUT)
						Serial.print("EVSE_setOnStopTransaction Callback: Closing Relays.\n");
					// added by sai
					//  evse_ChargePointStatus = Finishing;
					if (DEBUG_OUT)
						Serial.print("EVSE_setOnStopTransaction Callback: StopTransaction was successful\n");
					if (DEBUG_OUT)
						Serial.print("EVSE_setOnStopTransaction Callback: Reinitializing for new transaction. \n");
				}
			}
			else
			{
				Serial.println("[EVSE] EVSE_setOnStopTransaction already called. Skipping make OCPP operation to avoid duplication");
			}

			break;
		}

		case EVSE_STOP_TXN_REJECTED:
		{

			break;
		}

		case EVSE_STOP_TXN_DEFAULT:

		{

			break;
		}

		default:
			break;
		}

		break;
	}

	case flag_EVSE_UnAutharized:
	{
		if (onUnauthorizeUser != NULL)
		{
			onUnauthorizeUser();
		}

		break;
	}

	case flag_EVSE_Reboot_Request:

	{

		break;
	}

	case flag_EVSE_Reserve_Now:
	{

		break;
	}

	case flag_EVSE_Cancle_Reservation:
	{

		break;
	}

	case flag_EVSE_Local_Authantication:
	{

		break;
	}

	default:
		break;
	}
}

void EVSE_Reservation_loop()
{
#if 0
	int i = 0;
	while (i < ReserveNowSlotList.size()){
		reserveNow_slot *el = ReserveNowSlotList.get(i);
		boolean success = el->isAccepted;	
		if (success){
			flag_evseReserveNow = true;	
			currentIdTag = el->idTag;
			connectorId = el->connectorId ;
			reservationId = el->reservationId;
			reservation_start_time = el->start_time;
			reservation_expiry_time  = el->expiry_time; 
			
		ReserveNowSlotList.remove(i);
		//TODO Review: are all recources freed here?
		delete el;
		//go on with the next element in the queue, which is now at ReserveNowSlotList[i]
		} else {
		//There will be another attempt to send this conf message in a future loop call.
		//Go on with the next element in the queue, which is now at ReserveNowSlotList[i+1]
		i++;
		}
	}
#endif

	if (flag_evseReserveNow)
	{
		// Reserve Now execution.
		if (DEBUG_OUT)
			Serial.print("[EVSE] Reserve Now ...\n");

		// time_t start_reserve_time_delta = reservation_start_time - now();
		time_t reserve_time_delta = reserveDate - now();

		if ((reserve_time_delta <= reservedDuration) && (reserve_time_delta > 0))
		{
			// added by sai
			if (wifi_connect)
			{
#if LCD_DISPLAY
				lcd.clear();
				lcd.setCursor(15, 0);
				lcd.print("Wi-Fi");
				lcd.setCursor(0, 1);
				lcd.print("CHARGER RESERVED");
#endif
			}
			else if (gsm_connect)
			{
#if LCD_DISPLAY
				lcd.clear();
				lcd.setCursor(15, 0);
				lcd.print("4G");
				lcd.setCursor(0, 1);
				lcd.print("CHARGER RESERVED");
#endif
			}

/*
 * @brief : Trigger a status notification of Reserved only once.
 */
#if 0
			//fire StatusNotification
		//TODO check for online condition: Only inform CS about status change if CP is online
		//TODO check for too short duration condition: Only inform CS about status change if it lasted for longer than MinimumStatusDuration
		OcppOperation *statusNotification = makeOcppOperation(webSocket,
		new StatusNotification(currentStatus));
		initiateOcppOperation(statusNotification);
#endif

			if (lu8_send_status_flag && reservation_start_flag)
			{
				lu8_send_status_flag = false;
				getChargePointStatusService()->setReserved(true);
				evse_ChargePointStatus = Reserved;
			}

			if (!reservation_start_flag)
			{

				if (getChargePointStatusService()->getTransactionId() != -1)
				{
					Serial.println("****EVSE_StopSession_3*****");
					EVSE_StopSession();
				}
				requestLed(BLUE, START, 1);
				reservation_start_flag = true;
				flag_evseReadIdTag = true; // Entry condition for reading ID Tag.
				EvseDevStatus_connector_1 = flag_EVSE_Read_Id_Tag;
				flag_evseAuthenticate = false;
				lu8_send_status_flag = true;
				if (DEBUG_OUT)
					Serial.print("[EVSE] Reserve Now  2 1 ...\n");
			}
			if (DEBUG_OUT)
				Serial.print("[EVSE] Reserve Now  2 ...\n");
			Serial.print("[EVSE] reserve time delta ");
			Serial.println(reserve_time_delta);
			if (getChargePointStatusService()->getEvDrawsEnergy() == false)
			{
				requestLed(BLUE, START, 1);
			}
		}
		else
		{
			if ((reserve_time_delta <= 0))
			{
				flag_evseReserveNow = false;
				getChargePointStatusService()->setReserved(false);
				/*
				* @bug : Session should not stop just because the reservation expired.
				Resolved by G. Raja Sumant 20/07/2022
				if (getChargePointStatusService()->getTransactionId() != -1)
				{
					EVSE_StopSession();
				}*/
				if (DEBUG_OUT)
					Serial.print("[EVSE] Reserve Now is stopped due to Reservation timeout.....!\n");
				reservation_start_flag = false;
			}
		}

		if (flag_evseCancelReservation)
		{
			flag_evseCancelReservation = false;
			flag_evseReserveNow = false;
			getChargePointStatusService()->setReserved(false);
			// if(getChargePointStatusService()->getTransactionId() != -1)
			// {
			// 	EVSE_StopSession();
			// }
			if (DEBUG_OUT)
				Serial.print("[EVSE] Reserve Now is stopped due to Cancel Reservation.....!\n");
			reservation_start_flag = false;
		}
	}
}

// void emergencyRelayClose_Loop(){
// 	float volt = eic.GetLineVoltageA();
// 	float current = eic.GetLineCurrentA();
// 	float temp= eic.GetTemperature();
// 	Serial.println("Voltage: "+String(volt)+", Current: "+String(current)+", Temperature: "+String(temp));
// 	if (getChargePointStatusService() != NULL) {
// 		if(getChargePointStatusService()->getOverVoltage() == true ||
// 			getChargePointStatusService()->getUnderVoltage() == true ||
// 			getChargePointStatusService()->getUnderTemperature() == true ||
// 			getChargePointStatusService()->getOverTemperature() == true ||
// 			getChargePointStatusService()->getOverCurrent() == true){
// 				Serial.println("Fault Occurred.");
// 				getChargePointStatusService()->setEmergencyRelayClose(true);
// 				if (!timer_initialize){
// 					timeout_start = millis();
// 					timer_initialize = true;
// 				}
// 			} else if(getChargePointStatusService()->getOverVoltage() == false &&
// 					getChargePointStatusService()->getUnderVoltage() == false &&
// 					getChargePointStatusService()->getUnderTemperature() == false &&
// 					getChargePointStatusService()->getOverTemperature() == false &&
// 					getChargePointStatusService()->getOverCurrent() == false){
// 				Serial.println("Not Faulty.");
// 				getChargePointStatusService()->setEmergencyRelayClose(false);
// 				//if (!timer_initialize){
// 					timeout_start = 0;
// 					timer_initialize = false;
// 				//}
// 			}

// 		if (getChargePointStatusService()->getEmergencyRelayClose() == true){
// 			timeout_active = true;
// 			requestForRelay(STOP,1);
// 			delay(500);
// 			#if LED_ENABLED
// 			requestLed(RED,START,1);
// 			#endif

// 			flag_faultOccured = true;
// 		} else if (getChargePointStatusService()->getEmergencyRelayClose() == false && flag_faultOccured == true){
// 			timeout_active = false;
// 			if ( (getChargePointStatusService()->getTransactionId() != -1)){ //can be buggy
// 				if(fault_counter++ > 2){
// 					fault_counter = 0;
// 					requestForRelay(START,1);
// 					delay(500);
// 					Serial.println("[EmergencyRelay] Starting Txn");
// 					flag_faultOccured = false;
// 				}
// 			}
// 		}

// 		if (timeout_active && getChargePointStatusService()->getTransactionId() != -1) {
// 			if (millis() - timeout_start >= TIMEOUT_EMERGENCY_RELAY_CLOSE){
// 				Serial.println("Emergency Stop.");
// 				flag_evRequestsCharge = false;
// 				flag_evseStopTransaction = true;
// 				timeout_active = false;
// 				timer_initialize = false;
// 			}
// 		}
// 	}
// }

short EMGCY_counter = 0;
bool EMGCY_FaultOccured = false;
extern int8_t fault_code_A;
ulong faultTimer = 0;
bool EMGCY_StopTxn = 0;

/*
 * @brief: emergencyRelayClose_Loop - This loop checks for GFCI as well.
 * Bug identified : Testing team - No gfci
 * Fix: G. Raja Sumant - Added GFCI.
 */

void emergencyRelayClose_Loop()
{
	uint8_t err = 0;
	// Added a new condition to check the toggling of relays in no earth state.
	// G. Raja Sumant - 06/05/2022

	if (getChargePointStatusService()->getOverCurrent() == true)
	{
		if (getChargePointStatusService()->getTransactionId() != -1)
		{
			Serial.println("****EVSE_StopSession_4*****");
			EVSE_StopSession();
		}
	}

	if (millis() - faultTimer > 1500)
	{ // It was 3000 earlier
		faultTimer = millis();
		bool EMGCY_status = requestEmgyStatus();
#if V_charge_lite1_4
		bool GFCI_status_1 = digitalRead(GFCI_PIN);
		flag_GFCI_set_here = true;
#else
		bool GFCI_status = requestGfciStatus();
#endif

#if V_charge_lite1_3
		bool GFCI_status_2 = requestGfciStatus();
#endif

		Serial.println("EMGCY_Status: " + String(EMGCY_status));
		// Serial.println("GFCI_Status: " + String(GFCI_status));
		// added
		Serial.println("GFCI_Status_1: " + String(GFCI_status_1));
		Serial.println("GFCI_Status_2: " + String(GFCI_status_2));
		if (EMGCY_status == true || GFCI_status_1 == true || GFCI_status_2 == true)
		{
			// if(EMGCY_counter++ > 0){
			if (EMGCY_counter == 0)
			{ // For immediate emergency response.
				disp_evse_A = false;
				remotestart_txn = 0;
				requestForRelay(STOP, 1);
				Serial.println("EMERGENCY****************");
				// fault_code_A = emergencyStop;
				// evse_ChargePointStatus = Faulted;

				Fault_occured = 1;
				getChargePointStatusService()->setEmergencyRelayClose(true);
				EMGCY_FaultOccured = true;
#if 0
				uint8_t err = 0;
				// fault_emgy[4] = 0X51; // In the first page.
				// err = DWIN_SET(fault_emgy,sizeof(fault_emgy)/sizeof(fault_emgy[0]));
				fault_emgy[4] = 0X66; // In the fourth page.
				err = DWIN_SET(fault_emgy, sizeof(fault_emgy) / sizeof(fault_emgy[0]));
				fault_emgy[4] = 0X71; // In the fourth page.
				err = DWIN_SET(fault_emgy, sizeof(fault_emgy) / sizeof(fault_emgy[0]));
				fault_emgy[4] = 0X7B; // In the fourth page.
				err = DWIN_SET(fault_emgy, sizeof(fault_emgy) / sizeof(fault_emgy[0]));
#endif
				if (EMGCY_status == true)
				{
					notFaulty_A = false;
					fault_code_A = 8;
					reasonForStop = 0; // Emergency
					requestLed(BLINKYRED_EINS, START, 1);
					evse_ChargePointStatus = Faulted;
					// added by sai
#if LCD_DISPLAY
					lcd.clear();
					lcd.setCursor(0, 1);
					lcd.print("CHARGER FAULTED");
					lcd.setCursor(0, 2);
					lcd.print("EMERGENCY");
#endif
#if 0
#if LCD_ENABLED
				//lcd.clear();
				lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
    			lcd.print("                    ");//Clear the line
				lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
				lcd.print("FAULTED: EMERGENCY");
#endif
#endif
#if DWIN_ENABLED
					client_reconnect_flag = 1;
					fault_emgy[4] = 0x66;
					err = DWIN_SET(fault_emgy, sizeof(fault_emgy) / sizeof(fault_emgy[0]));
					fault_emgy[4] = 0x55;
					err = DWIN_SET(fault_emgy, sizeof(fault_emgy) / sizeof(fault_emgy[0]));
					cloud_no_rfid_dwin_print();
#endif
				}
				else
				{
					notFaulty_A = false;
					fault_code_A = 6;
					reasonForStop = 4; // Other
					requestLed(RED, START, 1);
					Serial.println("fault red2");
					// added by sai
#if LCD_DISPLAY
					lcd.clear();
					lcd.setCursor(0, 1);
					lcd.print("CHARGER FAULTED");
					lcd.setCursor(0, 2);
					lcd.print("GFCI");
#endif
#if 0
#if LCD_ENABLED
				//lcd.clear();
				lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
    			lcd.print("                    ");//Clear the line
				lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
				lcd.print("FAULTED: GFCI");
#endif
#endif
#if DWIN_ENABLED
					client_reconnect_flag = 1;
					avail[4] = 0x55;
					err = DWIN_SET(avail, sizeof(clear_avail) / sizeof(clear_avail[0]));
					err = DWIN_SET(GFCI_55, sizeof(GFCI_55) / sizeof(GFCI_55[0]));
					avail[4] = 0x66;
					err = DWIN_SET(avail, sizeof(clear_avail) / sizeof(clear_avail[0]));
					err = DWIN_SET(GFCI_66, sizeof(GFCI_66) / sizeof(GFCI_66[0]));
					cloud_no_rfid_dwin_print();
#endif
				}
				EMGCY_counter = 0;
			}
		}
		else
		{

			EMGCY_FaultOccured = false;
			EMGCY_counter = 0;
			EMGCY_StopTxn = false;

			if (Fault_occured == 1)
			{
				Fault_occured = 0;
				if (EvseDevStatus_connector_1 == flag_EVSE_Request_Charge)
				{
					Serial.println("Charging**********4");
					evse_ChargePointStatus = Charging;
				}
				else
				{
					Serial.println("Available**********4");
					Serial.println("...................0");
					if (gu8_ongoing_stop == 0)
					{
						// Serial.println("gu8_ongoing_stop**********4");
						Serial.print("Available.........4");
						evse_ChargePointStatus = Available;
					}
					Serial.println("Available");
					if (wifi_connect)
					{
#if LCD_ENABLED
						lcd.clear();
						lcd.setCursor(15, 0);
						lcd.print("Wi-Fi");
						lcd.setCursor(0, 1);
						lcd.print("CHARGER AVAILABLE");
						lcd.setCursor(0, 2);
						lcd.print("TAP RFID/SCAN QR");
						lcd.setCursor(0, 3);
						lcd.print("TO START");
#endif
					}
					else if (gsm_connect)
					{
#if LCD_ENABLED

						lcd.clear();
						lcd.setCursor(15, 0);
						lcd.print("4G");
						lcd.setCursor(0, 1);
						lcd.print("CHARGER AVAILABLE");
						lcd.setCursor(0, 2);
						lcd.print("TAP RFID/SCAN QR");
						lcd.setCursor(0, 3);
						lcd.print("TO START");
#endif
					}
				}
			}

			// else if(flag_evRequestsCharge == true)
			// {
			// 	evse_ChargePointStatus = Charging ;
			// }

			getChargePointStatusService()->setEmergencyRelayClose(false);
			//  gu8_fault_flag = 0;
		}

		if (EMGCY_FaultOccured == true && getChargePointStatusService()->getTransactionId() != -1 && EMGCY_StopTxn == false)
		{

			flag_evseReadIdTag = false;
			flag_evseAuthenticate = false;
			flag_evseStartTransaction = false;
			flag_evRequestsCharge = false;
			flag_evseStopTransaction = true;
			EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;
			evse_stop_txn_state = EVSE_STOP_TXN_INITIATED;
			EMGCY_StopTxn = true;
			Serial.println("Calling StopTransaction Block Because of Emergency_Stop");
		}
		else if (EMGCY_FaultOccured == false)
		{

			float volt = eic.GetLineVoltageA();
			float current = eic.GetLineCurrentA();
			float temp = eic.GetTemperature();
			Serial.println("Voltage_A: " + String(volt) + ", Current_A: " + String(current) + ", Temperature: " + String(temp));
			if (getChargePointStatusService() != NULL)
			{
				if (getChargePointStatusService()->getOverVoltage() == true ||
					getChargePointStatusService()->getUnderVoltage() == true ||
					getChargePointStatusService()->getUnderTemperature() == true ||
					getChargePointStatusService()->getOverTemperature() == true ||
					getChargePointStatusService()->getOverCurrent() == true)
				{
					Serial.println("[EVSE] Fault Occurred.");
					notFaulty_A = false;
					// added by sai

					evse_ChargePointStatus = Faulted;

					Fault_occured = 1;

					getChargePointStatusService()->setEmergencyRelayClose(true);

					if (!timer_initialize)
					{
						timeout_start = millis();
						timer_initialize = true;
					}
				}
				else if (getChargePointStatusService()->getOverVoltage() == false &&
						 getChargePointStatusService()->getUnderVoltage() == false &&
						 getChargePointStatusService()->getUnderTemperature() == false &&
						 getChargePointStatusService()->getOverTemperature() == false &&
						 getChargePointStatusService()->getOverCurrent() == false)
				{
					Serial.println("[EVSE] Not Faulty.");
					notFaulty_A = true;
					client_reconnect_flag = 2;
					getChargePointStatusService()->setEmergencyRelayClose(false);
					//  gu8_fault_flag = 0;
					// added by sai
					if (Fault_occured == 1)
					{
						Fault_occured = 0;
						if (flag_evRequestsCharge == true)
						{
							Serial.println("Charging**********4");
							evse_ChargePointStatus = Charging;
						}
						else
						{
							Serial.println("Available**********9");
							Serial.println("...................9");

							if (gu8_ongoing_stop == 0)
							{

								Serial.print("Available.........5");
								evse_ChargePointStatus = Available;
							}
							if (wifi_connect)
							{
#if LCD_ENABLED
								lcd.clear();
								lcd.setCursor(15, 0);
								lcd.print("Wi-Fi");
								lcd.setCursor(0, 1);
								lcd.print("CHARGER AVAILABLE");
								lcd.setCursor(0, 2);
								lcd.print("TAP RFID/SCAN QR");
								lcd.setCursor(0, 3);
								lcd.print("TO START");
#endif
							}
							else if (gsm_connect)
							{
#if LCD_ENABLED

								lcd.clear();
								lcd.setCursor(15, 0);
								lcd.print("4G");
								lcd.setCursor(0, 1);
								lcd.print("CHARGER AVAILABLE");
								lcd.setCursor(0, 2);
								lcd.print("TAP RFID/SCAN QR");
								lcd.setCursor(0, 3);
								lcd.print("TO START");
#endif
							}
						}
					}

					// if (!timer_initialize){
					timeout_start = 0;
					timer_initialize = false;
					//}
				}

				if (getChargePointStatusService()->getEmergencyRelayClose() == true)
				{
					timeout_active = true;
					requestForRelay(STOP, 1);
					delay(50);
#if LED_ENABLED

					requestLed(RED, START, 1);
					Serial.println("fault red1");

#endif

					flag_faultOccured = true;
				}

// commented by sai for earth disconnect faults
#if 0
				else if (getChargePointStatusService()->getEmergencyRelayClose() == false && flag_faultOccured == true)
				{
					timeout_active = false;
					if ((getChargePointStatusService()->getTransactionId() != -1))
					{ // can be buggy
						if (fault_counter++ > 1)
						{
							fault_counter = 0;
							requestForRelay(START, 1);
							delay(50);
							Serial.println(F("[EmergencyRelay] Starting Txn"));
							flag_evRequestsCharge = true;
							getChargePointStatusService()->startEvDrawsEnergy();
							getChargePointStatusService()->setEmergencyRelayClose(false);
							flag_faultOccured = false;
						}
					}
				}

#endif

				if (timeout_active && getChargePointStatusService()->getTransactionId() != -1)
				{
					if (millis() - timeout_start >= TIMEOUT_EMERGENCY_RELAY_CLOSE)
					{
						uint8_t lu_timeoutcount = 0;
						lu_timeoutcount++;
						if (lu_timeoutcount >= 2)
						{
							Serial.println("[EVSE] Emergency Stop.");
							flag_evRequestsCharge = false;
							flag_evseStopTransaction = true;
							EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;
							evse_stop_txn_state = EVSE_STOP_TXN_INITIATED;
							timeout_active = false;
							timer_initialize = false;
							lu_timeoutcount = 0;
						}
					}
				}
			}
		}
	}
}

/*
 * @param limit: expects current in amps from 0.0 to 32.0
 */
void EVSE_setChargingLimit(float limit)
{
	if (DEBUG_OUT)
		Serial.print("[EVSE] New charging limit set. Got ");
	if (DEBUG_OUT)
		Serial.print(limit);
	if (DEBUG_OUT)
		Serial.print("\n");
	chargingLimit = limit;
}

bool EVSE_EvRequestsCharge()
{
	return flag_evRequestsCharge;
}

bool EVSE_EvIsPlugged()
{
	return evIsPlugged;
}

void EVSE_setOnBoot(OnBoot onBt)
{
	onBoot = onBt;
}

void EVSE_setOnReadUserId(OnReadUserId onReadUsrId)
{
	onReadUserId = onReadUsrId;
}

void EVSE_setOnsendHeartbeat(OnSendHeartbeat onSendHeartbt)
{
	onSendHeartbeat = onSendHeartbt;
}

void EVSE_setOnAuthentication(OnAuthentication onAuthenticatn)
{
	onAuthentication = onAuthenticatn;
}

void EVSE_setOnStartTransaction(OnStartTransaction onStartTransactn)
{
	onStartTransaction = onStartTransactn;
}

void EVSE_setOnStopTransaction(OnStopTransaction onStopTransactn)
{
	onStopTransaction = onStopTransactn;
}

void EVSE_setOnUnauthorizeUser(OnUnauthorizeUser onUnauthorizeUsr)
{
	onUnauthorizeUser = onUnauthorizeUsr;
}

void EVSE_getSsid(String &out)
{
	out += "Pied Piper";
}
void EVSE_getPass(String &out)
{
	out += "plmzaq123";
}

void EVSE_getChargePointSerialNumber(String &out)
{

	preferences.begin("credentials", false);
	out += preferences.getString("chargepoint", "");
	preferences.end();

	/*
	#if STEVE
	out += "dummyCP002";
	#endif

	#if EVSECEREBRO
	out += "testpodpulkit";
	#endif
	*/
}

#if 0
char *EVSE_getChargePointVendor()
{
	return CHARGE_POINT_VENDOR;
}

char *EVSE_getChargePointModel()
{
	return CHARGE_POINT_MODEL;
}
#endif

String EVSE_getCurrnetIdTag(MFRC522 *mfrc522)
{
	String currentIdTag = "";
	// currentIdTag = EVSE_readRFID(mfrc522);

	if (getChargePointStatusService()->getIdTag().isEmpty() == false)
	{
		if (DEBUG_OUT)
			Serial.println("[EVSE] Reading from Charge Point Station Service ID Tag stored.");
		currentIdTag = getChargePointStatusService()->getIdTag();
		// added by sai
		//  requestLed(BLINKYBLUE, START, 1);

		if (DEBUG_OUT)
			Serial.print("[EVSE] ID Tag: ");
		if (DEBUG_OUT)
			Serial.println(currentIdTag);
		Serial.flush();
	}

	return currentIdTag;
}

String EVSE_readRFID(MFRC522 *mfrc522)
{
	String currentIdTag;
	currentIdTag = readRfidTag(true, mfrc522);
	Serial.print("currentIdTag: ");
	Serial.println(currentIdTag);
	return currentIdTag;
}

/********Added new funtion @Wamique***********************/
/*
 * @brief: EVSE_stopTransactionByRfid
 * G. Raja Sumant
 * Fixed the bug of currentIdTag being empty.
 */

void EVSE_stopTransactionByRfid()
{

	Ext_currentIdTag = EVSE_readRFID(&mfrc522);
	Serial.print("Ext_currentIdTag: ");
	Serial.println(Ext_currentIdTag);
	if (currentIdTag.equals("") == true)
	{
		if (DEBUG_OUT)
			Serial.println("[EVSE] No ID tag");
	}
	else if (currentIdTag.equals(Ext_currentIdTag) == true)
	{
		Serial.print("Ext_currentIdTag  stop: ");
		flag_evRequestsCharge = false;
		flag_evseStopTransaction = true;
		EvseDevStatus_connector_1 = flag_EVSE_Stop_Transaction;
		evse_stop_txn_state = EVSE_STOP_TXN_INITIATED;
		reasonForStop = 3; // Local.
		gu8_stoponlineoffline_flag = 1;
	}
	else
	{
		if (Ext_currentIdTag.equals("") == false)
			if (DEBUG_OUT)
				Serial.println("\n[EVSE] Incorrect ID tag\n");
	}
}

/**************CP Implementation @mwh*************/
void EVSE_StartCharging()
{

	if (getChargePointStatusService()->getEvDrawsEnergy() == false)
	{
		getChargePointStatusService()->startEvDrawsEnergy();
	}
	if (DEBUG_OUT)
		Serial.print(F("[EVSE] Opening Relays.\n"));
	//   pinMode(32,OUTPUT);
	//  digitalWrite(32, HIGH); //RELAY_1
	// digitalWrite(RELAY_2, RELAY_HIGH);
	requestForRelay(START, 1);

// added by sai
#if lednotuse
	if (ARAI_BOARD)
		requestLed(ORANGE, START, 1);
	delay(1200);
	if (ARAI_BOARD)
		requestLed(WHITE, START, 1);
	delay(1200);
	requestLed(GREEN, START, 1);
	delay(1000);
#endif
	Serial.println("[EVSE] EV is connected and Started charging");
	if (DEBUG_OUT)
		Serial.println("[EVSE] Started Drawing Energy");
	delay(500);
}

void EVSE_Suspended()
{

	if (getChargePointStatusService()->getEvDrawsEnergy() == true)
	{
		getChargePointStatusService()->stopEvDrawsEnergy();
	}
	requestLed(BLUE, START, 1);
	requestForRelay(STOP, 1);
	//	delay(1000);
	Serial.printf("[EVSE] EV Suspended");
}

void displayMeterValues()
{
	if (millis() - timerDisplay > 10000)
	{
		timerDisplay = millis();
		float instantCurrrent_A = eic.GetLineCurrentA();
		int instantVoltage_A = eic.GetLineVoltageA();
		float instantPower_A = 0.0f;

		if (instantCurrrent_A < minCurr)
		{
			instantPower_A = 0;
		}
		else
		{
			instantPower_A = (instantCurrrent_A * instantVoltage_A) / 1000.0;
		}

		/*float instantCurrrent_B = eic.GetLineCurrentB();
		int instantVoltage_B  = eic.GetLineVoltageB();
		float instantPower_B = 0.0f;

		if(instantCurrrent_B < 0.15){
			instantPower_B = 0;
		}else{
			instantPower_B = (instantCurrrent_B * instantVoltage_B)/1000.0;
		}

		float instantCurrrent_C = eic.GetLineCurrentC();
		int instantVoltage_C = eic.GetLineVoltageC();
		float instantPower_C = 0.0f;

		if(instantCurrrent_C < 0.15){
			instantPower_C = 0;
		}else{
			instantPower_C = (instantCurrrent_C * instantVoltage_C)/1000.0;
		}*/
		delay(100); // just for safety
#if 0
#if LCD_ENABLED
		lcd.clear();
		lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
		if (notFaulty_A)
		{
			lcd.print("*****CHARGING 1*****"); // You can make spaces using well... spaces
		}
		else
		{

			switch (fault_code_A)
			{
			case -1:
				break; // It means default.
			case 0:
				lcd.print("Connector1-Over Voltage");
				break;
			case 1:
				lcd.print("Connector1-Under Voltage");
				break;
			case 2:
				lcd.print("Connector1-Over Current");
				break;
			case 3:
				lcd.print("Connector1-Under Current");
				break;
			case 4:
				lcd.print("Connector1-Over Temp");
				break;
			case 5:
				lcd.print("Connector1-Under Temp");
				break;
			case 6:
				lcd.print("Connector1-GFCI"); // Not implemented in AC001
				break;
			case 7:
				lcd.print("Connector1-Earth Disc");
				break;
			default:
				lcd.print("*****FAULTED 1*****"); // You can make spaces using well... spacesbreak;
			}
		}
		/*
		lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
		lcd.print("V:");
		lcd.setCursor(4, 1); // Or setting the cursor in the desired position.
		lcd.print(String(instantVoltage_A));
		lcd.setCursor(9, 1); // Or setting the cursor in the desired position.
		lcd.print(String(instantVoltage_B));
		lcd.setCursor(15, 1); // Or setting the cursor in the desired position.
		lcd.print(String(instantVoltage_C));
		lcd.setCursor(0, 2);
		lcd.print("I:");
		lcd.setCursor(4, 2); // Or setting the cursor in the desired position.
		lcd.print(String(instantCurrrent_A));
		lcd.setCursor(9, 2); // Or setting the cursor in the desired position.
		lcd.print(String(instantCurrrent_B));
		lcd.setCursor(15, 2); // Or setting the cursor in the desired position.
		lcd.print(String(instantCurrrent_C));
		 lcd.setCursor(0, 3);
		lcd.print("P:");
		lcd.setCursor(4, 3); // Or setting the cursor in the desired position.
		lcd.print(String(instantPower_A));
		lcd.setCursor(9, 3); // Or setting the cursor in the desired position.
		lcd.print(String(instantPower_B));
		lcd.setCursor(15, 3); // Or setting the cursor in the desired position.
		lcd.print(String(instantPower_C));*/

		lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
		lcd.print("VOLTAGE(v):");
		lcd.setCursor(12, 1); // Or setting the cursor in the desired position.
		lcd.print(String(instantVoltage_A));
		lcd.setCursor(0, 2);
		lcd.print("CURRENT(A):");
		lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
		lcd.print(String(instantCurrrent_A));
		lcd.setCursor(0, 3);
		lcd.print("POWER(KW) :");
		lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
		lcd.print(String(instantPower_A));
#endif
#endif

#if DWIN_ENABLED
		uint8_t err = 0;
		change_page[9] = 4;
		v1[4] = 0X6A;
		instantVoltage_A = instantVoltage_A * 10;
		v1[6] = instantVoltage_A >> 8;
		v1[7] = instantVoltage_A & 0xff;
		/*v2[6] = instantVoltage_B >> 8;
		v2[7] = instantVoltage_B & 0xff;
		v3[6] = instantVoltage_C >> 8;
		v3[7] = instantVoltage_C & 0xff;*/
		i1[4] = 0X77;
		i1[7] = instantCurrrent_A * 10;
		/*i2[7] = instantCurrrent_B*10;
		i3[7] = instantCurrrent_C*10;*/
		e1[4] = 0X6E;
		e1[7] = instantPower_A * 10;
		// e2[7] = instantPower_B*10;
		// e3[7] = instantPower_C*10;
		err = DWIN_SET(cid1, sizeof(cid1) / sizeof(cid1[0]));
		// delay(50);
		err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0])); // page 0
		// delay(50);
		if (notFaulty_A)
		{
			charging[4] = 0X66;
			err = DWIN_SET(charging, sizeof(charging) / sizeof(charging[0]));
		}
		// delay(50);
		err = DWIN_SET(v1, sizeof(v1) / sizeof(v1[0]));
		// err = DWIN_SET(v2,sizeof(v2)/sizeof(v2[0]));
		// err = DWIN_SET(v3,sizeof(v3)/sizeof(v3[0]));
		err = DWIN_SET(i1, sizeof(i1) / sizeof(i1[0]));
		// err = DWIN_SET(i2,sizeof(i2)/sizeof(i2[0]));
		// err = DWIN_SET(i3,sizeof(i3)/sizeof(i3[0]));
		err = DWIN_SET(e1, sizeof(e1) / sizeof(e1[0]));
		// err = DWIN_SET(e2,sizeof(e2)/sizeof(e2[0]));
		// err = DWIN_SET(e3,sizeof(e3)/sizeof(e3[0]));
		delay(500);

#endif
	}
}

/**************************************************/
// bool offline_charging;
/****************New Offline Functions********************************/
/*
void startOfflineTxn(){
 offline_charging = true;
 requestForRelay(START,1);
 requestLed(ORANGE,START,1);
 delay(1200);
 requestLed(WHITE,START,1);
 delay(1200);
 requestLed(GREEN,START,1);
 delay(1000);
 requestLed(BLINKYGREEN_EINS,START, 1);
 Serial.println("[EVSE] EV is connected and Started charging");
 if(DEBUG_OUT) Serial.println("[EVSE] Started Drawing Energy");

 while(offline_charging){

		drawing_current = eic.GetLineCurrentA();
		if(drawing_current <= minCurr){
		 counter_drawingCurrent++;
		 if(counter_drawingCurrent > 150){
		   counter_drawingCurrent = 0;
		   Serial.println("Stopping Session Becoz of no Current");

		   offline_charging = false;
		 }

		}else{
		 counter_drawingCurrent = 0;
		 Serial.println("counter_drawingCurrent Reset");

		}

		emergencyRelayClose_Loop();

		if(DEBUG_OUT) Serial.println("[EVSE] Started Drawing Energy");
		delay(2000);

		if(millis() - t > 5000){

		 if(getChargePointStatusService()->getEmergencyRelayClose() == true){
			 // requestLed(BLINKYGREEN_EINS,START,1);
			 // t = millis();
			 offline_charging = false;
		   }

		  if(millis() - relay_timer > 15000 && offline_charging == true){

		   requestForRelay(START,1);
		   relay_timer = millis();
		   requestLed(BLINKYGREEN_EINS,START,1);

		  }

		}
		EVSE_stopTransactionByRfid_Off();

 }
 Serial.println("Stopping Offline Charging");
 requestForRelay(STOP,1);

}

void EVSE_stopTransactionByRfid_Off(){

 String Ext_currentIdTag = EVSE_readRFID(&mfrc522);
 if(currentIdTag_Off.equals(Ext_currentIdTag) == true){
   offline_charging = false;
 }else{
	 if(Ext_currentIdTag.equals("")==false)
	 if(DEBUG_OUT) Serial.println("\n[EVSE] Incorrect ID tag\n");
   }
}
*/

#if 0
/****************New Offline Functions********************************/

#if 0

void EVSE_stopOfflineTxn()
{
  disp_evse_A = false;
  requestForRelay(STOP, 1);
  getChargePointStatusService()->stopEvDrawsEnergy();
  getChargePointStatusService()->unauthorize();
  if( getChargePointStatusService()->getEmergencyRelayClose() == false)
  {
    // requestLed(GREEN, START, 1);
	requestLed(BLINKYWHITE_EINS, START, 1);
	
  }
  
  // Display transaction finished
//   energymeter.begin("MeterData", false);
  float meterStop =  1 /* energymeter.getFloat("currEnergy_A", 0) */;
//   energymeter.end();
  unsigned long stop_time = millis();

  flag_txndone_off_A = true;
#if LCD_ENABLED
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TRANSACTION FINISHED");
  lcd.setCursor(0, 1);
  lcd.print("KWH       :");
  lcd.setCursor(12, 1); // Or setting the cursor in the desired position.
  lcd.print(String(float((meterStop - globalmeterstartA) / 1000)));
  lcd.setCursor(0, 2);
  lcd.print("CONNECTOR A");
  lcd.setCursor(0, 3);
  lcd.print("DURATION  :");
  lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
  unsigned long seconds = (stop_time - st_timeA) / 1000;
  int hr = seconds / 3600;                                                 // Number of seconds in an hour
  int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
  int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
  String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
  lcd.print(String(hrMinSec));
  delay(5000);
#endif
}
#endif

#if 0
void EVSE_startOfflineTxn()
{
  offline_charging = true;
  disp_evse_A = true;
  requestForRelay(START, 1);
  requestLed(ORANGE, START, 1);
  delay(1200);
  requestLed(WHITE, START, 1);
  delay(1200);
  requestLed(GREEN, START, 1);
  delay(1000);
  requestLed(BLINKYGREEN, START, 1);
  Serial.println("[EVSE] EV is connected and Started charging");
  if (DEBUG_OUT)
    Serial.println("[EVSE] Started Drawing Energy");
  st_timeA = millis();
  offline_t_A = millis();
  lastsampledTimeA_off = now();
  energymeter.begin("MeterData", false);
  globalmeterstartA = energymeter.getFloat("currEnergy_A", 0);
  energymeter.end();
}
#endif

void EVSE_A_offline_Loop()
{
  if (offline_charging_A)
  {

    drawing_current_A = eic.GetLineCurrentA();
    if (drawing_current_A <= minCurr)
    {
      counter_drawingCurrent_A++;
      if (counter_drawingCurrent_A > currentCounterThreshold_A)
      {
        counter_drawingCurrent_A = 0;
        Serial.println("Stopping Session Becoz of no Current");
        offline_charging = false;
        Serial.println("Stopping Offline Charging by low current");
        EVSE_A_stopOfflineTxn();
        disp_evse_A = false;
#if LCD_ENABLED
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("No Power Drawn /");
        lcd.setCursor(3, 1);
        lcd.print("EV disconnected");
#endif
      }
    }
    else
    {
      counter_drawingCurrent_A = 0;
      currentCounterThreshold_A = 60; // ARAI expects 2
      Serial.println("counter_drawingCurrent Reset");
    }

    emergencyRelayClose_Loop_A();

    if (DEBUG_OUT)
      Serial.println("[EVSE] Drawing Energy");

    if (millis() - t_A > 5000)
    {
      float instantCurrrent_A = eic.GetLineCurrentA();
      float instantVoltage_A = eic.GetLineVoltageA();

      if (getChargePointStatusService()->getEmergencyRelayClose() == true)
      {

        offline_charging_A = false;
        Serial.println("Stopping Offline Charging by emergency");
        EVSE_A_stopOfflineTxn();
        disp_evse_A = false;
      }

      if (millis() - relay_timer_A > 15000 && offline_charging_A == true)
      {

        requestForRelay(START, 1);
        relay_timer_A = millis();
        if( getChargePointStatusService()->getEmergencyRelayClose() == false)
        {
          
          requestLed(BLINKYGREEN, START, 1);
        }
        
      }

      time_t sampledTimeA_off = now();
      time_t delta = sampledTimeA_off - lastsampledTimeA_off;
      energymeter.begin("MeterData", false);
      float lastEnergy_A = energymeter.getFloat("currEnergy_A", 0);
      float finalEnergy_A = lastEnergy_A + ((float)(instantVoltage_A * instantCurrrent_A * ((float)delta))) / 3600; // Whr

      // placing energy value back in EEPROM
      offline_charging_Enargy_A = finalEnergy_A;
      energymeter.putFloat("currEnergy_A", finalEnergy_A);
      Serial.println("[EnergyCSampler] currEnergy_A: " + String(finalEnergy_A));
      energymeter.end();

      lastsampledTimeA_off = sampledTimeA_off;
    }
  }
}

void EVSE_LED_loop()
{

  // if not faulted and not charging then take the led status to green once every 8 seconds

  if (getChargePointStatusService()->getEmergencyRelayClose() == false && offline_charging_A == false)
  {
    if (millis() - timer_green_offline > 8000)
    {
      timer_green_offline = millis();
      requestLed(GREEN, START, 1);
#if LCD_ENABLED
					lcd.clear();
					lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
					lcd.print("STATUS: AVAILABLE");
					lcd.setCursor(0, 1);
					lcd.print("TAP RFID/SCAN QR");
					lcd.setCursor(0, 2);
					lcd.print("CONNECTION");
					lcd.setCursor(0, 3);
					lcd.print("CLOUD: OFFLINE");
#endif
    }
  }
}

#endif
