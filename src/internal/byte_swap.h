/* Warning : this header has no multi-inclusion guard. It must be included only once in the
 *           translation unit of use. The reason is that all functions defined here are meant to
 *           be inlined for performance purpose
 */

#include "../global.h"

#ifdef FOUG_HAVE_MSVC_BUILTIN_BSWAP_FUNC
# include <stdlib.h>
#endif

/*! Returns \p val with the order of bytes reversed, uses compiler builtin functions if available */
FOUG_INLINE static uint16_t foug_uint16_bswap(uint16_t val)
{
#ifdef FOUG_HAVE_GCC_BUILTIN_BSWAP_FUNC
  return __builtin_bswap16(val);
#elif defined(FOUG_HAVE_MSVC_BUILTIN_BSWAP_FUNC)
  return _byteswap_ushort(val);
#else
  return ((val & 0x00FF) << 8) | ((val >> 8) & 0x00FF);
#endif
}

/*! Returns \p val with the order of bytes reversed, uses compiler builtin functions if available */
FOUG_INLINE static uint32_t foug_uint32_bswap(uint32_t val)
{
#ifdef FOUG_HAVE_GCC_BUILTIN_BSWAP_FUNC
  return __builtin_bswap32(val);
#elif defined(FOUG_HAVE_MSVC_BUILTIN_BSWAP_FUNC)
  return _byteswap_ulong(val);
#else
  return
      ((val & 0x000000FF) << 24)
      | ((val & 0x0000FF00) << 8)
      | ((val >> 8) & 0x0000FF00)
      | ((val >> 24) & 0x000000FF);
#endif
}
