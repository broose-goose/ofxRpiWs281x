
#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <utility>

#ifdef __arm__

#include "rpi_ws281x/clk.h"
#include "rpi_ws281x/gpio.h"
#include "rpi_ws281x/dma.h"
#include "rpi_ws281x/pwm.h"

#include "rpi_ws281x/ws2811.h"

#endif

#include "ofColor.h"


namespace ofxRpiWs281x {

    enum class GpioPins: int {
        GPIO_12 = 12,
        GPIO_18 = 18,
        GPIO_13 = 13,
        GPIO_19 = 19
    };

    enum class StripType: int {
        WS2811_RGB = 0x00100800,
        WS2811_RBG = 0x00100008,
        WS2811_GRB = 0x00081000,
        WS2811_GBR = 0x00080010,
        WS2811_BRG = 0x00001008,
        WS2811_BGR = 0x00000810,

        SK6812_RGBW = 0x18100800,
        SK6812_RBGW = 0x18100008,
        SK6812_GRBW = 0x18081000,
        SK6812_GBRW = 0x18080010,
        SK6812_BRGW = 0x18001008,
        SK6812_BGRW = 0x18000810,
    };

    class LedStrip;
    class ReturnValue {
        public:
            bool isFailure();
            int Ret() {
                return (int) _ret;
            }
        private:
            ReturnValue(ws2811_return_t ret) : _ret(ret) {};
            ws2811_return_t _ret;
            friend class LedStrip;
    };

    struct LedStripConfiguration {
        // some sane defaults
        LedStripConfiguration() {
            gpio_pin = GpioPins::GPIO_18;
            led_count = 1;
            strip_type = StripType::WS2811_GRB;
            invert = false;
            brightness = 255;

            dma_number = 10;
            frequency = 800000;
        }
        GpioPins gpio_pin;
        uint16_t led_count;
        StripType strip_type;
        bool invert; // True to invert the signal (when using NPN transistor level shift)
        uint8_t brightness;
        uint8_t dma_number; // cannot be 0, 1, 2, 3, 6, 7; must be smaller than 15
        uint32_t frequency; // usually 800000 (800kHz)
    };

    class LedStrip {

        public:
            LedStrip() = delete;
            ~LedStrip() = delete;
            LedStrip(const LedStrip&) = delete;
            LedStrip& operator=(const LedStrip&) = delete;

            static std::pair<LedStrip*, ReturnValue> CreateLedStrip(LedStripConfiguration);

            ReturnValue Initialize();
            ReturnValue Render();
            ReturnValue Teardown();

            void SetColorPixel(ofColor c, uint16_t pixel);
            void SetColorStrip(ofColor);

        private:

            LedStrip(LedStripConfiguration);

            static uint32_t wrgbFromOfColor(ofColor);

            ws2811_t _strip;
            GpioPins _gpio_pin;
            ws2811_channel_t *_channel;
    };

}