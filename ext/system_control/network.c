#include "system_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* :nodoc: */
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
 * Wake up a sleeping machine, using Wake-on-Lan.
 *
 * @param [String, Array] macaddr
 *   The argument as macaddress pass a value.
 * @example
 *   macaddress = "xx:xx:xx:xx:xx:xx"            # String
 *   System::Network.wake(macaddress)
 *
 *   macaddress = "xx:xx:xx:xx:xx:xx".split(":") # Array
 *   System::Network.wake(macaddress)
 *
 * @since
 *   This method is supported with MacRuby 0.7+.
 */
static VALUE
rb_sys_wake_on_lan(VALUE obj, VALUE macaddr)
{
    VALUE addr;
    int   i;
    int   ret;
    char* buffer;
    char* b_addr;

    switch (TYPE(arg)) {
    case T_STRING:
	addr = rb_str_split(macaddr, ":");
	break;

    case T_ARRAY:
	addr = macaddr;
	break;

    default:
	rb_raise(rb_eTypeError, "wrong type of argument");
    }

    const int length = rb_ary_len(addr);
    if (length != 6) {
	rb_raise(rb_eArgError, "invalid address");
    }

    buffer = (char*)xmalloc(6 * 17);
    b_addr = (char*)xmalloc(6);
    for (i = 0; i < length; i++) {
	VALUE value  = RARRAY_AT(addr, i);
	if (TYPE(value) != T_STRING) {
	    rb_raise(rb_eTypeError, "wrong type of argument");
	}

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
    int offset = 0;
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
    return Qnil;
}

void Init_Network(void)
{
    VALUE mNetwork =  rb_define_module_under(mSystem, "Network");
    rb_define_module_function(mNetwork, "wake", rb_sys_wake_on_lan, 1);
}
