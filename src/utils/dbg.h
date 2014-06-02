#ifndef __dbg_h__
#define __dbg_h__

#include <cstdio>
#include <cerrno>
#include <cstring>

// colored printing for unixes
#ifdef __WIN32__
	#define ERROR_TEXT "[ERROR]"
	#define WARN_TEXT "[WARN]"
	#define INFO_TEXT "[INFO]"
	#define DEBUG_TEXT "[DEBUG]"
#else
	#define ERROR_TEXT "[31m[ERROR][0m"
	#define WARN_TEXT "[33m[WARN][0m"
	#define INFO_TEXT "[34m[INFO][0m"
	#define DEBUG_TEXT "[36m[DEBUG][0m"
#endif

#if defined(__GCC__) || defined(__MINGW32__)
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

// extract basename
static const char* file_basename (const char * filename)
{
    const char * pos = filename;
    const char * current = filename;
    while(1)
    {
        if ((*current) == '\0') return pos;
        if ((*current) == '/') pos = current+1;
        if ((*current) == '\\') pos = current+1;

        current++;
    }
}

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, DEBUG_TEXT " %s:%d: " M "\n", file_basename(__FILE__), __LINE__, ##__VA_ARGS__)
#endif

// used in the macros
#define clean_errno() (errno == 0 ? "None" : strerror(errno))

// There's no need to use \n, it is included in all macros

// Log for an error, ex: log_err("Id cannot be %u", 4);
#define log_err(M, ...) fprintf(stderr, ERROR_TEXT " (%s:%d: errno: %s) " M "\n", file_basename(__FILE__), __LINE__, clean_errno(), ##__VA_ARGS__)

// the same for warning
#define log_warn(M, ...) fprintf(stderr, WARN_TEXT " (%s:%d: errno: %s) " M "\n", file_basename(__FILE__), __LINE__, clean_errno(), ##__VA_ARGS__)

// the same for info
#define log_info(M, ...) fprintf(stderr, INFO_TEXT " (%s:%d) " M "\n", file_basename(__FILE__), __LINE__, ##__VA_ARGS__)

// check condition A. If A is not true, print an error and go to error label
#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

// Used in switches with cases that should never happen, prints an error and then go to error label
#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

// checks for mem allocation, if it fails, goes to error label
#define check_mem(A) check((A), "Out of memory.")

// same as check but prints only if debug
#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

#endif
