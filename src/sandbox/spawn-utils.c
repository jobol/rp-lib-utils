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

#define _GNU_SOURCE

#include "spawn-utils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/signalfd.h>
#include <assert.h>

#ifndef MEMFD_CREATE_MISSING
    //long memfd_create (const char *__name, unsigned int __flags);
    #include <sys/mman.h>
#else
  // missing from Fedora, OpenSuse, ... !!!
  long memfd_create (const char *name, unsigned int flags) {
     #include <sys/syscall.h>
     #include <linux/memfd.h>
     return (syscall(SYS_memfd_create, name, flags));
  }
#endif

// Exec a command in a memory buffer and return stdout result as FD
const char* utilsExecCmd (afb_api_t api, const char* target, const char* command, int *filefd) {
	char fdstr[32];

    // create a valid string name for memfd from target name
    strncpy (fdstr, target, sizeof(fdstr));
    for (int idx=0; fdstr[idx] != '\0'; idx++) {
        if (fdstr[idx]=='/') fdstr[idx]=':';
    }
	int fd = (int)memfd_create(target, MFD_ALLOW_SEALING);
	if (fd <= 0) goto OnErrorExit;

	int pid = fork();
	if (pid != 0) {
		// wait for child to finish
		(void)waitpid(pid, 0, 0);
		lseek (fd, 0, SEEK_SET);
		syncfs(fd);
	} else {
		// redirect stdout to fd and exec command
		char *argv[4];
		argv[0]="shfdexec";
		argv[1]="-c";
		argv[2]=(char*)command;
		argv[3]=NULL;

		dup2(fd, 1);
		close (fd);
		execv("/usr/bin/sh", argv);
		fprintf (stderr, "hoops: utilsExecCmd execfd command return command=%s error=%s\n", command, strerror(errno));
	}

	// argv require string
    *filefd=fd;
    char *response;
    if(asprintf (&response, "%d", fd)<0){
        goto OnErrorExit;}
	return response;

OnErrorExit:
	AFB_API_ERROR(api, "error: utilsExecCmd target=%s Fail to exec command='%s' error='%s'\n", target, command, strerror(errno));
	return NULL;
}

int utilsFileModeIs (const char *filepath, int mode) {
    int err;
    struct stat statbuf;

    err = stat(filepath, &statbuf);
    if (err < 0 || !(statbuf.st_mode & mode)) {
        goto OnErrorExit;
    }
    return 1;

OnErrorExit:
    return 0;
}

// if string is not null extract umask and apply
mode_t utilsUmaskSetGet (const char *mask) {
	mode_t oldmask, newmask;
	if (!mask) {
        oldmask= umask(0);
    } else {
		sscanf (mask, "%o", &newmask);
	}
    // posix umask would need some helpers
	oldmask= umask(newmask);
	return oldmask;
}

// Exec a command in a memory buffer and return stdout result as FD
int utilsExecFdCmd (afb_api_t api, const char* source, const char* command) {
	int fd = (int)memfd_create(source, 0);
	if (fd <0) goto OnErrorExit;

	int pid = fork();
	if (pid != 0) {
		// wait for child to finish
		(void)waitpid(pid, 0, 0);
		lseek (fd, 0, SEEK_SET);
		(void)syncfs(fd);
	} else {
		// redirect stdout to fd and exec command
		char *argv[4];
		argv[0]="UtilsExecFd";
		argv[1]="-c";
		argv[2]=(char*)command;
		argv[3]=NULL;

		dup2(fd, 1);
		close (fd);
		execv("/usr/bin/sh", argv);
		AFB_API_NOTICE(api, "[execv returned] command return command=%s error=%s (utilsExecFdCmd)\n", command, strerror(errno));
        exit(1);
	}
	return (fd);

OnErrorExit:
	AFB_API_NOTICE(api, "[Fail to exec] command=%s (utilsExecFdCmd)\n", command);
	return -1;
}

int utilsTaskPrivileged(void) {

    // test privilege only one
    static int status=-1;
    if (status != -1) return status;

    // try temporally upscale to superuser
    if (seteuid(0) != 0) {
        status = 0;
    } else {
        if(seteuid(getuid())<0){
            return status;
        }
        status = 1;
    }
    return status;
}

// restore default signal handler behaviour
void utilsResetSigals(void) {
    signal(SIGSEGV, SIG_DFL);
}

// return file inode (use to check if two path are pointing on the same file)
long unsigned int utilsGetPathInod (const char* path) {
    struct stat fstat;
    int status;
    status = stat (path, &fstat);
    if (status <0) goto OnErrorExit;

    return (fstat.st_ino);

OnErrorExit:
    return -1;
}

// write group within corresponding sandbox/subgroup dir FD
int utilsFileAddControl (afb_api_t api, const char *uid, int dirFd, const char *ctrlname, const char *ctrlval) {
    int ctrlfd;
    size_t len, count;

    ctrlfd = openat (dirFd, ctrlname, O_WRONLY);
    if (ctrlfd < 0) {
        if (api)
            AFB_API_NOTICE(api, "[cgroup-ctrl-not-found] sandbox='%s' ctrlname='%s' error=%s (nsCgroupSetControl)", uid, ctrlname, strerror(errno));
        else
            fprintf(stderr, "[cgroup-ctrl-not-found] sandbox='%s' ctrlname='%s' error=%s (nsCgroupSetControl)\n", uid, ctrlname, strerror(errno));
        goto OnErrorExit;
    }

    len= strlen(ctrlval);
    count= write(ctrlfd, ctrlval, len);

    // when error add inod to help debuging
    if (count != len) {
        struct stat statfd;
        fstat (ctrlfd, &statfd);
        if (api)
            AFB_API_NOTICE(api, "[cgroup-control-refused] sandbox='%s' ctrlname='%s' inode=%ld value=%s error=%s (nsCgroupSetControl)", uid, ctrlname, statfd.st_ino, ctrlval, strerror(errno));
        else
            fprintf(stderr, "[cgroup-control-refused] sandbox='%s' ctrlname='%s' inode=%ld value=%s error=%s (nsCgroupSetControl)\n", uid, ctrlname, statfd.st_ino, ctrlval, strerror(errno));
        close (ctrlfd);
        goto OnErrorExit;
    }

    close (ctrlfd);
    return 0;

OnErrorExit:
    return 1;
}


// Extract $KeyName and replace with $Key Env or default Value
static int utilExpandEnvKey (spawnDefaultsT *defaults, int *idxIn, const char *inputS, int *idxOut, char *outputS, int maxlen, void *userdata) {
    char envkey[SPAWN_MAX_ARG_LABEL];
    char *envval=NULL;
    int index;

    // get envkey from $ to any 1st non alphanum character
    for (int idx=0; inputS[*idxIn] != '\0'; idx++) {

        (*idxIn)++; // intial $ is ignored
        if (inputS[*idxIn] >= '0' && inputS[*idxIn] <= 'z') {
            if (idx == sizeof(envkey)) goto OnErrorExit;
            envkey[idx]= inputS[*idxIn] & ~32; // move to uppercase
        } else {
            (*idxIn)--; // keep input separation character
            envkey[idx]='\0';
            break;
        }
    }

    // Search for a default key
    for (index=0; defaults[index].label; index++) {
        if (!strcmp (envkey, defaults[index].label)) {
            envval = (*(spawnGetDefaultCbT)defaults[index].callback) (defaults[index].label, defaults[index].ctx, userdata);
            if (!envval) goto OnErrorExit;
            for (int jdx=0; envval[jdx]; jdx++) {
                if (*idxOut >= maxlen) goto OnErrorExit;
                outputS[(*idxOut)++]= envval[jdx];
            }
            if (defaults[index].allocation == SPAWN_MEM_DYNAMIC) free (envval);
        }
    }

    // if label was not found but default callback is defined Warning default should use static memory
    if (!envval && defaults[index].callback) {
        envval = (*(spawnGetDefaultCbT)defaults[index].callback) (defaults[index].label, defaults[index].ctx, userdata);
        if (envval) {
            for (int jdx=0; envval[jdx]; jdx++) {
                if (*idxOut >= maxlen) goto OnErrorExit;
                outputS[(*idxOut)++]= envval[jdx];
            }
        }
    }
    if (defaults[index].allocation == SPAWN_MEM_DYNAMIC) free (envval);

    if (!envval) goto OnErrorExit;
    return 0;

OnErrorExit:
    { // label not expanded, provide some usefull debug information
        int index=0;
        envval="???unset_config_defaults????";
        outputS[index++]='$';
        for (int jdx=0; envkey[jdx]; jdx++) {
            if (index >= maxlen-2) {
            outputS[index] = '\0';
            return 1;
            }
            outputS[index++]= envkey[jdx];
        }
        outputS[index++] = '=';

        for (int jdx=0; envval[jdx]; jdx++) {
            if (index >= maxlen-1) {
                outputS[index] = '\0';
                return 1;
            }
            outputS[index++]= envval[jdx];
        }
        outputS[index] = '\0';
        return 1;
    }
}

const char *utilsExpandString (spawnDefaultsT *defaults, const char* inputS, const char* prefix, const char* trailer, void *userdata) {
    int count=0, idxIn, idxOut=0;
    char outputS[SPAWN_MAX_ARG_LEN];
    int err;

    if (prefix) {
        for (int idx=0; prefix[idx]; idx++) {
            if (idxOut == SPAWN_MAX_ARG_LEN) goto OnErrorExit;
            outputS[idxOut]=prefix[idx];
            idxOut++;
        }
    }

    // search for a $within string input format
    for (idxIn=0; inputS[idxIn] != '\0'; idxIn++) {

        if (inputS[idxIn] != '$') {
            if (idxOut == SPAWN_MAX_ARG_LEN)  goto OnErrorExit;
            outputS[idxOut++] = inputS[idxIn];

        } else {
            if (count == SPAWN_MAX_ARG_LABEL) goto OnErrorExit;
            err=utilExpandEnvKey (defaults, &idxIn, inputS, &idxOut, outputS, SPAWN_MAX_ARG_LEN, userdata);
            if (err) {
                fprintf (stderr, "ERROR: [utilsExpandString] ==> %s <== (check xxxx-defaults.c)\n", outputS);
                goto OnErrorExit;
            }
            count ++;
        }
    }

    // if we have a trailer add it now
    if (trailer) {
        for (int idx=0; trailer[idx]; idx++) {
            if (idxOut == SPAWN_MAX_ARG_LEN) goto OnErrorExit;
            outputS[idxOut]=trailer[idx];
            idxOut++;
        }
    }

    // close the string
    outputS[idxOut]='\0';

    // string is formated replace original with expanded value
    return strdup(outputS);

  OnErrorExit:
    return NULL;

}

// default basic string expansion
const char *utilsExpandKey (const char* src) {
    if (!src) goto OnErrorExit;
    const char *outputString= utilsExpandString (spawnVarDefaults, src, NULL, NULL, NULL);
    return outputString;

  OnErrorExit:
    return NULL;
}

// default basic string expansion
const char *utilsExpandKeyCtx (const char* src, void *ctx) {
    if (!src) goto OnErrorExit;
    const char *outputString= utilsExpandString (spawnVarDefaults, src, NULL, NULL, ctx);
    return outputString;

  OnErrorExit:
    return NULL;
}


// utilsExpandJson is call within forked process, let's keep a test instance within main process for debug purpose
void utilsExpandJsonDebug (void) {
    const char *response;

    json_object *tokenJ1= json_tokener_parse("{'dirname':'/my/test/sample'}"); assert(tokenJ1);
    json_object *tokenJ2= json_tokener_parse("{ 'filename': '/etc/passwd' }"); assert(tokenJ2);

    // should work
    response= utilsExpandJson ("%filename%", tokenJ2);    assert(response);
    response= utilsExpandJson ("--%dirname%--", tokenJ1);    assert(response);
    response= utilsExpandJson ("--%dirname%--", tokenJ1);   assert(response);
    response= utilsExpandJson ("--notexpanded=%%dirname%% expanded=%dirname%", tokenJ1);  assert(response);
    response= utilsExpandJson ("/home/test/fulup", tokenJ1);   assert(response);

    // should fail
    response= utilsExpandJson ("--notfound=%filename%%", tokenJ1);  assert(!response);

    return (void)response; //Useless, this is just to avoid warnings
}

// replace any %key% with its coresponding json value (warning: json is case sensitive)
const char *utilsExpandJson (const char* src, json_object *keysJ) {
    int srcIdx, destIdx=0, labelIdx, expanded=0;
    char dst[SPAWN_MAX_ARG_LEN], label[SPAWN_MAX_ARG_LABEL];
    const char *response;
    json_object *labelJ;
    char separator = -1;

    if (!keysJ) return (src);
    if (!src) goto OnErrorExit;

    for (srcIdx=0; src[srcIdx]; srcIdx++) {

        // replace "%%" by '%'
        if (src[srcIdx] == '%' || src[srcIdx] == '?') {
            separator= src[srcIdx];
            if (src[srcIdx+1] == separator) {
                dst[destIdx++]= src[srcIdx];
                srcIdx++;
                continue;
            }
        }

        if (src[srcIdx] != separator) {
            dst[destIdx++]= src[srcIdx];

        } else {
            expanded=1;
            labelIdx=0;
            // extract expansion label for source argument
            for (srcIdx=srcIdx+1; src[srcIdx]  ; srcIdx++) {
                if (src[srcIdx] !=  separator) {
                    label[labelIdx++]= src[srcIdx];
                    if (labelIdx == SPAWN_MAX_ARG_LABEL) goto OnErrorExit;
                } else break;
            }

            // close label string and remove trailling '%' from destination
            label[labelIdx]='\0';

            // search for expansion label within keysJ
            if (!json_object_object_get_ex (keysJ, label, &labelJ)) {
                if (separator == '%') goto OnErrorExit;
            } else {
                // add label value to destination argument
                const char *labelVal= json_object_get_string(labelJ);
                for (labelIdx=0; labelVal[labelIdx]; labelIdx++) {
                    dst[destIdx++] = labelVal[labelIdx];
                }
            }
        }
    }
    dst[destIdx++] = '\0';

    // when expanded make a copy of dst into params
    if (!expanded) {
        response=src;
    } else {
        // fprintf (stderr, "utilsExpandJson: '%s' => '%s'\n", src, dst);
        response= strdup(dst);
    }

    return response;

  OnErrorExit:
        return NULL;
}
