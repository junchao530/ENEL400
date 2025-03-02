#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>


/**
 * Recommended board settings for platformio.ini file
 * 
 * board = esp32dev
 * board_build.partitions = max_app_8MB.csv
 * 
 */

#define WIDTH 480
#define HEIGHT 320

class LGFX : public lgfx::LGFX_Device
{

    lgfx::Panel_ILI9488 _panel_instance;

    lgfx::Bus_SPI _bus_instance;

    lgfx::Light_PWM _light_instance;

public:
    LGFX(void)
    {
        {
            auto cfg = _bus_instance.config();

            cfg.spi_host = VSPI_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 40000000;
            cfg.freq_read  = 16000000;
            cfg.spi_3wire  = true;        
            cfg.use_lock   = true;        
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            cfg.pin_sclk = GPIO_NUM_18;
            cfg.pin_mosi = GPIO_NUM_23;
            cfg.pin_miso = GPIO_NUM_19;
            cfg.pin_dc   = GPIO_NUM_25;

            _bus_instance.config(cfg);              // Apply the settings to the bus.
            _panel_instance.setBus(&_bus_instance); // Sets the bus to the panel.
        }

        {                                        // Set display panel control.
            auto cfg = _panel_instance.config(); // Get the structure for display panel settings.

            cfg.pin_cs = GPIO_NUM_5;   // Pin number to which CS is connected (-1 = disable)
            cfg.pin_rst = GPIO_NUM_26;  // pin number where RST is connected (-1 = disable)
            cfg.pin_busy = -1; // pin number to which BUSY is connected (-1 = disable)

            // * The following setting values ​​are set to general default values ​​for each panel, and the pin number (-1 = disable) to which BUSY is connected, so please try commenting out any unknown items.

            cfg.memory_width = 320;  // Maximum width supported by driver IC
            cfg.memory_height = 480; // Maximum height supported by driver IC
            cfg.panel_width = 320;   // actual displayable width
            cfg.panel_height = 480;  // actual displayable height
            cfg.offset_x = 0;        // Panel offset in X direction
            cfg.offset_y = 0;        // Panel offset in Y directioncfg.offset_rotation = 2;
            cfg.offset_rotation = 0;
            // cfg.dummy_read_pixel = 8;
            // cfg.dummy_read_bits = 1;
            cfg.readable = true;
            cfg.invert = false;
            cfg.rgb_order = true;
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;

            _panel_instance.config(cfg);
        }
        {                                        // Set backlight control. (delete if not necessary)
            auto cfg = _light_instance.config(); // Get the structure for backlight configuration.

            cfg.pin_bl = GPIO_NUM_27;     // pin number to which the backlight is connected
            cfg.invert = false;  // true to invert backlight brightness
            cfg.freq = 44100;    // backlight PWM frequency
            cfg.pwm_channel = 0; // PWM channel number to use

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance); // Sets the backlight to the panel.
        }


        setPanel(&_panel_instance); // Sets the panel to use.
    }
};

LGFX tft;
