noinst_PROGRAMS = test-one2one-thruput test-one2one-batch-thruput              \
test-three2one-thruput
test_one2one_thruput_SOURCES = $(top_srcdir)/src/mem.c $(top_srcdir)/src/seq.c \
$(top_srcdir)/src/seqgrp.c $(top_srcdir)/src/waitstg.c                         \
$(top_srcdir)/src/seqbar.c $(top_srcdir)/src/seqr.c $(top_srcdir)/src/event.c  \
$(top_srcdir)/src/ringbuf.c $(top_srcdir)/src/util.c                           \
$(top_srcdir)/src/eventproc.c test-one2one-thruput.c
test_one2one_thruput_CPPFLAGS = -I$(top_srcdir)/include
test_one2one_thruput_CFLAGS = -Wall -Winline -pthread
test_one2one_thruput_LDFLAGS = -lrt -pthread
test_one2one_batch_thruput_SOURCES = $(top_srcdir)/src/mem.c                   \
$(top_srcdir)/src/seq.c $(top_srcdir)/src/seqgrp.c $(top_srcdir)/src/waitstg.c \
$(top_srcdir)/src/seqbar.c $(top_srcdir)/src/seqr.c $(top_srcdir)/src/event.c  \
$(top_srcdir)/src/ringbuf.c $(top_srcdir)/src/util.c                           \
$(top_srcdir)/src/eventproc.c test-one2one-batch-thruput.c
test_one2one_batch_thruput_CPPFLAGS = -I$(top_srcdir)/include
test_one2one_batch_thruput_CFLAGS = -Wall -Winline -pthread
test_one2one_batch_thruput_LDFLAGS = -lrt -pthread
test_three2one_thruput_SOURCES = test-three2one-thruput.c
test_three2one_thruput_CPPFLAGS = -I$(top_srcdir)/include
test_three2one_thruput_CFLAGS = -Wall -Winline -pthread
test_three2one_thruput_LDFLAGS = -lrt -pthread

