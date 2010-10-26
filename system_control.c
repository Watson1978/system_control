#include <ruby/macruby.h>
#include <ruby.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#import <CoreAudio/CoreAudio.h>
#import <AudioToolbox/AudioServices.h>
#include <IOKit/pwr_mgt/IOPMLib.h>

/*
 * call-seq: System::Power.sleep
 */
static void
rb_sys_sleep(VALUE obj, SEL sel)
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
}

static AudioDeviceID
get_audio_device_id(void)
{
    AudioDeviceID device;
    OSStatus      err;
    UInt32        size;

    size = sizeof(device);
    err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &size, &device);
    if (err != noErr) {
	rb_raise(rb_eRuntimeError, "Failed to get device id");
    }

    return device;
}

/*
 * call-seq: System::Sound.volume -> volume
 */
static VALUE
rb_sys_volume(VALUE obj, SEL sel)
{
    AudioDeviceID device;
    OSStatus      err;
    UInt32        size;
    UInt32        channels[2];
    float         volume[2];
    float         b_vol = 0.0;

    // get device
    device = get_audio_device_id();

    // try set master volume (channel 0)
    size = sizeof(b_vol);
    err = AudioDeviceGetProperty(device, 0, 0, kAudioDevicePropertyVolumeScalar, &size, &b_vol);
    if (err == noErr) {
	goto EXIT;
    }

    // otherwise, try seperate channels
    // get channel numbers
    size = sizeof(channels);
    err = AudioDeviceGetProperty(device, 0, 0,kAudioDevicePropertyPreferredChannelsForStereo, &size, &channels);
    if (err != noErr) {
	rb_raise(rb_eRuntimeError, "Failed to get channel numbers");
    }

    size = sizeof(float);
    err = AudioDeviceGetProperty(device, channels[0], 0, kAudioDevicePropertyVolumeScalar, &size, &volume[0]);
    if (err != noErr) {
	rb_raise(rb_eRuntimeError, "Failed to get volume of channel");
    }
    err = AudioDeviceGetProperty(device, channels[1], 0, kAudioDevicePropertyVolumeScalar, &size, &volume[1]);
    if (err != noErr) {
	rb_raise(rb_eRuntimeError, "Failed to get volume of channel");
    }

    b_vol = (volume[0] + volume[1]) / 2.0;

  EXIT:
    return DOUBLE2NUM(b_vol);
}

/*
 * call-seq: System::Sound.set_volume(volume)
 *
 * Set a system volume.
 *   range of volume = 0.0 .. 1.0
 *
 */
static void
rb_sys_set_volume(VALUE obj, SEL sel, VALUE volume)
{
    AudioDeviceID device;
    OSStatus      err;
    UInt32        size;
    Boolean       canset = false;
    UInt32        channels[2];
    float         involume;

    if (!FIXFLOAT_P(volume)) {
	rb_raise(rb_eTypeError, "wrong type of argument");
    }

    involume = NUM2DBL(volume);
    if (involume < 0.0 || involume > 1.0) {
	rb_raise(rb_eRangeError, "out of range");
    }

    // get device
    device = get_audio_device_id();

    size = sizeof canset;
    err = AudioDeviceGetPropertyInfo(device, 0, false, kAudioDevicePropertyVolumeScalar, &size, &canset);
    if (err == noErr && canset == true) {
	size = sizeof involume;
	err = AudioDeviceSetProperty(device, NULL, 0, false, kAudioDevicePropertyVolumeScalar, size, &involume);
	return;
    }

    // else, try seperate channes
    // get channels
    size = sizeof(channels);
    err = AudioDeviceGetProperty(device, 0, false, kAudioDevicePropertyPreferredChannelsForStereo, &size, &channels);
    if (err != noErr) {
	rb_raise(rb_eRuntimeError, "Failed to get channel numbers");
    }

    // set volume
    size = sizeof(float);
    err = AudioDeviceSetProperty(device, 0, channels[0], false, kAudioDevicePropertyVolumeScalar, size, &involume);
    if (err != noErr) {
	rb_raise(rb_eRuntimeError, "Failed to set volume of channel");
    }
    err = AudioDeviceSetProperty(device, 0, channels[1], false, kAudioDevicePropertyVolumeScalar, size, &involume);
    if (err != noErr) {
	rb_raise(rb_eRuntimeError, "Failed to set volume of channel");
    }
}

static int
conv_char2bin(char *str, int length, unsigned long *out)
{
    unsigned long d = 0;
    char c;
    int  i;

    for (i = 0; i < length; i++) {
	d = d << 4;

	c = str[i];
	if ((c >= '0') && (c <= '9')) {
	    d += c - '0';
	}
	else if ((c >= 'a') && (c <= 'f')) {
	    d += c - 'a' + 10;
	}
	else if ((c >= 'A') && (c <= 'F')) {
	    d += c - 'A' + 10;
	}
	else {
	    return false;
	}
    }

    *out = d;
    return true;
}

/*
 * call-seq: System::Network.wake(macaddress)
 *
 * Wake up a sleeping machine, using Wake-on-Lan.
 *   macaddress = "xx:xx:xx:xx:xx:xx"
 *
 * This method is unsupported with MacRuby 0.7.
 */
static char  buffer[6 * (16 + 1)];
static char  b_addr[6];

static void
rb_sys_wake_on_lan(VALUE obj, SEL sel, VALUE arg)
{
    VALUE addr;
    int   i;
    int   ret;

     switch (TYPE(arg)) {
     case T_STRING:
	 addr = rb_str_split(arg, ":"); // do not implement on MacRuby 0.7
	 break;

     default:
	 rb_raise(rb_eTypeError, "wrong type of argument");
     }

     const int length = rb_ary_len(addr);
     if (length != 6) {
	 rb_raise(rb_eArgError, "invalid address");
     }

     for (i = 0; i < length; i++) {
	 VALUE value  = RARRAY_AT(addr, i);
	 char* string = StringValuePtr(value);
	 int   len    = strlen(string);
	 unsigned long data = 0;

	 if (len > 2) {
	     rb_raise(rb_eArgError, "invalid address");
	 }

	 ret = conv_char2bin(string, len, &data);
	 if (ret == false) {
	     rb_raise(rb_eArgError, "invalid address");
	 }

	 b_addr[i] = (char)(data & 0xff);
     }

     // make a send data
     int offset;
     memset(&buffer[0], 0xff, 6);
     for (i = 0; i < 16; i++) {
	 offset += 6;
	 memcpy(&buffer[offset], (const void*)b_addr, 6);
     }

     int sock;
     int broadcast = 1;
     struct sockaddr_in dstaddr;

     sock = socket(AF_INET, SOCK_DGRAM, 0);
     if (sock == -1) {
	 rb_raise(rb_eRuntimeError, "Failed to open the UDP socket");
     }

     memset(&dstaddr, 0, sizeof(dstaddr));
     dstaddr.sin_family = AF_INET;
     dstaddr.sin_port   = htons(12345); // any
     dstaddr.sin_addr.s_addr = inet_addr("255.255.255.255");

     ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		      (char *)&broadcast, sizeof(broadcast));
     if (ret == -1) {
	 close(sock);
	 rb_raise(rb_eRuntimeError, "setsockopt() error");
     }

     ret = sendto(sock, buffer, sizeof(buffer) , 0,
		  (struct sockaddr *)&dstaddr, sizeof(dstaddr));
     if (ret == -1) {
	 close(sock);
	 rb_raise(rb_eRuntimeError, "sendto() error");
     }

     close(sock);
}

void Init_system_control(void)
{
    VALUE mSystem = rb_define_module("System");

    VALUE mPower =  rb_define_module_under(mSystem, "Power");
    rb_objc_define_module_function(mPower, "sleep", rb_sys_sleep, 0);

    VALUE mSound =  rb_define_module_under(mSystem, "Sound");
    rb_objc_define_module_function(mSound, "volume",     rb_sys_volume, 0);
    rb_objc_define_module_function(mSound, "volume=",    rb_sys_set_volume, 1);
    rb_objc_define_module_function(mSound, "set_volume", rb_sys_set_volume, 1);

    VALUE mNetwork =  rb_define_module_under(mSystem, "Network");
    rb_objc_define_module_function(mNetwork, "wake", rb_sys_wake_on_lan, 1);
}
