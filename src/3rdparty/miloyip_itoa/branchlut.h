#pragma once

#include "../../gmio_core/global.h"

char* u32toa_branchlut(uint32_t value, char* buffer);
char* i32toa_branchlut(int32_t value, char* buffer);
#ifdef GMIO_HAVE_INT64_TYPE
char* u64toa_branchlut(uint64_t value, char* buffer);
char* i64toa_branchlut(int64_t value, char* buffer);
#endif
