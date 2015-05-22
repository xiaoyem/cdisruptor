/*
 * Copyright (c) 2015, Dalian Futures Information Technology Co., Ltd.
 *
 * Xiaoye Meng <mengxiaoye at dce dot com dot cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <unistd.h>
#include "mem.h"
#include "seq.h"
#include "util.h"
#include "waitstg.h"
#include "seqr.h"

/* FIXME */
struct seqr_t {
	int		bufsize;
	waitstg_t	waitstg;
	seq_t		cursor;
	seq_t		gatingseqs;
	long		p1, p2, p3, p4, p5, p6, p7;
	long		next_value, cached_value;
	long		p9, p10, p11, p12, p13, p14, p15;
};

/* FIXME */
seqr_t seqr_new(int bufsize, waitstg_t waitstg) {
	seqr_t seqr;

	if (bufsize < 1)
		return NULL;
	if (bufsize & (bufsize - 1))
		return NULL;
	if (NEW(seqr) == NULL)
		return NULL;
	seqr->bufsize      = bufsize;
	seqr->waitstg      = waitstg;
	seqr->cursor       = seq_new();
	seqr->next_value   = -1L;
	seqr->cached_value = -1L;
	return seqr;
}

/* FIXME */
void seqr_free(seqr_t *sp) {
	if (sp == NULL || *sp == NULL)
		return;
	FREE(*sp);
}

/* has the buffer got capacity to allocate another sequence */
bool seqr_has_avail_cap(seqr_t seqr, int requiredcap) {
	long nextval, wrappnt;

	nextval = seqr->next_value;
	wrappnt = nextval + requiredcap - seqr->bufsize;
	if (wrappnt > seqr->cached_value || seqr->cached_value > nextval) {
		long minseq = get_min_seq(seqr->gatingseqs, nextval);

		seqr->cached_value = minseq;
		if (wrappnt > minseq)
			return false;
	}
	return true;
}

/* claim the next event in sequence for publishing */
inline long seqr_next(seqr_t seqr) {
	return seqr_next_n(seqr, 1);
}

/* claim the next n events in sequence for publishing */
long seqr_next_n(seqr_t seqr, int n) {
	long nextval, nextseq, wrappnt;

	if (n < 1)
		return -1L;
	nextval = seqr->next_value;
	nextseq = nextval + n;
	wrappnt = nextseq - seqr->bufsize;
	if (wrappnt > seqr->cached_value || seqr->cached_value > nextval) {
		long minseq;

		while (wrappnt > (minseq = get_min_seq(seqr->gatingseqs, nextval)))
			sleep(0);
		seqr->cached_value = minseq;
	}
	seqr->next_value = nextseq;
	return nextseq;
}

/* attempt to claim the next event in sequence for publishing */
inline long seqr_try_next(seqr_t seqr) {
	return seqr_try_next_n(seqr, 1);
}

/* attempt to claim the next n events in sequence for publishing */
long seqr_try_next_n(seqr_t seqr, int n) {
	if (n < 1)
		return -1L;
	if (!seqr_has_avail_cap(seqr, n))
		return -1L;
	return seqr->next_value += n;
}

/* claim a specific sequence */
inline void seqr_claim(seqr_t seqr, long seq) {
	seqr->next_value = seq;
}

/* publish a sequence */
void seqr_publish(seqr_t seqr, long seq) {
	seq_set(seqr->cursor, seq);
	waitstg_signal_all_when_blocking(seqr->waitstg);
}

/* batch publish sequences */
inline void seqr_publish_batch(seqr_t seqr, long lo, long hi) {
	seqr_publish(seqr, hi);
}

/* confirms if a sequence is published and the event is available for use */
inline bool seqr_is_avail(seqr_t seqr, long seq) {
	return seq <= seq_get(seqr->cursor);
}

/* get the highest sequence number that can be safely read from the ring buffer */
inline long seqr_get_highest_published_seq(long nextseq, long availseq) {
	return availseq;
}

