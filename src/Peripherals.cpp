// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\Peripherals.cpp"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#include "Peripherals.h"
#include "Variants.h"
#include "Master.h"
#include "CustomGsm.h"
extern bool webSocketConncted;
extern bool isInternetConnected;

extern bool wifi_connect;
extern bool gsm_connect;

bool wifi_reconnected_flag = false;

extern TinyGsmClient client;

short counter = 0;

short statusTmode;
short statusPrescale;
short statusTReloadH;
short statusTReloadL;
short statusTx;
// New function by Wamique
String readRfidTag(bool trigger, MFRC522 *mfrc522Input)
{

	String currentIdTag = "";
	if (trigger)
	{

		// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
		if (!mfrc522Input->PICC_IsNewCardPresent())
		{
			if (DEBUG_OUT)
				Serial.println("No RFID Card Present\n");
			delay(100); // if no new card is there, RFID will pause for 3sec
#if 1
			Serial.print(mfrc522Input->PCD_ReadRegister(MFRC522::TModeReg), DEC);
			Serial.print(":"); // 0x80 TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
			Serial.print(mfrc522Input->PCD_ReadRegister(MFRC522::TPrescalerReg), DEC);
			Serial.print(":"); // 0xA9 TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25μs.
			Serial.print(mfrc522Input->PCD_ReadRegister(MFRC522::TReloadRegH), DEC);
			Serial.print(":"); // 0x03 Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
			Serial.print(mfrc522Input->PCD_ReadRegister(MFRC522::TReloadRegL), DEC);
			Serial.print(":"); // 0xE8
			Serial.println(mfrc522Input->PCD_ReadRegister(MFRC522::TxASKReg), DEC);
			//   Serial.println(mfrc522Input->PCD_ReadRegister(MFRC522::TxControlReg),HEX);
			//  Serial.println(bytes,HEX);
#endif
			statusTmode = mfrc522Input->PCD_ReadRegister(MFRC522::TModeReg);		 // 0x80 TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
			statusPrescale = mfrc522Input->PCD_ReadRegister(MFRC522::TPrescalerReg); // 0xA9 TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25μs.
			statusTReloadH = mfrc522Input->PCD_ReadRegister(MFRC522::TReloadRegH);	 // 0x03 Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
			statusTReloadL = mfrc522Input->PCD_ReadRegister(MFRC522::TReloadRegL);	 // 0xE8
			statusTx = mfrc522Input->PCD_ReadRegister(MFRC522::TxASKReg);			 // 0x40

			if (statusTmode != 128 || statusPrescale != 169 || statusTReloadH != 3 || statusTReloadL != 232 || statusTx != 64)
			{
				Serial.println(F("Reinit RFID\n"));
				mfrc522Input->PCD_Init();
			}
			return "";
		}

		// Select one of the cards
		if (!mfrc522Input->PICC_ReadCardSerial())
		{
			if (DEBUG_OUT)
				Serial.println("Unable to read New Card\n");
			delay(500); // 1 sec delay for every failure
			counter++;
			if (counter > READ_RFID_FAIL)
			{
				mfrc522Input->PCD_Reset();
				if (DEBUG_OUT)
					Serial.println("Performed Soft Reset");
				counter = 0; // re-initializing counter on sucessful read.
			}
			return "";
		}

		// Show some details of the PICC (that is: the tag/card)

		// dump_byte_array(mfrc522Input->uid.uidByte, mfrc522Input->uid.size);
		for (byte i = 0; i < mfrc522Input->uid.size; i++)
		{
			currentIdTag.concat(String(mfrc522Input->uid.uidByte[i] < 0x10 ? "0" : ""));
			currentIdTag.concat(String(mfrc522Input->uid.uidByte[i], HEX));
		}
		Serial.println();
		Serial.print("Card UID:");
		Serial.println(currentIdTag);
		Serial.flush();
		Serial.print("PICC type: ");
		MFRC522::PICC_Type piccType = mfrc522Input->PICC_GetType(mfrc522Input->uid.sak);
		Serial.println(mfrc522Input->PICC_GetTypeName(piccType));

#if LED_ENABLED
		// requestLed(WHITE,START,1);
		// added by sai
		Serial.print("B2\n");
		requestLed(BLINKYBLUE, START, 1);

// delay(50);
// requestLed(WHITE,STOP,1);
#if 0
		if(wifi_connect == true){
			
			if((WiFi.status() == WL_CONNECTED) && (webSocketConncted == true) && (isInternetConnected == true)){
				requestLed(GREEN,START,1);
				wifi_reconnected_flag = true;
			}
		}else if(gsm_connect == true){
			if(client.connected() == true){
				requestLed(GREEN,START,1);
			}
		}
#endif
#endif

		delay(500);
		// MFRC522::StatusCode status;

		// Halt PICC
		mfrc522Input->PICC_HaltA();
		// Stop encryption on PCD
		mfrc522Input->PCD_StopCrypto1();

		// currentIdTag.toUpperCase();
	}
	return currentIdTag;
}
