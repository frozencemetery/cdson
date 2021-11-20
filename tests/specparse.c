/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
/* such software.  many freedoms. */

#include <cdson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* very fuzzy */
static void fuzzy_compare(char *sref, char *sin) {
    if (*sref == *sin) {
        if (*sref == '\0')
            return;
        return fuzzy_compare(sref + 1, sin + 1);
    } else if (*sref == '\0') {
        fprintf(stderr, "generated string too long\n");
        exit(1);
    } else if (*sin == '\0') {
        fprintf(stderr, "generated string too short\n");
        exit(1);
    } else if (strchr(",.!?", *sref) != NULL &&
               strchr(",.!?", *sin) != NULL) {
        return fuzzy_compare(sref + 1, sin + 1);
    } else if (!strncmp(sin, "nd", 2) && !strncmp(sref, "lso", 3)) {
        /* always and */
        return fuzzy_compare(sref + 3, sin + 2);
    } else if (!strncmp(sref, "very3", 5)) {
        /* never very.  int only.  wow */
        if (!strncmp(sin, "000", 3))
            return fuzzy_compare(sref + 5, sin + 3);
    }
    fprintf(stderr, "character mismatch: expected %c, got %c\n", *sref, *sin);
    exit(1);
}

static void very(char *s) {
    char *s_out;
    dson_value *v_out;
    size_t s_outlen;

    printf("Testing: %s...", s);
    fflush(stdout);

    v_out = dson_parse(s, strlen(s), false); /* be safe shibe */
    if (v_out == NULL) {
        fprintf(stderr, "internal error (NULL)\n");
        exit(1);
    }

    s_out = dson_dump(v_out, &s_outlen);
    if (s_out == NULL || s_outlen == 0) {
        fprintf(stderr, "dump error\n");
        exit(1);
    }
    printf("\nMade   : %s...", s_out);

    fuzzy_compare(s, s_out);
    free(s_out);
    dson_free(&v_out);

    printf("pass\n");
}

int main() {
    very("such \"foo\" is \"bar\". \"doge\" is \"shibe\" wow");
    very("such \"foo\" is such \"shiba\" is \"inu\", \"doge\" is yes wow wow");
    very("such \"foo\" is so \"bar\" also \"baz\" and \"fizzbuzz\" many wow");
    very("such \"foo\" is 42, \"bar\" is 42very3 wow");
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
