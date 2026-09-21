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
#define _GNU_SOURCE

#include "rp-curl.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "rp-escape.h"

static const char CONTENT_TYPE[] = "Content-Type";

/*
 * Perform the CURL operation for 'curl' and call given callback
 */
int rp_curl_do(
	CURL *curl,
	struct curl_slist *headers,
	size_t (*callback)(char *buffer, size_t size, size_t nitems, void *userdata),
	void *userdata
) {
	CURLcode code;

	/* Perform the request, res will get the return code */
	if (headers != NULL) {
		code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		if (code != CURLE_OK)
			return -1;
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, userdata);
	code = curl_easy_perform(curl);
	return -(code != CURLE_OK);
}

/* write callback for filling buffers with the response */
static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	rp_curl_buffer_t *buffer = userdata;
	size_t sz = size * nmemb;
	size_t old_size = buffer->size;
	size_t new_size = old_size + sz;
	size_t needed = new_size + 1;
	char *data = buffer->data;
	if (needed > buffer->capacity) {
		data = realloc(data, needed);
		if (data == NULL)
			return 0;
		buffer->data = data;
		buffer->capacity = needed;
	}
	memcpy(&data[old_size], ptr, sz);
	data[new_size] = 0;
	buffer->size = new_size;
	return sz;
}

/*
 * Perform the CURL operation for 'curl' and put the result in
 * memory managed by buffer.
 * Note that the real content is one byte greater than the read
 * size and the last byte zero. This facility allows to handle
 * the returned content as a null terminated C-string.
 */
int rp_curl_process(
	CURL *curl,
	struct curl_slist *headers,
	rp_curl_buffer_t *buffer
) {
	buffer->size = 0;
	return rp_curl_do(curl, headers, write_callback, buffer);
}

/* cleanup of curl and headers */
void rp_curl_cleanup(CURL *curl, struct curl_slist *headers)
{
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
}

/* get a response header */
const char *rp_curl_get_header(CURL * curl, const char *name)
{
#if LIBCURL_VERSION_NUM >= 0x078300
	struct curl_header *hdr;
	CURLHcode code;

	code = curl_easy_header(curl, name, 0, CURLH_HEADER, -1, &hdr);
	return code == CURLHE_OK ? hdr->value : NULL;
#else
	char *value;
	CURLcode code = ~CURLE_OK;

	if (strcasecmp(name, CONTENT_TYPE) == 0)
		code = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &value);
	return code == CURLE_OK ? value : NULL;
#endif
}

/* get the response code */
long rp_curl_get_response_code(CURL *curl)
{
	long rc;
	CURLcode code;

	code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rc);
	return (code == CURLE_OK) ? rc : 0;
}

/* get the content type */
const char *rp_curl_get_content_type(CURL *curl)
{
	return rp_curl_get_header(curl, CONTENT_TYPE);
}

/* check the content type value */
int rp_curl_content_type_is(CURL *curl, const char *value)
{
	const char *actual = rp_curl_get_content_type(curl);
	return actual != NULL && !strncasecmp(actual, value, strcspn(actual, "; "));
}

int rp_curl_headers_add(struct curl_slist **headers, const char *value)
{
	struct curl_slist *newlist = curl_slist_append(*headers, value);
	if (newlist == 0)
		return -1;
	*headers = newlist;
	return 0;
}

int rp_curl_headers_add_keyval(struct curl_slist **headers, const char *name, const char *value)
{
	char *h;
	int rc;

	rc = asprintf(&h, "%s: %s", name, value);
	if (rc >= 0) {
		rc = rp_curl_headers_add(headers, h);
		free(h);
	}
	return rc;
}

int rp_curl_headers_add_content_type(struct curl_slist **headers, const char *value)
{
	return rp_curl_headers_add_keyval(headers, CONTENT_TYPE, value);
}

CURL *rp_curl_prepare_url(const char *url)
{
	CURL *curl;
	CURLcode code;

	curl = curl_easy_init();
	if(curl) {
		code = curl_easy_setopt(curl, CURLOPT_URL, url);
		if (code == CURLE_OK)
			return curl;
		curl_easy_cleanup(curl);
	}
	return NULL;
}

CURL *rp_curl_prepare(const char *base, const char *path, const char * const *args)
{
	CURL *res;
	char *url;

	url = rp_escape_url(base, path, args, NULL);
	if (url == NULL)
		return NULL;

	res = rp_curl_prepare_url(url);
	free(url);
	return res;
}

int rp_curl_post_func(
	CURL *curl,
	size_t (*callback)(char *buffer, size_t size, size_t nitems, void *userdata),
	void *userdata
) {
	CURLcode code;

	code = curl_easy_setopt(curl, CURLOPT_POST, 1L);
	if (code == CURLE_OK)
		code = curl_easy_setopt(curl, CURLOPT_READDATA, userdata);
	if (code == CURLE_OK)
		code = curl_easy_setopt(curl, CURLOPT_READFUNCTION, callback);
	return -(code != CURLE_OK);
}

int rp_curl_post_data(CURL *curl, const char *data, size_t szdata, int copy)
{
	CURLcode code;

	code = curl_easy_setopt(curl, CURLOPT_POST, 1L);
	if (code == CURLE_OK)
		code = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, szdata);
	if (code == CURLE_OK)
		code = curl_easy_setopt(curl,
				copy ? CURLOPT_COPYPOSTFIELDS : CURLOPT_POSTFIELDS,
				data);
	return -(code != CURLE_OK);
}


/* vim: set cc=80: */
