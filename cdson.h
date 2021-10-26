/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

#ifndef _CDSON_H
#define _CDSON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Types of basic objects. */
#define DSON_NONE 0
#define DSON_BOOL 1 /* yes / no */
#define DSON_DOUBLE 2
#define DSON_STRING 3
#define DSON_ARRAY 4
#define DSON_DICT 5
typedef uint8_t dson_type; /* Can't put enum in header file. */

/* Basic dictionary type - keys and values are NULL-terminated. */
typedef struct dson_dict {
    char **keys;
    char **values;
} dson_dict;

/* A parsed tree. */
typedef struct dson_value {
    dson_type type;
    union {
        bool b;
        double n;
        char *s;
        struct dson_value **value;
        dson_dict *dict;
    };
} dson_value;

/* Parse DSON from a NUL-terminated utf-8 stream.  Length does not include the
 * trailing NUL.  Returns NULL on failure. */
dson_value *dson_parse(char *input, size_t length);

/* Free and NULL a DSON object. */
void dson_free(dson_value **v);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /* _CDSON_H */

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
