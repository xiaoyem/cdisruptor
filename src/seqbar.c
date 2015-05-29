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

#include "mem.h"
#include "seqbar.h"

/* FIXME */
struct seqbar_t {
	seqr_t		seqr;
	waitstg_t	waitstg;
	seq_t		cursorseq;
	seq_t		depseq;
	bool		alerted;
};

/* FIXME */
seqbar_t seqbar_new(seqr_t seqr, waitstg_t waitstg, seq_t cursorseq, seq_t depseqs) {
	seqbar_t seqbar;

	if (NEW(seqbar) == NULL)
		return NULL;
	seqbar->seqr      = seqr;
	seqbar->waitstg   = waitstg;
	seqbar->cursorseq = cursorseq;
	if (seq_length(depseqs) == 0)
		seqbar->depseq = cursorseq;
	else
		seqbar->depseq = depseqs;
	seqbar->alerted   = false;
	return seqbar;
}

/* FIXME */
void seqbar_free(seqbar_t *sp) {
	if (sp == NULL || *sp == NULL)
		return;
	FREE(*sp);
}

/* wait for the given sequence to be available for consumption */
long seqbar_wait_for(seqbar_t seqbar, long seq) {
	long availseq;

	if (seqbar == NULL)
		return -1L;
	if (seqbar_is_alerted(seqbar))
		return -1L;
	availseq = waitstg_wait_for(seqbar->waitstg, seq, seqbar->cursorseq, seqbar->depseq, seqbar);
	if (availseq < seq)
		return availseq;
	return seqr_get_highest_published_seq(seqbar->seqr, seq, availseq);
}

/* get the current cursor value that can be read */
long seqbar_get_cursor(seqbar_t seqbar) {
	if (seqbar == NULL)
		return -1L;
	return seq_get(seqbar->depseq);
}

/* the current alert status for the barrier */
bool seqbar_is_alerted(seqbar_t seqbar) {
	bool *alerted_;

	if (seqbar == NULL)
		return false;
	alerted_ = (void *)seqbar + offsetof(struct seqbar_t, alerted);
	return __atomic_load_n(alerted_, __ATOMIC_ACQUIRE);
}

/* FIXME */
void seqbar_alert(seqbar_t seqbar) {
	bool *alerted_;

	if (seqbar == NULL)
		return;
	alerted_ = (void *)seqbar + offsetof(struct seqbar_t, alerted);
	__atomic_store_n(alerted_, true, __ATOMIC_RELEASE);
}

/* clear the current alert status */
void seqbar_clear_alert(seqbar_t seqbar) {
	bool *alerted_;

	if (seqbar == NULL)
		return;
	alerted_ = (void *)seqbar + offsetof(struct seqbar_t, alerted);
	__atomic_store_n(alerted_, false, __ATOMIC_RELEASE);
}

