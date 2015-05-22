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

#ifndef WAITSTG_INCLUDED
#define WAITSTG_INCLUDED

#include "seq.h"
#include "seqbar.h"

/* FIXME */
#define BLOCKING        1
#define BUSYSPIN        2
#define LITEBLOCKING    3
#define PHASEDBACKOFF   4
#define SLEEPING        5
#define TIMEOUTBLOCKING 6
#define YIELDING        7

/* FIXME: exported types */
typedef struct seqbar_t *seqbar_t;
typedef struct waitstg_t *waitstg_t;

/* FIXME: exported functions */
extern waitstg_t waitstg_new(int type);
extern void      waitstg_free(waitstg_t *wp);
extern long      waitstg_wait_for(waitstg_t waitstg, long seq, seq_t cursor, seq_t depseq, seqbar_t seqbar);
extern void      waitstg_signal_all_when_blocking(waitstg_t waitstg);

#endif /* WAITSTG_INCLUDED */

