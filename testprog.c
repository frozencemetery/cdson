/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "cdson.c"

static inline void check_eq(double left, double right) {
    if (left == right)
        return;

    fprintf(stderr, "Failed: expected %f, but got %f\n", right, left);
    exit(1);
}

int main() {
    check_eq(parse_number("3"), 3);
    check_eq(parse_number("42"), 34);
    check_eq(parse_number("42very3"), 17408);
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
