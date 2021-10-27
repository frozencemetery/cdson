/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <cdson.h>
#include <stdio.h>

int main() {
    dson_dict *d = NULL;
    dson_value v = { 0 };

    v.type = DSON_DICT;
    v.dict = d;

    printf("Referencing: %p %p %p %p\n", &v, dson_parse, dson_free,
           dson_dict_get);

    return 0;
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
