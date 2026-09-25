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

#include <curl/curl.h>

/**
 * Buffer used for storing read data
 */
typedef
struct rp_curl_buffer {
	/** size of the data read */
	size_t size;
	/** allocated size of the buffer */
	size_t capacity;
	/** pointer to allocated data */
	char *data;
}
	rp_curl_buffer_t;

/**
 * Perform the CURL query of 'curl' and put the result in
 * the memory managed by 'buffer'.
 * For convenience, a trailing zero is added after the received
 * data but the size doesn't include it.
 *
 * The buffer must be valid. Using memset for setting all
 * its field to zero is valid.
 *
 * The memory returned must be freed by free but it can also
 * be reused in further queries.
 *
 * @param curl the CURL query to perform
 * @param headers NULL or a list of headers to add
 * @param buffer the buffer
 *
 * @result 0 on success or a negative code on error
 */
extern int rp_curl_process(
		CURL * curl,
		struct curl_slist *headers,
		rp_curl_buffer_t *buffer);

/**
 * Perform the CURL query of 'curl' with given 'headers' list.
 * the callback is called with download date and userdata.
 *
 * @param curl the CURL query to perform
 * @param headers NULL or a list of headers to add
 * @param callback callback to be called
 * @param userdata closure of the callback
 *
 * @result 0 on success or a negative code on error
 */
extern int rp_curl_do(
		CURL *curl,
		struct curl_slist *headers,
		size_t (*callback)(char *buffer, size_t size, size_t nitems, void *userdata),
		void *userdata);

/**
 * Cleanup memory used.
 *
 * @param curl NULL or the CURL to cleanup
 * @param headers NULL or the list to cleanup
 */
extern void rp_curl_cleanup(CURL *curl, struct curl_slist *headers);

/**
 * Extract the status of the reply of the query done by 'curl'.
 *
 * @param curl the CURL query object
 *
 * @return 0 on internal error or the response code (ex: 404)
 */
extern long rp_curl_get_response_code(CURL *curl);

/**
 * Get a header value (the first if repeated)
 *
 * @param curl the CURL query object
 * @param name name of the header (without colon), case insensitive
 *
 * @return the value or NULL if error
 */
extern const char *rp_curl_get_header(CURL * curl, const char *name);

/**
 * Get the content type of the response.
 *
 * @param curl the CURL query object
 *
 * @return the content-type or NULL if error
 */
extern const char *rp_curl_get_content_type(CURL * curl);

/**
 * Check if the content_type of the reply of the query done by 'curl'
 * is 'value'.
 *
 * @param curl the CURL query object
 * @param value the expected mimy type
 *
 * @return 0 on internal error or the HTTP status code (ex: 404)
 */
extern int rp_curl_content_type_is (CURL * curl, const char *value);

/**
 * Prepares a simple CURL query (defaulting to type GET)
 *
 * @param url the URL to be queried
 *
 * @return the simple CURL handler for the query
 */
extern CURL *rp_curl_prepare_url(const char *url);

/**
 * Prepares a simple CURL query of type GET (defaulting to type GET)
 *
 * @param base base name of the URL (ex: "https://u:p@server.int:4444/appli")
 * @param path NULL or path within base (ex: "api/v1/get")
 * @param args NULL or a NULL terminated array of pairs key (even) value (odd, can be NULL)
 *
 * @return the simple CURL handler for the query
 *
 * @note @see rp_escape_url
 */
extern CURL *rp_curl_prepare(const char *base, const char *path, const char * const *args);

/**
 * Add a header to the list of headers
 *
 * @param headers the list of headers (NULL if empty)
 * @param value the "key: value" string
 *
 * @return 0 on success or a negative code on error
 */
extern int rp_curl_headers_add(struct curl_slist **headers, const char *value);

/**
 * Add a header "key: value" to the list of headers
 *
 * @param headers the list of headers (NULL if empty)
 * @param key the key string
 * @param value the value string
 *
 * @return 0 on success or a negative code on error
 */
extern int rp_curl_headers_add_keyval(struct curl_slist **headers, const char *name, const char *value);

/**
 * Shortcup to rp_curl_headers_add_keyval(headers, "content-type" value)
 *
 * @param headers the list of headers (NULL if empty)
 * @param value the content type to add
 *
 * @return 0 on success or a negative code on error
 */
extern int rp_curl_headers_add_content_type(struct curl_slist **headers, const char *value);

/**
 * Make a post request with the given 'callback' and 'userdata'.
 *
 * @param curl the CURL object
 * @param callback the callback function for getting data
 * @param userdata to pass to the callback
 *
 * @return 0 on success or NULL on error
 */
extern int rp_curl_post_func(
		CURL *curl,
		size_t (*callback)(char *buffer, size_t size, size_t nitems, void *userdata),
		void *userdata);

/**
 * Make a post request with the given 'data' of given 'size'.
 * Data are copied if 'copy' isn't nul.
 *
 * @param curl the CURL object
 * @param data the data to pass
 * @param size the size of the data
 * @param copy a flag telling to copy the data if not null
 *
 * @return 0 on success or NULL on error
 */
extern int rp_curl_post_data(CURL *curl, const char *data, size_t szdata, int copy);

#ifdef	__cplusplus
}
#endif
