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

/**
 * Structure for TEXT <> INTEGER association
 *
 * th
 */
typedef struct
{
    const char *label; /**< text associated to the integer */
    const int  value;  /**< integer associated to the text */
}
    rp_enum_map_t;

/**
* search the 'value' in the label=NULL terminated array 'keyvals'
* if found, returns true else return false
*/
extern bool rp_enum_map_has_value(const rp_enum_map_t *keyvals, int value);

/** search the 'label' in the label=NULL terminated array 'keyvals'
* if found, returns true else return false
*/
extern bool rp_enum_map_has_label(const rp_enum_map_t *keyvals, const char *label);

/** search the 'value' in the label=NULL terminated array 'keyvals'
* if found, returns true else return false and emits a report with context
*/
extern bool rp_enum_map_check_value(const rp_enum_map_t *keyvals, int value, const char *context);

/** search the 'label' in the label=NULL terminated array 'keyvals'
* if found, returns true else return false and emits a report with context
*/
extern bool rp_enum_map_check_label(const rp_enum_map_t *keyvals, const char *label, const char *context);

/** search the 'label' in the label=NULL terminated array 'keyvals'
* if found, returns true and store the value in 'result' else return false
*/
extern bool rp_enum_map_value(const rp_enum_map_t *keyvals, const char *label, int *result);

/** search the 'value' in the label=NULL terminated array 'keyvals'
* if found, returns true and store the label in 'result' else return false
*/
extern bool rp_enum_map_label(const rp_enum_map_t *keyvals, int value, const char **result);

/** search the 'label' in the label=NULL terminated array 'keyvals'
* if found, returns its value, otherwise if not found, returns the default value 'def'
*/
extern int rp_enum_map_value_def(const rp_enum_map_t *keyvals, const char *label, int def);

/** search the 'value' in the label=NULL terminated array 'keyvals'
* if found, returns its label, otherwise if not found, returns the default label 'def'
*/
extern const char *rp_enum_map_label_def(const rp_enum_map_t *keyvals, int value, const char *def);

#ifdef	__cplusplus
}
#endif
