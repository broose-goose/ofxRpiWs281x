
#include "ofxRpiWs281x.h"

namespace ofxRpiWs281x {



    bool ReturnValue::isFailure() {
        return _ret != 0;
    }



    uint32_t LedStrip::wrgbFromOfColor(ofColor c) {
        return ((_white_mask & (unsigned char) c.a) << 24) |
        ((_red_mask & (unsigned char) c.r) << 16) |
		((_green_mask & (unsigned char) c.g) << 8) |
		((_blue_mask & (unsigned char) c.b));
    }



    std::pair<LedStrip*, ReturnValue> LedStrip::CreateLedStrip(LedStripConfiguration conf) {
        LedStrip *strip = new LedStrip(conf);
        return std::make_pair(strip, ReturnValue(WS2811_SUCCESS));
    }



    LedStrip::LedStrip(LedStripConfiguration conf) {
#ifdef __arm__
        ws2811_channel_t main_channel = { 0 };
        ws2811_channel_t dummy_channel = { 0 };

        main_channel.gpionum = (int)conf.gpio_pin;
        main_channel.count = conf.led_count;
        main_channel.strip_type = (int)conf.strip_type;
        main_channel.brightness = conf.brightness;
        main_channel.invert = conf.invert;

        ws2811_t strip_obj = ws2811_t();
        strip_obj.freq = conf.frequency;
        strip_obj.dmanum = conf.dma_number;
        if (conf.gpio_pin == GpioPins::GPIO_18 || conf.gpio_pin == GpioPins::GPIO_12) {
            strip_obj.channel[0] = main_channel;
            strip_obj.channel[1] = dummy_channel;
        } else {
            strip_obj.channel[0] = dummy_channel;
            strip_obj.channel[1] = main_channel;
        }

        _strip = strip_obj;
#else
        std::cout << "LedStrip: Not on rpi, dummy strip" << std::endl;
#endif
        _led_count = conf.led_count;
        _gpio_pin = conf.gpio_pin;

        _red_mask = conf.red_mask;
        _green_mask = conf.green_mask;
        _blue_mask = conf.blue_mask;
        _white_mask = conf.white_mask;
    }



    ReturnValue LedStrip::Initialize() {
#ifdef __arm__
        ws2811_return_t ret = ws2811_init(&_strip);
        if (_gpio_pin == GpioPins::GPIO_18 || _gpio_pin == GpioPins::GPIO_12) {
            _channel = &_strip.channel[0];
        }
        return ReturnValue(ret);
#else
        std::cout << "LedStrip: Initialize" << std::endl;
        return ReturnValue(0);
#endif
    }



    ReturnValue LedStrip::Render() {
#ifdef __arm__
        ws2811_return_t ret = ws2811_render(&_strip);
        return ReturnValue(ret);
#else
#ifdef DEBUG
        std::cout << "LedStrip: Render" << std::endl;
#endif
        return ReturnValue(0);
#endif
    }



    ReturnValue LedStrip::Teardown() {
#ifdef __arm__
        ws2811_fini(&_strip);
        return ReturnValue(WS2811_SUCCESS);
#else
        std::cout << "LedStrip: Teardown" << std::endl;
        return ReturnValue(0);
#endif
    }



    void LedStrip::SetColorPixel(ofColor c, uint16_t pixel) {
#ifdef __arm__
        if (pixel < _channel->count) {
            _channel->leds[0] = wrgbFromOfColor(c);
        } else {
            std::cout << "Pixel out of range" << std::endl;
        }
#else
        if (pixel >= _led_count) {
            std::cout << "Pixel out of range" << std::endl;
        }
#endif
    }



    void LedStrip::SetColorStrip(ofColor c) {
        uint32_t c_out = wrgbFromOfColor(c);
#ifdef __arm__
        for (int i = 0; i < _channel->count; i++) {
            _channel->leds[i] = c_out;
        }
#else
        std::cout << "LedStrip, SetColorStrip: All LEDS " << c << std::endl;
#endif
    }



}