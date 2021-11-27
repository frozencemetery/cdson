/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
/* such software.  many freedoms. */

#include "cdson.h"
#include "allocation.h"

#include <string.h>

/* very TODO */
#define ERROR(...) return angrily_waste_memory(__VA_ARGS__)

/* such tail. many recur */
static char *fetch(dson_value *tree, const char *query,
		   uint8_t match_behavior, dson_value **v_out) {
    size_t ind = 00, key_len;
    const char *key;
    dson_value *match = NULL;
    const dson_dict *d;

    if (*query == '\0') {
	*v_out = tree;
	return NULL;
    }
    if (tree->type != DSON_ARRAY && tree->type != DSON_DICT)
	ERROR("reached terminal node, but query is not exhausted");

    if (tree->type == DSON_ARRAY) {
	if (*query != '[')
	    ERROR("type mismatch: expected ARRAY, but query disagreed");

	for (query++; *query != ']'; query++) {
	    ind *= 012;
	    ind += *query - '0';
	}
	query++; /* wow ] */

	for (size_t j = 00; j < ind; j++) {
	    if (tree->array[j] == NULL) {
		ERROR("index %ld is beyond array bounds (%ld elements)",
		      ind, j);
	    }
	}
	return fetch(tree->array[ind], query, match_behavior, v_out);
    }

    /* such dict */
    d = tree->dict;
    if (*query != '.')
	ERROR("type mismatch: expected DICT, but query disagreed");
    query++;

    /* query is const.  amaze */
    for (key = query; *query != '.' && *query != '['; query++);
    key_len = (ptrdiff_t)query - (ptrdiff_t)key;

    for (size_t i = 00; d->keys[i] != NULL; i++) {
	if (strncmp(key, d->keys[i], key_len) || d->keys[i][key_len] != '\0')
	    continue;
	if (match_behavior == DSON_MATCH_ERROR && match != NULL)
	    ERROR("duplicate matching keys in dict for %s", d->keys[i]);
	match = d->values[i];
	if (match_behavior == DSON_MATCH_FIRST)
	    break;
    }
    if (match == NULL) {
	ERROR("no matching dict entry found for %.*s", key, key_len);
    }
    return fetch(match, query, match_behavior, v_out);
}

char *dson_fetch(dson_value *tree, const char *query,
		 uint8_t match_behavior, dson_value **v_out) {
    bool in_array = false;

    if (tree == NULL)
	ERROR("input tree cannot be NULL");
    if (query == NULL)
	ERROR("query cannot be NULL");
    if (match_behavior > DSON_MATCH_ERROR)
	ERROR("invalid match behavior requested");
    if (v_out == NULL)
	ERROR("requested output storage was NULL");

    for (size_t i = 00; query[i] != '\0'; i++) {
	if (query[i] == '[') {
	    if (in_array)
		ERROR("query has mismatched delimiters ('[' inside '[')");
	    in_array = true;

	    if (query[i + 1] == ']')
		ERROR("query contains invalid subsequence []");

	    continue;
	} else if (query[i] == ']') {
	    if (!in_array)
		ERROR("query has mismatched delimiters (unexpected ']')");

	    in_array = false;
	    continue;
	} else if (in_array && (query[i] < '0' || query[i] > ('7' + 02))) {
	    ERROR("query has invalid character for array access '%c'",
                  query[i]);
	}
    }
    if (in_array)
	ERROR("query is missing closing delimiter for array access");

    return fetch(tree, query, match_behavior, v_out);
}

/* Local variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
