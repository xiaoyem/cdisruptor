/*
 * Copyright (c) 2015-2017, Dalian Futures Information Technology Co., Ltd.
 *
 * Xiaoye Meng <mengxiaoye at dce dot com dot cn>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 *                             track to prevent wrap
 *             +--------------------+----------+----------+
 *             |                    |          |          |
 *             |                    v          v          v
 * +----+    +====+    +====+    +-----+    +-----+    +-----+
 * | P1 |--->| RB |<---| SB |    | EP1 |    | EP2 |    | EP3 |
 * +----+    +====+    +====+    +-----+    +-----+    +-----+
 *      claim      get    ^         |          |          |
 *                        |         |          |          |
 *                        +---------+----------+----------+
 *                                      waitFor
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "seq.h"
#include "waitstg.h"
#include "seqbar.h"
#include "event.h"
#include "ringbuf.h"
#include "eventproc.h"

/* FIXME */
static long iterations = 1000L * 1000L * 100L;
static int num_eventprocs = 3;
static long value[3];

/* FIXME */
static void *ep1_thread(void *data) {
	eventproc_t eventproc = (eventproc_t)data;
	ringbuf_t ringbuf = eventproc_get_ringbuf(eventproc);
	seqbar_t  seqbar  = eventproc_get_seqbar(eventproc);
	seq_t     seq     = eventproc_get_seq(eventproc);
	long next_seq = seq_get(seq) + 1L;
	long expcount = seq_get(seq) + iterations;

	while (1) {
		long avail_seq = seqbar_wait_for(seqbar, next_seq);

		while (next_seq <= avail_seq) {
			event_t *event = ringbuf_get(ringbuf, next_seq);

			/* fprintf(stdout, "%ld\n", event_get_long(event)); */
			value[0] += event_get_long(event);
			if (next_seq == expcount)
				goto end;
			++next_seq;
		}
		seq_set(seq, avail_seq);
	}

end:
	return NULL;
}

/* FIXME */
static void *ep2_thread(void *data) {
	eventproc_t eventproc = (eventproc_t)data;
	ringbuf_t ringbuf = eventproc_get_ringbuf(eventproc);
	seqbar_t  seqbar  = eventproc_get_seqbar(eventproc);
	seq_t     seq     = eventproc_get_seq(eventproc);
	long next_seq = seq_get(seq) + 1L;
	long expcount = seq_get(seq) + iterations;

	while (1) {
		long avail_seq = seqbar_wait_for(seqbar, next_seq);

		while (next_seq <= avail_seq) {
			event_t *event = ringbuf_get(ringbuf, next_seq);

			/* fprintf(stdout, "%ld\n", event_get_long(event)); */
			value[1] -= event_get_long(event);
			if (next_seq == expcount)
				goto end;
			++next_seq;
		}
		seq_set(seq, avail_seq);
	}

end:
	return NULL;
}

/* FIXME */
static void *ep3_thread(void *data) {
	eventproc_t eventproc = (eventproc_t)data;
	ringbuf_t ringbuf = eventproc_get_ringbuf(eventproc);
	seqbar_t  seqbar  = eventproc_get_seqbar(eventproc);
	seq_t     seq     = eventproc_get_seq(eventproc);
	long next_seq = seq_get(seq) + 1L;
	long expcount = seq_get(seq) + iterations;

	while (1) {
		long avail_seq = seqbar_wait_for(seqbar, next_seq);

		while (next_seq <= avail_seq) {
			event_t *event = ringbuf_get(ringbuf, next_seq);

			/* fprintf(stdout, "%ld\n", event_get_long(event)); */
			value[2] &= event_get_long(event);
			if (next_seq == expcount)
				goto end;
			++next_seq;
		}
		seq_set(seq, avail_seq);
	}

end:
	return NULL;
}

int main(int argc, char **argv) {
	ringbuf_t ringbuf = ringbuf_new_single(1024 * 8, waitstg_new_yielding());
	seqbar_t seqbar = ringbuf_new_bar(ringbuf, NULL, 0);
	eventproc_t eventproc[num_eventprocs];
	pthread_t thread[num_eventprocs];
	struct timespec start, end;
	long i, diff;

	for (i = 0; i < num_eventprocs; ++i) {
		seq_t seq;

		eventproc[i] = eventproc_new(&ringbuf, &seqbar, 1);
		seq = eventproc_get_seq(eventproc[i]);
		ringbuf_add_gatingseqs(ringbuf, &seq, 1);
	}
	if (pthread_create(&thread[0], NULL, ep1_thread, eventproc[0]) != 0) {
		fprintf(stderr, "Error initializing event processor\n");
		exit(1);
	}
	if (pthread_create(&thread[1], NULL, ep2_thread, eventproc[1]) != 0) {
		fprintf(stderr, "Error initializing event processor\n");
		exit(1);
	}
	if (pthread_create(&thread[2], NULL, ep3_thread, eventproc[2]) != 0) {
		fprintf(stderr, "Error initializing event processor\n");
		exit(1);
	}
	clock_gettime(CLOCK_MONOTONIC, &start);
	for (i = 0; i < iterations; ++i) {
		long next = ringbuf_next(ringbuf);
		event_t *event = ringbuf_get(ringbuf, next);

		event_set_long(event, i);
		ringbuf_publish(ringbuf, next);
	}
	for (i = 0; i < num_eventprocs; ++i)
		pthread_join(thread[i], NULL);
	clock_gettime(CLOCK_MONOTONIC, &end);
	diff = end.tv_sec * 1000000000 + end.tv_nsec - start.tv_sec * 1000000000 - start.tv_nsec;
	fprintf(stdout, "%ld ops/sec\n", iterations * 1000000000 / diff);
	return 0;
}

