/*
 * Copyright (c) 2015-2016, Dalian Futures Information Technology Co., Ltd.
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

#ifndef RINGBUF_INCLUDED
#define RINGBUF_INCLUDED

#include <stdbool.h>
#include "seq.h"
#include "waitstg.h"
#include "seqbar.h"

/* FIXME: exported types */
typedef struct ringbuf_t *ringbuf_t;

/* FIXME: exported functions */
extern ringbuf_t ringbuf_new_single(int bufsize, waitstg_t waitstg);
extern ringbuf_t ringbuf_new_multi(int bufsize, waitstg_t waitstg);
extern void      ringbuf_free(ringbuf_t *rp);
extern int       ringbuf_get_bufsize(ringbuf_t ringbuf);
extern long      ringbuf_get_cursor(ringbuf_t ringbuf);
extern void      ringbuf_add_gatingseqs(ringbuf_t ringbuf, seq_t *gatingseqs, int length);
extern void      ringbuf_remove_gatingseq(ringbuf_t ringbuf, seq_t gatingseq);
extern long      ringbuf_get_min_gatingseq(ringbuf_t ringbuf);
extern seqbar_t  ringbuf_new_bar(ringbuf_t ringbuf, seq_t *seqs, int length);
extern bool      ringbuf_has_avail_cap(ringbuf_t ringbuf, int requiredcap);
extern long      ringbuf_remaining_cap(ringbuf_t ringbuf);
extern void     *ringbuf_get(ringbuf_t ringbuf, long seq);
extern void     *ringbuf_claim_and_get(ringbuf_t ringbuf, long seq);
extern long      ringbuf_next(ringbuf_t ringbuf);
extern long      ringbuf_next_n(ringbuf_t ringbuf, int n);
extern long      ringbuf_try_next(ringbuf_t ringbuf);
extern long      ringbuf_try_next_n(ringbuf_t ringbuf, int n);
extern void      ringbuf_publish(ringbuf_t ringbuf, long seq);
extern void      ringbuf_publish_batch(ringbuf_t ringbuf, long lo, long hi);
extern void      ringbuf_reset_to(ringbuf_t ringbuf, long seq);
extern bool      ringbuf_is_published(ringbuf_t ringbuf, long seq);

#endif /* RINGBUF_INCLUDED */

