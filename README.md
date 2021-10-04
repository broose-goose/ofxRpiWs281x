# ofxRpiWs281x

A thin cpp wrapper for the OpenFrameworks environment of the library [jgarff/rpi_ws281x](https://github.com/jgarff/rpi_ws281x). I wrote it out of necessity; it gave me some trouble figuring out how to integrate the library with OF, all the "existing" plugins for OF of this library didn't work, and I couldn't find shit for examples online.

## Overview

This library attempts to offload managing actual LEDs. It has methods for controlling global brightness, gamma color correction, RGB to RGBW conversion, and so on. It also has include guards for the actual rpi_ws281x library so that this library can be used in development on an X86 windows of linux box. Everything resides in the ofxRpiWs281x namespace to avoid pollution, and scoped enums are used because I like type safety, as well as making things super explicit to idiot proof things I might mess up.

## Basic Usage

This library exposes 2 main objects; a configuration object, and an LED strip object. 


The configuration object is created with some sane defaults that can be changed like so.


```
auto config = ofxRpiWs281x::LedStripConfiguration();
config.gamma = 1.2;
config.strip_type = ofxRpiWs281x::StripType::WS2811_GRB;
config.led_count = 100;
```

You can then instantiate an LED strip with it. Right now, I'm pretty sure it will only work for one LED strip, but it could be fixed to support 2 led strips running on the separate PWM channels the Pi provides (see [RPi Hardware PWM Timers](https://jumpnowtek.com/rpi/Using-the-Raspberry-Pi-Hardware-PWM-timers.html)).

```
auto led_strip_and_ret_value = ofxRpiWs281x::LedStrip::CreateLedStrip(config);
auto create_ret_value = led_strip_and_ret_value.second;
if (create_ret_value.isFailure()) {
	std::cout << "I encountered an oopsie creating the led strip D:" << std::endl;
	exit(create_ret_value.Ret());
}
auto led_strip = led_strip_and_ret_value.first;
```

Notice how the CreateLedStrip function actually returns a tupple; a pointer to the LED object, as well as a return value. I program a good deal in golang, and it felt obscene not to...

Anyways, now that we have our led strip, we need to initialize it (sets up the actual driver on the Pi), set some pixels, write to the strip itself, and tear it down those walls.

```
auto init_ret_value = led_strip->Initialize();
if (init_ret_value.isFailure()) {
	std::cout << "I COULDN'T INITIALIZE SHIT D:" << std::endl;
	exit(init_ret_value.Ret());
}

led_strip->SetColorStrip(ofColor::cornflowerBlue);

auto render_ret_value = led_strip->Render();
if (render_ret_value.isFailure()) {
	std::cout << "I cOuLdN't WrItE tO tHe StRiPs - __ -" << std::endl;
	exit(render_ret_value.Ret());
}

led_strip->Teardown();
```

And you're now a super 1337 LED pro!

## Program Usage

You can set the value of individual pixels, calling the SetColorPixel for a LED position, like so:

```
ofColor Wheel(int pos) {
    if (pos < 85) {
        return ofColor(pos * 3, 255 - pos * 3, 0);
    } else if (pos < 170) {
        pos -= 85;
        return ofColor(255 - pos * 3, 0, pos * 3);
    } else {
        pos -= 170;
        return ofColor(0, pos * 3, 255 - pos * 3);
    }
}

void ofApp::update(){
  static uint8_t  iteration = 0;
  const int stripCount = strip->StripCount();
  for (int i = 0; i < stripCount; i++) {
    if (i % 3 != 0) {
      continue;
    }
    const int pos = (int)(i * 256 / (float)stripCount + iteration);
    const int usablePos = pos % 255;
    const ofColor c = Wheel(usablePos);
    strip->SetColorPixel(c, i); 
  }
  if (iteration == 255) {
    iteration = 0;
  } else {
    iteration += 1;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
}
```

But I don't recommend it. Then you gotta be passin that strip object everywhere, it enforces coupling, etc.

My preferred  usage, which I'm going to foist on you is as follows.

1. Create the global strip pointer on the ofApp instance.
2. Use the simple interface ofxRpiWs281x::LedOutputGenerator to pass the strip to another function handling lights; this way, we only expose one function, less coupling and what not.
3. Use the GetPixel function on the strip to get a pointer to the underlying ofColor at that led position. Write to that.

We can do this because, for each LED on the strip, we keep a local ofColor object that we write to the rpi_ws281x library objects when we call the render function. We are on a Pi, we have clock cycles to burn, and this makes things more explicit. W.e pixels are there after the update call will be pushed during the draw call. That lets us do multiple operations to the lights during the update, and other things I'm too tired to remember as benifits... 

Besides, if you are already using OF, you're probably doing some complex LED mapping that requires hire levels of abstraction. Look at me, thinking of other reasons I made decisions :D

It ends up looking like this

```
void ofApp::setup(){
	strip_ = POINTER_TO_STRIP_WE_SHOWED_HOW_TO_CREATE_ABOVE

	Light::SetupLights(strip_)
}

class Light {
public:
	static void SetupLights(ofxRpiWs281x::LedOutputGenerator* og) {
		auto num_lights = og->StripCount();
		for (int i = 0; i < num_lights; i++) {
			auto light = new Light(og->GetPixel(i));
			all_lights.push_back(light);
		}
	}
	static void DrawLight() {
		for (auto light: all_lights) {
			light->doDrawLight();
		}
	}
private:
	void doDrawLight() {
		memcpy(output_color_->v, current_color_.v, sizeof(uint8_t) * 3);
	}
	static std::vector<Light* > all_lights;
	ofColor current_color;
	ofColor* output_color;
};


void ofApp::update(){
	// SOMEHOW UPDATE THE CURRENT COLOR OF EACH LIGHT
}

void ofApp::draw(){
	Light::DrawLight();
	
	auto render_ret_value = led_strip->Render();
	if (render_ret_value.isFailure()) {
		std::cout << "I cOuLdN't WrItE tO tHe StRiPs - __ -" << std::endl;
		exit(render_ret_value.Ret());
	}
}

```

I'm going to throw a super complicated example in the examples folder if you want more details on how to implement things this way. It lets you abstract things away, and is super nice to work with once you get the hang of it.


## Dependencies

My ass.

Real talk though, technically the [jgarff/rpi_ws281x](https://github.com/jgarff/rpi_ws281x) library, but I just included the most recent version of it in the src files here. Shouldn't be a problem until they come out with an RPi 5.