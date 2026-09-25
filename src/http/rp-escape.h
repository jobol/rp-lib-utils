/*
 * Copyright (C) 2017-2026 IoT.bzh Company
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
* @brief build the url and escape its arguments in a preallocated buffer
* @param base NULL or base name of the URL (ex: "https://u:p@server.int:4444/appli")
* @param path NULL or path within base (ex: "api/v1/get")
* @param args NULL or a NULL terminated array of pairs key (even) value (odd, can be NULL)
* @param buffer the buffer where to store the result
* @param length available length of the buffer
* @return the length of the url
*/
extern size_t rp_escape_url_to(const char *base, const char *path, const char * const *args, char *buffer, size_t length);

/**
* @brief build the url and escape its arguments
* @param base NULL or base name of the URL (ex: "https://u:p@server.int:4444/appli")
* @param path NULL or path within base (ex: "api/v1/get")
* @param args NULL or a NULL terminated array of pairs key (even) value (odd, can be NULL)
* @param length NULL or address for storing URL length
* @return NULL or the URL text that must be freed using free
*/
extern char *rp_escape_url(const char *base, const char *path, const char * const *args, size_t *length);

/**
* @brief Synonym of rp_escape_url(NULL, NULL, args, length)
*/
extern char *rp_escape_args(const char * const *args, size_t *length);

/**
* @brief Synonym of rp_escape_url(NULL, NULL, { str, NULL, NULL }, length)
*/
extern char *rp_escape_str(const char *str, size_t *length);

/**
* @brief escape the given text of given length
* @param text the text to escape must not be NULL
* @param textlen the length of the text or 0 if unknown
* @param reslength NULL or address for storing escaped length
* @return NULL or the escaped text that must be freed using free
*/
extern char *rp_escape(const char *text, size_t textlen, size_t *reslength);

/**
* @brief unescape the given url arguments
* @param args the arguments to unescape must not be NULL
* @return NULL or a NULL terminated array of keys (even) and values (odd, might be NULL)
* that must be freed using free on the returned main array that contains all strings
*/
extern const char **rp_unescape_args(const char *args);

/**
 * @brief get value address matching a key in unescaped args
 * @param args unescaped args as returned by rp_unescape_args, must not be NULL
 * @param key identifier of the value to find, must not be NULL
 * @return pointer inside args of the value if key was found, NULL otherwise
 */
extern const char *rp_unescaped_args_get(const char **args, const char *key);

/**
* @brief unescape the given text of given length
* @param text the text to unescape must not be NULL
* @param textlen the length of the text or 0 if unknown
* @param reslength NULL or address for storing unescaped length
* @return NULL or the unescaped text that must be freed using free
*/
extern char *rp_unescape(const char *text, size_t textlen, size_t *reslength);

#ifdef	__cplusplus
}
#endif

/* vim: set colorcolumn=80: */
