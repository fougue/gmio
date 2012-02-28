#ifndef FOUG_STL_GLOBAL_H
#define FOUG_STL_GLOBAL_H

#include "foug_global.h"

#ifdef FOUG_STL_DLL
# ifdef FOUG_STL_MAKE_DLL
#  define FOUG_STL_EXPORT FOUG_DECL_EXPORT
# else
#  define FOUG_STL_EXPORT FOUG_DECL_IMPORT
# endif // FOUG_STL_MAKE_DLL
#else
# define FOUG_STL_EXPORT
#endif // FOUG_STL_DLL

#endif // FOUG_STL_GLOBAL_H
