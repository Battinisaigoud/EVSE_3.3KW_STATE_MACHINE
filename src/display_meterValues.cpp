// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\display_meterValues.cpp"
#include "display_meterValues.h"
#include "StatusNotification.h"
#include "Variants.h"
#include "ATM90E36.h"
#include <Preferences.h>
#include "LCD_I2C.h"
#include "MeteringService.h"
#include "OcppMessage.h"
#include "OcppEngine.h"
#include "Master.h"
#include "EVSE.h"
#include "SimpleOcppOperationFactory.h"
#include "ChargePointStatusService.h"
#include "CustomGsm.h"

extern LCD_I2C lcd;
extern TinyGsmClient client;

extern ATM90E36 eic;
extern bool EMGCY_FaultOccured;
extern int globalmeterstartA;
extern int globalmeterstartB;
extern int globalmeterstartC;
float online_charging_Enargy_A = 0;
float online_charging_Enargy_B = 0;
float online_charging_Enargy_C = 0;
extern Preferences energymeter;
extern bool EMGCY_FaultOccured;
// extern bool EMGCY_FaultOccured_B;
// extern bool EMGCY_FaultOccured_C;
bool offline_charging_A = false;
ChargePointStatus previousStatus_A;
// extern bool offline_charging_B;
// extern bool offline_charging_C;

float offline_charging_Enargy_A = 0;
// extern float offline_charging_Enargy_B;
// extern float offline_charging_Enargy_C;

unsigned long offline_t_A = 0;
// unsigned long offline_t_B = 0;
// unsigned long offline_t_C = 0;
extern uint8_t remotestart_txn;
time_t lastsampledTimeA_off = 0;
// time_t lastsampledTimeB_off = 0;
// time_t lastsampledTimeC_off = 0;

extern bool isInternetConnected;
extern bool wifi_connect;
extern bool gsm_connect;
extern bool ethernet_connect;

extern MeteringService *meteringService;

extern bool onlineoffline_1 ;

int client_reconnect_flag = 0;

extern uint8_t gu8_online_flag;

uint16_t gu8_fault_flag = 0;

// added by sai
extern ChargePointStatus evse_ChargePointStatus;
ChargePointStatus local_evse_ChargePointStatus;
extern evsetransactionstatus EVSE_transaction_status;
extern unsigned long st_timeA;
extern float current_energy;
extern bool webSocketConncted;
extern float discurrEnergy_A;
// uint8_t gu8_lcd_chargingflag = 0;

uint8_t gu8_rfidflag = 0;
extern uint8_t offline_c ;

#if LCD_DISPLAY
unsigned long onTime = 0;
uint8_t state_timer = 0;
uint8_t disp_evse = 0;
extern bool disp_evse_A;
// extern bool disp_evse_B;
// extern bool disp_evse_C;
extern bool notFaulty_A;
// extern bool notFaulty_B;
// extern bool notFaulty_C;
extern int8_t fault_code_A;
// extern int8_t fault_code_B;
// extern int8_t fault_code_C;
extern bool disp_evse_A_finished;
extern LCD_I2C lcd;
uint8_t led_C = 0;

uint8_t evse_state_timer = 0;
uint8_t cloud_evse_loop = 0;

void stateTimer()
{
  state_timer = 1;
  disp_evse = 1;
}

void evse_stateTimer()
{
  switch (evse_state_timer)
  {
  case 0:
    onTime = millis();
    evse_state_timer = 1;
    cloud_evse_loop = 1;
    break;
  case 1:
    if ((millis() - onTime) > 500)
    {
      evse_state_timer = 2;
    }
    break;
  case 2:
    onTime = millis();
    evse_state_timer = 3;
    cloud_evse_loop = 2;
    break;
  case 3:
    if ((millis() - onTime) > 2000)
    {
      evse_state_timer = 4;
    }
    break;
  case 4:
    evse_state_timer = 0;
    break;
    /*
  case 4:
    onTime = millis();
    state_timer = 5;
    disp_evse = 3;
    break;
  case 5:
    if ((millis() - onTime) > 3000)
    {
      state_timer = 6;
    }
    break;
  case 6:
    state_timer = 0;
    */
  }
}

#if 0


void disp_lcd_meter()
{
  float instantCurrrent_A = eic.GetLineCurrentA();
  float instantVoltage_A = eic.GetLineVoltageA();
  float instantPower_A = 0.0f;

  if (instantCurrrent_A < 0.15)
  {
    instantPower_A = 0;
  }
  else
  {
    instantPower_A = (instantCurrrent_A * instantVoltage_A) / 1000.0;
  }


  switch (disp_evse)
  {
  case 1:
    if (disp_evse_A)
    {
      lcd.clear();
      lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
      if (notFaulty_A )
      {
#if 0
        lcd.print("*****CHARGING A*****"); // You can make spaces using well... spaces
        lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
        lcd.print("VOLTAGE(v):");
        lcd.setCursor(12, 1); // Or setting the cursor in the desired position.
        lcd.print(String(instantVoltage_A));
        lcd.setCursor(0, 2);
        lcd.print("CURRENT(A):");
        lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
        lcd.print(String(instantCurrrent_A));
        // lcd.setCursor(0, 3);
        /*
        lcd.print("POWER(KW) :");
        lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
        lcd.print(String(instantPower_A));
        */

        lcd.setCursor(0, 3);
        lcd.print("KWH       :");
#endif
     
        
      
        //added by sai
        lcd.clear();
        if (gu8_online_flag == 1)
        {
          if (wifi_connect)
          {
            lcd.clear();
            lcd.setCursor(15, 0);
            lcd.print("WI-FI");
          }
          else if (gsm_connect)
          {
            lcd.clear();
            lcd.setCursor(15, 0);
            lcd.print("4G");
          }
        }
        else if (gu8_online_flag == 0)
        {
          lcd.setCursor(12, 0);
          lcd.print("OFFLINE");
        }

#if 0
        lcd.setCursor(0, 1);
        lcd.print("voltage(v):");
        lcd.setCursor(12, 1); // Or setting the cursor in the desired position.
        lcd.print(String(instantVoltage_A));
         lcd.setCursor(0, 2);
        lcd.print("CURRENT(A):");
        lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
        lcd.print(String(instantCurrrent_A));
        lcd.setCursor(0, 3);
        lcd.print("ENERGY(KWH):       ");
        lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
#endif
        // added by sai
         instantPower_A = (instantCurrrent_A * instantVoltage_A) / 1000.0;
        lcd.setCursor(0, 1);
        lcd.print("CHARGING @ kW:");
        lcd.setCursor(14, 1); // Or setting the cursor in the desired position.
        lcd.print(String(instantPower_A));
        lcd.setCursor(0, 2);
        lcd.print("ENERGY(kWh):       ");
        lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
        if (!offline_charging_A)
        {
          // float online_charging_Enargy_A = meteringService->currentEnergy_A();
          // lcd.print(String(float((online_charging_Enargy_A-globalmeterstartA)/1000)));
          // energymeter.begin("MeterData",false);
          //  float online_charging_Enargy_A = energymeter.getFloat("currEnergy_A",0);
          // energymeter.end();
          if ((online_charging_Enargy_A - globalmeterstartA) > 0)
          {
            Serial.println("discurrEnergy_A: "+String(discurrEnergy_A));
            // lcd.print(String(float((online_charging_Enargy_A - globalmeterstartA) / 1000)));
            
            // added by sai
            // energymeter.begin("MeterData", false); // opening nmespace with R/W access
            // float lastEnergy_A = energymeter.getFloat("currEnergy_A", 0);
            // energymeter.end();
            // lcd.print(String(lastEnergy_A));

            lcd.print(String(discurrEnergy_A));
          //  lcd.print(String(float((online_charging_Enargy_A - globalmeterstartA) / 1000)));
          }

          else
          {
            lcd.print("0");
          }

          if (gu8_online_flag == 1)
          {
            Serial.println(F("[DISP_METERVALUES]******* online charging***************"));
            Serial.println("online_charging_Enargy_A");
            Serial.println(online_charging_Enargy_A);
          }
          else
          {
            Serial.println(F("[DISP_METERVALUES]******* offline charging***************"));
            Serial.print("offline_charging_Enargy_A");
            Serial.println(online_charging_Enargy_A);
          }
        }
        else
        {
          lcd.print(String(float((offline_charging_Enargy_A - globalmeterstartA) / 1000)));
          // Serial.println(F("[DISP_METERVALUES]******* offline charging***************"));
          if (gu8_online_flag == 1)
          {
            Serial.println(F("[DISP_METERVALUES]******* online charging***************"));
          }
          else
          {
            Serial.println(F("[DISP_METERVALUES]******* offline charging***************"));
          }
        }
        // ADDED BY SAI FOR TIME DURATION
        unsigned long stop_time = millis();
        lcd.setCursor(0, 3);
        lcd.print("TIME:       ");
        lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
        unsigned long seconds = (stop_time - st_timeA) / 1000;
        int hr = seconds / 3600;                                                 // Number of seconds in an hour
        int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
        int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
        String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
        lcd.print(String(hrMinSec));
        // gu8_lcd_chargingflag = 0;
      
      }
      else
      {
#if 1
        if (webSocketConncted== 0)
        {
          lcd.clear();
          lcd.setCursor(13, 0);
          lcd.print("OFFLINE");
          if (notFaulty_A)
          {
            lcd.setCursor(0, 1);
            lcd.print("CHARGER UNAVAILABLE");
          }
        }
#endif
        switch (fault_code_A)
        {
        case -1:
          lcd.print("A: FAULTED");
          break; // It means default.
        case 0:
          // lcd.print("A: OVER VOLTAGE");

          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("CHARGER FAULTED ");
          lcd.setCursor(0, 2);
          lcd.print("OVER VOLTAGE");
          break;
        case 1:
          if (instantVoltage_A > 20)
          {
            // lcd.print("A: UNDER VOLTAGE");
            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("CHARGER FAULTED");
            lcd.setCursor(0, 2);
            lcd.print("UNDER VOLTAGE");
          }
          else
          {
            // lcd.print("A: NO POWER");
          }

          break;
        case 2:
          // lcd.print("A: OVER CURRENT");

          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("CHARGER FAULTED");
          lcd.setCursor(0, 2);
          lcd.print("OVER CURRENT");
          break;
        case 3:
          // lcd.print("A: UNDER CURRENT");
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("CHARGER FAULTED");
          lcd.setCursor(0, 2);
          lcd.print("UNDER CURRENT");
          break;
        case 4:
          lcd.print("A: OVER TEMPERATURE");
          break;
        case 5:
          lcd.print("A: UNDER TEMPERATURE");
          break;
        case 6:
          // lcd.print("A: GFCI"); // Not implemented in AC001
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("CHARGER FAULTED");
          lcd.setCursor(0, 2);
          lcd.print("GFCI");
          break;
        case 7:
          // lcd.print("A: EARTH DISCONNECTED");
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("CHARGER FAULTED");
          lcd.setCursor(0, 2);
          lcd.print("EARTH DISCONNECT");
          break;
        case 8:
          // added by sai
          if (gu8_online_flag == 1)
          {
            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("CHARGER FAULTED");
            lcd.setCursor(0, 2);
            lcd.print("EMERGENCY");
          }
          else if (gu8_online_flag == 0)
          {
            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("CHARGER FAULTED");
            lcd.setCursor(0, 2);
            lcd.print("EMERGENCY");
          }
          break;
        default:
          lcd.print("*****FAULTED 1*****"); // You can make spaces using well... spacesbreak;
          break;
        }
      }
    }

    else
    {
      // ChargePointStatus inferencedStatus = getChargePointStatusService()->inferenceStatus();
      
      if (evse_ChargePointStatus != local_evse_ChargePointStatus)
      {
        local_evse_ChargePointStatus = evse_ChargePointStatus;
        Serial.print("LCD ");

        switch (local_evse_ChargePointStatus)
        {
        case Available:
          Serial.println("Available");
          if (wifi_connect)
          {
            lcd.clear();
            lcd.setCursor(15, 0);
            lcd.print("Wi-Fi");
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
          break;
        case Preparing:
          Serial.print("Preparing");
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("RFID TAPPED");
          lcd.setCursor(0, 2);
          lcd.print("AUTHENTICATING");
          // gu8_rfidflag = 1;
          break;
        case Charging:
          Serial.print("Charging");
          if (webSocketConncted == 0)
          {
             led_C++;
            if (led_C >= 4)
            {
              led_C = 0;
              requestLed(BLINKYWHITE, START, 1);
            }
            else
            {
              requestLed(BLINKYGREEN, START, 1);
            }
          }
          break;
        case SuspendedEVSE:
          Serial.print("SuspendedEVSE");
          break;
        case SuspendedEV:
          Serial.print("SuspendedEV");
          break;
        case Finishing:
          Serial.print("Finishing");
          break;
        case Reserved:
          Serial.print("Reserved");
          break;
        case Unavailable:
          Serial.print("Unavailable");
          break;
        case Faulted:
          Serial.print("Faulted");
          switch (fault_code_A)
          {
          case 0:
            // ADDED BY SAI

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("CHARGER FAULTED ");
            lcd.setCursor(0, 2);
            lcd.print("OVER VOLTAGE");

            gu8_fault_flag = 1;

            break;
          case 1:
            if (instantVoltage_A > 20)
            {
              lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("CHARGER FAULTED");
            lcd.setCursor(0, 2);
            lcd.print("UNDER VOLTAGE");

            gu8_fault_flag = 2;
            }

            else
            {
#if EVSE_LCD_DISPLAY
              lcd.clear();
              lcd.setCursor(0, 2);
              lcd.print("***POWER FAILURE***");
#endif
            }

            break;
          case 2:

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("CHARGER FAULTED");
            lcd.setCursor(0, 2);
            lcd.print("OVER CURRENT");

            // lcd.print("A: OVER CURRENT");
            break;
          case 3:
            // ADDED BY SAI

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("CHARGER FAULTED");
            lcd.setCursor(0, 2);
            lcd.print("UNDER CURRENT");

            // lcd.print("A: UNDER CURRENT");
            break;
          case 4:
            lcd.print("A: OVER TEMPERATURE");
            break;
          case 5:
            lcd.print("A: UNDER TEMPERATURE");
            break;
          case 6:
            // ADDED BY SAI

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("CHARGER FAULTED");
            lcd.setCursor(0, 2);
            lcd.print("GFCI");

            gu8_fault_flag = 3;
            // lcd.print("A: GFCI"); // Not implemented in AC001
            break;
          case 7:
            // added by sai

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("CHARGER FAULTED");
            lcd.setCursor(0, 2);
            lcd.print("EARTH DISCONNECT");

            gu8_fault_flag = 4;
            // lcd.print("A:EARTH DISCONNECTED");
            break;
          case 8:
            // added by sai

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("CHARGER FAULTED");
            lcd.setCursor(0, 2);
            lcd.print("EMERGENCY");

            gu8_fault_flag = 5;
            break;
          default:

            break;
          }

          break;
        default:

          break;
        }
      }
    }

#if 0
    else
    {
      
      ChargePointStatus inferencedStatus;
      inferencedStatus = getChargePointStatusService()->inferenceStatus();
      //lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
      //lcd.print("                    ");
      lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
      if(inferencedStatus!=previousStatus_A)
      {
        lcd.clear();
      }
      lcd.setCursor(0, 0);
      switch (inferencedStatus)
      {
      case ChargePointStatus::Preparing:
        // ADDED BY SAI
        if (wifi_connect)
        {
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("STATUS : PREPARING");
          lcd.setCursor(0, 2);
          lcd.print("CONNECTIVITY : Wi-Fi");
          lcd.setCursor(0, 3);
          lcd.print("CLOUD : ONLINE");
        }
        else if (gsm_connect)
        {
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("STATUS : PREPARING");
          lcd.setCursor(0, 2);
          lcd.print("CONNECTIVITY : 4G");
          lcd.setCursor(0, 3);
          lcd.print("CLOUD : ONLINE");
        }
        break;
      case ChargePointStatus::SuspendedEVSE:
      lcd.print("A: SuspendedEVSE    ");
        break;
      case ChargePointStatus::SuspendedEV:
      lcd.print("A: SuspendedEV      ");
        break;
      case ChargePointStatus::Charging:
      // ADDED BY SAI
      if(gu8_online_flag == 1)
      {
        if (wifi_connect)
        {
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("STATUS : CHARGING");
          lcd.setCursor(0, 2);
          lcd.print("CONNECTIVITY : Wi-Fi");
          lcd.setCursor(0, 3);
          lcd.print("CLOUD : ONLINE");
        }
        else if (gsm_connect)
        {
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("STATUS : CHARGING");
          lcd.setCursor(0, 2);
          lcd.print("CONNECTIVITY : 4G");
          lcd.setCursor(0, 3);
          lcd.print("CLOUD : ONLINE");
        }
      }
      
        break;
      case ChargePointStatus::Available:
      
       //added by sai
#if 1
      while(millis() - offline_t_A > 5000)
      {
        offline_t_A = millis();
      if(isInternetConnected)
      {
      if(wifi_connect)
      {
        lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("STATUS : AVAILABLE");
        lcd.setCursor(0, 1);
        lcd.print(" TAP RFID/SCAN QR");
        lcd.setCursor(0, 2);
        lcd.print("CONNECTIVITY : Wi-Fi");
        lcd.setCursor(0, 3);
         lcd.print("CLOUD : ONLINE");
      }
      if(gsm_connect)
      {
       
			lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("STATUS : AVAILABLE");
        lcd.setCursor(0, 1);
        lcd.print(" TAP RFID/SCAN QR");
        lcd.setCursor(0, 2);
        lcd.print("CONNECTIVITY : 4G");
         lcd.setCursor(0, 3);
         lcd.print("CLOUD : ONLINE");
      }
      if(ethernet_connect)
      {
       lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("STATUS : AVAILABLE");
        lcd.setCursor(0, 1);
        lcd.print(" TAP RFID/SCAN QR");
        lcd.setCursor(0, 2);
        lcd.print("CONNECTIVITY :ETH");
         lcd.setCursor(0, 3);
         lcd.print("CLOUD : ONLINE");
      }
      }
#if 0
      else
      {
       
         lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
      lcd.print("                    ");
      lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
      lcd.print("A: UNAVAILABLE    ");
      lcd.setCursor(0, 1);               // Or setting the cursor in the desired position.
      	lcd.print("                    "); // Clear the line
				lcd.setCursor(0, 1);
        lcd.setCursor(0, 2);               // Or setting the cursor in the desired position.
      	lcd.print("                    "); // Clear the line
				lcd.setCursor(0, 2);
        lcd.setCursor(0, 3);               // Or setting the cursor in the desired position.
      	lcd.print("                    "); // Clear the line
				lcd.setCursor(0, 3);
				lcd.print("OFFLINE. NO RFID!");
        
      }
#endif
      }
#endif

        break;
      case ChargePointStatus::Unavailable:
        // ADDED BY SAI
        if (wifi_connect)
        {
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("STATUS : UNAVAILABLE");
          lcd.setCursor(0, 2);
          lcd.print("CONNECTIVITY : Wi-Fi");
          lcd.setCursor(0, 3);
          lcd.print("CLOUD : ONLINE");
        }
        else if (gsm_connect)
        {
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("STATUS : UNAVAILABLE");
          lcd.setCursor(0, 2);
          lcd.print("CONNECTIVITY : 4G");
          lcd.setCursor(0, 3);
          lcd.print("CLOUD : ONLINE");
        }

        break;
      case ChargePointStatus::Reserved:
        // added by sai
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("STATUS : RESERVED");
        lcd.setCursor(0, 2);
        if (wifi_connect)
        {
          lcd.print("CONNECTIVITY: Wi-Fi");
        }
        else if (gsm_connect)
        {
          lcd.print("CONNECTIVITY : 4G");
        }
        lcd.setCursor(0, 3);
        lcd.print("CLOUD : ONLINE");

        break;
      case ChargePointStatus::Faulted:
      
      switch (fault_code_A)
          {
          case -1:
            break; // It means default.
          case 0:
            // ADDED BY SAI

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("OVER VOLTAGE");
            lcd.setCursor(0, 2);
            lcd.print("CLOUD : ONLINE");

            gu8_fault_flag = 1;
            // lcd.print("A: OVER VOLTAGE");
            break;
          case 1:
            if (instantVoltage_A > 20)

              lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("UNDER VOLTAGE");
            lcd.setCursor(0, 2);
            lcd.print("CLOUD : ONLINE");

            gu8_fault_flag = 2;
            // lcd.print("A: UNDER VOLTAGE");
            // else
            // lcd.print("A: NO POWER");

            break;
          case 2:

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("OVER CURRENT");
            lcd.setCursor(0, 2);
            lcd.print("CLOUD : ONLINE");

            // lcd.print("A: OVER CURRENT");
            break;
          case 3:
            // ADDED BY SAI

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("UNDER CURRENT");
            lcd.setCursor(0, 2);
            lcd.print("CLOUD : ONLINE");

            // lcd.print("A: UNDER CURRENT");
            break;
          case 4:
            lcd.print("A: OVER TEMPERATURE");
            break;
          case 5:
            lcd.print("A: UNDER TEMPERATURE");
            break;
          case 6:
            // ADDED BY SAI

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("FAULT: GFCI");
            lcd.setCursor(0, 2);
            lcd.print("CLOUD : ONLINE");

            gu8_fault_flag = 3;
            // lcd.print("A: GFCI"); // Not implemented in AC001
            break;
          case 7:
            // added by sai

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("EARTH FAULT");
            lcd.setCursor(0, 2);
            lcd.print("CLOUD : ONLINE");

            gu8_fault_flag = 4;
            // lcd.print("A:EARTH DISCONNECTED");
            break;
          case 8:
            // added by sai

            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("FAULT : EMERGENCY");
            lcd.setCursor(0, 2);
            lcd.print("CLOUD : ONLINE");

            gu8_fault_flag = 5;
            break;
            /* case 9:
              lcd.print("A-POWER FAIL");
              break;

            default:
              lcd.print("*****FAULTED A*****"); // You can make spaces using well... spacesbreak;
              break;
              */
          }
        break;
      }
      
    }
#endif
    break;
 
  default:
    Serial.println(F("**Display default**"));
    break;
  }
}

#endif

/*LCD DRIVER*/

void disp_lcd_meter()
{
  // lcd.clear();
  if (webSocketConncted)
  {
    if (isInternetConnected)
    {
      if (wifi_connect)
      {
        lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        lcd.print("                    "); // Clear the line
        lcd.setCursor(15, 0);
        lcd.print("WI-FI");
      }
      if (gsm_connect && client.connected())
      {
        lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        lcd.print("                    "); // Clear the line
        lcd.setCursor(18, 0);
        lcd.print("4G");
      }
      if (ethernet_connect)
      {
        lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
        lcd.print("                    "); // Clear the line
        lcd.setCursor(12, 0);
        lcd.print("ETHERNET");
      }
    }

    lcd.setCursor(0, 1);
    // if (evse_ChargePointStatus != local_evse_ChargePointStatus)
    // {
    //   local_evse_ChargePointStatus = evse_ChargePointStatus;
    ChargePointStatus inferencedStatus;
    inferencedStatus = getChargePointStatusService()->inferenceStatus();

    if (notFaulty_A)
    {
      Serial.print("LCD ");

      switch (inferencedStatus)
      {
      case Available:
        Serial.println("Available");
        // if (wifi_connect)
        // {
        //   lcd.clear();
        //   lcd.setCursor(15, 0);
        //   lcd.print("Wi-Fi");
        //   lcd.setCursor(0, 1);
        //   lcd.print("CHARGER AVAILABLE");
        //   lcd.setCursor(0, 2);
        //   lcd.print("TAP RFID/SCAN QR");
        //   lcd.setCursor(0, 3);
        //   lcd.print("TO START");
        // }
        // else if (gsm_connect)
        // {

        //   lcd.clear();
        //   lcd.setCursor(15, 0);
        //   lcd.print("4G");
        //   lcd.setCursor(0, 1);
        //   lcd.print("CHARGER AVAILABLE");
        //   lcd.setCursor(0, 2);
        //   lcd.print("TAP RFID/SCAN QR");
        //   lcd.setCursor(0, 3);
        //   lcd.print("TO START");
        // }
        if(remotestart_txn == 0)
        {
          lcd.setCursor(0, 1);
          lcd.print("CHARGER AVAILABLE   ");
          lcd.setCursor(0, 2);
          lcd.print("TAP RFID/SCAN QR");
          lcd.setCursor(0, 3);
          lcd.print("TO START           ");
        }
        break;
      case Preparing:
        Serial.print("Preparing");
        if((evse_start_txn_state != EVSE_START_TXN_ACCEPTED) && (remotestart_txn == 0))
        {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("RFID TAPPED");
        lcd.setCursor(0, 2);
        lcd.print("AUTHENTICATING");
        }
        // gu8_rfidflag = 1;
        break;
      case Charging:
        charging_disp_A();
        Serial.print("Charging");
        if (webSocketConncted == 0)
        {
          led_C++;
          if (led_C >= 4)
          {
            led_C = 0;
            requestLed(BLINKYWHITE, START, 1);
          }
          else
          {
            requestLed(BLINKYGREEN, START, 1);
          }
        }

        break;
      case SuspendedEVSE:
        Serial.print("SuspendedEVSE");
        break;
      case SuspendedEV:
        Serial.print("SuspendedEV");
        break;
      case Finishing:
        Serial.print("Finishing");
        break;
      case Reserved:
        Serial.print("Reserved");
        break;
      case Unavailable:
        Serial.print("Unavailable");
        break;
      case Faulted:
        disp_fault_code_A();
        Serial.print("Faulted");
        break;

      default:
        Serial.println("**Display default**");
        break;
      }
    }
    // }
  }

  else if (onlineoffline_1 == 1 && gu8_online_flag == 0)
  {

    lcd.clear();
    lcd.setCursor(13, 0);
    lcd.print("OFFLINE");
    lcd.setCursor(0, 1);
    lcd.print("CHARGER AVAILABLE   ");
    lcd.setCursor(0, 2);
    lcd.print("TAP RFID/SCAN QR");
    lcd.setCursor(0, 3);
    lcd.print("TO START           ");

  }

  else if (gu8_online_flag == 0 && onlineoffline_1 == 0 && webSocketConncted == 0 && offline_c == 0 )
  {
    lcd.clear();
    lcd.setCursor(13, 0);
    lcd.print("OFFLINE");
    if (notFaulty_A)
    {
      lcd.setCursor(0, 1);
      lcd.print("CHARGER UNAVAILABLE");
    }
    else if (notFaulty_A == false)
    {

      disp_fault_code_A();
    }
  }
}

void charging_disp_A(void)
{
  // lcd.clear();
  if (isInternetConnected)
  {
    if (wifi_connect)
    {
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("                    "); // Clear the line
      lcd.setCursor(15, 0);
      lcd.print("WI-FI");
    }
    if (gsm_connect && client.connected())
    {
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("                    "); // Clear the line
      lcd.setCursor(18, 0);
      lcd.print("4G");
    }
    if (ethernet_connect)
    {
      lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
      lcd.print("                    "); // Clear the line
      lcd.setCursor(12, 0);
      lcd.print("ETHERNET");
    }
  }
  else if (isInternetConnected == 0 && webSocketConncted == 0)
  {
    lcd.setCursor(0, 0);               // Or setting the cursor in the desired position.
    lcd.print("                    "); // Clear the line
    lcd.setCursor(15, 0);
    lcd.print("OFFLINE");
  }

  float instantCurrrent_A = eic.GetLineCurrentA();
  float instantVoltage_A = eic.GetLineVoltageA();
  float instantPower_A = 0.0f;

  if (instantCurrrent_A < 0.15)
  {
    instantPower_A = 0;
  }
  else
  {
    instantPower_A = (instantCurrrent_A * instantVoltage_A) / 1000.0;
  }

  if (webSocketConncted == 1)
  {
    if (disp_evse_A)
    {
      // lcd.clear();
      // lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
      if (notFaulty_A)
      {
        // added by sai
        instantPower_A = (instantCurrrent_A * instantVoltage_A) / 1000.0;
        lcd.setCursor(0, 1);
        lcd.print("CHARGING @ kW:");
        lcd.setCursor(14, 1); // Or setting the cursor in the desired position.
        lcd.print(String(instantPower_A));
        lcd.setCursor(0, 2);
        lcd.print("ENERGY(kWh):       ");
        lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
        if (!offline_charging_A)
        {
          if ((online_charging_Enargy_A - globalmeterstartA) > 0)
          {
            Serial.println("discurrEnergy_A: " + String(discurrEnergy_A));
            // lcd.print(String(float((online_charging_Enargy_A - globalmeterstartA) / 1000)));

            lcd.print(String(discurrEnergy_A));
            //  lcd.print(String(float((online_charging_Enargy_A - globalmeterstartA) / 1000)));
          }

          else
          {
            lcd.print("0");
          }

          if (gu8_online_flag == 1)
          {
            Serial.println("[DISP_METERVALUES]******* online charging***************");
            Serial.println("online_charging_Enargy_A");
            Serial.println(online_charging_Enargy_A);
          }
          else
          {
            Serial.println("[DISP_METERVALUES]******* offline charging***************");
            Serial.print("offline_charging_Enargy_A");
            Serial.println(online_charging_Enargy_A);
          }
        }
        else
        {
          lcd.print(String(float((offline_charging_Enargy_A - globalmeterstartA) / 1000)));
          // Serial.println(F("[DISP_METERVALUES]******* offline charging***************"));
          if (gu8_online_flag == 1)
          {
            Serial.println("[DISP_METERVALUES]******* online charging***************");
          }
          else
          {
            Serial.println("[DISP_METERVALUES]******* offline charging***************");
          }
        }
        // ADDED BY SAI FOR TIME DURATION
        unsigned long stop_time = millis();
        lcd.setCursor(0, 3);
        lcd.print("TIME: ");
        lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
        unsigned long seconds = (stop_time - st_timeA) / 1000;
        int hr = seconds / 3600;                                                 // Number of seconds in an hour
        int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
        int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
        String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
        lcd.print(String(hrMinSec));
      }

      else
      {
        disp_fault_code_A();
      }
    }
  }

  else if (webSocketConncted == 0)
  {
    if (disp_evse_A)
    {
      // lcd.clear();
      // lcd.setCursor(0, 0); // Or setting the cursor in the desired position.
      if (notFaulty_A)
      {
        // added by sai
        instantPower_A = (instantCurrrent_A * instantVoltage_A) / 1000.0;
        lcd.setCursor(0, 1);
        lcd.print("CHARGING @ kW:");
        lcd.setCursor(14, 1); // Or setting the cursor in the desired position.
        lcd.print(String(instantPower_A));
        lcd.setCursor(0, 2);
        lcd.print("ENERGY(kWh):       ");
        lcd.setCursor(12, 2); // Or setting the cursor in the desired position.
        if (!offline_charging_A)
        {
          if ((online_charging_Enargy_A - globalmeterstartA) > 0)
          {
            Serial.println("discurrEnergy_A: " + String(discurrEnergy_A));
            // lcd.print(String(float((online_charging_Enargy_A - globalmeterstartA) / 1000)));

            lcd.print(String(discurrEnergy_A));
            //  lcd.print(String(float((online_charging_Enargy_A - globalmeterstartA) / 1000)));
          }

          else
          {
            lcd.print("0");
          }

          if (gu8_online_flag == 1)
          {
            Serial.println("[DISP_METERVALUES]******* online charging***************");
            Serial.println("online_charging_Enargy_A");
            Serial.println(online_charging_Enargy_A);
          }
          else
          {
            Serial.println("[DISP_METERVALUES]******* offline charging***************");
            Serial.print("offline_charging_Enargy_A");
            Serial.println(online_charging_Enargy_A);
          }
        }
        else
        {
          lcd.print(String(float((offline_charging_Enargy_A - globalmeterstartA) / 1000)));
          // Serial.println(F("[DISP_METERVALUES]******* offline charging***************"));
          if (gu8_online_flag == 1)
          {
            Serial.println("[DISP_METERVALUES]******* online charging***************");
          }
          else
          {
            Serial.println("[DISP_METERVALUES]******* offline charging***************");
          }
        }
        // ADDED BY SAI FOR TIME DURATION
        unsigned long stop_time = millis();
        lcd.setCursor(0, 3);
        lcd.print("TIME: ");
        lcd.setCursor(12, 3); // Or setting the cursor in the desired position.
        unsigned long seconds = (stop_time - st_timeA) / 1000;
        int hr = seconds / 3600;                                                 // Number of seconds in an hour
        int mins = (seconds - hr * 3600) / 60;                                   // Remove the number of hours and calculate the minutes.
        int sec = seconds - hr * 3600 - mins * 60;                               // Remove the number of hours and minutes, leaving only seconds.
        String hrMinSec = (String(hr) + ":" + String(mins) + ":" + String(sec)); // Converts to HH:MM:SS string. This can be returned to the calling function.
        lcd.print(String(hrMinSec));
      }

      else
      {
        disp_fault_code_A();
      }
    }
  }
}

void disp_fault_code_A(void)
{

  float instantVoltage_A = eic.GetLineVoltageA();

  switch (fault_code_A)
  {
  case -1:
    lcd.print("A: FAULTED");
    break; // It means default.
  case 0:
    // lcd.print("A: OVER VOLTAGE");

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("CHARGER FAULTED ");
    lcd.setCursor(0, 2);
    lcd.print("OVER VOLTAGE");
    break;
  case 1:
    if (instantVoltage_A > 20)
    {
      // lcd.print("A: UNDER VOLTAGE");
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("CHARGER FAULTED");
      lcd.setCursor(0, 2);
      lcd.print("UNDER VOLTAGE");
    }
    else
    {
      lcd.clear();
      lcd.setCursor(0, 2);
      lcd.print("***POWER FAILURE***");
    }

    break;
  case 2:

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("CHARGER FAULTED");
    lcd.setCursor(0, 2);
    lcd.print("OVER CURRENT");
    break;
  case 3:
    // lcd.print("A: UNDER CURRENT");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("CHARGER FAULTED");
    lcd.setCursor(0, 2);
    lcd.print("UNDER CURRENT");
    break;
  case 4:
    lcd.print("A: OVER TEMPERATURE");
    break;
  case 5:
    lcd.print("A: UNDER TEMPERATURE");
    break;
  case 6:
    // lcd.print("A: GFCI"); // Not implemented in AC001
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("CHARGER FAULTED");
    lcd.setCursor(0, 2);
    lcd.print("GFCI");
    break;
  case 7:

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("CHARGER FAULTED");
    lcd.setCursor(0, 2);
    lcd.print("EARTH DISCONNECT");
    break;
  case 8:

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("CHARGER FAULTED");
    lcd.setCursor(0, 2);
    lcd.print("EMERGENCY");

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("CHARGER FAULTED");
    lcd.setCursor(0, 2);
    lcd.print("EMERGENCY");

    break;
  default:
    lcd.print("*****FAULTED 1*****"); // You can make spaces using well... spacesbreak;
    break;
  }
}

#endif
