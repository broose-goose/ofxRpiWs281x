
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

    std::pair<LedStrip*, ReturnValue> CreateLedStrip(LedStripConfiguration conf) {
        LedStrip *strip = new LedStrip(conf);
        return std::make_pair(strip, ReturnValue(WS2811_SUCCESS));
    }

    LedStrip::LedStrip(LedStripConfiguration conf) {
        _channel = new ws2811_channel_t();
        ws2811_channel_t dummy_channel;

        _channel->gpionum = (int)conf.gpio_pin;
        _channel->count = conf.led_count;
        _channel->strip_type = (int)conf.strip_type;
        _channel->brightness = conf.brightness;
        _channel->invert = conf.invert;

        ws2811_t *strip_obj = new ws2811_t();
        strip_obj->freq = conf.frequency;
        strip_obj->dmanum = conf.dma_number;
        if (conf.gpio_pin == GpioPins::GPIO_18 || conf.gpio_pin == GpioPins::GPIO_12) {
            strip_obj->channel[0] = *_channel;
            strip_obj->channel[1] = dummy_channel;
        } else {
            strip_obj->channel[0] = dummy_channel;
            strip_obj->channel[1] = *_channel;
        }

        _strip = strip_obj;
    }

    ReturnValue LedStrip::Initialize() {
        ws2811_return_t ret = ws2811_init(_strip);
        return ReturnValue(ret);
    }

    ReturnValue LedStrip::Render() {
        ws2811_return_t ret = ws2811_render(_strip);
        return ReturnValue(ret);
    }

    ReturnValue LedStrip::Teardown() {
        ws2811_return_t ret = ws2811_render(_strip);
        return ReturnValue(ret);
    }


    void LedStrip::SetColorPixel(ofColor c, uint16_t pixel) {
        if (pixel < _channel->count) {
            _channel->leds[pixel] = wrgbFromOfColor(c);
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