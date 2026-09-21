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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <poll.h>

#include "http/rp-curl.h"
#include "sys/rp-socket.h"

#include <check.h>
#if !defined(ck_assert_ptr_null)
# define ck_assert_ptr_null(X)      ck_assert_ptr_eq(X, NULL)
# define ck_assert_ptr_nonnull(X)   ck_assert_ptr_ne(X, NULL)
#endif

/*********************************************************************/

char base[100];
int port;
int srvsock = -1;
int clisock = -1;
pthread_t tid;
void (*process)(int) = NULL;

int openserver()
{
	port = 11999;
	while(srvsock < 0 && port < 32768) {
		snprintf(base, sizeof base, "http://127.0.0.1:%d", ++port);
		srvsock = rp_socket_open_scheme(base+7, 1, "tcp");
	}
	return srvsock;
}

void *serve(void *nul)
{
	struct pollfd pfd = { .fd = srvsock, .events = POLLIN, .revents = 0 };
	int rc = poll(&pfd, 1, -1);
	if (rc == 1) {
		clisock = accept(srvsock, NULL, NULL);
		if (clisock >= 0) {
			process(clisock);
			fsync(clisock);
			shutdown(clisock, SHUT_WR);
		}
	}
	return NULL;
}

void disconnect()
{
	if (srvsock >= 0)
		close(srvsock);
	if (clisock >= 0)
		close(clisock);
	srvsock = -1;
	clisock = -1;
}

int start_server()
{
	int rc;

	disconnect();
	rc = openserver();
	if (rc < 0)
		return rc;
	rc = pthread_create(&tid, NULL, serve, NULL);
	if (rc < 0)
		disconnect();
	return rc;
}

void stop_server()
{
	disconnect();
	pthread_join(tid, NULL);
}

/*********************************************************************/

char query[8000];
char reply[8000];
size_t szreply;

void reply_begin(int retcode)
{
	int len = snprintf(reply, sizeof reply, "HTTP/1.0 %d OK\r\n", retcode);
	szreply = (size_t)(unsigned)len;
}

void reply_header(const char *key, const char *value)
{
	int len = snprintf(reply + szreply, sizeof reply - szreply, "%s: %s\r\n", key, value);
	szreply += (size_t)(unsigned)len;
}

void reply_end(const char *content)
{
	int len = snprintf(reply + szreply, sizeof reply - szreply,
			content == NULL ? "\r\n" : "content-length: %lu\r\n\r\n%s",
			content == NULL ? 0 : strlen(content),
			content);
	szreply += (size_t)(unsigned)len;
}

void reply_prepare(int rc, const char*ct, const char *c)
{
	reply_begin(rc);
	if (ct != NULL)
		reply_header("content-type", ct);
	reply_end(c);
}

/*********************************************************************/

void p(const char *hdr, const char *buf, size_t len)
{
	int fc = 1;
	while (len) {
		if (fc)
			printf("<%s> ",hdr);
		fc = *buf == '\n';
		printf("%c", *buf);
		buf++;
		len--;
	}
	if (!fc)
		printf("\n");
	printf("\n");
}

void process_just_reply(int fd)
{
	ssize_t ssz = read(fd, query, sizeof query);
	p("received", query, (size_t)ssz);
	write(fd, reply, szreply);
	p("replied", reply, szreply);
}

void basic_test(int ret, const char *ct, const char *c)
{
	int rc;
	rp_curl_buffer_t buf;
	CURL *curl;
	struct curl_slist *sheads = NULL;

	reply_prepare(ret, ct, c);
	process = process_just_reply;
	rc = start_server();
	ck_assert_int_eq(rc, 0);
	curl = rp_curl_prepare_url(base);
	ck_assert_ptr_nonnull(curl);
	memset(&buf, 0, sizeof buf);
	rc = rp_curl_process(curl, sheads, &buf);
	ck_assert_int_eq(rc, 0);
	if (ct != NULL)
		ck_assert_str_eq(ct, rp_curl_get_content_type(curl));
	if (c != NULL)
		ck_assert_str_eq(c, buf.data);
	stop_server();
	free(buf.data);
	rp_curl_cleanup(curl, sheads);
}

/*********************************************************************/

START_TEST (get_ok)
{
	basic_test(200, NULL, NULL);
	basic_test(200, NULL, "012345");
	basic_test(200, "text/html", "<a href=\"https://iot.bzh\">home</a>");
}

START_TEST (get_err)
{
	basic_test(505, NULL, NULL);
	basic_test(404, "text/html", "<a href=\"https://iot.bzh\">home</a>");
}

/*********************************************************************/

static Suite *suite;
static TCase *tcase;

void mksuite(const char *name) { suite = suite_create(name); }
void addtcase(const char *name) { tcase = tcase_create(name); suite_add_tcase(suite, tcase); }
#define addtest(test) tcase_add_test(tcase, test)
int srun()
{
	int nerr;
	SRunner *srunner = srunner_create(suite);
	srunner_run_all(srunner, CK_NORMAL);
	nerr = srunner_ntests_failed(srunner);
	srunner_free(srunner);
	return nerr;
}

int main(int ac, char **av)
{
	mksuite("curl");
		addtcase("curl");
			addtest(get_ok);
			addtest(get_err);
	return !!srun();
}
