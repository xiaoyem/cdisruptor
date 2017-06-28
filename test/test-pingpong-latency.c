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
 *               +----------+
 *               |          |
 *               |   get    V
 *  waitFor   +=====+    +=====+  claim
 *    +------>| SB2 |    | RB2 |<------+
 *    |       +=====+    +=====+       |
 *    |                                |
 * +-----+    +=====+    +=====+    +-----+
 * | EP1 |--->| RB1 |    | SB1 |<---| EP2 |
 * +-----+    +=====+    +=====+    +-----+
 *       claim   ^   get    |  waitFor
 *               |          |
 *               +----------+
 */

#define _GNU_SOURCE
#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "seq.h"
#include "waitstg.h"
#include "seqbar.h"
#include "event.h"
#include "ringbuf.h"
#include "eventproc.h"

/* FIXME */
static long iterations = 1000L * 1000L * 30L;
static long sum = 0L, min = LONG_MAX, max = LONG_MIN;
static ringbuf_t pingbuf, pongbuf;

/* FIXME */
static void *pong_thread(void *data) {
	/* cpu_set_t cpuset;
	pthread_t thread = pthread_self(); */
	eventproc_t pongproc = (eventproc_t)data;
	seqbar_t pingbar = eventproc_get_seqbar(pongproc);
	seq_t    pongseq = eventproc_get_seq(pongproc);
	long next_ping = seq_get(pongseq) + 1L;

	/* CPU_ZERO(&cpuset);
	CPU_SET(1, &cpuset);
	pthread_setaffinity_np(thread, sizeof (cpu_set_t), &cpuset); */
	pthread_detach(pthread_self());
	while (1) {
		long avail_ping = seqbar_wait_for(pingbar, next_ping);

		while (next_ping <= avail_ping) {
			event_t *pingevent = ringbuf_get(pingbuf, next_ping);
			long next_pong = ringbuf_next(pongbuf);
			event_t *pongevent = ringbuf_get(pongbuf, next_pong);

			event_set_long(pongevent, event_get_long(pingevent));
			ringbuf_publish(pongbuf, next_pong);
			++next_ping;
		}
		seq_set(pongseq, avail_ping);
	}
	return NULL;
}

/* FIXME */
static void *ping_thread(void *data) {
	/* cpu_set_t cpuset;
	pthread_t thread = pthread_self(); */
	eventproc_t pingproc = (eventproc_t)data;
	seqbar_t pongbar = eventproc_get_seqbar(pingproc);
	seq_t    pingseq = eventproc_get_seq(pingproc);
	struct timespec start, end;
	long next_ping, next_pong;
	event_t *pingevent;
	long counter = 0L;

	/* CPU_ZERO(&cpuset);
	CPU_SET(3, &cpuset);
	pthread_setaffinity_np(thread, sizeof (cpu_set_t), &cpuset); */
	sleep(1);
	clock_gettime(CLOCK_MONOTONIC, &start);
	next_ping = ringbuf_next(pingbuf);
	pingevent = ringbuf_get(pingbuf, next_ping);
	event_set_long(pingevent, counter++);
	ringbuf_publish(pingbuf, next_ping);
	next_pong = seq_get(pingseq) + 1L;
	while (1) {
		long avail_pong = seqbar_wait_for(pongbar, next_pong);

		while (next_pong <= avail_pong) {
			event_t *pongevent = ringbuf_get(pongbuf, next_pong);
			long diff;

			clock_gettime(CLOCK_MONOTONIC, &end);
			/* fprintf(stdout, "pingpong: %ld (%ldns)\n", event_get_long(pongevent), diff); */
			if (event_get_long(pongevent) == iterations)
				goto end;
			diff = end.tv_sec * 1000000000 + end.tv_nsec -
				start.tv_sec * 1000000000 - start.tv_nsec;
			sum += diff;
			if (diff < min) min = diff;
			if (diff > max) max = diff;
			clock_gettime(CLOCK_MONOTONIC, &start);
			diff = start.tv_sec * 1000000000 + start.tv_nsec -
				end.tv_sec * 1000000000 - end.tv_nsec;
			while (diff < 1000) {
				/* sleep(0); */
				sched_yield();
				clock_gettime(CLOCK_MONOTONIC, &start);
				diff = start.tv_sec * 1000000000 + start.tv_nsec -
					end.tv_sec * 1000000000 - end.tv_nsec;
			}
			clock_gettime(CLOCK_MONOTONIC, &start);
			next_ping = ringbuf_next(pingbuf);
			pingevent = ringbuf_get(pingbuf, next_ping);
			event_set_long(pingevent, counter++);
			ringbuf_publish(pingbuf, next_ping);
			++next_pong;
		}
		seq_set(pingseq, avail_pong);
	}

end:
	return NULL;
}

int main(int argc, char **argv) {
	seqbar_t pingbar, pongbar;
	eventproc_t pingproc, pongproc;
	seq_t pingseq, pongseq;
	pthread_t thread;

	/* pingbuf  = ringbuf_new_single(1024, waitstg_new_busyspin()); */
	/* pongbuf  = ringbuf_new_single(1024, waitstg_new_busyspin()); */
	pingbuf  = ringbuf_new_single(1024, waitstg_new_yielding());
	pongbuf  = ringbuf_new_single(1024, waitstg_new_yielding());
	pingbar  = ringbuf_new_bar(pingbuf, NULL, 0);
	pongbar  = ringbuf_new_bar(pongbuf, NULL, 0);
	pingproc = eventproc_new(&pongbuf, &pongbar, 1);
	pongproc = eventproc_new(&pingbuf, &pingbar, 1);
	pingseq  = eventproc_get_seq(pingproc);
	pongseq  = eventproc_get_seq(pongproc);
	ringbuf_add_gatingseqs(pingbuf, &pongseq, 1);
	ringbuf_add_gatingseqs(pongbuf, &pingseq, 1);
	if (pthread_create(&thread, NULL, pong_thread, pongproc) != 0) {
		fprintf(stderr, "Error initializing pong processor\n");
		exit(1);
	}
	if (pthread_create(&thread, NULL, ping_thread, pingproc) != 0) {
		fprintf(stderr, "Error initializing ping processor\n");
		exit(1);
	}
	pthread_join(thread, NULL);
	fprintf(stdout, "avg=%ldns, min=%ldns, max=%ldns\n", sum / iterations, min, max);
	return 0;
}

