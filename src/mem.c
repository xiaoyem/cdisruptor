/*
 * Copyright (c) 1994,1995,1996,1997 by David R. Hanson.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * <http://www.opensource.org/licenses/mit-license.php>
 */

/*
 * revised by xiaoyem
 */

#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "mem.h"

void *mem_alloc(size_t nbytes, const char *file, int line) {
	NOT_USED(file);
	NOT_USED(line);

	if (nbytes < 1)
		return NULL;
	return malloc(nbytes);
}

void *mem_calloc(size_t count, size_t nbytes, const char *file, int line) {
	NOT_USED(file);
	NOT_USED(line);

	if (count < 1 || nbytes < 1)
		return NULL;
	return calloc(count, nbytes);
}

void *mem_posixalign(size_t alignment, size_t nbytes, const char *file, int line) {
	void *p;
	NOT_USED(file);
	NOT_USED(line);

	if (nbytes < 1)
		return NULL;
	if (posix_memalign(&p, alignment, nbytes) != 0)
		return NULL;
	return p;
}

void mem_free(void *p, const char *file, int line) {
	NOT_USED(file);
	NOT_USED(line);

	if (p)
		free(p);
}

void *mem_resize(void *p, size_t nbytes, const char *file, int line) {
	NOT_USED(file);
	NOT_USED(line);

	if (nbytes < 1)
		return NULL;
	return realloc(p, nbytes);
}

char *mem_strndup(const char *str, size_t length) {
	char *p;

	if (str == NULL)
		return NULL;
	if ((p = malloc(length + 1)) == NULL)
		return NULL;
	memcpy(p, str, length);
	p[length] = '\0';
	return p;
}

inline char *mem_strdup(const char *str) {
	return mem_strndup(str, strlen(str));
}

