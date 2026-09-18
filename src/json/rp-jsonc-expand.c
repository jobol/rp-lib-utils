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

#include "rp-jsonc-expand.h"

/**
 * Structure recording the path of the expansion
 */
struct rp_jsonc_expand_path
{
	/** depth */
	int depth;

	/** previous, aka parent, path */
	rp_jsonc_expand_path_t previous;

	/** object in expansion at current depth */
	struct json_object *object;

	/** index of expanded child if object is an array */
	size_t index;

	/** key of expanded child if object is an object */
	const char *key;

};

/**
 * Structure recording callbacks
 */
struct cbs
{
	/** closure of the callbacks */
	void *closure;

	/** callback processing objects */
	rp_jsonc_expandcb expand_object;

	/** callback processing strings */
	rp_jsonc_expandcb expand_string;
};

/**
 * Get the path of index or NULL if index is invalid
 *
 * @param path the initial path (the deepest)
 * @param index   the required index
 *
 * @return the path of the index or NULL if the index is invalid
 */
static inline
rp_jsonc_expand_path_t
at(rp_jsonc_expand_path_t path, int index)
{
	if (index < 0 || path->depth < index)
		return NULL;
	while (index != path->depth)
		path = path->previous;
	return path;
}

/**
 * Internal function for expanding json objects
 *
 * @param object   the object to be expanded
 * @param cbs      structure holding callbacks
 * @param previous link to the parent object
 *
 * @return either the given object or its replacement
 */
static
struct json_object *
expand(
	struct json_object *object,
	const struct cbs *cbs,
	rp_jsonc_expand_path_t previous
) {
#if JSON_C_VERSION_NUM >= 0x000d00
	size_t idx, len;
#else
	int idx, len;
#endif
	enum json_type type;
	struct json_object_iterator it, end;
	struct json_object *curval, *nxtval;
	struct rp_jsonc_expand_path path;

	/* inspect type of the object */
	type = json_object_get_type(object);
	switch (type) {
	case json_type_object:
		path.index = 0;
		path.previous = previous;
		path.depth = previous->depth + 1;
		path.object = object;
		/* first, expand content of the object */
		it = json_object_iter_begin(object);
		end = json_object_iter_end(object);
		while (!json_object_iter_equal(&it, &end)) {
			curval = json_object_iter_peek_value(&it);
			path.key = json_object_iter_peek_name(&it);
			nxtval = expand(curval, cbs, &path);
			if (nxtval != curval)
				json_object_object_add(object, path.key, nxtval);
			json_object_iter_next(&it);
		}
		/* expand the result using the function */
		if (cbs->expand_object != NULL) {
			nxtval = cbs->expand_object(cbs->closure, object, previous);
			if (nxtval != object) {
				/* the function returned a new object, try recursive expansion of it */
				object = expand(nxtval, cbs, previous);
				if (nxtval != object)
					json_object_put(nxtval);
			}
		}
		break;
	case json_type_array:
		/* arrays are not expanded but their values yes */
		path.key = NULL;
		path.previous = previous;
		path.depth = previous->depth + 1;
		path.object = object;
		len = json_object_array_length(object);
		for (idx = 0 ; idx < len ; idx++) {
			curval = json_object_array_get_idx(object, idx);
			path.index = (size_t)idx;
			nxtval = expand(curval, cbs, &path);
			if (nxtval != curval)
				json_object_array_put_idx(object, idx, nxtval);
		}
		break;
	case json_type_string:
		/* expansion of strings using given function */
		if (cbs->expand_string != NULL)
			object = cbs->expand_string(cbs->closure, object, previous);
		break;
	default:
		/* no expansion on number, bool, null */
		break;
	}
	return object;
}

/* expand an object using user functions */
struct json_object *
rp_jsonc_expand(
	struct json_object *object,
	void *closure,
	rp_jsonc_expandcb expand_object,
	rp_jsonc_expandcb expand_string
) {
	struct cbs cbs;
	struct rp_jsonc_expand_path path;

	cbs.closure = closure;
	cbs.expand_object = expand_object;
	cbs.expand_string = expand_string;

	path.depth = -1;
	path.previous = NULL;
	path.index = 0;
	path.key = NULL;
	path.object = NULL;

	return expand(object, &cbs, &path);
}

/* length of the path */
int rp_jsonc_expand_path_length(rp_jsonc_expand_path_t path)
{
	return path->depth + 1;
}

/* object at index */
struct json_object *rp_jsonc_expand_path_get(rp_jsonc_expand_path_t path, int index)
{
	path = at(path, index);
	return path ? path->object : NULL;
}

/* is object at index an object? */
int rp_jsonc_expand_path_is_object(rp_jsonc_expand_path_t path, int index)
{
	path = at(path, index);
	return path && path->key != NULL;
}

/* is object at index an array */
int rp_jsonc_expand_path_is_array(rp_jsonc_expand_path_t path, int index)
{
	path = at(path, index);
	return path && path->key == NULL;
}

/* key of the subobject of the object at index */
const char *rp_jsonc_expand_path_key(rp_jsonc_expand_path_t path, int index)
{
	path = at(path, index);
	return path ? path->key : NULL;
}

/* index of the subobject of the array at index */
size_t rp_jsonc_expand_path_index(rp_jsonc_expand_path_t path, int index)
{
	path = at(path, index);
	return path ? path->index : 0;
}
