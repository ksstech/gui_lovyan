/*
 * gui_lovyan.cpp
 */

#define cmakeGUI 3
#if	(cmakeGUI == 3)

#define LGFX_USE_V1         // set to use new version of library
#include <LovyanGFX.hpp>    // main library
#include <driver/i2c.h>

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
#define guiINTERVAL_MS				500

#define TFT_WIDTH   320
#define TFT_HEIGHT  480

// ####################################### Private variables #######################################

static constexpr int I2C_PORT_NUM = I2C_NUM_0;
static constexpr int I2C_PIN_SDA = 38;
static constexpr int I2C_PIN_SCL = 39;
static constexpr int I2C_PIN_INT = 40;

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_ILI9488     _panel_instance;
    lgfx::Touch_FT5x06      _touch_instance;
    lgfx::Light_PWM         _light_instance;
    lgfx::Bus_Parallel16    _bus_instance;

	public: LGFX(void) {
        {   auto cfg = _bus_instance.config();
            cfg.port = 0;
            cfg.freq_write = 20000000;
            cfg.pin_wr = 35;
            cfg.pin_rd = 48;
            cfg.pin_rs = 36;
            cfg.pin_d0 = 47;
            cfg.pin_d1 = 21;
            cfg.pin_d2 = 14;
            cfg.pin_d3 = 13;
            cfg.pin_d4 = 12;
            cfg.pin_d5 = 11;
            cfg.pin_d6 = 10;
            cfg.pin_d7 = 9;
            cfg.pin_d8 = 3;
            cfg.pin_d9 = 8;
            cfg.pin_d10 = 16;
            cfg.pin_d11 = 15;
            cfg.pin_d12 = 7;
            cfg.pin_d13 = 6;
            cfg.pin_d14 = 5;
            cfg.pin_d15 = 4;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        {   auto cfg = _panel_instance.config();
            cfg.pin_cs = -1;
            cfg.pin_rst = -1;
            cfg.pin_busy = -1;
            cfg.memory_width = TFT_WIDTH;
            cfg.memory_height = TFT_HEIGHT;
            cfg.panel_width = TFT_WIDTH;
            cfg.panel_height = TFT_HEIGHT;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;
            cfg.readable = true;
            cfg.invert = false;
            cfg.rgb_order = false;
            cfg.dlen_16bit = true;
            cfg.bus_shared = true;
            _panel_instance.config(cfg);
        }

        {	auto cfg = _light_instance.config();
			cfg.pin_bl = 45;
			cfg.invert = false;
			cfg.freq   = 44100;
			cfg.pwm_channel = 7;
			_light_instance.config(cfg);
			_panel_instance.setLight(&_light_instance);
        }

        {	auto cfg = _touch_instance.config();
        	cfg.x_min      = 0;
        	cfg.x_max      = TFT_WIDTH;
        	cfg.y_min      = 0;
        	cfg.y_max      = TFT_HEIGHT;
        	cfg.pin_int    = I2C_PIN_INT;
        	cfg.bus_shared = true;
        	cfg.offset_rotation = 0;
        	cfg.i2c_port = I2C_PORT_NUM;
        	cfg.i2c_addr = 0x38;
        	cfg.pin_sda  = I2C_PIN_SDA;
        	cfg.pin_scl  = I2C_PIN_SCL;
        	cfg.freq = 400000;
        	_touch_instance.config(cfg);
        	_panel_instance.setTouch(&_touch_instance);
        }
        setPanel(&_panel_instance);
    }
};

static LGFX lcd;
static int x,y;

StaticTask_t ttsGUI;
StackType_t tsbGUI[guiSTACK_SIZE] = { 0 };

// ################################## Background/Backlight control #################################

void vGuiInit(void) {
	lcd.init();
	if (lcd.width() < lcd.height())			  // Setting display to landscape
		lcd.setRotation(lcd.getRotation() ^ 1);
	lcd.setCursor(0,0);
	lcd.printf("Ready to touch & draw!");
}

void vGuiUpdate(void) {
	if (lcd.getTouch(&x, &y)) {
		lcd.fillRect(x-2, y-2, 5, 5, TFT_RED);
		lcd.setCursor(380,0);
		lcd.printf("Touch:(%03d,%03d)", x,y);
	}
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
