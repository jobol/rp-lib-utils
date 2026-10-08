/*
 * Copyright (C) 2015-2026 IoT.bzh Company
 * Author "Fulup Ar Foll"
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

#ifndef _SPAWN_UTILS_INCLUDE_
#define _SPAWN_UTILS_INCLUDE_

#define  AFB_BINDING_VERSION 3
#include <afb/afb-binding.h>
#include <systemd/sd-event.h>

#include "spawn-defaults.h"
#include <json.h>

#ifdef MEMFD_CREATE_MISSING
  // missing from Fedora, OpenSuse, ... !!!
  long memfd_create (const char *name, unsigned int __flags);
#endif

typedef enum {
    SPAWN_MEM_STATIC=0,
    SPAWN_MEM_DYNAMIC,
} spawnMemDefaultsE;

typedef char*(*spawnGetDefaultCbT)(const char *label, void *ctx, void *userdata);
typedef struct {
    const char *label;
    spawnGetDefaultCbT callback;
    spawnMemDefaultsE  allocation;
    void *ctx;
} spawnDefaultsT;
extern spawnDefaultsT spawnVarDefaults[];


// spawn-utils.c
mode_t utilsUmaskSetGet (const char *mask);
int utilsTaskPrivileged(void);

int utilsFileModeIs (const char *filepath, int mode);
int utilsFileAddControl (afb_api_t api, const char *uid, int dirFd, const char *ctrlname, const char *ctrlval);
const char* utilsExecCmd (afb_api_t api, const char* source, const char* command, int *filefd);
int utilsExecFdCmd (afb_api_t api, const char* source, const char* command);
long unsigned int utilsGetPathInod (const char* path);
mode_t utilsUmaskSetGet (const char *mask);

const char* utilsExpandString (spawnDefaultsT *defaults, const char* inputS, const char* prefix, const char* trailer, void *ctx);
const char *utilsExpandKeyCtx (const char* src, void *ctx);
const char* utilsExpandKey (const char* inputString);
const char* utilsExpandJson (const char* src, json_object *keysJ);
void utilsExpandJsonDebug (void);
void utilsResetSigals(void);

#endif /* _SPAWN_UTILS_INCLUDE_ */
