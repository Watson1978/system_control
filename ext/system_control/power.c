#include "system_control.h"
#include <IOKit/pwr_mgt/IOPMLib.h>

/* == [System::Power] == */
/*
 *  Document-method: sleep
 *
 *  call-seq:
 *     System::Power.sleep
 *
 *  You can make your Mac sleep.
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
 *  Document-method: sleep_display
 *
 *  call-seq:
 *     System::Power.sleep_display
 *
 *  You can make your Mac's display sleep.
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

static VALUE
rb_sys_no_sleep_close(VALUE obj, VALUE arg)
{
    IOPMAssertionRelease(FIX2INT(arg));
}

void Init_Power(void)
{
    VALUE mPower =  rb_define_module_under(mSystem, "Power");
    rb_define_module_function(mPower, "sleep", rb_sys_sleep, 0);
    rb_define_module_function(mPower, "sleep_display", rb_sys_sleep_display, 0);
    rb_define_module_function(mPower, "no_sleep_open", rb_sys_no_sleep_open, 0);
    rb_define_module_function(mPower, "no_sleep_close", rb_sys_no_sleep_close, 1);
}
