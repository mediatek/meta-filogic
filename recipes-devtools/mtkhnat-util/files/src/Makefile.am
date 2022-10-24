AM_CFLAGS = -D_ANSC_LINUX
AM_CFLAGS += -D_ANSC_USER
AM_CFLAGS += -D_ANSC_LITTLE_ENDIAN_

AM_CPPFLAGS = -Wall -Werror
ACLOCAL_AMFLAGS = -I m4
hardware_platform = armv7ahf-neon-rdk-linux-musleabi

bin_PROGRAMS = mtkhnat
mtkhnat_CPPFLAGS = -I$(top_srcdir)/include $(CPPFLAGS) 
mtkhnat_SOURCES = mtkhnat.c


