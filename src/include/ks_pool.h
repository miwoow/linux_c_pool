#ifndef KS_POOL_H
#define KS_POOL_H
#include "ks_global.h"

typedef int ks_status_t;
typedef struct ks_pool_s ks_pool_t;

typedef struct {
	u_char *last;
	u_char *end;
	ks_pool_t *next;
	uintptr_t failed;
}ks_pool_data_t;

typedef struct _ks_pool_large_t {
	struct _ks_pool_large_t *next;
	void *alloc;
}ks_pool_large_t;

struct ks_pool_s {
	ks_pool_data_t d;
	ks_pool_t *current;
	size_t max;
	ks_pool_large_t *large;
};



ks_pool_t* ks_pool_create(size_t size);
void ks_pool_destroy(ks_pool_t *pool);
void ks_reset_pool(ks_pool_t *pool);
void* ks_palloc(ks_pool_t *pool, size_t size);
void* ks_pnalloc(ks_pool_t *pool, size_t size);
static void* ks_palloc_block(ks_pool_t *pool, size_t size);
static void* ks_palloc_large(ks_pool_t *pool, size_t size);
void* ks_pmemalign(ks_pool_t *pool, size_t size, size_t alignment);
intptr_t ks_pfree(ks_pool_t *pool, void *p);
void* ks_pcalloc(ks_pool_t *pool, size_t size);

#endif
