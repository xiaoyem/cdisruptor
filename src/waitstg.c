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

#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "macros.h"
#include "mem.h"
#include "waitstg.h"

/* FIXME */
#define BLOCKING        1
#define BUSYSPIN        2
#define LITEBLOCKING    3
#define PHASEDBACKOFF   4
#define SLEEPING        5
#define TIMEOUTBLOCKING 6
#define YIELDING        7

/* FIXME */
struct waitstg_t {
	int		type;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
	bool		signeeded;
	long		spinto, yieldto;
	waitstg_t	fallbackstg;
	int		retries;
	long		timeout;
};

/* FIXME */
waitstg_t waitstg_new_blocking(void) {
	waitstg_t waitstg;
	pthread_mutexattr_t mattr;

	if (unlikely(NEW(waitstg) == NULL))
		return NULL;
	waitstg->type = BLOCKING;
	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ADAPTIVE_NP);
	pthread_mutex_init(&waitstg->lock, &mattr);
	pthread_mutexattr_destroy(&mattr);
	pthread_cond_init(&waitstg->cond, NULL);
	return waitstg;
}

/* FIXME */
waitstg_t waitstg_new_busyspin(void) {
	waitstg_t waitstg;

	if (unlikely(NEW(waitstg) == NULL))
		return NULL;
	waitstg->type = BUSYSPIN;
	return waitstg;
}

/* FIXME */
waitstg_t waitstg_new_liteblocking(void) {
	waitstg_t waitstg;
	pthread_mutexattr_t mattr;

	if (unlikely(NEW(waitstg) == NULL))
		return NULL;
	waitstg->type      = LITEBLOCKING;
	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ADAPTIVE_NP);
	pthread_mutex_init(&waitstg->lock, &mattr);
	pthread_mutexattr_destroy(&mattr);
	pthread_cond_init(&waitstg->cond, NULL);
	waitstg->signeeded = false;
	return waitstg;
}

/* FIXME */
waitstg_t waitstg_new_phasedbackoff(long spinto, long yieldto, waitstg_t fallbackstg) {
	waitstg_t waitstg;

	if (unlikely(NEW(waitstg) == NULL))
		return NULL;
	waitstg->type        = PHASEDBACKOFF;
	waitstg->spinto      = spinto;
	waitstg->yieldto     = yieldto;
	waitstg->fallbackstg = fallbackstg;
	return waitstg;
}

/* FIXME */
waitstg_t waitstg_new_sleeping(int retries) {
	waitstg_t waitstg;

	if (unlikely(NEW(waitstg) == NULL))
		return NULL;
	waitstg->type    = SLEEPING;
	waitstg->retries = retries > 0 ? retries : 200;
	return waitstg;
}

/* FIXME */
waitstg_t waitstg_new_timeoutblocking(long timeout) {
	waitstg_t waitstg;
	pthread_mutexattr_t mattr;

	if (unlikely(NEW(waitstg) == NULL))
		return NULL;
	waitstg->type    = TIMEOUTBLOCKING;
	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ADAPTIVE_NP);
	pthread_mutex_init(&waitstg->lock, &mattr);
	pthread_mutexattr_destroy(&mattr);
	pthread_cond_init(&waitstg->cond, NULL);
	waitstg->timeout = timeout;
	return waitstg;
}

/* FIXME */
waitstg_t waitstg_new_yielding(void) {
	waitstg_t waitstg;

	if (unlikely(NEW(waitstg) == NULL))
		return NULL;
	waitstg->type = YIELDING;
	return waitstg;
}

/* FIXME */
void waitstg_free(waitstg_t *wp) {
	if (unlikely(wp == NULL || *wp == NULL))
		return;
	switch ((*wp)->type) {
	case BLOCKING:
	case LITEBLOCKING:
	case TIMEOUTBLOCKING:
		pthread_mutex_destroy(&(*wp)->lock);
		pthread_cond_destroy(&(*wp)->cond);
		break;
	case BUSYSPIN:
	case PHASEDBACKOFF:
	case SLEEPING:
	case YIELDING:
		break;
	}
	FREE(*wp);
}

/* wait for the given sequence to be available */
long waitstg_wait_for(waitstg_t waitstg, long seq,
	seq_t cursor, seqgrp_t depseqs, seqbar_t seqbar) {
	long availseq = -1L, starttime = 0;
	int count;

	if (unlikely(waitstg == NULL))
		return -1L;
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
		while ((availseq = seqgrp_get(depseqs)) < seq)
			if (seqbar_is_alerted(seqbar))
				return -1L;
		break;
	case BUSYSPIN:
		while ((availseq = seqgrp_get(depseqs)) < seq)
			if (seqbar_is_alerted(seqbar))
				return -1L;
		break;
	case LITEBLOCKING:
		if ((availseq = seq_get(cursor)) < seq) {
			pthread_mutex_lock(&waitstg->lock);
			do {
				(void)__atomic_exchange_n(&waitstg->signeeded, true, __ATOMIC_RELEASE);
				if ((availseq = seq_get(cursor)) >= seq)
					break;
				if (seqbar_is_alerted(seqbar)) {
					pthread_mutex_unlock(&waitstg->lock);
					return -1L;
				}
				pthread_cond_wait(&waitstg->cond, &waitstg->lock);
			} while ((availseq = seq_get(cursor)) < seq);
			pthread_mutex_unlock(&waitstg->lock);
		}
		while ((availseq = seqgrp_get(depseqs)) < seq)
			if (seqbar_is_alerted(seqbar))
				return -1L;
		break;
	case PHASEDBACKOFF:
		count = 10000;
		do {
			if ((availseq = seqgrp_get(depseqs)) >= seq)
				return availseq;
			if (--count == 0) {
				struct timespec ts;

				if (starttime == 0) {
					clock_gettime(CLOCK_REALTIME, &ts);
					starttime = ts.tv_nsec;
				} else {
					long delta;

					clock_gettime(CLOCK_REALTIME, &ts);
					delta = ts.tv_nsec - starttime;
					if (delta > waitstg->yieldto)
						return waitstg_wait_for(waitstg->fallbackstg,
							seq, cursor, depseqs, seqbar);
					else if (delta > waitstg->spinto)
						sched_yield();
				}
				count = 10000;
			}
		} while (1);
		break;
	case SLEEPING:
		count = waitstg->retries;
		while ((availseq = seqgrp_get(depseqs)) < seq) {
			if (seqbar_is_alerted(seqbar))
				return -1L;
			if (count > 100)
				--count;
			else if (count > 0) {
				--count;
				sched_yield();
			} else
				sleep(0);
		}
		break;
	case TIMEOUTBLOCKING:
		if ((availseq = seq_get(cursor)) < seq) {
			struct timespec ts;

			ts.tv_nsec = waitstg->timeout;
			pthread_mutex_lock(&waitstg->lock);
			while ((availseq = seq_get(cursor)) < seq) {
				int ret;

				if (seqbar_is_alerted(seqbar)) {
					pthread_mutex_unlock(&waitstg->lock);
					return -1L;
				}
				ret = pthread_cond_timedwait(&waitstg->cond, &waitstg->lock, &ts);
				if (ret == ETIMEDOUT) {
					pthread_mutex_unlock(&waitstg->lock);
					return -1L;
				}
			}
			pthread_mutex_unlock(&waitstg->lock);
		}
		while ((availseq = seqgrp_get(depseqs)) < seq)
			if (seqbar_is_alerted(seqbar))
				return -1L;
		break;
	case YIELDING:
		count = 100;
		while ((availseq = seqgrp_get(depseqs)) < seq) {
			if (seqbar_is_alerted(seqbar))
				return -1L;
			if (count == 0)
				sched_yield();
			else
				--count;
		}
		break;
	}
	return availseq;
}

/* FIXME */
void waitstg_signal_all_when_blocking(waitstg_t waitstg) {
	if (unlikely(waitstg == NULL))
		return;
	switch (waitstg->type) {
	case BLOCKING:
		pthread_mutex_lock(&waitstg->lock);
		pthread_cond_broadcast(&waitstg->cond);
		pthread_mutex_unlock(&waitstg->lock);
		break;
	case LITEBLOCKING:
		if (__atomic_exchange_n(&waitstg->signeeded, false, __ATOMIC_RELEASE)) {
			pthread_mutex_lock(&waitstg->lock);
			pthread_cond_broadcast(&waitstg->cond);
			pthread_mutex_unlock(&waitstg->lock);
		}
		break;
	case PHASEDBACKOFF:
		waitstg_signal_all_when_blocking(waitstg->fallbackstg);
		break;
	case TIMEOUTBLOCKING:
		pthread_mutex_lock(&waitstg->lock);
		pthread_cond_broadcast(&waitstg->cond);
		pthread_mutex_unlock(&waitstg->lock);
		break;
	case BUSYSPIN:
	case SLEEPING:
	case YIELDING:
		break;
	}
}

