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

#include <unistd.h>
#include "macros.h"
#include "mem.h"
#include "eventproc.h"

/* FIXME */
struct eventproc_t {
	size_t		length;
	ringbuf_t	*ringbufs;
	seqbar_t	*seqbars;
	seq_t		*seqs;
};

/* FIXME */
eventproc_t eventproc_new(ringbuf_t *ringbufs, seqbar_t *seqbars, size_t length) {
	eventproc_t eventproc;
	size_t i;

	if (unlikely(NEW0(eventproc) == NULL))
		return NULL;
	eventproc->length   = length;
	eventproc->ringbufs = ringbufs;
	eventproc->seqbars  = seqbars;
	if (length > 0 &&
		(eventproc->seqs = POSIXALIGN(sysconf(_SC_PAGESIZE), length * sizeof (seq_t))) == NULL) {
		FREE(eventproc);
		return NULL;
	}
	for (i = 0; i < length; ++i)
		eventproc->seqs[i] = seq_new();
	return eventproc;
}

/* FIXME */
void eventproc_free(eventproc_t *epp) {
	size_t i;

	if (unlikely(epp == NULL || *epp == NULL))
		return;
	for (i = 0; i < (*epp)->length; ++i)
		seq_free(&(*epp)->seqs[i]);
	FREE((*epp)->seqs);
	FREE(*epp);
}

/* FIXME */
ringbuf_t eventproc_get_ringbuf(eventproc_t eventproc) {
	if (unlikely(eventproc == NULL || eventproc->length == 0))
		return NULL;
	return eventproc->ringbufs[0];
}

/* FIXME */
ringbuf_t *eventproc_get_ringbufs(eventproc_t eventproc) {
	if (unlikely(eventproc == NULL))
		return NULL;
	return eventproc->ringbufs;
}

/* FIXME */
seqbar_t eventproc_get_seqbar(eventproc_t eventproc) {
	if (unlikely(eventproc == NULL || eventproc->length == 0))
		return NULL;
	return eventproc->seqbars[0];
}

/* FIXME */
seqbar_t *eventproc_get_seqbars(eventproc_t eventproc) {
	if (unlikely(eventproc == NULL))
		return NULL;
	return eventproc->seqbars;
}

/* FIXME */
seq_t eventproc_get_seq(eventproc_t eventproc) {
	if (unlikely(eventproc == NULL || eventproc->length == 0))
		return NULL;
	return eventproc->seqs[0];
}

/* FIXME */
seq_t *eventproc_get_seqs(eventproc_t eventproc) {
	if (unlikely(eventproc == NULL))
		return NULL;
	return eventproc->seqs;
}

/* FIXME */
size_t eventproc_get_length(eventproc_t eventproc) {
	if (unlikely(eventproc == NULL))
		return 0;
	return eventproc->length;
}

