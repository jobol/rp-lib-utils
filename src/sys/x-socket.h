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

#if __ZEPHYR__

#include <zephyr/net/socket.h>

#define socket(x,y,z)  zsock_socket(x,y,z)
#define setsockopt     zsock_setsockopt

#define accept(x,y,z)  zsock_accept(x,y,z)
#define bind(x,y,z)    zsock_bind(x,y,z)
#define connect(x,y,z) zsock_connect(x,y,z)
#define listen(x,y)    zsock_listen(x,y)

#define addrinfo       zsock_addrinfo
#define getaddrinfo    zsock_getaddrinfo
#define freeaddrinfo   zsock_freeaddrinfo
#define EAI_MEMORY     DNS_EAI_MEMORY

#undef WITH_UNIX_SOCKET

#else

#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/un.h>

#endif /* __ZEPHYR */

#ifdef	__cplusplus
}
#endif
