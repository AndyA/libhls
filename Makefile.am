DIST_SUBDIRS = t

SUBDIRS = . t

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libhls.la

include_HEADERS = hls.h

bin_PROGRAMS = hls

libhls_la_SOURCES = \
	hls_m3u8.c \
	hls_m3u8_formatter.c \
	hls_m3u8_io.c \
	hls_m3u8_parser.c \
	hls_m3u8_syntax.c \
	hls.h

libhls_la_LDFLAGS = -avoid-version

hls_SOURCES = hls.c hls.h
hls_LDADD = libhls.la

hls_m3u8_syntax.c: syntax.pl
	perl $< | perl tools/json2c.pl -n hls__m3u8_syntax > $@

test: all
	cd t && $(MAKE) test

clean-local:
	rm -f hls_m3u8_syntax.c

