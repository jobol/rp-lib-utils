/*
 Copyright (C) 2015-2026 IoT.bzh Company

 Author: José Bollo <jose.bollo@iot.bzh>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is furnished
 to do so, subject to the following conditions:

 The above copyright notice and this permission notice (including the next
 paragraph) shall be included in all copies or substantial portions of the
 Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/

#pragma once

#ifdef	__cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/** no error status */
#define rp_base64_ok        0
/** out of memory error status */
#define rp_base64_nomem     -1
/** invalid data status */
#define rp_base64_invalid   -2

/**
 * encode a buffer of data as a fresh allocated base64 string
 *
 * @param data        pointer to the head of the data to encode
 * @param datalen     length in byt of the data to encode
 * @param encoded     pointer to a pointer receiving the encoded value
 * @param encodedlen  pointer to a size receiving the encoded size
 * @param width       width of the lines or zero for one long line
 * @param pad         if not zero pads with = according standard
 * @param url         if not zero emit url variant of base64
 *
 * @return rp_base64_ok in case of success
 *    or rp_base64_nomem if allocation memory failed
 */
extern
int rp_base64_encode(
		const uint8_t *data,
		size_t datalen,
		char **encoded,
		size_t *encodedlen,
		int width,
		int pad,
		int url);

/**
 * decode a base64 string as a fresh buffer of data
 *
 * @param data        pointer to base64 string to be decode
 * @param datalen     length of the string to decode
 * @param encoded     pointer to a pointer receiving the decoded value
 * @param encodedlen  pointer to a size receiving the decoded size
 * @param url         indicates processing of variants
 *                     - url = 0: any variant (even mixed)
 *                     - url > 0: only url variant
 *                     - url < 0: only standard variant
 *
 * @return rp_base64_ok in case of success,
 *  rp_base64_nomem if allocation memory failed,
 *  or rp_base64_invalid if the data isn't a valid base64 input
 */
extern
int rp_base64_decode(
	const char *data,
	size_t datalen,
	uint8_t **decoded,
	size_t *decodedlen,
	int url);

#ifdef	__cplusplus
}
#endif
