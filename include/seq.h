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

#ifndef SEQ_INCLUDED
#define SEQ_INCLUDED

#include <stdbool.h>

/* FIXME: exported types */
typedef struct seq_t {
	long	p1, p2, p3, p4, p5, p6, p7;
	long	value;
	long	p9, p10, p11, p12, p13, p14, p15;
} *seq_t;

/* FIXME: exported functions */
extern seq_t seq_new(void);
extern seq_t seq_new_val(long val);
extern void  seq_free(seq_t *sp);
extern long  seq_get(seq_t seq);
extern void  seq_set(seq_t seq, long val);
extern bool  seq_comp_and_set(seq_t seq, long oldval, long newval);
extern long  seq_incr_and_get(seq_t seq);
extern long  seq_add_and_get(seq_t seq, long incr);

#endif /* SEQ_INCLUDED */

