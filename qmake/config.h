#ifndef FOUG_CONFIG_H
#define FOUG_CONFIG_H

#ifdef __GNUC__
# ifndef FOUG_HAVE_GCC_BUILTIN_BSWAP_FUNC
#  define FOUG_HAVE_GCC_BUILTIN_BSWAP_FUNC
# endif

# ifndef FOUG_HAVE_STDINT_H
#  define FOUG_HAVE_STDINT_H
# endif
#endif /* __GNUC__ */

#ifdef _MSC_VER
# ifndef FOUG_HAVE_MSVC_BUILTIN_BSWAP_FUNC
#  define FOUG_HAVE_MSVC_BUILTIN_BSWAP_FUNC
# endif

# if !defined(FOUG_HAVE_STDINT_H) && _MSV_VER >= 1600 /* Visual C++ 2010 */
#  define FOUG_HAVE_STDINT_H
# endif
#endif /* _MSC_VER */

#endif /* FOUG_CONFIG_H */
