// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\ChargePointStatusService.h"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef CHARGEPOINTSTATUSSERVICE_H
#define CHARGEPOINTSTATUSSERVICE_H

#include "libraries/arduinoWebSockets-master/src/WebSocketsClient.h"

#if 0
enum class ChargePointStatus {
	Available,
	Preparing,
	Charging,
	SuspendedEVSE,
	SuspendedEV,
	Finishing,    //not supported by this client
	Reserved,     //Implemented reserve now
	Unavailable,  //Implemented Change Availability
	Faulted,      //Implemented Faulted.
	NOT_SET 	  //not part of OCPP 1.6
};
#endif

enum ChargePointStatus {
	Available,
	Preparing,
	Charging,
	SuspendedEVSE,
	SuspendedEV,
	Finishing,    //not supported by this client
	Reserved,     //Implemented reserve now
	Unavailable,  //Implemented Change Availability
	Faulted,      //Implemented Faulted.
	NOT_SET 	  //not part of OCPP 1.6
};

extern ChargePointStatus evse_ChargePointStatus;

#if 0
enum evsetransactionstatus
{
   EVSE_NO_TRANSACTION,
   EVSE_START_TRANSACTION,
   EVSE_STOP_TRANSACTION
};

 extern enum evsetransactionstatus EVSE_transaction_status;

#endif

class ChargePointStatusService {
private:
	bool authorized = false;
	String idTag = String('\0');
	int connectorId = 1;
	bool transactionRunning = false;
	//Making this public 
	//int transactionId = -1;
	bool evDrawsEnergy = false;
	bool evseOffersEnergy = false;
	bool overVoltage = false;
	bool underVoltage = false;
	bool EarthDisconnect = false;
	bool overTemperature = false;
	bool underTemperature = false;
	bool overCurrent = false;
	bool underCurrent = false;
	bool emergencyRelayClose = false;
	//Feature added by G. Raja Sumant for charge point status service.
	bool unavailable = false;
	//Feature added by G. Raja Sumant for reservation status service.
	bool reserved = false;
	ChargePointStatus currentStatus = ChargePointStatus::NOT_SET;
	WebSocketsClient *webSocket;
public:
	
	ChargePointStatusService(WebSocketsClient *webSocket);
	void authorize(String &idTag, int connectorId);
	void authorize(String &idTag);
	void authorize();
	void unauthorize();
	String &getIdTag();
	int &getConnectorId();
	bool getOverVoltage();
	void setOverVoltage(bool ov);
	bool getUnderVoltage();
	void setUnderVoltage(bool uv);
	//Feature added by G. Raja Sumant for Change availabilty
	void setUnavailable(bool su);
	bool getUnavailable();
	//Feature added by G. Raja Sumant for reservation
	void setReserved(bool);
	bool getReserved();
	bool getOverCurrent();
	void setOverCurrent(bool oc);
	bool getUnderCurrent();
	void setUnderCurrent(bool uc);
	bool getOverTemperature();
	void setOverTemperature(bool ot);
	bool getUnderTemperature();
	void setUnderTemperature(bool ut);
	bool getEmergencyRelayClose();
	void setEmergencyRelayClose(bool erc);
	void startTransaction(int transactionId);
	void stopTransaction();
	int getTransactionId();
	void boot();
	void startEvDrawsEnergy();
	void stopEvDrawsEnergy();
	void startEnergyOffer();
	void stopEnergyOffer();
	void loop();
	void setEarthDisconnect(bool ed);

	ChargePointStatus inferenceStatus();

	/***Adding new funtion @wamique**/
	bool getEvDrawsEnergy();
	/*******************************/
};

#endif
