#include "system_control.h"
#include <unistd.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <ApplicationServices/ApplicationServices.h>
#define MAX_DISPLAYS 16

static const CFStringRef kDisplayBrightness = CFSTR(kIODisplayBrightnessKey);

static CGDisplayCount
get_displays(CGDirectDisplayID *display_ids)
{
    CGDisplayCount num_displays;
    CGDisplayErr err;
    err = CGGetActiveDisplayList(MAX_DISPLAYS, display_ids, &num_displays);
    if (err != CGDisplayNoErr) {
	rb_raise(rb_eRuntimeError, "cannot get list of displays (error %d)", err);
    }
    return num_displays;
}

/*
 * Gets the brightness of display
 *
 * @param [Float] brightness
 *   The value of brightness
 *   range of brightness is 0.0 .. 1.0.
 * @example
 *   System::Sound.brightness
 */
static VALUE
rb_sys_brightness(VALUE obj)
{
    CGDirectDisplayID display[MAX_DISPLAYS];
    CGDisplayCount num_displays;
    CGDisplayErr err;

    num_displays = get_displays(display);

    for (CGDisplayCount i = 0; i < num_displays; ++i) {
	CGDirectDisplayID id_display = display[i];
	CFDictionaryRef mode = CGDisplayCurrentMode(id_display);
	if (mode == NULL) {
	    continue;
	}

	io_service_t service = CGDisplayIOServicePort(id_display);
	float brightness;
        err = IODisplayGetFloatParameter(service, kNilOptions, kDisplayBrightness, &brightness);
        if (err != kIOReturnSuccess) {
	    rb_raise(rb_eRuntimeError, "failed to get brightness of display %d (error %d)", id_display, err);
        }
	return rb_float_new((double)brightness);
    }

    return Qnil;
}

/*
 * Sets the brightness of display
 *
 * @param [Float] brightness
 *   The value that set brightness
 *   range of brightness is 0.0 .. 1.0.
 * @example
 *   System::Sound.brightness = 0.5
 */
static VALUE
rb_sys_setbrightness(VALUE obj, VALUE arg)
{
    CGDirectDisplayID display[MAX_DISPLAYS];
    CGDisplayCount num_displays;
    CGDisplayErr err;
    float brightness;

    if (!FIXFLOAT_P(arg)) {
	rb_raise(rb_eTypeError, "wrong type of argument");
    }
    num_displays = get_displays(display);
    brightness = FIXFLOAT2DBL(arg);
    if (brightness < 0.0 || brightness > 1.0) {
	rb_raise(rb_eRangeError, "out of range");
    }

    for (CGDisplayCount i = 0; i < num_displays; ++i) {
	CGDirectDisplayID id_display = display[i];
	CFDictionaryRef mode = CGDisplayCurrentMode(id_display);
	if (mode == NULL) {
	    continue;
	}

	io_service_t service = CGDisplayIOServicePort(id_display);
        err = IODisplaySetFloatParameter(service, kNilOptions, kDisplayBrightness, brightness);
        if (err != kIOReturnSuccess) {
	    rb_raise(rb_eRuntimeError, "failed to set brightness of display %d (error %d)", id_display, err);
        }
    }

    return Qnil;
}

void Init_Screen(void)
{
    VALUE mScreen =  rb_define_module_under(mSystem, "Screen");
    rb_define_module_function(mScreen, "brightness",     rb_sys_brightness, 0);
    rb_define_module_function(mScreen, "set_brightness", rb_sys_setbrightness, 1);
    rb_define_module_function(mScreen, "brightness=",    rb_sys_setbrightness, 1);
}
