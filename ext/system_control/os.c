#include "system_control.h"


/*
 * Gets the Mac OS X's version
 *
 * @return
 *   The version of Mac OS X that is decimal number.
 * @example
 *   version = System::OS::version
 *   sprintf("%x", version) # => "1071" if Mac OS X 10.7.1
 */
static VALUE
rb_sys_version(VALUE obj)
{
    SInt32 version = 0;
    Gestalt(gestaltSystemVersion, &version);
    return INT2NUM(version);
}

void Init_OS(void)
{
    VALUE mOS = rb_define_module_under(mSystem, "OS");
    rb_define_module_function(mOS, "version", rb_sys_version, 0);
}
