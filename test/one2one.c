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

#include <pthread.h>
#include <stdio.h>
#include "seq.h"
#include "waitstg.h"
#include "seqbar.h"
#include "event.h"
#include "ringbuf.h"
#include "eventproc.h"

/* FIXME */
static long iterations = 1000L;

/* FIXME */
static void *ep_thread(void *data) {
	eventproc_t eventproc = (eventproc_t)data;
	seqbar_t seqbar = eventproc_get_seqbar(eventproc);
	seq_t seq = eventproc_get_seq(eventproc);
	long next_seq = seq_get(seq) + 1L;
	long expcount = seq_get(seq) + iterations;

	while (1) {
		long avail_seq = seqbar_wait_for(seqbar, next_seq);

		while (next_seq <= avail_seq) {
			ringbuf_t ringbuf = eventproc_get_ringbuf(eventproc);
			event_t *event = ringbuf_get(ringbuf, next_seq);

			printf("%ld\n", event_get_long(event));
			++next_seq;
		}
		seq_set(seq, avail_seq);
		if (seq_get(seq) == expcount)
			break;
	}
	return NULL;
}

int main(int argc, char **argv) {
	ringbuf_t ringbuf = ringbuf_new_single(1024 * 64, waitstg_new_yielding());
	seqbar_t seqbar = ringbuf_new_bar(ringbuf, NULL, 0);
	eventproc_t eventproc = eventproc_new(ringbuf, seqbar);
	seq_t seq = eventproc_get_seq(eventproc);
	pthread_t thread;
	long i;

	ringbuf_add_gatingseqs(ringbuf, &seq, 1);
	/* FIXME */
	pthread_create(&thread, NULL, ep_thread, eventproc);
	for (i = 0; i < 1000L; ++i) {
		long next = ringbuf_next(ringbuf);
		event_t *event = ringbuf_get(ringbuf, next);

		event_set_long(event, i);
		ringbuf_publish(ringbuf, next);
	}
	pthread_join(thread, NULL);
	return 0;
}

