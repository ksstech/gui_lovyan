/*
 * lgfx_conf_wrover_dk41.h
 * Copyright (c) 2022 Andre M. Maree / KSS Technologies (Pty) Ltd.
 */

#pragma once

#define LGFX_ESP_WROVER_KIT

#if !defined LGFX_USE_V1
  #define LGFX_USE_V1
#endif

#if !defined LGFX_AUTODETECT
  #define LGFX_AUTODETECT
#endif

#include <LovyanGFX.hpp>

/*

#define SD_SUPPORTED				0				// SD CARD - SPI
#if (SD_SUPPORTED > 0)
	#define SDSPI_HOST_ID			SPI3_HOST
	#define SD_MISO					GPIO_NUM_41
	#define SD_MOSI					GPIO_NUM_2
	#define SD_SCLK					GPIO_NUM_42
	#define SD_CS					GPIO_NUM_1
#endif

// Landscape
#define TFT_WIDTH   				320
#define TFT_HEIGHT  				240

#define SPI_HOST_ID 				SPI2_HOST
#define TFT_MOSI    				GPIO_NUM_23
#define TFT_MISO    				GPIO_NUM_25  // Set this PIN for using shared SPI option
#define TFT_SCLK    				GPIO_NUM_19
#define TFT_DC      				GPIO_NUM_21
#define TFT_CS      				GPIO_NUM_22
#define TFT_RST     				GPIO_NUM_18
#define TFT_BL      				GPIO_NUM_5

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ILI9341     _panel_instance;
  lgfx::Bus_SPI			_bus_instance;
  lgfx::Light_PWM         _light_instance;

  public:
    LGFX(void)
    {
        {
            auto cfg = _bus_instance.config();
            cfg.port = 0;
            cfg.freq_write = 20000000;
            cfg.pin_wr = 35;
            cfg.pin_rd = 48;
            cfg.pin_rs = 36;

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        {
            auto cfg = _panel_instance.config();

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

    {
      auto cfg = _light_instance.config();

      cfg.pin_bl = 45;
      cfg.invert = false;
      cfg.freq   = 44100;
      cfg.pwm_channel = 7;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    {
      auto cfg = _touch_instance.config();

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
*/
