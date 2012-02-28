#ifndef FOUG_GLOBAL_H
#define FOUG_GLOBAL_H

#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
    || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
# define FOUG_DECL_EXPORT __declspec(dllexport)
# define FOUG_DECL_IMPORT __declspec(dllimport)
#else
# define FOUG_DECL_EXPORT
# define FOUG_DECL_IMPORT
#endif // WIN

//#include <QtCore/QtGlobal>

namespace foug {

typedef char Int8;
typedef unsigned char UInt8;

typedef short Int16;
typedef unsigned short UInt16;

typedef int Int32;
typedef unsigned int UInt32;

#ifdef _MSC_VER
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
#else
typedef long long Int64;
typedef unsigned long long UInt64;
#endif // _MSC_VER

typedef float Real32;
typedef double Real64;

} // namespace foug

#endif // FOUG_GLOBAL_H
