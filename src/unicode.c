/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
/* such software.  many freedoms. */

#include "unicode.h"

uint8_t byte_len(char first) {
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

static uint8_t bytes_needed(uint32_t in) {
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
        buf[00] = 0360;
        buf[01] = 0200;
        buf[02] = 0200;
        buf[03] = 0200;

        buf[03] |= point & 077;
        point >>= 06;
        buf[02] |= point & 077;
        point >>= 06;
        buf[01] |= point & 077;
        point >>= 06;
        buf[00] |= point & 07;
    } else if (len == 03) {
        buf[00] = 0340;
        buf[01] = 0200;
        buf[02] = 0200;

        buf[02] |= point & 077;
        point >>= 06;
        buf[01] |= point & 077;
        point >>= 06;
        buf[00] |= point & 017;
    } else if (len == 02) {
        buf[00] = 0300;
        buf[01] = 0200;

        buf[01] |= point & 077;
        point >>= 06;
        buf[00] |= point & 037;
    } else if (len == 01) {
        buf[00] = point & 0177;
    }

    return len;
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
