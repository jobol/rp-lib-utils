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

#include <stddef.h>

/**
 * Compute the length of string as escaped for JSON
 * without the enclosing double quotes.
 *
 * @param string the string to escape
 * @param maxlen the maximum length of the string
 *
 * @return the length of the escaped JSON string (without the enclosing double quotes)
 *
 * @example
 *   rp_jsonstr_string_escape_length("hello", 10000) = 5
 *   rp_jsonstr_string_escape_length("hello", 3) = 3
 *   rp_jsonstr_string_escape_length("hello\n", 10000) = 13
 */
extern size_t rp_jsonstr_string_escape_length(const char *string, size_t maxlen);

/**
 * Compute the possibly escaped JSON string value of
 * the given string. Doesn't put the enclosing double quotes.
 *
 * @param dest   buffer where to store the escaped JSON string
 * @param destmaxlen the maximum length of the dest
 * @param string the string to escape
 * @param stringmaxlen the maximum length of the string
 *
 * @return the length of the escaped JSON string without the enclosing double quotes
 * and without the added terminating zero. The returned length can be greater than
 * given destmaxlen but overflowing characters are not written to dest in that case.
 * If the returned length is greater or equal to the given length then the escaped
 * string is not terminated by a zero.
 */
extern size_t rp_jsonstr_string_escape(char *dest, size_t destlenmax, const char *string, size_t stringlenmax);

/**
 * Compute the possibly escaped JSON string value of
 * the given string. Doesn't put the enclosing double quotes.
 * This routine is unsafe because it does not check the overflow
 * of the destination. However the check can be done before by using
 * the function @see rp_jsonstr_string_escape_length.
 *
 * @param dest   buffer where to store the escaped JSON string
 * @param string the string to escape
 * @param stringmaxlen the maximum length of the string
 *
 * @return the length of the escaped JSON string without the enclosing double quotes
 * and without the added terminating zero.
 *
 * @see rp_jsonstr_string_escape_length
 * @see rp_jsonstr_string_escape
 */
extern size_t rp_jsonstr_string_escape_unsafe(char *dest, const char *string, size_t stringlenmax);

/**
 * test if a string is a valid json utf8 stream.
 *
 * @param string        the string to be tested
 * @param stringlenmax  maximum length of the string to test
 * @param size          if not NULL, the consumed size until status
 *
 * @return 0 if the string is not a valid JSON or otherwise 1
 */
extern int rp_jsonstr_test(const char *string, size_t stringlenmax, size_t *size);

#ifdef	__cplusplus
}
#endif
