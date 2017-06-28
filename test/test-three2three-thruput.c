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
 *             track to prevent wrap
 *             +--------------------+
 *             |                    |
 *             |                    |
 * +----+    +====+    +====+       |
 * | P1 |--->| RB |--->| SB |--+    |
 * +----+    +====+    +====+  |    |
 *                             |    v
 * +----+    +====+    +====+  | +----+
 * | P2 |--->| RB |--->| SB |--+>| EP |
 * +----+    +====+    +====+  | +----+
 *                             |
 * +----+    +====+    +====+  |
 * | P3 |--->| RB |--->| SB |--+
 * +----+    +====+    +====+
 */

#include <pthread.h>
#include <sched.h>
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
static long iterations = 1000L * 1000L * 180L;
static int num_publishers = 3;
static long value = 0L;

/* FIXME */
static void *ep_thread(void *data) {
	eventproc_t eventproc = (eventproc_t)data;
	ringbuf_t *ringbufs = eventproc_get_ringbufs(eventproc);
	seqbar_t  *seqbars  = eventproc_get_seqbars(eventproc);
	seq_t     *seqs     = eventproc_get_seqs(eventproc);
	size_t    length    = eventproc_get_length(eventproc);
	long count = 0, expcount = iterations;

	while (1) {
		long i;

		for (i = 0; i < length; ++i) {
			long l, prev_seq = seq_get(seqs[i]);
			long avail_seq = seqbar_wait_for(seqbars[i], -1L);

			for (l = prev_seq + 1; l <= avail_seq; ++l) {
				event_t *event = ringbuf_get(ringbufs[i], l);

				/* fprintf(stdout, "%ld\n", event_get_long(event)); */
				value += event_get_long(event);
			}
			seq_set(seqs[i], avail_seq);
			count += avail_seq - prev_seq;
			if (count == expcount)
				goto end;
		}
		sched_yield();
	}

end:
	return NULL;
}

/* FIXME */
static void *pb_thread(void *data) {
	ringbuf_t ringbuf = (ringbuf_t)data;
	long i;

	pthread_detach(pthread_self());
	for (i = 0; i < iterations / num_publishers; ++i) {
		long next = ringbuf_next(ringbuf);
		event_t *event = ringbuf_get(ringbuf, next);

		event_set_long(event, i);
		ringbuf_publish(ringbuf, next);
	}
	return NULL;
}

int main(int argc, char **argv) {
	ringbuf_t ringbufs[num_publishers];
	seqbar_t seqbars[num_publishers];
	eventproc_t eventproc;
	seq_t *seqs;
	pthread_t thread;
	struct timespec start, end;
	long i, diff;

	for (i = 0; i < num_publishers; ++i) {
		ringbufs[i] = ringbuf_new_single(1024 * 64, waitstg_new_yielding());
		seqbars[i] = ringbuf_new_bar(ringbufs[i], NULL, 0);
	}
	eventproc = eventproc_new(ringbufs, seqbars, num_publishers);
	seqs = eventproc_get_seqs(eventproc);
	for (i = 0; i < num_publishers; ++i)
		ringbuf_add_gatingseqs(ringbufs[i], &seqs[i], 1);
	if (pthread_create(&thread, NULL, ep_thread, eventproc) != 0) {
		fprintf(stderr, "Error initializing event processor\n");
		exit(1);
	}
	clock_gettime(CLOCK_MONOTONIC, &start);
	for (i = 0; i < num_publishers; ++i) {
		pthread_t thread;

		if (pthread_create(&thread, NULL, pb_thread, ringbufs[i]) != 0) {
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

