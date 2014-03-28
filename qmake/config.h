#ifndef GMIO_CONFIG_H
#define GMIO_CONFIG_H

#ifdef __GNUC__
# ifndef GMIO_HAVE_GCC_BUILTIN_BSWAP_FUNC
#  define GMIO_HAVE_GCC_BUILTIN_BSWAP_FUNC
# endif

# ifndef GMIO_HAVE_STDINT_H
#  define GMIO_HAVE_STDINT_H
# endif
#endif /* __GNUC__ */

#ifdef _MSC_VER
# ifndef GMIO_HAVE_MSVC_BUILTIN_BSWAP_FUNC
#  define GMIO_HAVE_MSVC_BUILTIN_BSWAP_FUNC
# endif

# if !defined(GMIO_HAVE_STDINT_H) && _MSV_VER >= 1600 /* Visual C++ 2010 */
#  define GMIO_HAVE_STDINT_H
# endif
#endif /* _MSC_VER */

#endif /* GMIO_CONFIG_H */
