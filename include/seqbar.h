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

#ifndef SEQBAR_INCLUDED
#define SEQBAR_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include "seq.h"
#include "waitstg.h"
#include "seqr.h"

/* FIXME: exported types */
typedef struct waitstg_t *waitstg_t;
typedef struct seqbar_t *seqbar_t;
typedef struct seqr_t *seqr_t;

/* FIXME: exported functions */
extern seqbar_t seqbar_new(seqr_t seqr, waitstg_t waitstg, seq_t cursor, seq_t* depseqs, size_t length);
extern void     seqbar_free(seqbar_t *sbp);
extern long     seqbar_wait_for(seqbar_t seqbar, long seq);
extern long     seqbar_get_cursor(seqbar_t seqbar);
extern bool     seqbar_is_alerted(seqbar_t seqbar);
extern void     seqbar_alert(seqbar_t seqbar);
extern void     seqbar_clean_alert(seqbar_t seqbar);

#endif /* SEQBAR_INCLUDED */

