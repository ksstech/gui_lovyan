/*
 * gui_lovyan.cpp
 */

#include "main.h"
#if	(cmakeGUI == 3)
#include "gui_lovyan.h"
#include "lgfx_conf_wrover_dk41.h"

// IRMACOS specific
#include "hal_variables.h"
#include "printfx.h"
#include "syslog.h"
#include "systiming.h"
#include "hal_timer.h"

// ESP32 specific
#include "esp_freertos_hooks.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

// #################################### Build macros ###############################################

#define	debugFLAG					0xF000

#define	debugTIMING					(debugFLAG_GLOBAL & debugFLAG & 0x1000)
#define	debugTRACK					(debugFLAG_GLOBAL & debugFLAG & 0x2000)
#define	debugPARAM					(debugFLAG_GLOBAL & debugFLAG & 0x4000)
#define	debugRESULT					(debugFLAG_GLOBAL & debugFLAG & 0x8000)

// ################################# Device definitions ############################################

#define	guiPRIORITY					2
#define	guiSTACK_SIZE				(configMINIMAL_STACK_SIZE + 2048 + (flagSTACK * 512))
#define guiINTERVAL_MS				100

#define	sizeTEXTBUF					3072

// ####################################### Private variables #######################################

u8_t TxtBuf[sizeTEXTBUF];
StaticTask_t ttsGUI;
StackType_t tsbGUI[guiSTACK_SIZE] = { 0 };

// ################################## Background/Backlight control #################################

void vGuiInit(void) { }

void vGuiUpdate(void) {
	int iRV = xRtosReportTasks(makeMASK09x23(0,0,1,1,1,1,1,0,1,0x000FFFCF), TxtBuf, sizeTEXTBUF);
	iRV += snprintfx(TxtBuf+iRV, sizeTEXTBUF - iRV, "%!R  %Z\r\n", RunTime, &sTSZ);
//	iRV += snprintfx(TxtBuf+iRV, sizeTEXTBUF - iRV, "Value=%u  ", Level);
	iRV += snprintfx(TxtBuf+iRV, sizeTEXTBUF - iRV, "Evt=0x%08X\r\n", xEventGroupGetBits(xEventStatus));
	iRV += snprintfx(TxtBuf+iRV, sizeTEXTBUF - iRV, "Run=0x%08X\r\n", xEventGroupGetBits(TaskRunState));
	iRV += snprintfx(TxtBuf+iRV, sizeTEXTBUF - iRV, "Del=0x%08X\r\n", xEventGroupGetBits(TaskDeleteState));
	iRV += snprintfx(TxtBuf+iRV, sizeTEXTBUF - iRV, "Sys=0x%08X\r\n", SFcur);
}

void vGuiRefresh(void) { }

void vTaskGUI(void * pVoid) {
	IF_SYSTIMER_INIT(debugTIMING, stGUI0, stMILLIS, "GUI0", 3, 30);
	IF_SYSTIMER_INIT(debugTIMING, stGUI1, stMILLIS, "GUI1", 20, 100);

	vGuiInit();
	vTaskSetThreadLocalStoragePointer(NULL, 1, (void *)taskGUI_MASK);
	TickType_t GUI_LWtime = xTaskGetTickCount();
	xRtosSetStateRUN(taskGUI_MASK);

    while(bRtosVerifyState(taskGUI_MASK)) {
    	IF_SYSTIMER_START(debugTIMING, stGUI0);
    	vGuiUpdate();
    	IF_SYSTIMER_STOP(debugTIMING, stGUI0);

    	IF_SYSTIMER_START(debugTIMING, stGUI1);
    	vGuiRefresh();
    	IF_SYSTIMER_STOP(debugTIMING, stGUI1);

		vTaskDelayUntil(&GUI_LWtime, pdMS_TO_TICKS(guiINTERVAL_MS));
	}
	vRtosTaskDelete(NULL);
}

void vTaskGUI_Init(void * pvPara) {
	xRtosTaskCreateStatic(vTaskGUI, "lovyan", guiSTACK_SIZE, pvPara, guiPRIORITY, tsbGUI, &ttsGUI, tskNO_AFFINITY);
}
#endif
