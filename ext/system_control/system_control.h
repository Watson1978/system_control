#ifndef __SYSTEM_CONTROL_H__
#define __SYSTEM_CONTROL_H__

#include <ruby.h>
#import <CoreFoundation/CoreFoundation.h>
#import <Carbon/Carbon.h>

extern VALUE mSystem;

void Init_Screen(void);
void Init_Power(void);
void Init_Sound(void);
void Init_Network(void);
void Init_OS(void);

#endif /* __SYSTEM_CONTROL_H__ */
