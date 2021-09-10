
#pragma once

#include <vector>
#include <memory>
#include <iostream>

#include "ofColor.h"

struct ws2811_t;
enum ws2811_return_t;

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

    class ReturnValue {
        public:
            bool isFailure();
            friend std::ostream &operator<<(std::ostream &os, const ReturnValue& ret) {
                return _ret;
            }
        private:
            ReturnValue(ws2811_return_t ret) : _ret(ret);
            ws2811_return_t _ret
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
            // static ReturnValue InitializeAll();
            ReturnValue Render();
            // static ReturnValue RenderAll();
            ReturnValue Teardown();
            // static ReturnValue TeardownAll();

            void SetColorPixel(ofColor c, uint16_t pixel);
            void SetColorStrip(ofColor);
            // void SetColorAll(ofColor);

        private:

            static uint32_t wrgbFromOfColor(ofColor);

            LedStrip(LedStripConfiguration);

            // static std::vector<LedStrip*> _strips;
            ws2811_t *_strip;
            ws2811_channel_t *_channel;
    };

}