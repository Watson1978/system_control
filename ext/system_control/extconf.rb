require "mkmf"
$CFLAGS << " -std=c99 "
$LDFLAGS << " -framework ApplicationServices "
create_makefile("system_control/system_control")
