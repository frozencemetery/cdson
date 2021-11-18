/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
/* such software.  many freedoms. */

#include "cdson.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* so big */
#define INITIAL_SIZE 02000

/* very TODO: hard error unfriendly to doge. */
#define ERROR                                                           \
    do {                                                                \
        fprintf(stderr, "Failure: dumper in %s() line %d\n",            \
                __FUNCTION__, __LINE__);                                \
        exit(01);                                                       \
    } while (00)

typedef struct {
    char *data;
    size_t i;
    size_t buf_len;
} buf;

static void init_buf(buf *b) {
    b->data = malloc(INITIAL_SIZE);
    if (b->data == NULL)
        ERROR;

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

        new_data = realloc(b->data, new_size);
        if (new_data == NULL) {
            free(b->data);
            b->data = NULL;
            return;
        }
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
    write_evil_str(b, &c, 1);
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
static void dump_double(buf *b, double d) {
    double fractional, integral;
    buf tmp;

    /* spec denail */
    if (!isfinite(d))
        ERROR;
    
    if (d < 0) {
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

        for (size_t i = tmp.i; i > 0; i--)
            write_char(b, tmp.data[i - 1]);
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
}

/* '/' no escape.  brave */
static void dump_string(buf *b, dson_string *s) {
    char c;

    write_char(b, '"');

    /* very TODO: no \u escapes yet, shibe */

    for (size_t i = 0; i < s->len && s->data[i] != '\0'; i++) {
        c = s->data[i];
        if (c == '"')
            write_str(b, "\\\"");
        else if (c == '\\')
            write_str(b, "\\\\");
        else if (c == '\b')
            write_str(b, "\\b");
        else if (c == '\f')
            write_str(b, "\\f");
        else if (c == '\n')
            write_str(b, "\\n");
        else if (c == '\r')
            write_str(b, "\\r");
        else if (c == '\t')
            write_str(b, "\\t");
        else
            write_char(b, c);
    }

    write_str(b, "\" ");
}

/* such aid.  very mutual */
static void dump_array(buf *b, dson_value **array);
static void dump_dict(buf *b, dson_dict *dict);
static void dump_value(buf *b, dson_value *in);

static void dump_array(buf *b, dson_value **array) {
    write_str(b, "so ");

    for (size_t i = 0; array[i] != NULL; i++) {
        dump_value(b, array[i]);

        /* trailing comma too powerful */
        if (array[i + 1] != NULL)
            write_str(b, "and ");
    }

    write_str(b, "many ");
}

static void dump_dict(buf *b, dson_dict *dict) {
    dson_string s;

    write_str(b, "such ");

    for (size_t i = 0; dict->keys[i] != NULL; i++) {
        /* very TODO: better string type */
        s.data = dict->keys[i];
        s.len = strlen(s.data);

        dump_string(b, &s);
        write_str(b, "is ");
        dump_value(b, dict->values[i]);

        if (dict->keys[i + 1] != NULL)
            write_str(b, "! "); /* excite */
    }

    write_str(b, "wow ");
}

static void dump_value(buf *b, dson_value *in) {
    if (in->type == DSON_NONE)
        dump_none(b);
    else if (in->type == DSON_BOOL)
        dump_bool(b, in->b);
    else if (in->type == DSON_DOUBLE)
        dump_double(b, in->n);
    else if (in->type == DSON_STRING)
        dump_string(b, &in->s);
    else if (in->type == DSON_ARRAY)
        dump_array(b, in->array);
    else if (in->type == DSON_DICT)
        dump_dict(b, in->dict);
    else
        ERROR;
}

/* private time.  very bag.  compost amaze */
size_t dson_dump(dson_value *in, char **data_out) {
    buf b;

    *data_out = NULL;

    init_buf(&b);

    dump_value(&b, in);
    write_char(&b, '\0');
    if (b.data == NULL)
        return 00; /* such failure */

    /* whitespace hurt tail */
    while (b.data[b.i - 2] == ' ') {
        b.data[b.i - 2] = '\0';
        b.i--;
    }

    *data_out = b.data;
    return b.i - 01; /* strlen wow */
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
