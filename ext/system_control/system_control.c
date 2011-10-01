#include "system_control.h"

VALUE mSystem;

/*
 * The <code>System</code> module contains module function for
 * simple controlling a system.
 */
void Init_system_control(void)
{
    mSystem = rb_define_module("System");

    Init_Power();
    Init_Sound();
    Init_Network();
    Init_OS();
}
