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

#include "macros.h"
#include "mem.h"
#include "eventproc.h"

/* FIXME */
struct eventproc_t {
	ringbuf_t	ringbuf;
	seqbar_t	seqbar;
	seq_t		seq;
};

/* FIXME */
eventproc_t eventproc_new(ringbuf_t ringbuf, seqbar_t seqbar) {
	eventproc_t eventproc;

	if (unlikely(NEW(eventproc) == NULL))
		return NULL;
	eventproc->ringbuf = ringbuf;
	eventproc->seqbar  = seqbar;
	eventproc->seq     = seq_new();
	return eventproc;
}

/* FIXME */
void eventproc_free(eventproc_t *epp) {
	if (unlikely(epp == NULL || *epp == NULL))
		return;
	seq_free(&(*epp)->seq);
	FREE(*epp);
}

/* FIXME */
ringbuf_t eventproc_get_ringbuf(eventproc_t eventproc) {
	if (unlikely(eventproc == NULL))
		return NULL;
	return eventproc->ringbuf;
}

/* FIXME */
seqbar_t eventproc_get_seqbar(eventproc_t eventproc) {
	if (unlikely(eventproc == NULL))
		return NULL;
	return eventproc->seqbar;
}

/* FIXME */
seq_t eventproc_get_seq(eventproc_t eventproc) {
	if (unlikely(eventproc == NULL))
		return NULL;
	return eventproc->seq;
}

