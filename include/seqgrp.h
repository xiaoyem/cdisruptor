/*
 * Copyright (c) 2015-2016, Dalian Futures Information Technology Co., Ltd.
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

#ifndef SEQGRP_INCLUDED
#define SEQGRP_INCLUDED

#include "seq.h"

/* FIXME: exported types */
typedef struct seqgrp_t {
	int	length;
	seq_t	*seqs;
} *seqgrp_t;

/* FIXME: exported functions */
extern seqgrp_t seqgrp_new(seq_t *seqs, int length);
extern void     seqgrp_free(seqgrp_t *sgp);
extern int      seqgrp_size(seqgrp_t seqgrp);
extern long     seqgrp_get(seqgrp_t seqgrp);
extern void     seqgrp_set(seqgrp_t seqgrp, long val);
extern void     seqgrp_add(seqgrp_t seqgrp, seq_t seq);
extern bool     seqgrp_remove(seqgrp_t seqgrp, seq_t seq);

#endif /* SEQGRP_INCLUDED */

