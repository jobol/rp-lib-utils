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
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include <check.h>
#if !defined(ck_assert_ptr_null)
# define ck_assert_ptr_null(X)      ck_assert_ptr_eq(X, NULL)
# define ck_assert_ptr_nonnull(X)   ck_assert_ptr_ne(X, NULL)
#endif

/*********************************************************************/

#include <rp-utils/rp-expand-vars.h>
#include <rp-utils/rp-jsonc-default-expand.h>
#include <rp-utils/rp-jsonc.h>
#include <rp-utils/rp-yaml-default-expand.h>

/*********************************************************************/

const char template[] = "/tmp/check-rplib-XXXXXX";
char dirbuf[100];
char *dirname;
char buffer[200];

const char j1root[] =
#include "j1root.inc"
;

const char j1inc1[] =
#include "j1inc1.inc"
;

const char j1inc2[] =
#include "j1inc2.inc"
;

const char j1subinc[] =
#include "j1subinc.inc"
;

const char j1resu[] =
#include "j1resu.inc"
;

/*********************************************************************/

int wjf(const char *filename, const char *content)
{
	int rc = -1;
	size_t len = strlen(content);
	FILE *file = fopen(filename, "w");
	if (file != NULL) {
		if (len == fwrite(content, 1, len, file))
			rc = 0;
		fclose(file);
	}
	if (rc != 0)
		printf ("error while writing %s\n", filename);
	return rc;
}

/*********************************************************************/

START_TEST (check_jsonc_default_expand)
{
	int rc;
	struct json_object *obj, *resu;

	obj = NULL;
	rc = rp_jsonc_default_expanding(&obj, "root.json", NULL, NULL, RP_JSONEXP_ALL_ALL);
	json_object_to_fd(1, obj, JSON_C_TO_STRING_PRETTY);
	ck_assert_int_eq(rc, 0);
	resu = json_object_from_file("resu.json");
	ck_assert_ptr_nonnull(resu);
	rc = rp_jsonc_cmp(obj, resu);
	ck_assert_int_eq(rc, 0);
	json_object_put(obj);
	json_object_put(resu);
}
END_TEST

/*********************************************************************/

START_TEST (check_yaml_default_expand)
{
	int rc;
	struct json_object *obj, *resu;

	obj = NULL;
	rc = rp_yaml_default_expand(&obj, "root.json", RP_YAMLEXP_ALL_ALL);
	json_object_to_fd(1, obj, JSON_C_TO_STRING_PRETTY);
	ck_assert_int_eq(rc, 0);
	resu = json_object_from_file("resu.json");
	ck_assert_ptr_nonnull(resu);
	rc = rp_jsonc_cmp(obj, resu);
	ck_assert_int_eq(rc, 0);
	json_object_put(obj);
	json_object_put(resu);
}
END_TEST

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
	int rc;

	/* declares the tests */
	mksuite("default-expand");
		addtcase("default-expand-json");
			addtest(check_jsonc_default_expand);
		addtcase("default-expand-yaml");
			addtest(check_yaml_default_expand);

	/* creates the buffer */
	strcpy(dirbuf, template);
	dirname = mkdtemp(dirbuf);
	if (dirname == NULL) {
		printf("creation of temporary directory failed\n");
		exit(EXIT_FAILURE);
	}

	/* creates the files */
	rc = chdir(dirname);
	if (rc == 0)
		rc = mkdir("dir", 0755);
	if (rc == 0)
		rc = wjf("root.json", j1root);
	if (rc == 0)
		rc = wjf("dir/inc1.json", j1inc1);
	if (rc == 0)
		rc = wjf("dir/inc2.json", j1inc2);
	if (rc == 0)
		rc = wjf("dir/subinc.json", j1subinc);
	if (rc == 0)
		rc = wjf("resu.json", j1resu);

	/* run the tests */
	if (rc == 0)
		rc = srun();

	/* cleanup */
	chdir("/");
	sprintf(buffer, "rm -r %s", dirname);
	system(buffer);

	return rc ? EXIT_FAILURE : EXIT_SUCCESS;
}
