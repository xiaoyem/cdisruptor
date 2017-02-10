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

#include <limits.h>
#include <unistd.h>
#include "macros.h"
#include "mem.h"
#include "util.h"
#include "seqgrp.h"

/* FIXME */
seqgrp_t seqgrp_new(seq_t *seqs, size_t length) {
	seqgrp_t seqgrp;
	size_t i;

	if (unlikely(NEW0(seqgrp) == NULL))
		return NULL;
	seqgrp->length = length;
	if (length > 0 &&
		(seqgrp->seqs = POSIXALIGN(sysconf(_SC_PAGESIZE), length * sizeof (seq_t))) == NULL) {
		FREE(seqgrp);
		return NULL;
	}
	for (i = 0; i < length; ++i)
		seqgrp->seqs[i] = seqs[i];
	return seqgrp;
}

/* FIXME */
void seqgrp_free(seqgrp_t *sgp) {
	if (unlikely(sgp == NULL || *sgp == NULL))
		return;
	FREE((*sgp)->seqs);
	FREE(*sgp);
}

/* get the size of the group */
int seqgrp_size(seqgrp_t seqgrp) {
	if (unlikely(seqgrp == NULL))
		return 0;
	return seqgrp->length;
}

/* get the minimum sequence value for the group */
long seqgrp_get(seqgrp_t seqgrp) {
	if (unlikely(seqgrp == NULL))
		return LONG_MAX;
	return get_min_seq(seqgrp->seqs, seqgrp->length, LONG_MAX);
}

/* FIXME */
void seqgrp_set(seqgrp_t seqgrp, long val) {
	NOT_USED(seqgrp);
	NOT_USED(val);
}

/* FIXME!!! */
void seqgrp_add(seqgrp_t seqgrp, seq_t* seqs, size_t length) {
	size_t i;

	if (unlikely(seqgrp == NULL))
		return;
	if (length > 0 && RESIZE(seqgrp->seqs, (seqgrp->length + length) * sizeof (seq_t)) == NULL)
		return;
	for (i = 0; i < length; ++i)
		seqgrp->seqs[seqgrp->length + i] = seqs[i];
	seqgrp->length += length;
}

/* FIXME!!! */
bool seqgrp_remove(seqgrp_t seqgrp, seq_t seq) {
	NOT_USED(seqgrp);
	NOT_USED(seq);

	return false;
}

