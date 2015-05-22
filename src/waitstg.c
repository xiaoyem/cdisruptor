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

#include <pthread.h>
#include "mem.h"
#include "waitstg.h"

/* FIXME */
struct waitstg_t {
	int		type;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
};

/* FIXME */
waitstg_t waitstg_new(int type) {
	waitstg_t waitstg;
	pthread_mutexattr_t mattr;

	if (NEW(waitstg) == NULL)
		return NULL;
	waitstg->type = type;
	switch (waitstg->type) {
	case BLOCKING:
		pthread_mutexattr_init(&mattr);
		pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ADAPTIVE_NP);
		pthread_mutex_init(&waitstg->lock, &mattr);
		pthread_mutexattr_destroy(&mattr);
		pthread_cond_init(&waitstg->cond, NULL);
		break;
	case BUSYSPIN:
		break;
	}
	return waitstg;
}

/* FIXME */
void waitstg_free(waitstg_t *wp) {
	if (wp == NULL || *wp == NULL)
		return;
	switch ((*wp)->type) {
	case BLOCKING:
		pthread_mutex_destroy(&(*wp)->lock);
		pthread_cond_destroy(&(*wp)->cond);
		break;
	case BUSYSPIN:
		break;
	}
	FREE(*wp);
}

/* wait for the given sequence to be available */
long waitstg_wait_for(waitstg_t waitstg, long seq, seq_t cursor, seq_t depseq, seqbar_t seqbar) {
	long availseq = -1L;

	switch (waitstg->type) {
	case BLOCKING:
		if ((availseq = seq_get(cursor)) < seq) {
			pthread_mutex_lock(&waitstg->lock);
			while ((availseq = seq_get(cursor)) < seq) {
				if (seqbar_is_alerted(seqbar)) {
					pthread_mutex_unlock(&waitstg->lock);
					return -1L;
				}
				pthread_cond_wait(&waitstg->cond, &waitstg->lock);
			}
			pthread_mutex_unlock(&waitstg->lock);
		}
		while ((availseq = seq_get(cursor)) < seq)
			if (seqbar_is_alerted(seqbar))
				return -1L;
		break;
	case BUSYSPIN:
		while ((availseq = seq_get(depseq)) < seq)
			if (seqbar_is_alerted(seqbar))
				return -1L;
		break;
	}
	return availseq;
}

/* FIXME */
void waitstg_signal_all_when_blocking(waitstg_t waitstg) {
	switch (waitstg->type) {
	case BLOCKING:
		pthread_mutex_lock(&waitstg->lock);
		pthread_cond_signal(&waitstg->cond);
		pthread_mutex_unlock(&waitstg->lock);
		break;
	case BUSYSPIN:
		break;
	}
}

