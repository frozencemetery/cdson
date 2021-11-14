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

/* Opaque dictionary type - keys and values are NULL-terminated.  Do not put
 * NUL-bytes in your keys: you will have a bad time.  Methods for use are
 * below. */
struct dson_dict;
typedef struct dson_dict dson_dict;

typedef struct dson_string {
    /* Can be invalid UTF-8 and contain non-terminating NULs if unsafe
     * is enabled in dson_parse(); otherwise, is a NUL-terminated
     * UTF-8 string. */
    char *data;
    size_t len; /* Does not include terminating '\0', as in strlen(). */
} dson_string;

/* A parsed tree. */
typedef struct dson_value {
    dson_type type;
    union {
        bool b;
        double n;
        dson_string s;
        struct dson_value **array;
        dson_dict *dict;
    };
} dson_value;

/* Parse DSON from a NUL-terminated utf-8 stream.  Length does not include the
 * trailing '\0'.  Returns NULL on failure.
 *
 * Per spec, DSON permits placing all unicode characters (except control
 * characters) directly in strings, with a few optional backslash escapes
 * supported.  If this is not enough for your purposes and you with to handle
 * the arbitrary digit escaping in your strings as well, pass unsafe=true.  If
 * you are unsure if you need this, you probably don't.  Be safe.
 */
dson_value *dson_parse(const char *input, size_t length, bool unsafe);

/* Recursively free and NULL a DSON object. */
void dson_free(dson_value **v);

/* Retrieve a value from the given dict, or return NULL if not present.
 * Memory is owned by the dson_dict and should not be modified.  Keys will be
 * compared using the current locale.  Duplicates are discouraged, but if
 * encountered, the last match will be returned.  key must be valid UTF-8. */
dson_value *dson_dict_get(dson_dict *d, char *key);

/* Return a NULL-terminated, order-preserving list of the keys in a dict.
 * Keys should not be modified (but are not marked const because C doesn't
 * nest that well). */
char **dson_dict_keys(dson_dict *d);

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
