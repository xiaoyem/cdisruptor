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

#ifndef SEQR_INCLUDED
#define SEQR_INCLUDED

#include <stdbool.h>
#include "waitstg.h"

/* FIXME */
#define SINGLE 1
#define MULTI  2

/* FIXME: exported types */
typedef struct waitstg_t *waitstg_t;
typedef struct seqr_t *seqr_t;

/* FIXME: exported functions */
extern seqr_t seqr_new(int bufsize, waitstg_t waitstg);
extern void   seqr_free(seqr_t *sp);
extern bool   seqr_has_avail_cap(seqr_t seqr, int requirecap);
extern long   seqr_next(seqr_t seqr);
extern long   seqr_next_n(seqr_t seqr, int n);
extern long   seqr_try_next(seqr_t seqr);
extern long   seqr_try_next_n(seqr_t seqr, int n);
extern long   seqr_remaining_cap(seqr_t seqr);
extern void   seqr_claim(seqr_t seqr, long seq);
extern void   seqr_publish(seqr_t seqr, long seq);
extern void   seqr_publish_batch(seqr_t seqr, long lo, long hi);
extern bool   seqr_is_avail(seqr_t seqr, long seq);
extern long   seqr_get_highest_published_seq(long nextseq, long availseq);

#endif /* SEQR_INCLUDED */

