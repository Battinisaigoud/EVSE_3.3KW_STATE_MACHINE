// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\Variants.h"
// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2020
// MIT License

#ifndef VARIANTS_H
#define VARIANTS_H

/*****************/
/*
In order to Enable Ethernet/WiFi,
* Edit this "virtual void begin(uint16_t port=0) =0;" for WIFI :C:\Users\Mr Cool\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\cores\esp32\Server.h
* Set proper flag in OCPP_ETH_WIFI file kept in arduinoWebsockets/src
*/

#define VERSION String("EVSE_3.3KW_S_V1.3.2")
#define CHARGE_POINT_VENDOR String("EVRE")
#define CHARGE_POINT_MODEL String("3.3KW")
#define CHARGE_POINT_VENDOR_SIZE strlen("EVRE")
#define CHARGE_POINT_MODEL_SIZE strlen("3.3KW")
// #define CHARGE_POINT_VENDOR_SIZE     strlen(CHARGE_POINT_VENDOR)
// #define CHARGE_POINT_MODEL_SIZE      strlen(CHARGE_POINT_MODEL)

#define EVSE_CHARGE_POINT_FIRMWARE_OTA_VERSION String("132")
#define DEVICE_ID String("evse_001")

#define GSM_ENABLED 1
#define WIFI_ENABLED 1
#define ETHERNET_ENABLED 0

#define BLE_ENABLE 1

#define CP_ACTIVE 0

#define DISPLAY_ENABLED 0

#define MASTERPIN_ENABLED 0

#define LED_ENABLED 1

#define EVSECEREBRO 1
#define STEVE 0

#define EARTH_DISCONNECT 1

#define DWIN_ENABLED 0

#define LCD_ENABLED 0

#define LCD_DISPLAY 1

#define GSM_PING 1

#define LCD_ENABLED_FAULT 0

#define V_charge_lite1_4 1

#define V_charge_lite1_3 1

#define SIMCOM_A7672S 1

#define SIMCOM_A7670C 0

/*****************/
#define DEBUG_OUT true
#define DEBUG_EXTRA false
#define DEBUG_OUT_M true

#define ARAI_BOARD true // This is a reverse logic

#define ALPR_ENABLED 0

// #define OCPP_SERVER //comment out if this should be compiled as server <--- needs to be implemented again

#define EVSE_FOTA_ENABLE_4G (1)
#define EVSE_FOTA_ENABLE_WIFI (1)

#define TEST_OTA 1

#if 1

#define EVSE_CHANGE_STATE_ZERO (0)
#define EVSE_CHANGE_STATE_ONE (1)

#define EVSE_BOOT (1)
#define EVSE_READ_RFID (2)
#define EVSE_AUTHENTICATION (3)
#define EVSE_START_TXN (4)
#define EVSE_CHARGING (5)
#define EVSE_STOP_TXN (6)
#define EVSE_UNAUTHENTICATION (7)
#define EVSE_REBOOT (8)
#define EVSE_RESERVE_NOW (9)
#define EVSE_CANCEL_RESERVATION (10)
#define EVSE_LOCAL_AUTHENTICATION (11)

#endif

/* EVSE to OCPP message update state machine */

enum EvseDevStatuse
{

    flag_EVSE_is_Booted,
    flag_EVSE_Read_Id_Tag,
    flag_EVSE_Authentication,
    flag_EVSE_Start_Transaction,
    flag_EVSE_Request_Charge,
    flag_EVSE_Stop_Transaction,
    flag_EVSE_UnAutharized,
    flag_EVSE_Reboot_Request,
    flag_EVSE_Reserve_Now,
    flag_EVSE_Cancle_Reservation,
    flag_EVSE_Local_Authantication

};

extern enum EvseDevStatuse EvseDevStatus_connector_1;

/* boot notification update state machine */
enum evse_boot_stat_t
{
    EVSE_BOOT_INITIATED = 0,
    EVSE_BOOT_SENT,
    EVSE_BOOT_ACCEPTED,
    EVSE_BOOT_REJECTED,
    EVSE_BOOT_DEFAULT,
};
extern enum evse_boot_stat_t evse_boot_state;

/* Authorize update state machine */
enum evse_authorize_stat_t
{
    EVSE_AUTHORIZE_INITIATED = 0,
    EVSE_AUTHORIZE_SENT,
    EVSE_AUTHORIZE_ACCEPTED,
    EVSE_AUTHORIZE_REJECTED,
    EVSE_AUTHORIZE_DEFAULT,
};
extern enum evse_authorize_stat_t evse_authorize_state;

/* Start Transaction update state machine */
enum evse_start_txn_stat_t
{
    EVSE_START_TXN_INITIATED = 0,
    EVSE_START_TXN_SENT,
    EVSE_START_TXN_ACCEPTED,
    EVSE_START_TXN_REJECTED,
    EVSE_START_TXN_DEFAULT,
};
extern enum evse_start_txn_stat_t evse_start_txn_state;

/* Stop Transaction update state machine */
enum evse_stop_txn_stat_t
{
    EVSE_STOP_TXN_INITIATED = 0,
    EVSE_STOP_TXN_SENT,
    EVSE_STOP_TXN_ACCEPTED,
    EVSE_STOP_TXN_REJECTED,
    EVSE_STOP_TXN_DEFAULT,
};
extern enum evse_stop_txn_stat_t evse_stop_txn_state;

enum evsetransactionstatus
{
    EVSE_NO_TRANSACTION,
    EVSE_START_TRANSACTION,
    EVSE_STOP_TRANSACTION
};

extern enum evsetransactionstatus EVSE_transaction_status;

enum evse_connection_sta
{
    EVSE_OFFLINE,
    EVSE_ONLINE

};
extern enum evse_connection_sta evse_connection_status;

#endif
