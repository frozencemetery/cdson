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
typedef uint8_t dson_type; /* Can take only the above values. */

/* Dictionary type.  Arrays are NULL-terminated.  dson_dicts created by
 * dson_parse() will be valid, \0-terminated UTF-8. */
typedef struct dson_dict {
    char **keys;
    struct dson_value **values;
} dson_dict;

/* A parsed tree. */
typedef struct dson_value {
    dson_type type;
    union {
        bool b;
        double n;
        char *s; /* string - valid, \0-terminated UTF-8. */
        struct dson_value **array;
        dson_dict *dict;
    };
} dson_value;

/* Parse DSON from a NUL-terminated UTF-8 stream.  length does not include the
 * trailing '\0' (it behaves like strlen()).  Returns NULL success, or an error
 * message on failure.  Pass error message to free().
 *
 * Per spec, DSON permits placing all unicode characters (except control
 * characters) directly in strings, with a few optional backslash escapes
 * supported.  If this is not enough for your purposes and you with to handle
 * the arbitrary control character escaping in your strings as well, pass
 * unsafe=true.  If you are unsure if you need this, you probably don't.  Be
 * safe. */
char *dson_parse(const char *input, size_t length, bool unsafe,
                 dson_value **out);

/* Retrieve a specific value from the parsed DSON tree.  This is a shortcut
 * method for traversing the tree by hand.  v_out is owned by tree; do not
 * free() v_out.  Returns NULL on success or an error message on failure.
 * Pass error message to free().
 *
 * match_behavior indicates what to do when duplicate keys are encountered in
 * a dict.  The current options are to return the FIRST result from the input
 * (i.e., no overriding), the LAST result (i.e., input can override itself),
 * or to ERROR (i.e., not permit duplicate keys).
 *
 * query syntax uses [] for array access and . for dict access, and is
 * \0-terminated.  An example valid query (excluding quotes) is:
 * ".alpha[3].beta", while something like "[3{alpha}][2]" would be invalid, as
 * would "[alpha].beta[2]".  Note that arrays are zero-indexed, and strings
 * cannot be quoted (else the quotes will be treated as part of the string).
 * Behavior when query strings contain control characters is undefined
 * (suggest you don't do that), and strings cannot contain any of "[].". */
#define DSON_MATCH_FIRST 0
#define DSON_MATCH_LAST 1
#define DSON_MATCH_ERROR 2
char *dson_fetch(dson_value *tree, const char *query, uint8_t match_behavior,
                 dson_value **v_out);

/* Serialize a DSON object into a UTF-8 bytestream.  All strings must be valid
 * UTF-8.  Pass the returned string to free() to release allocated storage.
 * Returns NULL on success, or an error message on failure.  Pass error
 * message to free(). */
char *dson_dump(dson_value *in, char **out, size_t *len_out);

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
