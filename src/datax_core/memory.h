#ifndef FOUG_C_MEMORY_H
#define FOUG_C_MEMORY_H

#include <string.h>

/*! Type alias for "pointer on function that allocates memory" (like standard malloc()) */
typedef void* (*foug_malloc_func_t)(size_t);

/*! Type alias for "pointer on function that frees memory" (like standard free()) */
typedef void  (*foug_free_func_t)(void*);

#endif /* FOUG_C_MEMORY_H */
