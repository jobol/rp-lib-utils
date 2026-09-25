/*
 * Copyright (C) 2015-2026 IoT.bzh Company
 * Author: José Bollo <jose.bollo@iot.bzh>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#ifdef	__cplusplus
extern "C" {
#endif

#include <json-c/json.h>

/**
 * The expansion path is used to retrieve information on
 * the path of an object to "expand"
 */
typedef struct rp_jsonc_expand_path *rp_jsonc_expand_path_t;

/**
 * Callback function called during expansion of json.
 * These callbacks receive a json object that it can
 * process. In result, it returns either the same object
 * or its replacement.
 *
 * When the replacement is returned, there is no need
 * to call json_object_put on the given object.
 *
 * The other parameters it receives are a closure and
 * an object representing the path of the object to process.
 *
 * @param closure the closure given to @see rp_jsonc_expand
 * @param object the json object to process
 * @param path an object for querying data on path of object
 *
 * @return the object or its replacement
 */
typedef struct json_object *(*rp_jsonc_expandcb)(void *closure, struct json_object* object, rp_jsonc_expand_path_t path);

/**
 * Expand the given object using the given expansion functions
 *
 * @param object  the object to expand
 * @param closure the closure for the expansion functions
 * @param expand_object the expansion function called on object of type object (dictionaries)
 * @param expand_string the expansion function called on object of type string
 *
 * @return the result of the expansion that can be equal to object
 */
extern struct json_object *rp_jsonc_expand(
	struct json_object *object,
	void *closure,
	rp_jsonc_expandcb expand_object,
	rp_jsonc_expandcb expand_string
);

/**
 * Returns the length of the path
 *
 * @param path the queried path
 *
 * @return the length of the path
 */
extern int rp_jsonc_expand_path_length(rp_jsonc_expand_path_t path);

/**
 * Returns the object at given index in the path.
 * Valid index are from 0 for the root to LENGTH-1
 *
 * @param path the queried path
 * @param index the index in the path
 *
 * @return the object at index or NULL if index is invalid
 */
extern struct json_object *rp_jsonc_expand_path_get(rp_jsonc_expand_path_t path, int index);

/**
 * Returns if the object at given index is an object.
 * Valid index are from 0 for the root to LENGTH-1
 *
 * @param path the queried path
 * @param index the index in the path
 *
 * @return true if index is valid and object at index is an object
 */
extern int rp_jsonc_expand_path_is_object(rp_jsonc_expand_path_t path, int index);

/**
 * Returns if the object at given index is an array.
 * Valid index are from 0 for the root to LENGTH-1
 *
 * @param path the queried path
 * @param index the index in the path
 *
 * @return true if index is valid and object at index is an array
 */
extern int rp_jsonc_expand_path_is_array(rp_jsonc_expand_path_t path, int index);

/**
 * Returns the key inspected for the object at index
 * Valid index are from 0 for the root to LENGTH-1
 *
 * @param path the queried path
 * @param index the index in the path
 *
 * @return the key if index is valid and object at index is an object,
 *         or otherwise NULL
 */
extern const char *rp_jsonc_expand_path_key(rp_jsonc_expand_path_t path, int index);

/**
 * Returns the index inspected for the array at index
 * Valid index are from 0 for the root to LENGTH-1
 *
 * @param path the queried path
 * @param index the index in the path
 *
 * @return the index if index is valid and object at index is an array,
 *         or otherwise zero
 */
extern size_t rp_jsonc_expand_path_index(rp_jsonc_expand_path_t path, int index);

#ifdef	__cplusplus
}
#endif
