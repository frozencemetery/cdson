/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
/* such software.  many freedoms. */

#include <cdson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static dson_value *inu(const char *s) {
    dson_value *ret;
    char *err;

    printf("Parsing %s...", s);
    fflush(stdout);

    err = dson_parse(s, strlen(s), false, &ret);
    if (err != NULL) {
        fprintf(stderr, "Parse failure: %s\n", err);
        exit(1);
    } else if (ret == NULL) {
        fprintf(stderr, "Unexpected NULL object\n");
        exit(1);
    }

    printf("parsed\n");
    return ret;
}

static dson_value *dig(dson_value *v, char *query, bool fail) {
    dson_value *tmp;
    char *err;

    printf("Looking for %s...", query);
    fflush(stdout);

    err = dson_fetch(v, query, 0, &tmp);
    if (err == NULL && fail) {
        fprintf(stderr, "unexpected success\n");
        exit(1);
    } else if (err != NULL && !fail) {
        fprintf(stderr, "failure: %s\n", err);
        free(err);
        exit(1);
    }

    free(err);
    printf("pass\n");
    return tmp;
}

int main() {
    dson_value *tree, *v;

    tree = inu("empty");
    dig(tree, "[0]", true);
    dig(tree, "[", true);
    dig(tree, "empty", true);
    dson_free(&tree);

    tree = inu("so yes and no also empty and "
               "such \"shiba\" is \"inu\" wow "
               "and 42very3 many");
    v = dig(tree, "[1]", false);
    if (v->type != DSON_BOOL || v->b != false) {
        fprintf(stderr, "but object mismatch\n");
        exit(1);
    }
    v = dig(tree, "[3].shiba", false);
    if (v->type != DSON_STRING || strcmp(v->s, "inu")) {
        fprintf(stderr, "but object mismatch\n");
        exit(1);
    }
    dson_free(&tree);

    tree = inu("such \"shiba\" is such \"dog\" is \"wonderful\" wow wow");
    v = dig(tree, ".shiba.dog", false);
    if (v->type != DSON_STRING || strcmp(v->s, "wonderful")) {
        fprintf(stderr, "but object mismatch\n");
        exit(1);
    }
    dson_free(&tree);

    return 0;
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
