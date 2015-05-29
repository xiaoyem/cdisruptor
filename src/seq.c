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
#include "seq.h"

/* FIXME */
struct jieobj {
	int		length;
	struct seq_t	seq[0];
};

/* create a sequence initialised to -1 */
inline seq_t seq_new(void) {
	return seq_new_val(-1L);
}

/* create a sequence with a specified initial value */
seq_t seq_new_val(long val) {
	struct jieobj *obj;

	if ((obj = ALLOC(sizeof *obj + sizeof *obj->seq)) == NULL)
		return NULL;
	obj->length     = 1;
	obj->seq->value = val;
	return obj->seq;
}

/* FIXME */
seq_t seq_new_grp(seq_t *sp, int len) {
	struct jieobj *obj;
	int i;

	if ((obj = ALLOC(sizeof *obj + len * sizeof *obj->seq)) == NULL)
		return NULL;
	obj->length = len;
	for (i = 0; i < len; ++i)
		obj->seq[i].value = seq_get(sp[i]);
	return obj->seq;
}

/* FIXME */
void seq_free(seq_t *sp) {
	struct jieobj *obj;

	if (sp == NULL || *sp == NULL)
		return;
	obj = (struct jieobj *)((char *)*sp - sizeof (int));
	FREE(obj);
	*sp = NULL;
}

/* FIXME */
int seq_length(seq_t seq) {
	struct jieobj *obj;

	if (seq == NULL)
		return 0;
	obj = (struct jieobj *)((char *)seq - sizeof (int));
	return obj->length;
}

/* FIXME */
long seq_get(seq_t seq) {
	long *val_;

	if (seq == NULL)
		return -1L;
	val_ = (void *)seq + offsetof(struct seq_t, value);
	return __atomic_load_n(val_, __ATOMIC_ACQUIRE);
}

/* FIXME */
void seq_set(seq_t seq, long val) {
	long *val_;

	if (seq == NULL)
		return;
	val_ = (void *)seq + offsetof(struct seq_t, value);
	__atomic_store_n(val_, val, __ATOMIC_RELEASE);
}

/* perform a compare and set operation on the sequence */
bool seq_comp_and_set(seq_t seq, long oldval, long newval) {
	long *val_;

	if (seq == NULL)
		return false;
	val_ = (void *)seq + offsetof(struct seq_t, value);
	return __atomic_compare_exchange_n(val_, &oldval, newval, 1, __ATOMIC_RELEASE, __ATOMIC_RELAXED);
}

/* atomically increment the sequence by one */
inline long seq_incr_and_get(seq_t seq) {
	return seq_add_and_get(seq, 1L);
}

/* atomically add the supplied value */
long seq_add_and_get(seq_t seq, long incr) {
	long *val_;

	if (seq == NULL)
		return -1L;
	val_ = (void *)seq + offsetof(struct seq_t, value);
	return __atomic_add_fetch(val_, incr, __ATOMIC_RELEASE);
}

