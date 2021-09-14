
#include "ofxRpiWs281x.h"

namespace ofxRpiWs281x {

    bool ReturnValue::isFailure() {
        return _ret != 0;
    }

    uint32_t LedStrip::wrgbFromOfColor(ofColor c) {
        return ((0xff & (unsigned char) c.a) << 24) |
        ((0xff & (unsigned char) c.r) << 16) |
		((0xff & (unsigned char) c.g) << 8) |
		((0xff & (unsigned char) c.b));
    }

    std::pair<LedStrip*, ReturnValue> LedStrip::CreateLedStrip(LedStripConfiguration conf) {
        LedStrip *strip = new LedStrip(conf);
        return std::make_pair(strip, ReturnValue(WS2811_SUCCESS));
    }

    LedStrip::LedStrip(LedStripConfiguration conf) {
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
        _gpio_pin = conf.gpio_pin;
        if (conf.gpio_pin == GpioPins::GPIO_18 || conf.gpio_pin == GpioPins::GPIO_12) {
            _is_channel_0 = true;
            strip_obj.channel[0] = main_channel;
            strip_obj.channel[1] = dummy_channel;
        } else {
            _is_channel_0 = false;
            strip_obj.channel[0] = dummy_channel;
            strip_obj.channel[1] = main_channel;
        }

        _strip = strip_obj;
    }

    ReturnValue LedStrip::Initialize() {
        ws2811_return_t ret = ws2811_init(&_strip);
        if (_gpio_pin == GpioPins::GPIO_18 || _gpio_pin == GpioPins::GPIO_12) {
            _channel = &_strip.channel[0];
        }
        return ReturnValue(ret);
    }

    ReturnValue LedStrip::Render() {
        ws2811_return_t ret = ws2811_render(&_strip);
        return ReturnValue(ret);
    }

    ReturnValue LedStrip::Teardown() {
        ws2811_fini(&_strip);
        return ReturnValue(WS2811_SUCCESS);
    }


    void LedStrip::SetColorPixel(ofColor c, uint16_t pixel) {
        if (pixel < _channel->count) {
            _channel->leds[0] = wrgbFromOfColor(c);
        } else {
            std::cout << "Pixel out of range" << std::endl;
        }
    }

    void LedStrip::SetColorStrip(ofColor c) {
        uint32_t c_out = wrgbFromOfColor(c);
        for (int i = 0; i < _channel->count; i++) {
            _channel->leds[i] = c_out;
        }
    }

    /*
    LedStrip::SetColorAll(ofColor c) {
        for (auto &led_strip : _strips) {
            led_strip->SetColorStrip(c);
        }
    }
    */
}