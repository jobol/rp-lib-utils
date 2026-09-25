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

#include <errno.h>

#ifndef X_E2BIG
#define X_E2BIG           -E2BIG
#endif
#ifndef X_EAGAIN
#define X_EAGAIN          -EAGAIN
#endif
#ifndef X_EBADMSG
#define X_EBADMSG         -EBADMSG
#endif
#ifndef X_EBUSY
#define X_EBUSY           -EBUSY
#endif
#ifndef X_ECANCELED
#define X_ECANCELED       -ECANCELED
#endif
#ifndef X_ECONNABORTED
#define X_ECONNABORTED    -ECONNABORTED
#endif
#ifndef X_EEXIST
#define X_EEXIST          -EEXIST
#endif
#ifndef X_EFAULT
#define X_EFAULT          -EFAULT
#endif
#ifndef X_EINTR
#define X_EINTR           -EINTR
#endif
#ifndef X_EINVAL
#define X_EINVAL          -EINVAL
#endif
#ifndef X_EMSGSIZE
#define X_EMSGSIZE        -EMSGSIZE
#endif
#ifndef X_ENAMETOOLONG
#define X_ENAMETOOLONG    -ENAMETOOLONG
#endif
#ifndef X_ENODATA
#define X_ENODATA         -ENODATA
#endif
#ifndef X_ENOENT
#define X_ENOENT          -ENOENT
#endif
#ifndef X_ENOMEM
#define X_ENOMEM          -ENOMEM
#endif
#ifndef X_ENOTDIR
#define X_ENOTDIR         -ENOTDIR
#endif
#ifndef X_ENOTSUP
#define X_ENOTSUP         -ENOTSUP
#endif
#ifndef X_EOVERFLOW
#define X_EOVERFLOW       -EOVERFLOW
#endif
#ifndef X_EPERM
#define X_EPERM           -EPERM
#endif
#ifndef X_EPIPE
#define X_EPIPE           -EPIPE
#endif
#ifndef X_EPROTO
#define X_EPROTO          -EPROTO
#endif
#ifndef X_ERANGE
#define X_ERANGE          -ERANGE
#endif
#ifndef X_ETIMEDOUT
#define X_ETIMEDOUT       -ETIMEDOUT
#endif

#ifdef	__cplusplus
}
#endif
