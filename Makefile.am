DIST_SUBDIRS = t

SUBDIRS = . t

lib_LTLIBRARIES = libhls.la

include_HEADERS = hls.h

bin_PROGRAMS = hls

libhls_la_SOURCES = \
	hls_m3u8.c \
	hls.h

libhls_la_LDFLAGS = -avoid-version

hls_SOURCES = hls.c hls.h
hls_LDADD = libhls.la

test: all
	cd t && $(MAKE) test

