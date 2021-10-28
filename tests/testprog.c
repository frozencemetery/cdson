/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <cdson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void values_match(dson_value *d1, dson_value *d2) {
    size_t i;

    if (d1 == NULL && d2 == NULL) {
        return;
    } else if (d1 == NULL || d2 == NULL) {
        fprintf(stderr, "unexpected NULL object\n");
        exit(1);
    } else if (d1->type != d2->type) {
        fprintf(stderr, "type mismatch: %d vs. %d\n", d1->type, d2->type);
        exit(1);
    } else if (d1->type == DSON_NONE) {
        return;
    } else if (d1->type == DSON_BOOL && d1->b != d2->b) {
        fprintf(stderr, "bool mismatch: %d vs. %d\n", d1->b, d2->b);
        exit(1);
    } else if (d1->type == DSON_DOUBLE && d1->n != d2->n) {
        fprintf(stderr, "double mismatch: %g vs. %g\n", d1->n, d2->n);
        exit(1);
    } else if (d1->type == DSON_STRING) {
        if (d1->s_len != d2->s_len) {
            fprintf(stderr, "string length mismatch: %ld vs. %ld\n",
                    d1->s_len, d2->s_len);
            exit(1);
        } else if (memcmp(d1->s, d2->s, d1->s_len)) {
            fprintf(stderr, "strings differ: \"%s\" vs. \"%s\" (len %ld)\n",
                    d1->s, d2->s, d1->s_len);
            exit(1);
        }
    } else if (d1->type == DSON_ARRAY) {
        for (i = 0; d1->array[i] != NULL && d2->array[i] != NULL; i++)
            values_match(d1->array[i], d2->array[i]);
        if (d1->array[i] != NULL || d2->array[i] != NULL) {
            fprintf(stderr, "array length mismatch\n");
            exit(1);
        }
    } else if (d1->type == DSON_DICT) {
        /* haha, oops... uh... TODO */
    }

    dson_free(&d2);
}

#define SHIBA(v, s) values_match(&v, dson_parse(s, strlen(s)))

int main() {
    dson_value v;

    v.type = DSON_NONE;
    SHIBA(v, "empty");

    v.type = DSON_BOOL;
    v.b = false;
    SHIBA(v, "no");

    v.b = true;
    SHIBA(v, "yes");

    v.type = DSON_DOUBLE;
    v.n = 3;
    SHIBA(v, "3");

    v.n = 34;
    SHIBA(v, "42");

    v.n = 17408;
    SHIBA(v, "42very3");
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
