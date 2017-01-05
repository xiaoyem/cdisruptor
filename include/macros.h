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

#ifndef MACROS_INCLUDED
#define MACROS_INCLUDED

#include <ctype.h>
#include <stddef.h>
#include <string.h>

/* exported macros */
#define NOT_USED(x) ((void)x)
#define NELEMS(x)   (sizeof x / sizeof x[0])
#define MIN(a, b)   (a < b ? a : b)
#define MAX(a, b)   (a > b ? a : b)
#define LTRIM(str)  \
	do { \
		size_t i = 0; \
		while (str[i] != '\0' && isspace(str[i])) ++i; \
		memmove(str, str + i, strlen(str) - i + 1); \
	} while (0);
#define RTRIM(str)  \
	do { \
		size_t i = strlen(str); \
		while (i > 0 && isspace(str[i - 1])) --i; \
		str[i] = '\0'; \
	} while (0);
#define RMCHR(str, c) \
	do { \
		char *p, *q; \
		for (p = q = str; *p != '\0'; ++p) \
			if (*p != c) *q++ = *p; \
		*q = '\0'; \
	} while (0);
#define STR2LOWER(str) \
	do { \
		char *p = str; \
		while (*p) { \
			*p = tolower(*p); \
			++p; \
		} \
	} while (0);
#define STR2UPPER(str) \
	do { \
		char *p = str; \
		while (*p) { \
			*p = toupper(*p); \
			++p; \
		} \
	} while (0);

#ifdef __GNUC__
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x)   !!(x)
#define unlikely(x) !!(x)
#endif

#endif /* MACROS_INCLUDED */

