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

#include <stdlib.h>
#include "macros.h"
#include "mem.h"
#include "seqr.h"
#include "ringbuf.h"

/* FIXME */
struct ringbuf_t {
	long	p1, p2, p3, p4, p5, p6, p7;
	long	idxmask;
	void	**entries;
	int	bufsize;
	seqr_t	seqr;
	long	p9, p10, p11, p12, p13, p14, p15;
};

/* FIXME */
ringbuf_t ringbuf_new_single(int bufsize, waitstg_t waitstg) {
	ringbuf_t ringbuf;

	if (unlikely(bufsize < 1))
		return NULL;
	if (bufsize & (bufsize - 1))
		return NULL;
	if (NEW(ringbuf) == NULL)
		return NULL;
	if ((ringbuf->entries = ALLOC(bufsize * sizeof (void *) + 2 * 128)) == NULL)
		return NULL;
	ringbuf->idxmask = bufsize - 1;
	ringbuf->seqr    = seqr_new_single(bufsize, waitstg);
	ringbuf->bufsize = seqr_get_bufsize(ringbuf->seqr);
	return ringbuf;
}

/* FIXME */
ringbuf_t ringbuf_new_multi(int bufsize, waitstg_t waitstg) {
	ringbuf_t ringbuf;

	if (unlikely(bufsize < 1))
		return NULL;
	if (bufsize & (bufsize - 1))
		return NULL;
	if (NEW(ringbuf) == NULL)
		return NULL;
	if ((ringbuf->entries = ALLOC(bufsize * sizeof (void *) + 2 * 128)) == NULL)
		return NULL;
	ringbuf->idxmask = bufsize - 1;
	ringbuf->seqr    = seqr_new_multi(bufsize, waitstg);
	ringbuf->bufsize = seqr_get_bufsize(ringbuf->seqr);
	return ringbuf;
}

/* FIXME */
void ringbuf_free(ringbuf_t *rp) {
	if (unlikely(rp == NULL || *rp == NULL))
		return;
	FREE((*rp)->entries);
	seqr_free(&(*rp)->seqr);
	FREE(*rp);
}

/* the size of the buffer */
int ringbuf_get_bufsize(ringbuf_t ringbuf) {
	if (unlikely(ringbuf == NULL))
		return 0;
	return ringbuf->bufsize;
}

/* get the current cursor value for the ring buffer */
long ringbuf_get_cursor(ringbuf_t ringbuf) {
	if (unlikely(ringbuf == NULL))
		return -1L;
	return seqr_get_cursor(ringbuf->seqr);
}

/* add the specified gating sequences to this instance of the disruptor */
void ringbuf_add_gatingseqs(ringbuf_t ringbuf, seq_t *gatingseqs, int length) {
	if (unlikely(ringbuf == NULL))
		return;
	seqr_add_gatingseqs(ringbuf->seqr, gatingseqs, length);
}

/* remove the specified sequence from this ring buffer */
void ringbuf_remove_gatingseq(ringbuf_t ringbuf, seq_t gatingseq) {
	if (unlikely(ringbuf == NULL))
		return;
	seqr_remove_gatingseq(ringbuf->seqr, gatingseq);
}

/* get the minimum sequence value from all of the gating sequences added to this ring buffer */
long ringbuf_get_min_gatingseq(ringbuf_t ringbuf) {
	if (unlikely(ringbuf == NULL))
		return -1L;
	return seqr_get_min_seq(ringbuf->seqr);
}

/* FIXME */
seqbar_t ringbuf_new_bar(ringbuf_t ringbuf, seq_t *seqs, int length) {
	if (unlikely(ringbuf == NULL))
		return NULL;
	return seqr_new_bar(ringbuf->seqr, seqs, length);
}

/* FIXME */
bool ringbuf_has_avail_cap(ringbuf_t ringbuf, int requiredcap) {
	if (unlikely(ringbuf == NULL))
		return false;
	return seqr_has_avail_cap(ringbuf->seqr, requiredcap);
}

/* get the remaining capacity for this ring buffer */
long ringbuf_remaining_cap(ringbuf_t ringbuf) {
	if (unlikely(ringbuf == NULL))
		return -1L;
	return seqr_remaining_cap(ringbuf->seqr);
}

/* get the event for a given sequence in the ring buffer */
void *ringbuf_get(ringbuf_t ringbuf, long seq) {
	void *base;

	if (unlikely(ringbuf == NULL))
		return NULL;
	base = (char *)ringbuf->entries + 128;
	return base + (seq & ringbuf->idxmask);
}

/* set the cursor to a specific sequence and return the preallocated entry that is stored there */
void *ringbuf_claim_and_get(ringbuf_t ringbuf, long seq) {
	if (unlikely(ringbuf == NULL))
		return NULL;
	seqr_claim(ringbuf->seqr, seq);
	return ringbuf_get(ringbuf, seq);
}

/* increment and return the next sequence for the ring buffer */
long ringbuf_next(ringbuf_t ringbuf) {
	if (unlikely(ringbuf == NULL))
		return -1L;
	return seqr_next(ringbuf->seqr);
}

/* FIXME */
long ringbuf_next_n(ringbuf_t ringbuf, int n) {
	if (unlikely(ringbuf == NULL))
		return -1L;
	return seqr_next_n(ringbuf->seqr, n);
}

/* FIXME */
long ringbuf_try_next(ringbuf_t ringbuf) {
	if (unlikely(ringbuf == NULL))
		return -1L;
	return seqr_try_next(ringbuf->seqr);
}

/* FIXME */
long ringbuf_try_next_n(ringbuf_t ringbuf, int n) {
	if (unlikely(ringbuf == NULL))
		return -1L;
	return seqr_try_next_n(ringbuf->seqr, n);
}

/* publish the specified sequence */
void ringbuf_publish(ringbuf_t ringbuf, long seq) {
	if (unlikely(ringbuf == NULL))
		return;
	seqr_publish(ringbuf->seqr, seq);
}

/* FIXME */
void ringbuf_publish_batch(ringbuf_t ringbuf, long lo, long hi) {
	if (unlikely(ringbuf == NULL))
		return;
	seqr_publish_batch(ringbuf->seqr, lo, hi);
}

/* reset the cursor to a specific value */
void ringbuf_reset_to(ringbuf_t ringbuf, long seq) {
	if (unlikely(ringbuf == NULL))
		return;
	seqr_claim(ringbuf->seqr, seq);
	seqr_publish(ringbuf->seqr, seq);
}

/* determines if a particular entry has been published */
bool ringbuf_is_published(ringbuf_t ringbuf, long seq) {
	if (unlikely(ringbuf == NULL))
		return false;
	return seqr_is_avail(ringbuf->seqr, seq);
}

