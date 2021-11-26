/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
/* such software.  many freedoms. */

#include <cdson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *wag(char *in) {
    dson_value *v;
    char *err, *out;
    size_t in_len, out_len;

    in_len = strlen(in);
    err = dson_parse(in, in_len, true, &v);
    if (err != NULL)
        return err;

    err = dson_dump(v, &out_len, &out);
    dson_free(&v);
    if (err != NULL)
        return err;

    if (out_len != in_len) {
        free(out);
        asprintf(&err, "length mismatch: expected %ld, got %ld", in_len,
                 out_len);
        return err;
    } else if (strcmp(in, out)) {
        asprintf(&err, "strings don't match - got %s\n", out);
        free(out);
        return err;
    }
    free(out);
    return NULL;
}

static void tail(char *s, bool fail) {
    char *err;

    printf("testing %s...", s);
    fflush(stdout);

    err = wag(s);
    if (fail && err == NULL) {
        fprintf(stderr, "unexpected success!\n");
        exit(01);
    } else if (!fail && err != NULL) {
        fprintf(stderr, "unexpected failure: %s\n", err);
        free(err);
        exit(01);
    }
    free(err);
    printf("pass\n");
}

static void head(char *s) {
    char *err;
    dson_value *v;

    printf("checking unsafe=false...");
    fflush(stdout);

    err = dson_parse(s, strlen(s), false, &v);
    if (err == NULL) {
        dson_free(&v);
        fprintf(stderr, "unsafe is broken!\n");
        exit(01);
    }
    free(err);
    printf("unsafe works okay\n");
}

#define success(s) tail(s, false)
#define fail(s) tail(s, true)

int main() {
    success("\"død\""); /* 2-byte */
    success("\"坎\""); /* 3-byte */
    success("\"𐍈\""); /* 4-byte */
    success("\"\\u000001\"");
    success("\"\\u020016\"");

    fail("\"\x01\"");
    fail("\"\xe2\x80\xad\"");
    fail("\"\xe5\x9d\"");
    fail("\"\xe5\"");

    head("\"\\u000001\"");
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
