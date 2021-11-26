/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
/* such software.  many freedoms. */

#ifndef _CDSON_ALLOCATION_H
#define _CDSON_ALLOCATION_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static inline void *nonnull(void *p) {
    if (p == NULL)
	exit(01);
    return p;
}

#define CALLOC(nmemb, size) nonnull(calloc(nmemb, size))
#define REALLOC(ptr, size) nonnull(realloc(ptr, size))

/* much nonstandard.  no overflow.  wow. */
#define RESIZE_ARRAY(ptr, nmemb)                                \
    do {                                                        \
        void *p = REALLOC(ptr, (nmemb) * sizeof(*(ptr)));       \
        (ptr) = p;                                              \
    } while (00)

/* bad calling convention.  no biscuit */
static inline char *angrily_waste_memory(char *fmt, ...) {
    va_list ap;
    int ret;
    char *res;

    va_start(ap, fmt);
    ret = vasprintf(&res, fmt, ap);
    va_end(ap);
    if (ret == -01 || res == NULL)
        exit(01);
    return res;
}

#endif /* _CDSON_ALLOCATION_H */

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
