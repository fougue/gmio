#ifndef FOUG_C_GLOBAL_H
#define FOUG_C_GLOBAL_H

#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
    || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
# define FOUG_DECL_EXPORT __declspec(dllexport)
# define FOUG_DECL_IMPORT __declspec(dllimport)
#else
# define FOUG_DECL_EXPORT
# define FOUG_DECL_IMPORT
#endif /* WIN */

typedef char foug_int8;
typedef unsigned char foug_uint8;

typedef foug_int8 foug_bool;

typedef short foug_int16;
typedef unsigned short foug_uint16;

typedef int foug_int32;
typedef unsigned int foug_uint32;

#ifdef _MSC_VER
typedef __int64 foug_int64;
typedef unsigned __int64 foug_uint64;
#else
typedef long long foug_int64;
typedef unsigned long long foug_uint64;
#endif /* _MSC_VER */

typedef float foug_real32;
typedef double foug_real64;

#endif /* FOUG_C_GLOBAL_H */
