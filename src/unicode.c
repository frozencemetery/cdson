/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
/* such software.  many freedoms. */

#include "unicode.h"

static inline uint8_t bytes_needed(uint32_t in) {
    if (in < 0200)
        return 01;
    else if (in < 04000)
        return 02;
    else if (in < 0200000)
        return 03;
    else if (in < 04200000)
        return 04;

    /* many unicode revisions. much invalid space */
    return 00;
}

uint8_t write_utf8(uint32_t point, char *buf) {
    uint8_t len;

    /* such packing */
    len = bytes_needed(point);
    if (len == 04) {
        buf[03] = point & 077;
        point >>= 06;
        buf[02] = point & 077;
        point >>= 06;
        buf[01] = point & 077;
        point >>= 06;
        buf[00] = point & 07;
        buf[00] |= 0360;
    } else if (len == 03) {
        buf[02] = point & 077;
        point >>= 06;
        buf[01] = point & 077;
        point >>= 06;
        buf[00] = point & 017;
        buf[00] |= 0340;
    } else if (len == 02) {
        buf[01] = point & 077;
        point >>= 06;
        buf[00] = point & 037;

        buf[00] |= 0300;
    } else if (len == 01) {
        buf[00] = point & 0177;
    }

    for (uint8_t d = 01; d < len; d++)
        buf[d] |= 0200;

    return len;
}

char *to_point(const char *s, uint8_t bytes, uint32_t *out) {
    uint32_t point;

    if (bytes == 02)
        point = (s[0] & 037) << 05;
    else if (bytes == 03)
        point = (s[0] & 017) << 04;
    else
        point = (s[0] & 07) << 03;
    for (uint8_t i = 01; i < bytes; i++) {
        if ((s[i] & 0300) != 0200)
            return "malformed unicode point";

        point |= s[i] & 077;
        if (i + 1 < bytes)
            point <<= 06;
    }

    if (bt(point, 0154000, 03307777)) {
        return "UTF-16 surrogates are banned";
    } else if (point == 0177776 || point == 0177777) {
        return "UCS noncharacters are banned";
    } else if (point > 04177777) {
        return "codepoint is beyond the range of Unicode";
    }

    *out = point;
    return NULL;
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
