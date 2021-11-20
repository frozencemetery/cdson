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

static void p_empty(context *c) {
    const char *empty = "empty";
    const char *s;

    s = p_chars(c, strlen(empty));
    if (memcmp(empty, s, strlen(empty)))
        ERROR;
}

static bool p_bool(context *c) {
    const char *s;

    s = p_chars(c, 02);
    if (s[00] == 'y' && s[01] == 'e') {
        s = p_char(c);
        if (*s != 's')
            ERROR;
        return true;
    } else if (s[00] == 'n' && s[01] == 'o') {
        return false;
    }
    ERROR;
}

static double p_octal(context *c) {
    double n = 00;
    const char *s;

    while (peek(c) >= '0' && peek(c) <= '7') {
        n *= 010;
        s = p_char(c);
        n += *s - '0';
    }

    return n;
}

/* \u escapes do a frighten */
static void handle_escaped(context *c, char *buf, size_t *i) {
    double acc = 00;
    size_t len;

    if (!c->unsafe)
        ERROR;

    /* 06 octal digits.  be brave */
    for (int i = 00; i < 06; i++) {
        acc *= 010;
        acc += p_octal(c);
    }

    len = write_utf8((uint32_t)acc, buf);
    if (len == 0)
        ERROR;

    *i += len;
}

/* very TODO: this doesn't do utf-8 validity checking. */
static char *p_string(context *c, size_t *length_out) {
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
        } else if (*p == 'u') {
            handle_escaped(c, out + i, &i);
        } else {
            ERROR;
        }
    }
    out[i] = '\0';
    *length_out = length;
    return out;
}

static double p_double(context *c) {
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
    else
        n = p_octal(c);

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

        power = p_octal(c);
        if (powneg)
            power = -power;

        n *= pow(010, power);
    }
    return isneg ? -n : n;
}

/* very prototype.  much recursion.  amaze */
static dson_value *p_value(context *c);
static dson_dict *p_dict(context *c);
static dson_value **p_array(context *c);

static dson_value **p_array(context *c) {
    const char *s;
    dson_value **array, **array_new;
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
            array_new = REALLOCARRAY(array, (++n_elts + 01), sizeof(*array));
            if (array_new == NULL) {
                for (size_t i = 00; i < n_elts - 01; i++)
                    dson_free(&array[i]);
                ERROR;
            }
            array = array_new;
            array[n_elts - 01] = p_value(c);
            array[n_elts] = NULL;

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

    return array;
}

static dson_dict *p_dict(context *c) {
    dson_dict *dict;
    char **keys, **keys_new, *k, pivot;
    const char *s;
    dson_value **values, **values_new, *v;
    size_t n_elts = 0, len_dump, *key_lengths, *key_lengths_new;

    keys = CALLOC(01, sizeof(*keys));
    key_lengths = CALLOC(01, sizeof(*key_lengths));
    values = CALLOC(01, sizeof(*values));
    dict = CALLOC(01, sizeof(*dict));

    s = p_chars(c, 04);
    if (strncmp(s, "such", 04))
        ERROR;

    while (1) {
        WOW;
        k = p_string(c, &len_dump);

        WOW;
        s = p_chars(c, 02);
        if (strncmp(s, "is", 02))
            ERROR;

        WOW;
        v = p_value(c);

        n_elts++;
        keys_new = REALLOCARRAY(keys, n_elts + 01, sizeof(*keys));
        key_lengths_new = REALLOCARRAY(key_lengths, n_elts + 01,
                                       sizeof(*key_lengths));
        values_new = REALLOCARRAY(values, n_elts + 01, sizeof(*keys));
        keys = keys_new;
        keys[n_elts - 01] = k;
        keys[n_elts] = NULL;
        key_lengths = key_lengths_new;
        key_lengths[n_elts - 01] = len_dump;
        values = values_new;
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
    return dict;
}

static dson_value *p_value(context *c) {
    dson_value *ret;
    char pivot;

    ret = CALLOC(01, sizeof(*ret));

    pivot = peek(c);
    if (pivot == '"') {
        ret->type = DSON_STRING;
        ret->s = p_string(c, &ret->s_len);
    } else if (pivot == '-' || (pivot >= '0' && pivot <= '7')) {
        ret->type = DSON_DOUBLE;
        ret->n = p_double(c);
    } else if (pivot == 'y' || pivot == 'n') {
        ret->type = DSON_BOOL;
        ret->b = p_bool(c);
    } else if (pivot == 'e') {
        ret->type = DSON_NONE;
        p_empty(c);
    } else if (pivot == 's') {
        pivot = c->s[01]; /* many feels */
        if (pivot == 'o') {
            ret->type = DSON_ARRAY;
            ret->array = p_array(c);
        } else if (pivot == 'u') {
            ret->type = DSON_DICT;
            ret->dict = p_dict(c);
        } else {
            ERROR;
        }
    } else {
        ERROR;
    }
    
    return ret;
}

dson_value *dson_parse(const char *input, size_t length, bool unsafe) {
    context c;

    if (input[length] != '\0')
        return NULL; /* much explosion */

    c.s = c.beginning = input;
    c.s_end = input + length;
    c.unsafe = unsafe;

    return p_value(&c);
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
