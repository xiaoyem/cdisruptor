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

#ifndef WAITSTG_INCLUDED
#define WAITSTG_INCLUDED

#include "seq.h"
#include "seqgrp.h"
#include "seqbar.h"

/* FIXME: exported types */
typedef struct waitstg_t *waitstg_t;
typedef struct seqbar_t *seqbar_t;

/* FIXME: exported functions */
extern waitstg_t waitstg_new_blocking(void);
extern waitstg_t waitstg_new_busyspin(void);
extern waitstg_t waitstg_new_liteblocking(void);
extern waitstg_t waitstg_new_phasedbackoff(long spinto, long yieldto, waitstg_t fallbackstg);
extern waitstg_t waitstg_new_sleeping(int retries);
extern waitstg_t waitstg_new_timeoutblocking(long timeout);
extern waitstg_t waitstg_new_yielding(void);
extern void      waitstg_free(waitstg_t *wp);
extern long      waitstg_wait_for(waitstg_t waitstg, long seq,
			seq_t cursor, seqgrp_t depseqs, seqbar_t seqbar);
extern void      waitstg_signal_all_when_blocking(waitstg_t waitstg);

#endif /* WAITSTG_INCLUDED */

