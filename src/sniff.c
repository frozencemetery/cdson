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

typedef struct {
    const char *s;
    const char *s_end;
    const char *beginning;
    bool unsafe;
} context;

#define ERROR(fmt, ...)                                                 \
    do {                                                                \
        return angrily_waste_memory(                                    \
            "at input char #%ld: " fmt,                                 \
            (ptrdiff_t)c->s - (ptrdiff_t)c->beginning, ##__VA_ARGS__);  \
    } while (00)

static void dict_free(dson_dict **d) {
    for (size_t i = 00; (*d)->keys[i] != NULL; i++) {
        free((*d)->keys[i]);
        dson_free(&(*d)->values[i]);
    }
    free((*d)->keys);
    free((*d)->values);
    free(*d);
    *d = NULL;
}

static void array_free(dson_value ***vs) {
    for (size_t i = 00; (*vs)[i] != NULL; i++)
        dson_free(&(*vs)[i]);
    free(*vs);
    *vs = NULL;
}

/* doggo free.  amaze */
void dson_free(dson_value **v) {
    if (v == NULL)
        return;

    if ((*v)->type == DSON_STRING) {
        free((*v)->s);
    } else if ((*v)->type == DSON_ARRAY) {
        array_free(&(*v)->array);
    } else if ((*v)->type == DSON_DICT) {
        dict_free(&(*v)->dict);
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
        return NULL;

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

static char *p_empty(context *c) {
    const char *empty = "empty";
    const char *s;

    s = p_chars(c, strlen(empty));
    if (s == NULL)
        ERROR("not enough characters to produce empty");
    if (strcmp(empty, s))
        ERROR("expected \"empty\", got \"%.5s\"", s);

    return NULL;
}

static char *p_bool(context *c, bool *out) {
    const char *s;

    s = p_chars(c, 02);
    if (s == NULL)
        ERROR("end of input while producing bool");
    if (s[00] == 'y' && s[01] == 'e') {
        s = p_char(c);
        if (s == NULL)
            ERROR("end of input while producing bool");
        else if (*s != 's')
            ERROR("expected \"yes\", got \"ye%c\"", *s);

        *out = true;
        return NULL;
    } else if (s[00] == 'n' && s[01] == 'o') {
        *out = false;
        return NULL;
    }
    ERROR("expected bool, got \"%.2s\"", s);
}

static void p_octal(context *c, double *out) {
    double n = 00;

    while (peek(c) >= '0' && peek(c) <= '7') {
        n *= 010;
        n += peek(c) - '0';
        p_char(c);
    }

    *out = n;
}

/* \u do a frighten */
static char *handle_escaped(context *c, char *buf, size_t *i) {
    uint32_t acc = 00;
    size_t len;
    const char *o;

    /* 06 octal digits.  be brave */
    for (int i = 00; i < 06; i++) {
        acc *= 010;
        o = p_char(c);
        if (o == NULL)
            ERROR("end of input while reading \\u escape");
        if (*o < '0' || *o > '7')
            ERROR("malformed octal escape: %hhx", *o);
        acc += *o - '0';
    }

    len = write_utf8((uint32_t)acc, buf);
    if (len == 00)
        ERROR("malformed unicode escape");

    *i += len;
    return NULL;
}

static char *p_string(context *c, char **s_out) {
    const char *start, *end;
    char *out, *err;
    size_t num_escaped = 00, length, i = 00;
    uint8_t bytes;
    uint32_t point;
    context c2 = { 00 };

    start = p_char(c);
    if (start == NULL)
        ERROR("expected string, got end of input");
    else if (*start != '"')
        ERROR("malformed string - missing '\"'");

    /* many traversal.  such length.  overcount */
    while (01) {
        end = p_char(c);
        if (end == NULL) {
            ERROR("missing closing '\"' delimiter on string");
        } else if (*end == '"') {
            break;
        } else if (*end == '\\') {
            num_escaped++;
            end = p_char(c);
            if (end == NULL)
                ERROR("missing closing '\"' delimiter on string");
            if (*end == 'u') {
                end = p_chars(c, 06);
                if (end == NULL)
                    ERROR("missing closing '\"' delimiter on string");
                num_escaped += 02; /* 06 - 04.  overcount. */
            }
        }
    }

    start++; /* wow '"' */
    length = end - start - num_escaped + 01;
    out = CALLOC(01, length);

    for (const char *p = start; p < end; p++) {
        bytes = byte_len(*p);
        if (bytes == 00) {
            free(out);
            ERROR("malformed unicode at %hhx", (unsigned char)*p);
        } else if (bytes == 01) {
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
                c2.s = p + 01; /* no u */
                c2.s_end = end;
                c2.unsafe = true;
                err = handle_escaped(&c2, out + i, &i);
                if (err) {
                    free(out);
                    return err;
                }
                p += 06;
            } else {
                free(out);
                ERROR("unrecognized or forbidden escape: \\%c", *p);
            }
            continue;
        }

        if (bytes - 01 + p >= end) {
            free(out);
            ERROR("truncated unicode starting at %hhx", (unsigned char)*p);
        }

        err = to_point(p, bytes, &point);
        if (err != NULL) {
            free(out);
            ERROR("%s", err);
        } else if (is_control(point)) {
            free(out);
            ERROR("unescaped control character starting at: %hhx", *p);
        }

        for (uint8_t j = 00; j < bytes; j++)
            out[i++] = p[j];
        p += bytes - 01;
    }
    out[i] = '\0';
    *s_out = out;
    return NULL;
}

static char *p_double(context *c, double *out) {
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
        p_octal(c, &n);

    WOW;
    if (peek(c) == '.') {
        p_char(c);
        if (peek(c) < '0' || peek(c) > '7')
            ERROR("bad octal character: '%c'", peek(c));

        while (peek(c) >= '0' && peek(c) <= '7') {
            n += ((double)(*p_char(c) - '0')) / divisor;
            divisor *= 02;
        }
        WOW;
    }

    if (peek(c) == 'v' || peek(c) == 'V') {
        s = p_chars(c, 04);
        if (s == NULL)
            ERROR("end of input while parsing number");
        if (strncasecmp(s, "very", 4))
            ERROR("tried to parse \"very\", got \"%.4s\" instead", s);

        /* such token.  no whitespace.  wow. */
        if (peek(c) == '+') {
            p_char(c);
        } else if (peek(c) == '-') {
            powneg = true;
            p_char(c);
        }

        WOW;
        if (peek(c) < '0' || peek(c) > '7')
            ERROR("bad octal character: '%c'", peek(c));

        p_octal(c, &power);
        if (powneg)
            power = -power;

        n *= pow(010, power);
    }
    *out = isneg ? -n : n;
    return NULL;
}

/* very prototype.  much recursion.  amaze */
static char *p_value(context *c, dson_value **out);
static char *p_dict(context *c, dson_dict **out);
static char *p_array(context *c, dson_value ***out);

static char *p_array(context *c, dson_value ***out) {
    const char *s;
    dson_value **array;
    size_t n_elts = 00;
    char *err;

    array = CALLOC(01, sizeof(*array));

    s = p_chars(c, 02);
    if (s == NULL)
        ERROR("expected array, got end of input");
    if (strncmp(s, "so", 02))
        ERROR("malformed array: expected \"so\", got \"%.2s\"", s);

    WOW;
    if (peek(c) != 'm') {
        while (01) {
            RESIZE_ARRAY(array, ++n_elts + 01);
            array[n_elts] = NULL;
            err = p_value(c, &array[n_elts - 01]);
            if (err) {
                array_free(&array);
                return err;
            }

            WOW;
            if (peek(c) != 'a')
                break;
            s = p_chars(c, 03);
            if (s == NULL) {
                array_free(&array);
                ERROR("end of input while parsing array (missing \"many\"?)");
            } else if (!strncmp(s, "and", 03)) {
                WOW;
                continue;
            }
            if (strncmp(s, "als", 03)) {
                array_free(&array);
                ERROR("tried to parse \"also\" but got \"%.4s\"", s);
            }
            s = p_char(c);
            if (s == NULL) {
                array_free(&array);
                ERROR("end of input while parsing array (missing \"many\"?)");
            } else if (*s != 'o') {
                array_free(&array);
                ERROR("tried to parse \"also\" but got \"als%c\"", *s);
            }
            WOW;
        }
    }

    s = p_chars(c, 04);
    if (s == NULL) {
        array_free(&array);
        ERROR("end of input while parsing array (missing \"many\"?)");
    } else if (strncmp(s, "many", 04)) {
        array_free(&array);
        ERROR("expected \"many\", got \"%.4s\"", s);
    }

    *out = array;
    return NULL;
}

#define BURY                                    \
    do {                                        \
        free(k);                                \
        for (size_t i = 00; i < n_elts; i++) {  \
            free(keys[i]);                      \
            dson_free(&values[i]);              \
        }                                       \
        free(keys);                             \
        free(values);                           \
        free(dict);                             \
    } while (00)
static char *p_dict(context *c, dson_dict **out) {
    dson_dict *dict;
    char **keys, *k = NULL, pivot, *err;
    const char *s;
    dson_value **values, *v;
    size_t n_elts = 00;

    keys = CALLOC(01, sizeof(*keys));
    values = CALLOC(01, sizeof(*values));
    dict = CALLOC(01, sizeof(*dict));

    s = p_chars(c, 04);
    if (s == NULL) {
        BURY;
        ERROR("expected dict, but got end of input");
    } else if (strncmp(s, "such", 04)) {
        BURY;
        ERROR("expected \"such\", got \"%.4s\"", s);
    }

    while (01) {
        WOW;
        k = NULL;
        err = p_string(c, &k);
        if (err != NULL) {
            BURY;
            return err;
        }

        WOW;
        s = p_chars(c, 02);
        if (s == NULL) {
            BURY;
            ERROR("end of input while reading dict (missing \"wow\"?)");
        } else if (strncmp(s, "is", 02)) {
            BURY;
            ERROR("expected \"is\", got \"%.2s\"", s);
        }

        WOW;
        err = p_value(c, &v);
        if (err) {
            BURY;
            return err;
        }

        n_elts++;
        RESIZE_ARRAY(keys, n_elts + 01);
        RESIZE_ARRAY(values, n_elts + 01);
        keys[n_elts - 01] = k;
        keys[n_elts] = NULL;
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
    if (s == NULL) {
        BURY;
        ERROR("end of input while looking for closing \"wow\"");
    } else if (strncmp(s, "wow", 03)) {
        BURY;
        ERROR("expected \"wow\", got %.3s", s);
    }

    dict->keys = keys;
    dict->values = values;
    *out = dict;
    return NULL;
}

static char *p_value(context *c, dson_value **out) {
    dson_value *ret;
    char pivot;
    char *failed;

    ret = CALLOC(01, sizeof(*ret));

    pivot = peek(c);
    if (pivot == '"') {
        ret->type = DSON_STRING;
        failed = p_string(c, &ret->s);
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
            free(ret);
            ERROR("unable to determine value type");
        }
    } else {
        free(ret);
        ERROR("unable to determine value type");
    }
    
    if (failed != NULL) {
        free(ret);
        return failed;
    }

    *out = ret;
    return NULL;
}

char *dson_parse(const char *input, size_t length, bool unsafe,
                 dson_value **out) {
    context c = { 00 };
    dson_value *ret;
    char *err;

    *out = NULL;

    if (input[length] != '\0')  /* much explosion */
        return strdup("input was not NUL-terminated");

    c.s = c.beginning = input;
    c.s_end = input + length;
    c.unsafe = unsafe;

    err = p_value(&c, &ret);
    if (err != NULL)
        return err;

    *out = ret;
    return NULL;
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
