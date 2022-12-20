/*
 * gui_lovyan.cpp
 */

#include "hal_config.h"

#if	(cmakeGUI == 3)
#include "lgfx_conf_wrover_dk41.hpp"
#include "FreeRTOS_Support.h"
#include "printfx.h"
#include "syslog.h"
#include "systiming.h"

// #################################### Build macros ###############################################

#define	debugFLAG					0xF000

#define	debugTIMING					(debugFLAG_GLOBAL & debugFLAG & 0x1000)
#define	debugTRACK					(debugFLAG_GLOBAL & debugFLAG & 0x2000)
#define	debugPARAM					(debugFLAG_GLOBAL & debugFLAG & 0x4000)
#define	debugRESULT					(debugFLAG_GLOBAL & debugFLAG & 0x8000)

// ################################# Device definitions ############################################

#define	guiPRIORITY					2
#define	guiSTACK_SIZE				(configMINIMAL_STACK_SIZE + 2048 + (flagSTACK * 512))
#define guiINTERVAL_MS				500

#define	sizeTEXTBUF					2048

// ####################################### Private variables #######################################

static LGFX lcd;
static char TxtBuf[sizeTEXTBUF];
StaticTask_t ttsGUI;
StackType_t tsbGUI[guiSTACK_SIZE] = { 0 };

// ################################## Background/Backlight control #################################

void vGuiInit(void) {
	lcd.init();
	lcd.clear();
	lcd.setRotation(3);									// ensure landscape
	lcd.cp437(true);
//	lcd.setFont(&Font8x8C64);

	lcd.setColorDepth(1);
}

void vGuiUpdate(void) {
	int iRV = xRtosReportTasks(TxtBuf, sizeTEXTBUF, (fm_t)makeMASK09x23(0,0,1,1,1,1,1,0,1,0x000FFFFF));
	char * pcBuf = TxtBuf + iRV;
	size_t Size = sizeTEXTBUF - iRV;
	wsnprintfx(&pcBuf, &Size, "Sys=0x%08X\r\n", SFcur);
	wsnprintfx(&pcBuf, &Size, "%!R  %Z\r\n", RunTime, &sTSZ);
}

void vGuiRefresh(void) {
//	lcd.clear();
//	lcd.setTextColor(1);
//	lcd.setBrightness(255);
//	lcd.print(TxtBuf);
//    lcd.printFixed(0, 0, (const char *) TxtBuf, STYLE_NORMAL);
    lcd.clear(0x0000);
	lcd.setColor(255,255,255);
	lcd.setCursor(0,0);
	lcd.print(TxtBuf);
}

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

extern "C" void vTaskGUI_Start(void * pvPara) {
	xRtosTaskCreateStatic(vTaskGUI, "lovyan", guiSTACK_SIZE, pvPara, guiPRIORITY, tsbGUI, &ttsGUI, tskNO_AFFINITY);
}
#endif
