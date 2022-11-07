AM_CFLAGS = -D_ANSC_LINUX
AM_CFLAGS += -D_ANSC_USER
AM_CFLAGS += -D_ANSC_LITTLE_ENDIAN_

AM_CPPFLAGS = -Wall -Werror
ACLOCAL_AMFLAGS = -I m4
hardware_platform = armv7ahf-neon-rdk-linux-musleabi

bin_PROGRAMS = wifi_test_tool
wifi_test_tool_CPPFLAGS = -I$(top_srcdir)/include $(CPPFLAGS) -D_WIFI_HAL_TEST_
wifi_test_tool_SOURCES = wifi-test-tool.c
wifi_test_tool_LDFLAGS = -lhal_wifi

