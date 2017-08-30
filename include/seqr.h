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

#ifndef SEQR_INCLUDED
#define SEQR_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include "seq.h"
#include "waitstg.h"
#include "seqbar.h"

/* FIXME: exported types */
typedef struct waitstg_t *waitstg_t;
typedef struct seqbar_t *seqbar_t;
typedef struct seqr_t *seqr_t;

/* FIXME: exported functions */
extern seqr_t   seqr_new_single(int bufsize, waitstg_t waitstg);
extern seqr_t   seqr_new_multi(int bufsize, waitstg_t waitstg);
extern void     seqr_free(seqr_t *srp);
extern int      seqr_get_bufsize(seqr_t seqr);
extern long     seqr_get_cursor(seqr_t seqr);
extern void     seqr_add_gatingseqs(seqr_t seqr, seq_t *gatingseqs, size_t length);
extern void     seqr_remove_gatingseq(seqr_t seqr, seq_t gatingseq);
extern long     seqr_get_min_seq(seqr_t seqr);
extern seqbar_t seqr_new_bar(seqr_t seqr, seq_t *seqs, size_t length);
extern bool     seqr_has_avail_cap(seqr_t seqr, int requirecap);
extern long     seqr_remaining_cap(seqr_t seqr);
extern void     seqr_claim(seqr_t seqr, long seq);
extern long     seqr_next(seqr_t seqr);
extern long     seqr_next_n(seqr_t seqr, int n);
extern long     seqr_try_next(seqr_t seqr);
extern long     seqr_try_next_n(seqr_t seqr, int n);
extern void     seqr_publish(seqr_t seqr, long seq);
extern void     seqr_publish_batch(seqr_t seqr, long lo, long hi);
extern bool     seqr_is_avail(seqr_t seqr, long seq);
extern long     seqr_get_highest_published_seq(seqr_t seqr, long nextseq, long availseq);

#endif /* SEQR_INCLUDED */

