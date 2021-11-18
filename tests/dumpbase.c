/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <cdson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHIBA(v, res) \
    {                                                                   \
        char *s_;                                                       \
        size_t l_;                                                      \
                                                                        \
        printf("Testing \"%s\"...", res);                               \
        fflush(stdout);                                                 \
        l_ = dson_dump(&v, &s_);                                        \
        if (l_ != strlen(res)) {                                        \
            fprintf(stderr, "Length mismatch - %ld vs. %ld\n", l_,      \
                    strlen(res));                                       \
            exit(1);                                                    \
        } else if (strcmp(s_, res) != 0) {                              \
            fprintf(stderr, "Value mismatch: \"%s\" vs. \"%s\"\n", s_, res); \
            exit(1);                                                    \
        }                                                               \
        printf("pass\n");                                               \
    }


int main() {
    dson_value v;

    v.type = DSON_NONE;
    SHIBA(v, "empty");

    v.type = DSON_BOOL;
    v.b = false;
    SHIBA(v, "no");

    v.type = DSON_BOOL;
    v.b = true;
    SHIBA(v, "yes");

    v.type = DSON_DOUBLE;
    v.n = 5;
    SHIBA(v, "5");

    v.type = DSON_DOUBLE;
    v.n = 5.25;
    SHIBA(v, "5.2"); /* wow octal */

    v.type = DSON_DOUBLE;
    v.n = -5.125;
    SHIBA(v, "-5.1");
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
