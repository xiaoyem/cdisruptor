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
#include "seqgrp.h"
#include "seqbar.h"

/* FIXME */
struct seqbar_t {
	seqr_t		seqr;
	waitstg_t	waitstg;
	seq_t		cursor;
	seqgrp_t	depseqs;
	bool		alerted;
};

/* FIXME */
seqbar_t seqbar_new(seqr_t seqr, waitstg_t waitstg, seq_t cursor, seq_t* depseqs, int length) {
	seqbar_t seqbar;

	if (unlikely(NEW(seqbar) == NULL))
		return NULL;
	seqbar->seqr    = seqr;
	seqbar->waitstg = waitstg;
	seqbar->cursor  = cursor;
	seqbar->depseqs = length == 0 ? seqgrp_new(&cursor, 1) : seqgrp_new(depseqs, length);
	seqbar->alerted = false;
	return seqbar;
}

/* FIXME */
void seqbar_free(seqbar_t *sbp) {
	if (unlikely(sbp == NULL || *sbp == NULL))
		return;
	seqgrp_free(&(*sbp)->depseqs);
	FREE(*sbp);
}

/* wait for the given sequence to be available for consumption */
long seqbar_wait_for(seqbar_t seqbar, long seq) {
	long availseq;

	if (unlikely(seqbar == NULL))
		return -1L;
	if (seqbar_is_alerted(seqbar))
		return -1L;
	availseq = waitstg_wait_for(seqbar->waitstg, seq, seqbar->cursor, seqbar->depseqs, seqbar);
	if (availseq < seq)
		return availseq;
	return seqr_get_highest_published_seq(seqbar->seqr, seq, availseq);
}

/* get the current cursor value that can be read */
long seqbar_get_cursor(seqbar_t seqbar) {
	if (unlikely(seqbar == NULL))
		return -1L;
	return seqgrp_get(seqbar->depseqs);
}

/* the current alert status for the barrier */
bool seqbar_is_alerted(seqbar_t seqbar) {
	bool *alerted_;

	if (unlikely(seqbar == NULL))
		return false;
	alerted_ = (void *)seqbar + offsetof(struct seqbar_t, alerted);
	return __atomic_load_n(alerted_, __ATOMIC_ACQUIRE);
}

/* FIXME */
void seqbar_alert(seqbar_t seqbar) {
	bool *alerted_;

	if (unlikely(seqbar == NULL))
		return;
	alerted_ = (void *)seqbar + offsetof(struct seqbar_t, alerted);
	__atomic_store_n(alerted_, true, __ATOMIC_RELEASE);
}

/* clear the current alert status */
void seqbar_clear_alert(seqbar_t seqbar) {
	bool *alerted_;

	if (unlikely(seqbar == NULL))
		return;
	alerted_ = (void *)seqbar + offsetof(struct seqbar_t, alerted);
	__atomic_store_n(alerted_, false, __ATOMIC_RELEASE);
}

