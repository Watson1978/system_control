#ifndef __SYSTEM_CONTROL_H__
#define __SYSTEM_CONTROL_H__

#include <ruby.h>
#import <CoreFoundation/CoreFoundation.h>

extern VALUE mSystem;

void Init_Power(void);
void Init_Sound(void);
void Init_Network(void);

#endif /* __SYSTEM_CONTROL_H__ */
