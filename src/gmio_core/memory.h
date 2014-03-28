#ifndef GMIO_MEMORY_H
#define GMIO_MEMORY_H

#include <string.h>

/*! Type alias for "pointer on function that allocates memory" (like standard malloc()) */
typedef void* (*gmio_malloc_func_t)(size_t);

/*! Type alias for "pointer on function that frees memory" (like standard free()) */
typedef void  (*gmio_free_func_t)(void*);

#endif /* GMIO_MEMORY_H */
