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

#ifndef MEM_INCLUDED
#define MEM_INCLUDED

#include <stddef.h>

/* exported functions */
extern void *mem_alloc(size_t nbytes, const char *file, int line);
extern void *mem_calloc(size_t count, size_t nbytes, const char *file, int line);
extern void *mem_posixalign(size_t alignment, size_t nbytes, const char *file, int line);
extern void  mem_free(void *p, const char *file, int line);
extern void *mem_resize(void *p, size_t nbytes, const char *file, int line);
extern char *mem_strndup(const char *str, size_t length);
extern char *mem_strdup(const char *str);

/* exported macros */
#define ALLOC(nbytes) \
	mem_alloc(nbytes, __FILE__, __LINE__)
#define CALLOC(count, nbytes) \
	mem_calloc(count, nbytes, __FILE__, __LINE__)
#define POSIXALIGN(alignment, nbytes) \
	mem_posixalign(alignment, nbytes, __FILE__, __LINE__)
#define NEW(p) \
	(p = ALLOC(sizeof *(p)))
#define NEW0(p) \
	(p = CALLOC(1, sizeof *(p)))
#define FREE(p) \
	((void)(mem_free(p, __FILE__, __LINE__), p = 0))
#define RESIZE(p, nbytes) \
	(p = mem_resize(p, nbytes, __FILE__, __LINE__))

#endif /* MEM_INCLUDED */

