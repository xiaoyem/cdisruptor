/*
 * Copyright (c) 2015-2017 by Xiaoye Meng.
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
 *             track to prevent wrap
 *             +--------------------+
 *             |                    |
 *             |                    v
 * +----+    +====+    +====+    +-----+
 * | P1 |--->| RB |<---| SB |    | EP1 |
 * +----+    +====+    +====+    +-----+
 *             ^   get    ^         |
 * +----+      |          |         |
 * | P2 |------+          +---------+
 * +----+      |            waitFor
 *             |
 * +----+      |
 * | P3 |------+
 * +----+
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
static int num_publishers = 3;
static long value = 0L;

/* FIXME */
static void *ep_thread(void *data) {
	eventproc_t eventproc = (eventproc_t)data;
	ringbuf_t ringbuf = eventproc_get_ringbuf(eventproc);
	seqbar_t  seqbar  = eventproc_get_seqbar(eventproc);
	seq_t     seq     = eventproc_get_seq(eventproc);
	long next_seq = seq_get(seq) + 1L;
	long expcount = seq_get(seq) + iterations / num_publishers * num_publishers;

	while (1) {
		long avail_seq = seqbar_wait_for(seqbar, next_seq);

		while (next_seq <= avail_seq) {
			event_t *event = ringbuf_get(ringbuf, next_seq);

			/* fprintf(stdout, "%ld\n", event_get_long(event)); */
			value += event_get_long(event);
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
static void *pb_thread(void *data) {
	ringbuf_t ringbuf = (ringbuf_t)data;
	long i;

	pthread_detach(pthread_self());
	for (i = 0; i < iterations / num_publishers; i += 10) {
		long hi = ringbuf_next_n(ringbuf, 10);
		long lo = hi - 10 + 1;
		long j;

		for (j = lo; j <= hi; ++j) {
			event_t *event = ringbuf_get(ringbuf, j);

			event_set_long(event, i);
		}
		ringbuf_publish_batch(ringbuf, lo, hi);
	}
	return NULL;
}

int main(int argc, char **argv) {
	ringbuf_t ringbuf = ringbuf_new_multi(1024 * 64, waitstg_new_busyspin());
	seqbar_t seqbar = ringbuf_new_bar(ringbuf, NULL, 0);
	eventproc_t eventproc = eventproc_new(&ringbuf, &seqbar, 1);
	seq_t seq = eventproc_get_seq(eventproc);
	pthread_t thread;
	struct timespec start, end;
	long i, diff;

	ringbuf_add_gatingseqs(ringbuf, &seq, 1);
	if (pthread_create(&thread, NULL, ep_thread, eventproc) != 0) {
		fprintf(stderr, "Error initializing event processor\n");
		exit(1);
	}
	clock_gettime(CLOCK_MONOTONIC, &start);
	for (i = 0; i < num_publishers; ++i) {
		pthread_t thread;

		if (pthread_create(&thread, NULL, pb_thread, ringbuf) != 0) {
			fprintf(stderr, "Error initializing publisher\n");
			exit(1);
		}
	}
	pthread_join(thread, NULL);
	clock_gettime(CLOCK_MONOTONIC, &end);
	diff = end.tv_sec * 1000000000 + end.tv_nsec - start.tv_sec * 1000000000 - start.tv_nsec;
	fprintf(stdout, "%ld ops/sec\n", iterations * 1000000000 / diff);
	return 0;
}

