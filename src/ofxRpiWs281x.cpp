
#include "ofxRpiWs281x.h"

namespace ofxRpiWs281x {



    bool ReturnValue::isFailure() {
        return _ret != 0;
    }



    uint32_t LedStrip::wrgbFromOfColor(ofColor *c) {
        return ((_white_mask & (unsigned char) c->a) << 24) |
        ((_red_mask & (unsigned char) c->r) << 16) |
		((_green_mask & (unsigned char) c->g) << 8) |
		((_blue_mask & (unsigned char) c->b));
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

        _is_initialized = false;

        _led_count = conf.led_count;
        _gpio_pin = conf.gpio_pin;

        _red_mask = conf.red_mask;
        _green_mask = conf.green_mask;
        _blue_mask = conf.blue_mask;
        _white_mask = conf.white_mask;

        _pixels = std::vector<ofColor*>{};
        for (uint16_t pix = 0; pix < _led_count; pix++) {
            _pixels.push_back( new ofColor(0, 0, 0));
        }
    }



    ReturnValue LedStrip::Initialize() {
#ifdef __arm__
        if (_is_initialized) {
            std::cout << "LedStrip, Initialize: nothing to do..." << std::endl;
            return ReturnValue(WS2811_SUCCESS);
        }
        _is_initialized = true;
        ws2811_return_t ret = ws2811_init(&_strip);
        if (_gpio_pin == GpioPins::GPIO_18 || _gpio_pin == GpioPins::GPIO_12) {
            _channel = &_strip.channel[0];
        }
        return ReturnValue(ret);
#else
        if (_is_initialized) {
            std::cout << "LedStrip, Initialize: nothing to do..." << std::endl;
            return ReturnValue(0);
        }
        _is_initialized = true;
        std::cout << "LedStrip: Initialize" << std::endl;
        return ReturnValue(0);
#endif
    }



    ReturnValue LedStrip::Render() {

#ifdef __arm__
        if (!_is_initialized) {
            std::cout << "LedStrip, Render: can't render shit..." << std::endl;
            return ReturnValue(WS2811_ERROR_GPIO_INIT);
        }
        for (auto it = _pixels.begin(); it != _pixels.end(); ++it) {
            int index = std::distance(_pixels.begin(), it);
            auto pixel = *it;
            _channel->leds[index] = wrgbFromOfColor(pixel);
        }
        ws2811_return_t ret = ws2811_render(&_strip);
        return ReturnValue(ret);
#else
        if (!_is_initialized) {
            std::cout << "LedStrip, Render: can't render shit..." << std::endl;
            return ReturnValue(-1);
        }
#ifdef DEBUG
        std::cout << "LedStrip: Render" << std::endl;
#endif
        return ReturnValue(0);
#endif
    }



    ReturnValue LedStrip::Teardown() {
#ifdef __arm__
        if (!_is_initialized) {
            std::cout << "LedStrip, Teardown: nothing to do..." << std::endl;
            return ReturnValue(WS2811_SUCCESS);
        }
        _is_initialized = false;
        ws2811_fini(&_strip);
        return ReturnValue(WS2811_SUCCESS);
#else
        if (!_is_initialized) {
            std::cout << "LedStrip, Teardown: nothing to do..." << std::endl;
            return ReturnValue(0);
        }
        _is_initialized = false;
        std::cout << "LedStrip: Teardown" << std::endl;
        return ReturnValue(0);
#endif
    }


    ofColor* LedStrip::GetPixel(uint16_t pixel) {
        if (pixel >= _led_count) {
            std::cout << "LedStrip, GetPixel: Pixel out of range, returning dummy pixel" << std::endl;
            return new ofColor(0,0,0);
        } else {
            return _pixels.at(pixel);
        }
    }



    void LedStrip::SetColorPixel(const ofColor &c, uint16_t pixel) {
        if (pixel >= _led_count) {
            std::cout << "LedStrip, SetColorPixel: Pixel out of range" << std::endl;
        } else {
            _pixels.at(pixel)->set(c);
        }
    }



    void LedStrip::SetColorStrip(const ofColor &c) {
        for (auto pixel : _pixels) {
            pixel->set(c);
        }
    }



}