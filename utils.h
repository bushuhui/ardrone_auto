/******************************************************************************

  ArDrone Auto

  Copyright (c) 2007-2014 Shuhui Bu <bushuhui@nwpu.edu.cn>
  http://www.adv-ci.com

  ----------------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/


#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>


////////////////////////////////////////////////////////////////////////////////
// time functions
////////////////////////////////////////////////////////////////////////////////

// debug level
#define RTK_DEBUG_LEVEL_ERROR	1
#define RTK_DEBUG_LEVEL_WARN	2
#define RTK_DEBUG_LEVEL_INFO    3
#define RTK_DEBUG_LEVEL_TRACE	4       // default
#define RTK_DEBUG_LEVEL_NORM	5

// debug level functions
void dbg_set_level(int i);
int  dbg_get_level(void);
void dbg_push_level(int i);
int  dbg_pop_level(void);


// debug print functions
void dbg_printf(int level,
                const char *fname, int line, const char *func,
                const char *szFmtString, ...);

void dbg_p(const char *fmt, ...);

// level 1: error message
#define dbg_pe(...) dbg_printf(1, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// level 2: warning message
#define dbg_pw(...) dbg_printf(2, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// level 3: information message (default)
#define dbg_pi(...) dbg_printf(3, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// level 4: trace message
#define dbg_pt(...) dbg_printf(4, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// level 5: normal message
#define dbg_pn(...) dbg_printf(5, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

// debug stack trace functions
int dbg_stacktrace_setup(void);

////////////////////////////////////////////////////////////////////////////////
// time functions
////////////////////////////////////////////////////////////////////////////////
u_int64_t tm_get_millis(void);
u_int64_t tm_get_ms(void);
u_int64_t tm_get_us(void);

void   tm_sleep(u_int32_t t);

////////////////////////////////////////////////////////////////////////////////
// string functions
////////////////////////////////////////////////////////////////////////////////
typedef std::vector<std::string> StringArray;

// split given string by delims
StringArray split_text(const std::string &intext, const std::string &delims);

// string trim functions
std::string ltrim(const std::string &s);
std::string rtrim(const std::string &s);
std::string trim(const std::string &s);

// string lower & upper
std::string str_tolower(std::string &s);
std::string str_toupper(std::string &s);

// string to int, float, double
int    str_to_int(const std::string &s);
float  str_to_float(const std::string &s);
double str_to_double(const std::string &s);

#endif // __UTILS_H__
