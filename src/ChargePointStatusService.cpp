// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\ChargePointStatusService.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Variants.h"

#include "ChargePointStatusService.h"
#include "StatusNotification.h"
#include "OcppEngine.h"
#include "Master.h"
#include "EVSE.h"
#include "SimpleOcppOperationFactory.h"
#include <Preferences.h>
#include "LCD_I2C.h"

#include <string.h>

ChargePointStatus evse_ChargePointStatus;
//  evsetransactionstatus EVSE_transaction_status;

#if DWIN_ENABLED
#include "dwin.h"
extern unsigned char charging[28];
extern unsigned char change_page[10];
extern unsigned char avail[28];
extern unsigned char not_avail[28];
extern unsigned char fault_emgy[28];
extern unsigned char fault_noearth[28];
extern unsigned char fault_overVolt[28];
extern unsigned char fault_underVolt[28];
extern unsigned char fault_overTemp[28];
extern unsigned char fault_overCurr[28];
extern unsigned char fault_underCurr[28];
extern unsigned char fault_suspEV[28];
extern unsigned char fault_suspEVSE[28];
extern unsigned char GFCI[20];
#endif

extern bool webSocketConncted;
extern bool gsm_connect;

extern LCD_I2C lcd;
/*
 * @brief: Feature added by Raja
 * Resume txn.
 */

// uint8_t //reasonForStop = 0;

int transactionId = -1;

extern bool flag_GFCI_set_here;

extern bool ongoingTxn_m;

extern bool flag_start_accepted;

bool flag_stop_finishing = false;
// added by sai
extern uint8_t gu8_online_flag;
extern bool flag_evseIsBooted;
extern uint8_t Fault_occured;

// extern Preferences resumeTxn;

// extern Preferences preferences;
// extern Preferences resumeTxn;

Preferences resumeTxn2;

// ChargePointStatusService ChargePoint_StatusService;

/*
 * @brief: Feature added by Raja
 * This feature will avoid hardcoding of messages.
 */
// typedef enum resonofstop { EmergencyStop, EVDisconnected , HardReset, Local , Other , PowerLoss, Reboot,Remote, Softreset,UnlockCommand,DeAuthorized};

// static const char *resonofstop_str[] = { "EmergencyStop", "EVDisconnected" , "HardReset", "Local" , "Other" , "PowerLoss", "Reboot","Remote", "SoftReset","UnlockCommand","DeAuthorized"};

ChargePointStatusService::ChargePointStatusService(WebSocketsClient *webSocket)
	: webSocket(webSocket)
{
	setChargePointStatusService(this);
}

ChargePointStatus ChargePointStatusService::inferenceStatus()
{
#if 0
		/*
		 * @brief : For finsihing
		 */
		if (flag_stop_finishing)
		{
			return ChargePointStatus::Finishing;
		}

	if (reserved)
	{
		// reserved = false;
		return ChargePointStatus::Reserved;
	}

	if (ongoingTxn_m && !emergencyRelayClose)
	{
		return ChargePointStatus::Charging;
	}

		
	/*
	 * @brief : Feature added by G. Raja Sumant
	 * 09/07/2022 as part of change availability
	 */
	if (unavailable)
	{
		return ChargePointStatus::Unavailable;
	}

	if (!authorized)
	{
		// Preparing should be sent only when it has been accepted.
		
		if (emergencyRelayClose)
		{
			return ChargePointStatus::Faulted;
		}
		else
		{
			return ChargePointStatus::Available;
		}
	}
	else if (!transactionRunning)
	{
		// Preparing should be sent only when it has been accepted.
		if (flag_start_accepted)
		{
			return ChargePointStatus::Preparing;
		}
		else
		{
			return ChargePointStatus::Available;
		}
	}
	else
	{
		// Transaction is currently running
		if (emergencyRelayClose)
		{
			return ChargePointStatus::Faulted;
		}
		else
		{
			if (!evDrawsEnergy)
			{
				// return ChargePointStatus::SuspendedEV;
				return ChargePointStatus::Preparing;
			}
			if (!evseOffersEnergy)
			{
				return ChargePointStatus::SuspendedEVSE;
			}
			
			return ChargePointStatus::Charging;
		}
	}
#endif
	return evse_ChargePointStatus;
}

void ChargePointStatusService::loop()
{
#if 0
	ChargePointStatus inferencedStatus = inferenceStatus();

	if (inferencedStatus != currentStatus)
	{
		currentStatus = inferencedStatus;
#if DWIN_ENABLED

		uint8_t err = 0;
#endif
#if 0
		if (!authorized)
		{

			if (emergencyRelayClose)
			{

				// return ChargePointStatus::Faulted;
				// Capture the reason for stop.
				if (getChargePointStatusService()->getOverVoltage() == true)
				{
					// reasonForStop = Other;
				}
				else if (getChargePointStatusService()->getUnderVoltage() == true)
				{
					if (eic.GetLineVoltageA() < 170 && eic.GetLineVoltageA() > 50)
					{
						// EVSE_StopSession();//No earth
						getChargePointStatusService()->stopTransaction();
						// reasonForStop = Other;
					}
					else
					{

						// reasonForStop = Other;
					}
				}
				else if (getChargePointStatusService()->getUnderCurrent() == true)
				{
					// reasonForStop = EVDisconnected;
				}
				else if (getChargePointStatusService()->getOverCurrent() == true)
				{
					// reasonForStop = Other;
				}
				else if (getChargePointStatusService()->getUnderTemperature() == true)
				{
					// reasonForStop = Other;
				}
				else if (getChargePointStatusService()->getOverTemperature() == true)
				{
					// reasonForStop = Other;
				}
				else if (emergencyRelayClose)
				{

					bool EMGCY_status = requestEmgyStatus();
					bool GFCI_status = requestGfciStatus();

					if (EMGCY_status)
					{
						// reasonForStop = EmergencyStop;
					}
					else if (GFCI_status)
					{
						// reasonForStop = Other;
					}
				}
			}
			else
			{
#if DWIN_ENABLED
				change_page[9] = 0;
				// avail[4] = 0X51;
				// uint8_t err = DWIN_SET(avail,sizeof(avail)/sizeof(avail[0]));
				// delay(50);
				err = DWIN_SET(change_page, sizeof(change_page) / sizeof(change_page[0]));
				delay(50);
#endif

				// return ChargePointStatus::Available;
			}
		}
		else if (!transactionRunning)
		{
			// reasonForStop = Local;
			/*bool GFCI_status  = requestGfciStatus();
			if(GFCI_status)
			{
				if(DEBUG_OUT) Serial.println(F("**** reason for stop = GFCI 1******"));
				//reasonForStop = Other;
			}*/

			if (flag_GFCI_set_here)
			{
				if (DEBUG_OUT)
					Serial.println(F("**** reason for stop = GFCI 1******"));
				// reasonForStop = Other;
			}

			// return ChargePointStatus::Preparing;
		}
		else
		{

			// lcd.setCursor(0, 0);
			// Transaction is currently running
			if (emergencyRelayClose)
			{

				// reasonForStop = Local;

				bool EMGCY_status = requestEmgyStatus();
				// bool GFCI_status  = requestGfciStatus();

				if (EMGCY_status)
				{
					// reasonForStop = EmergencyStop;
				}
				/*if(GFCI_status)
				{
					if(DEBUG_OUT) Serial.println(F("**** reason for stop = GFCI 2******"));
					//reasonForStop = Other;
				}*/
				if (flag_GFCI_set_here)
				{
					if (DEBUG_OUT)
						Serial.println(F("**** reason for stop = GFCI 2******"));
					// reasonForStop = Other;
				}

				if (getChargePointStatusService()->getOverVoltage() == true)
				{
					// Added a new condition to check the toggling of relays in no earth state.
					// G. Raja Sumant - 06/05/2022
					getChargePointStatusService()->stopEvDrawsEnergy();
					// reasonForStop = Other;
				}
				else if (getChargePointStatusService()->getUnderVoltage() == true)
				{
					if (eic.GetLineVoltageA() < 170 && eic.GetLineVoltageA() > 50)
					{
						// getChargePointStatusService()->stopTransaction();//No earth
						//  Added a new condition to check the toggling of relays in no earth state.
						// G. Raja Sumant - 06/05/2022
						getChargePointStatusService()->stopEvDrawsEnergy();
						// reasonForStop = Other;
					}
					else
					{
						// Added a new condition to check the toggling of relays in no earth state.
						// G. Raja Sumant - 06/05/2022
						getChargePointStatusService()->stopEvDrawsEnergy();
						// reasonForStop = Other;
					}
				}
				else if (getChargePointStatusService()->getUnderCurrent() == true)
				{
					// if(reasonForStop!= 3 || reasonForStop!= 4)
					// reasonForStop = EVDisconnected;
				}
				else if (getChargePointStatusService()->getOverCurrent() == true)
				{
					// reasonForStop = Other;
				}
				else if (getChargePointStatusService()->getUnderTemperature() == true)
				{
					// Added a new condition to check the toggling of relays in no earth state.
					// G. Raja Sumant - 06/05/2022
					getChargePointStatusService()->stopEvDrawsEnergy();
					// reasonForStop = Other;
				}
				else if (getChargePointStatusService()->getOverTemperature() == true)
				{
					// Added a new condition to check the toggling of relays in no earth state.
					// G. Raja Sumant - 06/05/2022
					getChargePointStatusService()->stopEvDrawsEnergy();
					// reasonForStop = Other;
				}
			}
			else
			{

				if (!evDrawsEnergy)
				{
					// return ChargePointStatus::SuspendedEV;
					////reasonForStop = Local;
				}

				if (!evseOffersEnergy)
				{
					// return ChargePointStatus::SuspendedEVSE;
					////reasonForStop = Local;
				}
			}
		}
#endif
#if DISPLAY_ENABLED
		displayStatus((int)currentStatus);
#endif
		if (DEBUG_OUT)
			Serial.print(F("[ChargePointStatusService] Status changed\n"));

		// fire StatusNotification
		// TODO check for online condition: Only inform CS about status change if CP is online
		// TODO check for too short duration condition: Only inform CS about status change if it lasted for longer than MinimumStatusDuration
		OcppOperation *statusNotification = makeOcppOperation(webSocket,
															  new StatusNotification(currentStatus));
		initiateOcppOperation(statusNotification);
	}
#endif
	ChargePointStatus inferencedStatus = inferenceStatus();

	if (inferencedStatus != currentStatus)
	{
		currentStatus = inferencedStatus;
		Serial.print("CP LOOP:");
		switch (currentStatus)
		{
		case Available:
			Serial.println("Available");
#if 0
				if (gsm_connect)
				{

					lcd.clear();
					lcd.setCursor(18, 0);
					lcd.print("4G");
					lcd.setCursor(0, 1);
					lcd.print("CHARGER AVAILABLE");
					lcd.setCursor(0, 2);
					lcd.print("TAP RFID/SCAN QR");
					lcd.setCursor(0, 3);
					lcd.print("TO START");
				}
#endif
			break;
		case Preparing:
			Serial.print("Preparing");
			break;
		case Charging:
			Serial.print("Charging");
			break;
		case SuspendedEVSE:
			Serial.print("SuspendedEVSE");
			break;
		case SuspendedEV:
			Serial.print("SuspendedEV");
			break;
		case Finishing: // not supported by this client
			Serial.print("Finishing");
			break;
		case Reserved: // Implemented reserve now
			Serial.print("Reserved");
			break;
		case Unavailable: // Implemented Change Availability
			Serial.print("Unavailable");
			break;
		case Faulted: // Implemented Faulted.
			Serial.print("Faulted");
			break;
		default:
			// evse_ChargePointStatus = Available;
			// Serial.print("Available");
			break;
		}

		if (DEBUG_OUT)
			Serial.print("[ChargePointStatusService] Status changed\n");
#if 1
		// if(currentStatus != Unavailable)
		// {

		// if(EvseDevStatus_connector_1 != flag_evseIsBooted)
		// {

		// fire StatusNotification
		// TODO check for online condition: Only inform CS about status change if CP is online
		// TODO check for too short duration condition: Only inform CS about status change if it lasted for longer than MinimumStatusDuration
		if ((gu8_online_flag == 1) && (currentStatus != NOT_SET) && (flag_evseIsBooted != false))
		{
			OcppOperation *statusNotification = makeOcppOperation(webSocket,
																  new StatusNotification(currentStatus));
			initiateOcppOperation(statusNotification);
		}
		if ((gu8_online_flag == 1) && (currentStatus == NOT_SET) && (flag_evseIsBooted != false))
		{
			if (evse_ChargePointStatus == NOT_SET)
			{

				Serial.print("Fault_occured NOT set status");

				Fault_occured = 1;
			}
		}
// }
#endif
	}
}

void ChargePointStatusService::authorize(String &idTag, int connectorId)
{
	this->idTag = String(idTag);
	this->connectorId = connectorId;
	authorize();
}

void ChargePointStatusService::authorize(String &idTag)
{
	this->idTag = String(idTag);
	authorize();
}

void ChargePointStatusService::authorize()
{
	if (authorized == true)
	{
		if (DEBUG_OUT)
			Serial.print("[ChargePointStatusService] Warning: authorized twice or didn't unauthorize before\n");
		return;
	}
	authorized = true;
}

int &ChargePointStatusService::getConnectorId()
{
	return connectorId;
}

String &ChargePointStatusService::getIdTag()
{
	return idTag;
}

bool ChargePointStatusService::getOverVoltage()
{
	return overVoltage;
}

void ChargePointStatusService::setOverVoltage(bool ov)
{
	this->overVoltage = ov;
}

bool ChargePointStatusService::getUnderVoltage()
{
	return underVoltage;
}

void ChargePointStatusService::setUnderVoltage(bool uv)
{
	this->underVoltage = uv;
}

bool ChargePointStatusService::getOverTemperature()
{
	return overTemperature;
}

void ChargePointStatusService::setOverTemperature(bool ot)
{
	this->overTemperature = ot;
}

bool ChargePointStatusService::getUnderTemperature()
{
	return underTemperature;
}

void ChargePointStatusService::setUnderTemperature(bool ut)
{
	this->underTemperature = ut;
}

bool ChargePointStatusService::getOverCurrent()
{
	return overCurrent;
}

void ChargePointStatusService::setOverCurrent(bool oc)
{
	this->overCurrent = oc;
}

bool ChargePointStatusService::getUnderCurrent()
{
	return underCurrent;
}

void ChargePointStatusService::setUnderCurrent(bool uc)
{
	this->underCurrent = uc;
}

bool ChargePointStatusService::getEmergencyRelayClose()
{
	return emergencyRelayClose;
}
void ChargePointStatusService::setEmergencyRelayClose(bool erc)
{
	this->emergencyRelayClose = erc;
}

void ChargePointStatusService::unauthorize()
{
	if (authorized == false)
	{
		if (DEBUG_OUT)
			Serial.print("[ChargePointStatusService] Warning: unauthorized twice or didn't authorize before\n");
		return;
	}
	if (DEBUG_OUT)
		Serial.print("[ChargePointStatusService] idTag value will be cleared.\n");
	this->idTag.clear();
	authorized = false;
}

void ChargePointStatusService::startTransaction(int transId)
{
	if (transactionRunning == true)
	{
		if (DEBUG_OUT)
			Serial.print("[ChargePointStatusService] Warning: started transaction twice or didn't stop transaction before\n");
	}
	transactionId = transId;
	/*
	* @brief: Feature added by G. Raja Sumant 14/05/2022
			  Session should resume with this txn id itself.
			  Hence first store it.
	*/
	resumeTxn2.begin("resume", false); // opening preferences in R/W mode
	resumeTxn2.putInt("TxnIdData", transId);
	resumeTxn2.end();
	transactionRunning = true;
}

void ChargePointStatusService::boot()
{
	// TODO Review: Is it necessary to check in inferenceStatus(), if the CP is booted at all? Problably not ...
}

void ChargePointStatusService::stopTransaction()
{
	if (transactionRunning == false)
	{
		if (DEBUG_OUT)
			Serial.print("[ChargePointStatusService] Warning: stopped transaction twice or didn't start transaction before\n");
	}
	transactionRunning = false;

	resumeTxn2.begin("resume", false); // opening preferences in R/W mode
	resumeTxn2.putInt("TxnIdData", -1);
	resumeTxn2.end();
	transactionId = -1;
	// EEPROM.begin(sizeof(EEPROM_Data));
	// EEPROM.put(68,transactionId);
	// EEPROM.commit();
	// EEPROM.end();
}

int ChargePointStatusService::getTransactionId()
{
	if (transactionId < 0)
	{
		if (DEBUG_OUT)
			Serial.print("[ChargePointStatusService] Warning: getTransactionId() returns invalid transactionId. Have you called it after stopTransaction()? (can only be called before) Have you called it before startTransaction?\n");
	}
	if (transactionRunning == false)
	{
		if (DEBUG_OUT)
			Serial.print("[ChargePointStatusService] Warning: getTransactionId() called, but there is no transaction running. Have you called it after stopTransaction()? (can only be called before) Have you called it before startTransaction?\n");
	}
	return transactionId;
}

void ChargePointStatusService::startEvDrawsEnergy()
{
	if (evDrawsEnergy == true)
	{
		if (DEBUG_OUT)
			Serial.print("[ChargePointStatusService] Warning: startEvDrawsEnergy called twice or didn't call stopEvDrawsEnergy before\n");
	}
	evDrawsEnergy = true;
}

void ChargePointStatusService::stopEvDrawsEnergy()
{
	if (evDrawsEnergy == false)
	{
		if (DEBUG_OUT)
			Serial.print("[ChargePointStatusService] Warning: stopEvDrawsEnergy called twice or didn't call startEvDrawsEnergy before\n");
	}
	evDrawsEnergy = false;
	// added by sai
	//  evse_ChargePointStatus = Preparing;
}
void ChargePointStatusService::startEnergyOffer()
{
	if (evseOffersEnergy == true)
	{
		if (DEBUG_OUT)
			Serial.print("[ChargePointStatusService] Warning: startEnergyOffer called twice or didn't call stopEnergyOffer before\n");
	}
	evseOffersEnergy = true;
}

void ChargePointStatusService::stopEnergyOffer()
{
	if (evseOffersEnergy == false)
	{
		if (DEBUG_OUT)
			Serial.print("[ChargePointStatusService] Warning: stopEnergyOffer called twice or didn't call startEnergyOffer before\n");
	}
	evseOffersEnergy = false;
}

/*****Added new Definition @wamique***********/
bool ChargePointStatusService::getEvDrawsEnergy()
{
	return evDrawsEnergy;
}

/*********************************************/

/*
 * @brief : Feature added by G. Raja Sumant
 * 09/07/2022 For Change availability
 */
void ChargePointStatusService::setUnavailable(bool su)
{
	this->unavailable = su;
}

bool ChargePointStatusService::getUnavailable()
{
	return unavailable;
}

/*
 * @brief : Feature added by G. Raja Sumant
 * 19/07/2022 For ReserveNow
 */
void ChargePointStatusService::setReserved(bool re)
{
	this->reserved = re;
}

bool ChargePointStatusService::getReserved()
{
	return reserved;
}
