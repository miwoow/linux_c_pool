#ifndef KS_ALLOC_H
#define KS_ALLOC_H

#include "ks_global.h"

#define ks_align_ptr(p, a) (u_char *) (((uintptr_t)(p) + ((uintptr_t)a - 1)) & ~((uintptr_t)a - 1))
#define KS_ALIGNMENT sizeof(unsigned long)

void* ks_alloc(size_t size);

void* ks_calloc(size_t size);

void* ks_memalign(size_t alignment, size_t size);

#endif
