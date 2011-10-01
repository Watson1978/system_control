#include "system_control.h"
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/ps/IOPowerSources.h>

/*
 * Sleeps the machine.
 *
 * @example
 *   System::Power.sleep
 */
static VALUE
rb_sys_sleep(VALUE obj)
{
    mach_port_t  port;
    io_connect_t manage;

    if (IOMasterPort(bootstrap_port, &port) != kIOReturnSuccess) {
	rb_raise(rb_eRuntimeError, "Failed to get master port");
    }

    manage = IOPMFindPowerManagement(port);
    if (!manage) {
	rb_raise(rb_eRuntimeError, "Failed to find power management");
    }

    if (IOPMSleepSystem(manage) != kIOReturnSuccess) {
	rb_raise(rb_eRuntimeError, "Failed to sleep");
    }

    IOServiceClose(manage);
    return Qnil;
}

/*
 * Sleeps the only display.
 *
 * @example
 *   System::Power.sleep_display
 */
static VALUE
rb_sys_sleep_display(VALUE obj)
{
    io_registry_entry_t regist =
	IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/IOResources/IODisplayWrangler");
    if(!regist) {
	return Qnil;
    }

    IORegistryEntrySetCFProperty(regist, CFSTR("IORequestIdle"), kCFBooleanTrue);
    IOObjectRelease(regist);
    return Qnil;
}

/*
 * Prepares the non-sleep. This method is used by no_sleep.
 *
 * @see
 *   System::Power.no_sleep
 */
static VALUE
rb_sys_no_sleep_open(VALUE obj)
{
    IOPMAssertionID id;
    IOReturn ret = IOPMAssertionCreate(kIOPMAssertionTypeNoDisplaySleep,
				       kIOPMAssertionLevelOn, &id);
    if (ret != kIOReturnSuccess) {
	rb_raise(rb_eRuntimeError, "no sleep failed");
    }

    return INT2FIX((int)id);
}

/*
 * Cleans up the non-sleep. This method is used by no_sleep.
 *
 * @see
 *   System::Power.no_sleep
 */
static VALUE
rb_sys_no_sleep_close(VALUE obj, VALUE arg)
{
    IOPMAssertionRelease(FIX2INT(arg));
    return Qnil;
}

/*
 * Gets the battery informataions.
 *
 * @returns [Hash]
 *   battery informataions
 */
static VALUE
rb_sys_battery(VALUE obj)
{
    VALUE hash = rb_hash_new();
    CFDictionaryRef battery = (CFDictionaryRef)IOPSCopyPowerSourcesInfo();
    const long count = CFDictionaryGetCount(battery);
    const void **keys = (const void **)malloc(sizeof(void *) * count);
    const void **values = (const void **)malloc(sizeof(void *) * count);
    long i;

    CFDictionaryGetKeysAndValues(battery, keys, values);
    for (i = 0; i < count; i++) {
	rb_hash_aset(hash, (VALUE)keys[i], (VALUE)values[i]);
    }
    CFRelease((CFTypeRef)battery);

    return hash;
}

void Init_Power(void)
{
    VALUE mPower =  rb_define_module_under(mSystem, "Power");
    rb_define_module_function(mPower, "sleep", rb_sys_sleep, 0);
    rb_define_module_function(mPower, "sleep_display", rb_sys_sleep_display, 0);
    rb_define_module_function(mPower, "no_sleep_open", rb_sys_no_sleep_open, 0);
    rb_define_module_function(mPower, "no_sleep_close", rb_sys_no_sleep_close, 1);
    rb_define_module_function(mPower, "battery", rb_sys_battery, 0);
}
