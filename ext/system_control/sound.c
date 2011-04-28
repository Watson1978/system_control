#include "system_control.h"
#import <CoreAudio/CoreAudio.h>
#import <AudioToolbox/AudioServices.h>

/* == [System::Sound] == */
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
 *  Document-method: volume
 *
 *  call-seq:
 *     System::Sound.volume -> volume
 *
 *  You could get a current sound volume of system.
 *     range of volume = 0.0 .. 1.0
 */
static VALUE
rb_sys_volume(VALUE obj)
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
 *  Document-method: set_volume
 *
 *  call-seq:
 *     System::Sound.set_volume(volume)
 *
 *  Set a sound volume of system.
 *     range of volume = 0.0 .. 1.0
 */
static VALUE
rb_sys_set_volume(VALUE obj, VALUE volume)
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

    size = sizeof(canset);
    err = AudioDeviceGetPropertyInfo(device, 0, false, kAudioDevicePropertyVolumeScalar, &size, &canset);
    if (err == noErr && canset == true) {
	size = sizeof involume;
	err = AudioDeviceSetProperty(device, NULL, 0, false, kAudioDevicePropertyVolumeScalar, size, &involume);
	return Qnil;
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

    return Qnil;
}

/*
 *  Document-method: volume=
 *
 *  call-seq:
 *     System::Sound.volume = volume
 *
 *  Set a sound volume of system.
 *     range of volume = 0.0 .. 1.0
 */
static VALUE
rb_sys_set_volume_aset(VALUE obj, VALUE volume)
{
    return rb_sys_set_volume(obj, volume);
}

void Init_Sound(void)
{
    VALUE mSound =  rb_define_module_under(mSystem, "Sound");
    rb_define_module_function(mSound, "volume",     rb_sys_volume, 0);
    rb_define_module_function(mSound, "volume=",    rb_sys_set_volume_aset, 1);
    rb_define_module_function(mSound, "set_volume", rb_sys_set_volume, 1);
}
