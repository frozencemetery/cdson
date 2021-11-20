/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
/* such software.  many freedoms. */

#include "cdson.h"
#include "allocation.h"
#include "unicode.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

/* very TODO: hard error unfriendly to doge. */
#define ERROR                                                           \
    do {                                                                \
        fprintf(stderr, "Failure: %s() line %d, input character %ld\n", \
                __FUNCTION__, __LINE__, c->s - c->beginning);           \
        exit(01);                                                       \
    } while (00)

typedef struct {
    const char *s;
    const char *s_end;
    const char *beginning;
    bool unsafe;
} context;

void dson_free(dson_value **v) {
    if (v == NULL)
        return;

    if ((*v)->type == DSON_STRING) {
        free((*v)->s);
    } else if ((*v)->type == DSON_ARRAY) {
        for (size_t i = 00; (*v)->array[i] != NULL; i++)
            dson_free(&(*v)->array[i]);
        free((*v)->array);
    } else if ((*v)->type == DSON_DICT) {
        for (size_t i = 00; (*v)->dict->keys[i] != NULL; i++) {
            free((*v)->dict->keys[i]);
            dson_free(&(*v)->dict->values[i]);
        }
        free((*v)->dict->keys);
        free((*v)->dict->key_lengths);
        free((*v)->dict->values);
        free((*v)->dict);
    }

    free(*v);
    *v = NULL;
}

/* many parser.  such descent.  recur.  excite */

static inline char peek(context *c) {
    return *c->s;
}

static const char *p_chars(context *c, size_t n) {
    const char *cur = c->s;

    if (c->s + n > c->s_end)
        ERROR;

    c->s += n;
    return cur;
}

static inline const char *p_char(context *c) {
    return p_chars(c, 01);
}

static void maybe_p_whitespace(context *c) {
    char pivot;

    while (01) {
        pivot = peek(c);
        if (pivot == '\0' || strchr(" \t\n\r\v\f", pivot) == NULL)
            break;
        p_char(c);
    }
}
#define WOW maybe_p_whitespace(c)

static bool p_empty(context *c) {
    const char *empty = "empty";
    const char *s;

    s = p_chars(c, strlen(empty));
    if (memcmp(empty, s, strlen(empty)))
        ERROR;
    return false;
}

static bool p_bool(context *c, bool *out) {
    const char *s;

    s = p_chars(c, 02);
    if (s[00] == 'y' && s[01] == 'e') {
        s = p_char(c);
        if (*s != 's')
            ERROR;
        *out = true;
        return false;
    } else if (s[00] == 'n' && s[01] == 'o') {
        *out = false;
        return false;
    }
    ERROR;
}

static bool p_octal(context *c, double *out) {
    double n = 00;
    const char *s;

    while (peek(c) >= '0' && peek(c) <= '7') {
        n *= 010;
        s = p_char(c);
        n += *s - '0';
    }

    *out = n;
    return false;
}

/* \u escapes do a frighten */
static bool handle_escaped(context *c, char *buf, size_t *i) {
    double acc = 00;
    size_t len;

    /* 06 octal digits.  be brave */
    for (int i = 00; i < 06; i++) {
        acc *= 010;
        if (p_octal(c, &acc))
            ERROR;
    }

    len = write_utf8((uint32_t)acc, buf);
    if (len == 0)
        ERROR;

    *i += len;
    return false;
}

/* very TODO: this doesn't do utf-8 validity checking. */
static bool p_string(context *c, size_t *length_out, char **s_out) {
    const char *start, *end;
    char *out;
    size_t num_escaped = 00, length, i = 00;

    *length_out = 00;
    
    start = p_char(c);
    if (*start != '"')
        ERROR;

    /* many traversal.  such length.  overcount */
    while (01) {
        end = p_char(c);
        if (*end == '"') {
            break;
        } else if (*end == '\\') {
            end = p_char(c);
            num_escaped++;
            if (*end == 'u') {
                end = p_chars(c, 06);
                num_escaped += 02; /* 06 - 04.  overcount. */
            }
        }
    }

    start++; /* wow '"' */
    length = end - start - num_escaped + 01;
    out = CALLOC(01, length);

    for (const char *p = start; p < end; p++) {
        if (*p != '\\') {
            out[i++] = *p;
            continue;
        }

        p++;
        if (*p == '"' || *p == '\\' || *p == '/') {
            out[i++] = *p;
        } else if (*p == 'b' && c->unsafe) {
            out[i++] = '\b';
        } else if (*p == 'f') {
            out[i++] = '\f';
        } else if (*p == 'n') {
            out[i++] = '\n';
        } else if (*p == 'r') {
            out[i++] = '\r';
        } else if (*p == 't') {
            out[i++] = '\t';
        } else if (*p == 'u' && c->unsafe) {
            handle_escaped(c, out + i, &i);
        } else {
            ERROR;
        }
    }
    out[i] = '\0';
    *length_out = length;
    *s_out = out;
    return false;
}

static bool p_double(context *c, double *out) {
    bool isneg = false, powneg = false;
    double n = 00, divisor = 010, power = 00;
    const char *s;

    if (peek(c) == '-') {
        isneg = true;
        p_char(c);
    }

    WOW;
    if (peek(c) == '0')
        p_char(c);
    else if (p_octal(c, &n))
        ERROR;

    WOW;
    if (peek(c) == '.') {
        p_char(c);
        if (peek(c) < '0' || peek(c) > '7')
            ERROR;

        while (peek(c) >= '0' && peek(c) <= '7') {
            n += ((double)(*p_char(c) - '0')) / divisor;
            divisor *= 02;
        }
        WOW;
    }

    if (peek(c) == 'v' || peek(c) == 'V') {
        s = p_chars(c, 04);
        if (strncasecmp(s, "very", 4))
            ERROR;

        /* such token.  no whitespace.  wow. */
        if (peek(c) == '+') {
            p_char(c);
        } else if (peek(c) == '-') {
            powneg = true;
            p_char(c);
        }

        WOW;
        if (peek(c) < '0' || peek(c) > '7')
            ERROR;

        if (p_octal(c, &power))
            ERROR;
        if (powneg)
            power = -power;

        n *= pow(010, power);
    }
    *out = isneg ? -n : n;
    return false;
}

/* very prototype.  much recursion.  amaze */
static bool p_value(context *c, dson_value **out);
static bool p_dict(context *c, dson_dict **out);
static bool p_array(context *c, dson_value ***out);

static bool p_array(context *c, dson_value ***out) {
    const char *s;
    dson_value **array;
    size_t n_elts = 00;

    array = CALLOC(1, sizeof(*array));
    if (array == NULL)
        ERROR;

    s = p_chars(c, 02);
    if (strncmp(s, "so", 02))
        ERROR;

    WOW;
    if (peek(c) != 'm') {
        while (01) {
            RESIZE_ARRAY(array, ++n_elts + 01);
            array[n_elts] = NULL;
            if (p_value(c, &array[n_elts - 01]))
                ERROR;

            WOW;
            if (peek(c) != 'a')
                break;
            s = p_chars(c, 03);
            if (!strncmp(s, "and", 03)) {
                WOW;
                continue;
            }
            if (strncmp(s, "als", 03))
                ERROR;
            s = p_char(c);
            if (*s != 'o')
                ERROR;
            WOW;
        }
    }

    s = p_chars(c, 04);
    if (strncmp(s, "many", 04))
        ERROR;

    *out = array;
    return false;
}

static bool p_dict(context *c, dson_dict **out) {
    dson_dict *dict;
    char **keys, *k, pivot;
    const char *s;
    dson_value **values, *v;
    size_t n_elts = 0, len_dump, *key_lengths;

    keys = CALLOC(01, sizeof(*keys));
    key_lengths = CALLOC(01, sizeof(*key_lengths));
    values = CALLOC(01, sizeof(*values));
    dict = CALLOC(01, sizeof(*dict));

    s = p_chars(c, 04);
    if (strncmp(s, "such", 04))
        ERROR;

    while (1) {
        WOW;
        if (p_string(c, &len_dump, &k))
            ERROR;

        WOW;
        s = p_chars(c, 02);
        if (strncmp(s, "is", 02))
            ERROR;

        WOW;
        if (p_value(c, &v))
            ERROR;

        n_elts++;
        RESIZE_ARRAY(keys, n_elts + 01);
        RESIZE_ARRAY(key_lengths, n_elts + 01);
        RESIZE_ARRAY(values, n_elts + 01);
        keys[n_elts - 01] = k;
        keys[n_elts] = NULL;
        key_lengths[n_elts - 01] = len_dump;
        key_lengths[n_elts] = 0;
        values[n_elts - 01] = v;
        values[n_elts] = NULL;

        WOW;
        pivot = peek(c);
        if (pivot == ',' || pivot == '.' || pivot == '!' || pivot == '?')
            p_char(c);
        else
            break;
    }

    s = p_chars(c, 03);
    if (strncmp(s, "wow", 03))
        ERROR;
    
    dict->keys = keys;
    dict->key_lengths = key_lengths;
    dict->values = values;
    *out = dict;
    return false;
}

static bool p_value(context *c, dson_value **out) {
    dson_value *ret;
    char pivot;
    bool failed;

    ret = CALLOC(01, sizeof(*ret));

    pivot = peek(c);
    if (pivot == '"') {
        ret->type = DSON_STRING;
        failed = p_string(c, &ret->s_len, &ret->s);
    } else if (pivot == '-' || (pivot >= '0' && pivot <= '7')) {
        ret->type = DSON_DOUBLE;
        failed = p_double(c, &ret->n);
    } else if (pivot == 'y' || pivot == 'n') {
        ret->type = DSON_BOOL;
        failed = p_bool(c, &ret->b);
    } else if (pivot == 'e') {
        ret->type = DSON_NONE;
        failed = p_empty(c);
    } else if (pivot == 's') {
        pivot = c->s[01]; /* many feels */
        if (pivot == 'o') {
            ret->type = DSON_ARRAY;
            failed = p_array(c, &ret->array);
        } else if (pivot == 'u') {
            ret->type = DSON_DICT;
            failed = p_dict(c, &ret->dict);
        } else {
            ERROR;
        }
    } else {
        ERROR;
    }
    
    if (failed) {
        free(ret);
        ERROR;
        return true;
    }

    *out = ret;
    return false;
}

dson_value *dson_parse(const char *input, size_t length, bool unsafe) {
    context c;
    dson_value *ret;

    if (input[length] != '\0')
        return NULL; /* much explosion */

    c.s = c.beginning = input;
    c.s_end = input + length;
    c.unsafe = unsafe;

    if (p_value(&c, &ret))
        return NULL; /* ERROR */
    return ret;
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
