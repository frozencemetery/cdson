/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
/* such software.  many freedoms. */

#include <cdson.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void wag(char *s) {
    char *err;
    dson_value *v;

    printf("Testing \"%s\"...", s);
    fflush(stdout);

    err = dson_parse(s, strlen(s), false, &v);
    if (err == NULL) {
        printf("unexpected success!\n");
        exit(1);
    }
    printf("expected failure: %s\n", err);
    free(err);
}

static void bark(dson_value *v) {
    char *err, *s;
    size_t s_len;

    printf("barking...");
    fflush(stdout);
    err = dson_dump(v, &s_len, &s);
    if (err == NULL) {
        printf("unexpected success\n");
        exit(1);
    }
    printf("expected failure: %s\n", err);
    free(err);
}

int main() {
    dson_value v;

    wag("sdf");
    wag("such \"foo\"");
    wag("42ver");
    wag("yea");

    v.type = DSON_DOUBLE;
    v.n = NAN;
    bark(&v);
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
