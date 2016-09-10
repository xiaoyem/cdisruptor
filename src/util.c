/*
 * Copyright (c) 2015-2016, Dalian Futures Information Technology Co., Ltd.
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
#include "util.h"

long get_min_seq(seq_t *seqs, int length, long min) {
	int i;

	for (i = 0; i < length; ++i)
		min = MIN(min, seq_get(seqs[i]));
	return min;
}

int lg2(int i) {
	int r = 0;

	while ((i >>= 1) != 0)
		++r;
	return r;
}

