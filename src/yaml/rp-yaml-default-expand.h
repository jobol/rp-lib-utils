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

#include <json-c/json.h>
#include "rp-jsonc-default-expand.h"

/** for replacing "true" by true */
#define RP_YAMLEXP_SCAN_TRUE      RP_JSONEXP_SCAN_TRUE
/** for replacing "false" by false */
#define RP_YAMLEXP_SCAN_FALSE     RP_JSONEXP_SCAN_FALSE
/** for replacing "null" by null */
#define RP_YAMLEXP_SCAN_NULL      RP_JSONEXP_SCAN_NULL
/** for replacing "0xHHH" by N */
#define RP_YAMLEXP_SCAN_HEXA      RP_JSONEXP_SCAN_HEXA
/** for replacing "0oOOO" or "0OOO" by N */
#define RP_YAMLEXP_SCAN_OCTAL     RP_JSONEXP_SCAN_OCTAL
/** for replacing "0bBBB" by N */
#define RP_YAMLEXP_SCAN_BINARY    RP_JSONEXP_SCAN_BINARY
/** for replacing "DDDDD" by N */
#define RP_YAMLEXP_SCAN_DECIMAL   RP_JSONEXP_SCAN_DECIMAL
/** for replacing "D.DeD" by N */
#define RP_YAMLEXP_SCAN_DOUBLE    RP_JSONEXP_SCAN_DOUBLE

/** for replacing "${X}" by getenv(X) */
#define RP_YAMLEXP_ENVVAR         RP_JSONEXP_ENVVAR

/** for expanding {"$ref":"PATH"} to content(PATH) */
#define RP_YAMLEXP_$REFS          RP_JSONEXP_$REFS
/** for removing fields whose value is null */
#define RP_YAMLEXP_DELETE_NULLS   RP_JSONEXP_DELETE_NULLS
/** allow expanding multiple refs {"$ref":["PATH",...]} */
#define RP_YAMLEXP_$REFS_MULTIPLE RP_JSONEXP_$REFS_MULTIPLE
/** allow expanding to files of directories (not recursive) */
#define RP_YAMLEXP_$REFS_DIR      RP_JSONEXP_$REFS_DIR

/** for replacing strings encoding booleans */
#define RP_YAMLEXP_SCAN_BOOLEAN   RP_JSONEXP_SCAN_BOOLEAN

/** for replacing strings encoding integers */
#define RP_YAMLEXP_SCAN_INT       RP_JSONEXP_SCAN_INT

/** for replacing strings encoding numbers */
#define RP_YAMLEXP_SCAN_NUMBER    RP_JSONEXP_SCAN_NUMBER

/** for replacing strings encoding any value */
#define RP_YAMLEXP_SCAN_ANY       RP_JSONEXP_SCAN_ANY

/** for expanding all directories and multiple files */
#define RP_YAMLEXP_$REFS_ALL      RP_JSONEXP_$REFS_ALL

/** for replacing strings encoding any value, expanding $ref and removing null fields */
#define RP_YAMLEXP_ALL            RP_JSONEXP_ALL

/** for replacing strings encoding any value, expanding all $ref and removing null fields */
#define RP_YAMLEXP_ALL_ALL        RP_JSONEXP_ALL_ALL

/**
 * Expands environment variables reference in strings and
 * expands include references "$ref" of 'object' according
 * to requirements of 'flags'.
 *
 * The parameter 'object' is a pointer to a pointer to a json-c object.
 * It can not be NULL but the pointed pointer can be NULL. If 'object'
 * points to a NULL pointer, the parameter 'path' is used to read the
 * initial object to expand.
 *
 * The parameter 'path' is the path of the json-c object. It is used to
 * resolve relative inclusions of "$ref". If NULL, the current directory
 * is used.
 *
 * @param object   pointer to the object to process (if pointing NULL, path is read)
 * @param path     path of the object (or NULL)
 * @param flags    bit or of allowed operations (see constants RP_YAMLEXP_...)
 *
 * @return 0 on success or a negative value on error.
 */
extern
int
rp_yaml_default_expand(struct json_object **object, const char *path, int flags);

