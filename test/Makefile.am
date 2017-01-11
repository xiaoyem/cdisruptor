noinst_PROGRAMS = one2one three2one
one2one_SOURCES = $(top_srcdir)/src/mem.c $(top_srcdir)/src/seq.c              \
$(top_srcdir)/src/seqgrp.c $(top_srcdir)/src/waitstg.c                         \
$(top_srcdir)/src/seqbar.c $(top_srcdir)/src/seqr.c $(top_srcdir)/src/event.c  \
$(top_srcdir)/src/ringbuf.c $(top_srcdir)/src/util.c                           \
$(top_srcdir)/src/eventproc.c one2one.c
one2one_CPPFLAGS = -I$(top_srcdir)/include
one2one_CFLAGS = -Wall -Winline -pthread
one2one_LDFLAGS = -lrt -pthread
three2one_SOURCES = three2one.c
three2one_CPPFLAGS = -I$(top_srcdir)/include
three2one_CFLAGS = -Wall -Winline

