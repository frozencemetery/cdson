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

/* Dictionary type.  Arrays are NULL-terminated.  dson_dicts created by
 * dson_parse() will be valid, \0-terminated UTF-8 unless unsafe=true was
 * passed.  Lengths do not include terminating \0 (as in strlen). */
typedef struct dson_dict {
    char **keys;
    size_t *key_lengths;
    struct dson_value **values;
} dson_dict;

/* A parsed tree. */
typedef struct dson_value {
    dson_type type;
    union {
        bool b;
        double n;
        struct { /* string - valid, \0-terminated UTF-8 unless unsafe=true. */
            char *s;
            size_t s_len;
        };
        struct dson_value **array;
        dson_dict *dict;
    };
} dson_value;

/* Parse DSON from a NUL-terminated UTF-8 stream.  Length does not include the
 * trailing '\0'.  Returns NULL success, or an error message on failure.  Pass
 * error message to free().
 *
 * Per spec, DSON permits placing all unicode characters (except control
 * characters) directly in strings, with a few optional backslash escapes
 * supported.  If this is not enough for your purposes and you with to handle
 * the arbitrary digit escaping in your strings as well, pass unsafe=true.  If
 * you are unsure if you need this, you probably don't.  Be safe.
 */
char *dson_parse(const char *input, size_t length, bool unsafe,
                 dson_value **out);

/* Serialize a DSON object into a character stream.  Will be valid UTF-8 so
 * long as no unsafe escapes are used within strings.  Pass the returned
 * string to free() to release allocated storage.  Returns NULL on success, or
 * an error message on failure.  Pass error message to free(). */
char *dson_dump(dson_value *in, size_t *len_out, char **out);

/* Recursively free and NULL a DSON object. */
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
