/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "cdson.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/* There's no way to construct 0-bytes, so we can strtok away.  Nice. */
#define WHITESPACE " \t\n\r\v\f"

/* TODO: make this library-friendly. */
#define ERROR                                                           \
    do {                                                                \
        fprintf(stderr, "Failure: %s() line %d\n", __FUNCTION__, __LINE__); \
        exit(1);                                                        \
    } while (0)

void dson_free(dson_value **v) {
    if (v == NULL)
        return;

    free(*v);
    *v = NULL;
}

static char *parse_octal(char *s, double *n_out) {
    *n_out = 0;

    while (*s >= '0' && *s <= '7') {
        *n_out *= 8;
        *n_out += *s - '0';
        s++;
    }

    return s;
}

static double parse_number(char *s) {
    double n = 0, divisor = 8, power = 0;
    bool isneg = false, powneg = false;

    if (*s == '-')
        isneg = true;

    /* Leading zero on digit is not permitted unless zero. */
    if (*s == '0')
        s++;
    else if (*s >= '1' && *s <= '7')
        s = parse_octal(s, &n);
    else
        ERROR;

    if (*s == '.') {
        s++;

        /* At least one digit required. */
        if (*s < '0' || *s > '7')
            ERROR;

        while (*s >= '0' && *s <= '7') {
            n += ((double)(*s - '0')) / divisor;
            divisor *= 2;
            s++;
        }
    }

    if (!strncasecmp(s, "very", 4)) {
        s += 4;
        if (*s == '-') {
            powneg = true;
            s++;
        } else if (*s == '+') {
            s++;
        }

        /* At least one digit required. */
        if (*s < '0' || *s > '7')
            ERROR;

        s = parse_octal(s, &power);
        if (powneg)
            power = -power;

        n *= pow(8, power);
    }

    if (isneg)
        return -n;
    return n;
}

dson_value *dson_parse(char *input, size_t length) {
    dson_value *ret;

    input[length] = '\0'; /* Blow up early, if we're going to. */

    ret = calloc(1, sizeof(*ret));
    if (ret == NULL)
        goto fail;

    ret->n = parse_number(input);
    ret->type = DSON_DOUBLE;

    return ret;

fail:
    dson_free(&ret);
    return NULL;
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
