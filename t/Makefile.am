TESTBIN = basic 

TESTPERL = basic.t

noinst_PROGRAMS = wrap $(TESTBIN)

noinst_LTLIBRARIES = libtest-support.la

libtest_support_la_SOURCES = tap.c tap.h framework.c framework.h jd_test.c jd_test.h
libtest_support_la_LDFLAGS = -static

LDADD = libtest-support.la ../libhls.la

test: $(TESTBIN)
	prove -e ./wrap $(TESTBIN) $(TESTPERL)

valgrind: $(TESTBIN)
	prove -e 'valgrind -q' $(addprefix ./,$(TESTBIN))
