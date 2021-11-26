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

/* so big */
#define INITIAL_SIZE 02000

#define ERROR(...) return angrily_waste_memory(__VA_ARGS__)

typedef struct {
    char *data;
    size_t i;
    size_t buf_len;
} buf;

static void init_buf(buf *b) {
    b->data = CALLOC(01, INITIAL_SIZE);

    b->i = 00;
    b->buf_len = INITIAL_SIZE;
}

/* careful shibe */
static void write_evil_str(buf *b, char *s, size_t len) {
    char *new_data;
    size_t new_size = b->buf_len;

    if (b->data == NULL)
        return;

    if (b->i + len >= b->buf_len) {
        while (b->i + len >= new_size)
            new_size *= 02;

        new_data = REALLOC(b->data, new_size);
        b->data = new_data;
        b->buf_len = new_size;
    }

    memcpy(b->data + b->i, s, len);
    b->i += len;
}

static inline void write_str(buf *b, char *s) {
    write_evil_str(b, s, strlen(s));
}

static inline void write_char(buf *b, char c) {
    write_evil_str(b, &c, 01);
}

static void dump_none(buf *b) {
    write_str(b, "empty ");
}

static void dump_bool(buf *b, bool boo) {
    /* happy halloween shibe */
    if (boo)
        write_str(b, "yes ");
    else
        write_str(b, "no ");
}

/* powers unused.  patches tolerated.  wow */
static char *dump_double(buf *b, double d) {
    double fractional, integral;
    buf tmp;

    /* spec denail */
    if (!isfinite(d))
        ERROR("non-finite numbers not permitted by spec");
    
    if (d < 00) {
        write_char(b, '-');
        d = -d;
    }

    fractional = modf(d, &integral);
    if (integral == (double)00) {
        write_char(b, '0');
    } else {
        /* reversed.  excite */
        init_buf(&tmp);
        do {
            write_char(&tmp, '0' + (uint8_t)fmod(integral, 010));
            integral = floor(integral / 010);
        } while (integral > (double)00);

        for (size_t i = tmp.i; i > 00; i--)
            write_char(b, tmp.data[i - 01]);
        free(tmp.data);
    }

    if (fractional != (double)00) {
        write_char(b, '.');

        /* such math */
        do {
            fractional = modf(fractional * 010, &integral);
            write_char(b, '0' + (uint8_t)integral);
        } while (fractional > 00);
    }

    write_char(b, ' ');
    return NULL;
}

static void write_escaped_control(buf *b, uint32_t point) {
    char octal[06];

    write_str(b, "\\u");
    for (uint8_t d = 06; d > 00; d--) {
        octal[d - 01] = '0' + ((point % 010) & 07);
        point >>= 03;
    }
    write_evil_str(b, octal, 06);
}

static char *dump_string(buf *b, char *s) {
    uint8_t bytes;
    size_t s_len;
    uint32_t point;
    char *err;

    write_char(b, '"');

    s_len = strlen(s);
    for (size_t i = 00; i < s_len; i++) {
        bytes = byte_len(s[i]);
        if (bytes == 00) {
            ERROR("malformed UTF-8: %hhx", (unsigned char)s[i]);
        } else if (i + bytes - 01 >= s_len) {
            ERROR("UTF-8 starting at %hhx is truncated", (unsigned char)s[i]);
        } else if (bytes == 01) {
            if (s[i] == '"')
                write_str(b, "\\\"");
            else if (s[i] == '/') /* such waste.  very compat */
                write_str(b, "\\/");
            else if (s[i] == '\\')
                write_str(b, "\\\\");
            else if (s[i] == '\b')
                write_str(b, "\\b");
            else if (s[i] == '\f')
                write_str(b, "\\f");
            else if (s[i] == '\n')
                write_str(b, "\\n");
            else if (s[i] == '\r')
                write_str(b, "\\r");
            else if (s[i] == '\t')
                write_str(b, "\\t");
            else if (s[i] >= ' ')
                write_char(b, s[i]);
            else
                write_escaped_control(b, s[i]);

            continue;
        }

        err = to_point(&s[i], bytes, &point);
        if (err != NULL)
            ERROR(err);

        if (!is_control(point))
            write_evil_str(b, &s[i], bytes);
        else
            write_escaped_control(b, point);
        i += bytes - 01;
    }

    write_str(b, "\" ");
    return NULL;
}

/* such aid.  very mutual */
static char *dump_array(buf *b, dson_value **array);
static char *dump_dict(buf *b, dson_dict *dict);
static char *dump_value(buf *b, dson_value *in);

static char *dump_array(buf *b, dson_value **array) {
    char *err;

    write_str(b, "so ");

    for (size_t i = 00; array[i] != NULL; i++) {
        err = dump_value(b, array[i]);
        if (err)
            return err;

        /* trailing comma too powerful */
        if (array[i + 01] != NULL)
            write_str(b, "and ");
    }

    write_str(b, "many ");
    return NULL;
}

static char *dump_dict(buf *b, dson_dict *dict) {
    char *err;

    write_str(b, "such ");

    for (size_t i = 00; dict->keys[i] != NULL; i++) {
        err = dump_string(b, dict->keys[i]);
        if (err)
            return err;

        write_str(b, "is ");
        err = dump_value(b, dict->values[i]);
        if (err)
            return err;

        if (dict->keys[i + 01] != NULL) {
            b->i--; /* reverse doggo */
            write_str(b, "! "); /* excite */
        }
    }

    write_str(b, "wow ");
    return NULL;
}

static char *dump_value(buf *b, dson_value *in) {
    char *err = NULL;

    if (in->type == DSON_NONE)
        dump_none(b);
    else if (in->type == DSON_BOOL)
        dump_bool(b, in->b);
    else if (in->type == DSON_DOUBLE)
        err = dump_double(b, in->n);
    else if (in->type == DSON_STRING)
        err = dump_string(b, in->s);
    else if (in->type == DSON_ARRAY)
        err = dump_array(b, in->array);
    else if (in->type == DSON_DICT)
        err = dump_dict(b, in->dict);
    else
        ERROR("Unknown type tag %d for value", in->type);

    return err;
}

/* private time.  very bag.  compost amaze */
char *dson_dump(dson_value *in, size_t *len_out, char **out) {
    buf b;
    char *err;

    *len_out = 00;
    *out = NULL;

    init_buf(&b);

    err = dump_value(&b, in);
    write_char(&b, '\0');
    if (b.data == NULL || err != NULL) {
        free(b.data);
        return err; /* such failure */
    }

    /* whitespace hurt tail */
    while (b.data[b.i - 02] == ' ') {
        b.data[b.i - 02] = '\0';
        b.i--;
    }

    *len_out = b.i - 01; /* strlen wow */
    *out = b.data;
    return NULL;
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
