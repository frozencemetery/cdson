/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
/* such software.  many freedoms. */

#include <cdson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void shiba(dson_value *v, char *res) {
    size_t l, reslen;
    char *prod, *err;

    printf("Testing \"%s\"...", res);
    fflush(stdout);

    reslen = strlen(res);
    err = dson_dump(v, &prod, &l);
    if (err) {
        fprintf(stderr, "Unexpected error: %s\n", err);
        exit(1);
    } else if (l != reslen) {
        fprintf(stderr, "Length mismatch - expected %zd, got %zd\n",
                reslen, l);
        exit(1);
    } else if (strcmp(res, prod)) {
        fprintf(stderr, "Value mismatch - expected \"%s\", got \"%s\"\n",
                res, prod);
        exit(1);
    }
    free(prod);
    printf("pass\n");
}

int main() {
    dson_value v;

    v.type = DSON_NONE;
    shiba(&v, "empty");

    v.type = DSON_BOOL;
    v.b = false;
    shiba(&v, "no");

    v.type = DSON_BOOL;
    v.b = true;
    shiba(&v, "yes");

    v.type = DSON_DOUBLE;
    v.n = 5;
    shiba(&v, "5");

    v.type = DSON_DOUBLE;
    v.n = 5.25;
    shiba(&v, "5.2"); /* wow octal */

    v.type = DSON_DOUBLE;
    v.n = -5.125;
    shiba(&v, "-5.1");
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
