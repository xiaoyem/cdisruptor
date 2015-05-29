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
#define SINGLE 1
#define MULTI  2

/* FIXME */
struct seqr_t {
	int		type;
	int		bufsize;
	waitstg_t	waitstg;
	seq_t		cursor;
	seq_t		gatingseqs;
	union {
	struct {
		long	p1, p2, p3, p4, p5, p6, p7;
		long	next_value;
		long	cached_value;
		long	p9, p10, p11, p12, p13, p14, p15;
	}		s;
	struct {
		seq_t	gatingseqcache;
		int	*availbuf;
		int	idxmask;
		int	idxshift;
	}		m;
	}		u;
};

/* FIXME */
static inline int seqr_cal_idx(seqr_t seqr, long seq) {
	return ((int)seq) & seqr->u.m.idxmask;
}

/* FIXME */
static inline int seqr_cal_avail_flag(seqr_t seqr, long seq) {
	return (int)(seq >> seqr->u.m.idxshift);
}

/* FIXME */
static void seqr_set_avail(seqr_t seqr, long seq) {
	int index = seqr_cal_idx(seqr, seq);
	int flag = seqr_cal_avail_flag(seqr, seq);
	int *flag_ = seqr->u.m.availbuf + index;

	__atomic_store_n(flag_, flag, __ATOMIC_RELEASE);
}

/* FIXME */
seqr_t seqr_new_single(int bufsize, waitstg_t waitstg) {
	seqr_t seqr;

	if (bufsize < 1)
		return NULL;
	if (bufsize & (bufsize - 1))
		return NULL;
	if (NEW(seqr) == NULL)
		return NULL;
	seqr->type             = SINGLE;
	seqr->bufsize          = bufsize;
	seqr->waitstg          = waitstg;
	seqr->cursor           = seq_new();
	seqr->gatingseqs       = seq_new_grp(NULL, 0);
	seqr->u.s.next_value   = -1L;
	seqr->u.s.cached_value = -1L;
	return seqr;
}

/* FIXME */
seqr_t seqr_new_multi(int bufsize, waitstg_t waitstg) {
	seqr_t seqr;

	if (bufsize < 1)
		return NULL;
	if (bufsize & (bufsize - 1))
		return NULL;
	if (NEW(seqr) == NULL)
		return NULL;
	seqr->type               = MULTI;
	seqr->bufsize            = bufsize;
	seqr->waitstg            = waitstg;
	seqr->cursor             = seq_new();
	seqr->gatingseqs         = seq_new_grp(NULL, 0);
	seqr->u.m.gatingseqcache = seq_new();
	if ((seqr->u.m.availbuf = ALLOC(seqr->bufsize * sizeof (int))) == NULL) {
		FREE(seqr);
		return NULL;
	}
	seqr->u.m.idxmask        = seqr->bufsize - 1;
	seqr->u.m.idxshift       = lg2(seqr->bufsize);
	return seqr;
}

/* FIXME */
void seqr_free(seqr_t *sp) {
	if (sp == NULL || *sp == NULL)
		return;
	FREE(*sp);
}

/* the capacity of the data structure to hold entries */
int seqr_get_bufsize(seqr_t seqr) {
	if (seqr == NULL)
		return 0;
	return seqr->bufsize;
}

/* get the current cursor value */
long seqr_get_cursor(seqr_t seqr) {
	if (seqr == NULL)
		return 0;
	return seq_get(seqr->cursor);
}

/* add the specified gating sequences to this instance of the disruptor */
void seqr_add_gatingseqs(seqr_t seqr, seq_t gatingseqs) {
	/* FIXME */
}

/* remove the specified sequence from this sequencer */
void seqr_remove_gatingseq(seqr_t seqr, seq_t seq) {
	/* FIXME */
}

/* get the minimum sequence value from all of the gating sequences added to this ringBuffer */
long seqr_get_min_seq(seqr_t seqr) {
	if (seqr == NULL)
		return 0;
	return get_min_seq(seqr->gatingseqs, seq_get(seqr->cursor));
}

/* FIXME */
seqbar_t seqr_new_bar(seqr_t seqr, seq_t seqs) {
	if (seqr == NULL)
		return NULL;
	return seqbar_new(seqr, seqr->waitstg, seqr->cursor, seqs);
}

/* has the buffer got capacity to allocate another sequence */
bool seqr_has_avail_cap(seqr_t seqr, int requiredcap) {
	long nextval, wrappnt, cachedval;

	if (seqr == NULL)
		return false;
	nextval   = seqr->type == SINGLE ? seqr->u.s.next_value : seq_get(seqr->cursor);
	wrappnt   = nextval + requiredcap - seqr->bufsize;
	cachedval = seqr->type == SINGLE ? seqr->u.s.cached_value : seq_get(seqr->u.m.gatingseqcache);
	if (wrappnt > cachedval || cachedval > nextval) {
		long minseq = get_min_seq(seqr->gatingseqs, nextval);

		if (seqr->type == SINGLE)
			seqr->u.s.cached_value = minseq;
		else
			seq_set(seqr->u.m.gatingseqcache, minseq);
		if (wrappnt > minseq)
			return false;
	}
	return true;
}

/* get the remaining capacity for this sequencer */
long seqr_remaining_cap(seqr_t seqr) {
	long nextval, consumed, produced;

	if (seqr == NULL)
		return -1L;
	nextval  = seqr->type == SINGLE ? seqr->u.s.next_value : seq_get(seqr->cursor);
	consumed = get_min_seq(seqr->gatingseqs, nextval);
	produced = nextval;
	return seqr_get_bufsize(seqr) - (produced - consumed);
}

/* claim a specific sequence */
void seqr_claim(seqr_t seqr, long seq) {
	if (seqr == NULL)
		return;
	if (seqr->type == SINGLE)
		seqr->u.s.next_value = seq;
	else
		seq_set(seqr->cursor, seq);
}

/* claim the next event in sequence for publishing */
long seqr_next(seqr_t seqr) {
	if (seqr == NULL)
		return -1L;
	return seqr_next_n(seqr, 1);
}

/* claim the next n events in sequence for publishing */
long seqr_next_n(seqr_t seqr, int n) {
	long nextval, nextseq, wrappnt, cachedval;

	if (seqr == NULL)
		return -1L;
	if (n < 1)
		return -1L;
	if (seqr->type == SINGLE) {
		nextval   = seqr->u.s.next_value;
		nextseq   = nextval + n;
		wrappnt   = nextseq - seqr->bufsize;
		cachedval = seqr->u.s.cached_value;
		if (wrappnt > cachedval || cachedval > nextval) {
			long minseq;

			while (wrappnt > (minseq = get_min_seq(seqr->gatingseqs, nextval)))
				sleep(0);
			seqr->u.s.cached_value = minseq;
		}
		seqr->u.s.next_value = nextseq;
	} else
		do {
			nextval   = seq_get(seqr->cursor);
			nextseq   = nextval + n;
			wrappnt   = nextseq - seqr->bufsize;
			cachedval = seq_get(seqr->u.m.gatingseqcache);
			if (wrappnt > cachedval || cachedval > nextval) {
				long minseq;

				if (wrappnt > (minseq = get_min_seq(seqr->gatingseqs, nextval))) {
					sleep(0);
					continue;
				}
				seq_set(seqr->u.m.gatingseqcache, minseq);
			} else if (seq_comp_and_set(seqr->cursor, nextval, nextseq))
				break;
		} while (1);
	return nextseq;
}

/* attempt to claim the next event in sequence for publishing */
long seqr_try_next(seqr_t seqr) {
	if (seqr == NULL)
		return -1L;
	return seqr_try_next_n(seqr, 1);
}

/* attempt to claim the next n events in sequence for publishing */
long seqr_try_next_n(seqr_t seqr, int n) {
	if (seqr == NULL)
		return -1L;
	if (n < 1)
		return -1L;
	if (seqr->type == SINGLE) {
		if (!seqr_has_avail_cap(seqr, n))
			return -1L;
		return seqr->u.s.next_value += n;
	} else {
		long nextval, nextseq;

		do {
			nextval = seq_get(seqr->cursor);
			nextseq = nextval + n;
			if (!seqr_has_avail_cap(seqr, n))
				return -1L;
		} while (!seq_comp_and_set(seqr->cursor, nextval, nextseq));
		return nextseq;
	}
}

/* publish a sequence */
void seqr_publish(seqr_t seqr, long seq) {
	if (seqr == NULL)
		return;
	if (seqr->type == SINGLE)
		seq_set(seqr->cursor, seq);
	else
		seqr_set_avail(seqr, seq);
	waitstg_signal_all_when_blocking(seqr->waitstg);
}

/* batch publish sequences */
void seqr_publish_batch(seqr_t seqr, long lo, long hi) {
	if (seqr == NULL)
		return;
	if (seqr->type == SINGLE)
		seq_set(seqr->cursor, hi);
	else {
		long l;

		for (l = lo; l <= hi; ++l)
			seqr_set_avail(seqr, l);
	}
	waitstg_signal_all_when_blocking(seqr->waitstg);
}

/* confirms if a sequence is published and the event is available for use */
bool seqr_is_avail(seqr_t seqr, long seq) {
	if (seqr == NULL)
		return false;
	if (seqr->type == SINGLE)
		return seq <= seq_get(seqr->cursor);
	else {
		int index = seqr_cal_idx(seqr, seq);
		int flag = seqr_cal_avail_flag(seqr, seq);
		int *flag_ = seqr->u.m.availbuf + index;

		return __atomic_load_n(flag_, __ATOMIC_ACQUIRE) == flag;
	}
}

/* get the highest sequence number that can be safely read from the ring buffer */
long seqr_get_highest_published_seq(seqr_t seqr, long nextseq, long availseq) {
	if (seqr == NULL)
		return -1L;
	if (seqr->type == MULTI) {
		long seq;

		for (seq = nextseq; seq <= availseq; ++seq)
			if (seqr_is_avail(seqr, seq))
				return seq - 1;
	}
	return availseq;
}

