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

#include "macros.h"
#include "util.h"

long get_min_seq(seq_t seqs, long min) {
	int i, n = seq_length(seqs);

	for (i = 0; i < n; ++i)
		min = MIN(min, seq_get(&seqs[i]));
	return min;
}

int lg2(int i) {
	int r = 0;

	while ((i >>= 1) != 0)
		++r;
	return r;
}

