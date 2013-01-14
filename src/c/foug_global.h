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

#ifdef FOUG_USE_STDINT_H
# include <stdint.h>
#else
typedef char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef int int32_t;
typedef unsigned int uint32_t;

# ifdef _MSC_VER
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
# else
typedef long long int64_t;
typedef unsigned long long uint64_t;
# endif /* _MSC_VER */

#endif /* FOUG_USE_STDINT_H */

typedef int8_t foug_bool_t;
typedef float foug_real32_t;
typedef double foug_real64_t;

#endif /* FOUG_C_GLOBAL_H */
