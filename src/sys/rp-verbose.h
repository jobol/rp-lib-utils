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

#include <stdarg.h>

/*
 Log level is defined by syslog standard:
       KERN_EMERG             0        System is unusable
       KERN_ALERT             1        Action must be taken immediately
       KERN_CRIT              2        Critical conditions
       KERN_ERR               3        Error conditions
       KERN_WARNING           4        Warning conditions
       KERN_NOTICE            5        Normal but significant condition
       KERN_INFO              6        Informational
       KERN_DEBUG             7        Debug-level messages
*/
enum
{
	rp_Log_Level_Emergency = 0,
	rp_Log_Level_Alert = 1,
	rp_Log_Level_Critical = 2,
	rp_Log_Level_Error = 3,
	rp_Log_Level_Warning = 4,
	rp_Log_Level_Notice = 5,
	rp_Log_Level_Info = 6,
	rp_Log_Level_Debug = 7,
	rp_Log_Level_Extra_Debug = 8
};

enum
{
	rp_Log_Bit_Emergency = 1 << rp_Log_Level_Emergency,
	rp_Log_Bit_Alert = 1 << rp_Log_Level_Alert,
	rp_Log_Bit_Critical = 1 << rp_Log_Level_Critical,
	rp_Log_Bit_Error = 1 << rp_Log_Level_Error,
	rp_Log_Bit_Warning = 1 << rp_Log_Level_Warning,
	rp_Log_Bit_Notice = 1 << rp_Log_Level_Notice,
	rp_Log_Bit_Info = 1 << rp_Log_Level_Info,
	rp_Log_Bit_Debug = 1 << rp_Log_Level_Debug,
	rp_Log_Bit_Extra_Debug = 1 << rp_Log_Level_Extra_Debug
};

enum
{
	rp_Log_Mask_Emergency = (2 << rp_Log_Level_Emergency) - 1,
	rp_Log_Mask_Alert = (2 << rp_Log_Level_Alert) - 1,
	rp_Log_Mask_Critical = (2 << rp_Log_Level_Critical) - 1,
	rp_Log_Mask_Error = (2 << rp_Log_Level_Error) - 1,
	rp_Log_Mask_Warning = (2 << rp_Log_Level_Warning) - 1,
	rp_Log_Mask_Notice = (2 << rp_Log_Level_Notice) - 1,
	rp_Log_Mask_Info = (2 << rp_Log_Level_Info) - 1,
	rp_Log_Mask_Debug = (2 << rp_Log_Level_Debug) - 1,
	rp_Log_Mask_Extra_Debug = (2 << rp_Log_Level_Extra_Debug) - 1
};

extern int rp_logmask;

extern void rp_set_logmask(int mask);

extern void rp_verbose(int loglevel, const char *file, int line, const char *function, const char *fmt, ...) __attribute__((format(printf, 5, 6)));
extern void rp_vverbose(int loglevel, const char *file, int line, const char *function, const char *fmt, va_list args);

/*
* If defined, macros belaow change the behaviour of messages.
*
*  - RP_VERBOSE_NO_DATA
*
*  - RP_VERBOSE_NO_DETAILS
*/
#if defined(RP_VERBOSE_NO_DATA)
# define __RP_VERBOSE__(lvl,...)      do{if((lvl)<=rp_Log_Level_Error) rp_verbose(lvl, __FILE__, __LINE__, NULL, __VA_ARGS__)\
					else rp_verbose(lvl, __FILE__, __LINE__, NULL, NULL);}while(0)
#elif defined(RP_VERBOSE_NO_DETAILS)
# define __RP_VERBOSE__(lvl,...)      rp_verbose(lvl, NULL, 0, NULL, __VA_ARGS__)
#else
# define __RP_VERBOSE__(lvl,...)      rp_verbose(lvl, __FILE__, __LINE__, __func__, __VA_ARGS__)
#endif

#define _RP_LOGMASK_(lvl)	((lvl) < 0 ? -1 : (1 << (lvl)))
#define _RP_WANTLOG_(lvl)	(rp_logmask & _RP_LOGMASK_(lvl))
#define _RP_VERBOSE_(lvl,...)	do{ if (_RP_WANTLOG_(lvl)) __RP_VERBOSE__((lvl), __VA_ARGS__); } while(0)

#define RP_EMERGENCY(...)	_RP_VERBOSE_(rp_Log_Level_Emergency, __VA_ARGS__)
#define RP_ALERT(...)		_RP_VERBOSE_(rp_Log_Level_Alert, __VA_ARGS__)
#define RP_CRITICAL(...)	_RP_VERBOSE_(rp_Log_Level_Critical, __VA_ARGS__)
#define RP_ERROR(...)		_RP_VERBOSE_(rp_Log_Level_Error, __VA_ARGS__)
#define RP_WARNING(...)		_RP_VERBOSE_(rp_Log_Level_Warning, __VA_ARGS__)
#define RP_NOTICE(...)		_RP_VERBOSE_(rp_Log_Level_Notice, __VA_ARGS__)
#define RP_INFO(...)		_RP_VERBOSE_(rp_Log_Level_Info, __VA_ARGS__)
#define RP_DEBUG(...)		_RP_VERBOSE_(rp_Log_Level_Debug, __VA_ARGS__)
#define RP_EXTRA_DEBUG(...)	_RP_VERBOSE_(rp_Log_Level_Extra_Debug, __VA_ARGS__)

static inline int rp_verbose_wants(int lvl) { return _RP_WANTLOG_(lvl); }

extern void rp_verbose_dec();
extern void rp_verbose_inc();
extern void rp_verbose_clear();
extern void rp_verbose_add(int level);
extern void rp_verbose_sub(int level);
extern int rp_verbose_colorize(int value);
extern int rp_verbose_is_colorized();

extern int rp_verbose_level_of_name(const char *name);
extern const char *rp_verbose_name_of_level(int level);

extern void rp_verbose_push(const char *context);
extern void rp_verbose_pop();


typedef void (*rp_verbose_observer_cb)(
	int loglevel,
	const char *file,
	int line,
	const char *function,
	const char *fmt,
	va_list args);

extern rp_verbose_observer_cb rp_verbose_observer;


#define RP_VERBOSE_COLOR_EMERGENCY	"\x1B[101m"
#define RP_VERBOSE_COLOR_ALERT		"\x1B[43m"
#define RP_VERBOSE_COLOR_CRITICAL	"\x1B[41m"
#define RP_VERBOSE_COLOR_ERROR		"\x1B[91m"
#define RP_VERBOSE_COLOR_WARNING	"\x1B[93m"
#define RP_VERBOSE_COLOR_NOTICE		"\x1B[96m"
#define RP_VERBOSE_COLOR_INFO		"\x1B[94m"
#define RP_VERBOSE_COLOR_DEBUG		"\x1B[95m"
#define RP_VERBOSE_COLOR_API		"\x1B[1m"
#define RP_VERBOSE_COLOR_FILE		"\x1B[90m"
#define RP_VERBOSE_COLOR_DEFAULT	"\x1B[0m"

#ifdef	__cplusplus
}
#endif
