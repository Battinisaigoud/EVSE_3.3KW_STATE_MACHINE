// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\display_meterValues.h"
#ifndef DISPLAY_METERVALUES_H
#define DISPLAY_METERVALUES_H

#include "Master.h"
#include "OcppMessage.h"
#include "ChargePointStatusService.h"
#include "OcppEngine.h"


void stateTimer();
void disp_dwin_meter();
void disp_lcd_meter();
void cloud_no_rfid_dwin_print(void);
void disp_fault_code_A(void);
void charging_disp_A(void);
#endif