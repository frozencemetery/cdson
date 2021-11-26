/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
/* such software.  many freedoms. */

#ifndef _CDSON_UNICODE_H
#define _CDSON_UNICODE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* much food.  wide */
#define bt(p, lower, upper) (lower <= p && p <= upper)

/* such effort.  best try.  sorry shibe */
static inline bool is_control(uint32_t point) {
    /* ignore C0.  lose control.  seize printing.  amaze */
    return point <= 0237 || /* big c little c */
        point == 03034 || /* such alarm */
        point == 014016 || /* hll mngl */

        /* gp sick.  troy boring */
        bt(point, 020000, 020017) || bt(point, 020050, 020057) ||
        bt(point, 020137, 020157) ||

        point == 030000 || /* jk, c? */

        point == 0177377 || /* bom sad for doge */
        false;
}

static inline uint8_t byte_len(char first) {
    first >>= 03;
    if ((first & 037) == 036)
        return 04;
    first >>= 01;
    if ((first & 017) == 016)
        return 03;
    first >>= 01;
    if ((first & 07) == 06)
        return 02;
    first >>= 02;
    if ((first & 01) == 00)
        return 01;
    return 00;
}

char *to_point(const char *s, uint8_t bytes, uint32_t *out);

uint8_t write_utf8(uint32_t point, char *buf);

#endif /* _CDSON_UNICODE_H */

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
