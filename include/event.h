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

#ifndef EVENT_INCLUDED
#define EVENT_INCLUDED

/* FIXME: exported types */
typedef union event_t {
	long	l;
	void	*value;
} event_t;

/* FIXME: exported functions */
extern long  event_get_long(event_t *event);
extern void *event_get_value(event_t *event);
extern void  event_set_long(event_t *event, long l);
extern void  event_set_value(event_t *event, void *value);

#endif /* EVENT_INCLUDED */

