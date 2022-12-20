/*
 * gui_lovyan.cpp
 */

#if	(cmakeGUI == 3)

#include "lgfx_conf_wrover_dk41.hpp"
#include "systiming.h"
#include "FreeRTOS_Support.h"

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

// ####################################### Private variables #######################################

static LGFX lcd;
static LGFX_Sprite canvas;
StaticTask_t ttsGUI;
StackType_t tsbGUI[guiSTACK_SIZE] = { 0 };
static constexpr char text[] = "Hello world ! こんにちは世界！ this is long long string sample. 寿限無、寿限無、五劫の擦り切れ、海砂利水魚の、水行末・雲来末・風来末、喰う寝る処に住む処、藪ら柑子の藪柑子、パイポ・パイポ・パイポのシューリンガン、シューリンガンのグーリンダイ、グーリンダイのポンポコピーのポンポコナの、長久命の長助";
static constexpr size_t textlen = sizeof(text) / sizeof(text[0]);
size_t textpos = 0;

// ################################## Background/Backlight control #################################

void vGuiInit(void) {
	  lcd.init();
	  if (lcd.width() < lcd.height())			  // Setting display to landscape
		  lcd.setRotation(lcd.getRotation() ^ 1);
	  canvas.setColorDepth(8);
	  canvas.setFont(&fonts::lgfxJapanMinchoP_32);
	  canvas.setTextWrap(false);
	  canvas.createSprite(lcd.width() + 36, 36);
}

void vGuiUpdate(void) {
	  int32_t cursor_x = canvas.getCursorX() - 1;
	  if (cursor_x <= 0) {
	    textpos = 0;
	    cursor_x = lcd.width();
	  }
	  canvas.setCursor(cursor_x, 0); // カーソル位置を更新
	  canvas.scroll(-1, 0);          // キャンバスの内容を1ドット左にスクロール
	  while (textpos < textlen && cursor_x <= lcd.width()) {
	    canvas.print(text[textpos++]);
	    cursor_x = canvas.getCursorX();
	  }
	  canvas.pushSprite(&lcd, 0, 0);
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

extern "C" void vTaskGUI_Start(void * pvPara) {
	xRtosTaskCreateStatic(vTaskGUI, "lovyan", guiSTACK_SIZE, pvPara, guiPRIORITY, tsbGUI, &ttsGUI, tskNO_AFFINITY);
}
#endif
