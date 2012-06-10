require "mkmf"
$CFLAGS << " -std=c99 "
$LDFLAGS << " -framework CoreAudio -framework ApplicationServices "
create_makefile("system_control/system_control")
