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
 * Reads the 'file' relative to 'dfd' (see openat) in a freshly
 * allocated memory and returns it in 'content' and 'size' (if not NULL).
 * To help in reading text files, a pending null is always added at the
 * end of the content but not counted reported in the returned size.
 *
 * @param dfd the directory file descriptor number
 * @param file filename to be read (absolute or relative to dfd)
 * @param content if not NULL, where to store the content of the file
 * @param size if not NULL, where to store the length read
 *
 * @return 0 in case of success or else -errno
 */
extern int rp_file_get_at(int dfd, const char *file, char **content, size_t *size);

/**
 * Reads the 'file' in a freshly allocated memory and returns it
 * in 'content' and 'size' (if not NULL).
 * To help in reading text files, a pending null is always added at the
 * end of the content but not counted reported in the returned size.
 *
 * alias for rp_file_get_at(AT_FDCWD, file, content, size)
 *
 * @param file filename to be read
 * @param content if not NULL, where to store the content of the file
 * @param size if not NULL, where to store the length read
 *
 * @return 0 in case of success or else -errno
 */
extern int rp_file_get(const char *file, char **content, size_t *size);

/**
 * Writes the 'file' relative to 'dfd' (see openat) with the 'content' of 'size'.
 *
 * @param dfd the directory file descriptor number
 * @param file filename to be written
 * @param content the content to write
 * @param size the length of the content
 *
 * @return 0 in case of success or else -errno
 */
extern int rp_file_put_at(int dfd, const char *file, const void *content, size_t size);

/**
 * Writes the 'file'  with the 'content' of 'size'.
 *
 * alias for putfile_at(AT_FDCWD, file, content, size)
 *
 * @param file filename to be written
 * @param content the content to write
 * @param size the length of the content
 *
 * @return 0 in case of success or else -errno
 */
extern int rp_file_put(const char *file, const void *content, size_t size);

#ifdef	__cplusplus
}
#endif
