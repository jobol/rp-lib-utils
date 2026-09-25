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

#include <stdbool.h>
#include <json-c/json.h>

extern bool rp_jconf_any(
		json_object *conf,
		const char *key,
		bool required,
		json_object **store
);

extern bool rp_jconf_int(
		json_object *conf,
		const char *key,
		bool required,
		int *store,
		int defvalue
);

extern bool rp_jconf_bool(
		json_object *conf,
		const char *key,
		bool required,
		bool *store,
		bool defvalue
);

extern bool rp_jconf_string(
		json_object *conf,
		const char *key,
		bool required,
		const char **store,
		const char *defvalue);

#include "rp-enum-map.h"

extern bool rp_jconf_enum(
		json_object *conf,
		const char *key,
		bool required,
		int *store,
		int defvalue,
		const rp_enum_map_t *keyvals);

#ifdef	__cplusplus
}
#endif
