#include "ks_alloc.h"
#include "ks_pool.h"
#include "ks_log.h"

ks_pool_t *
ks_pool_create(size_t size)
{
	ks_pool_t *p;

	size = (size-sizeof(ks_pool_t) < KS_MAX_ALLOC_FROM_POOL) ? size : KS_MAX_ALLOC_FROM_POOL;

	p = ks_memalign(KS_POOL_ALIGNMENT, size);
	if (p == NULL) 
		return NULL;
	p->d.last = (u_char *)p + sizeof(ks_pool_t);
	p->d.end = (u_char *)p + size;
	p->d.failed = 0;
	p->d.next = NULL;

	size = size - sizeof(ks_pool_t);
	p->max = size;
	p->current = p;
	p->large = NULL;

	ks_log_error(KS_LOG_LEVEL_DEBUG, "ks_pool_create ok max=%ld\n", p->max);

	return p;
}

void
ks_pool_destroy(ks_pool_t *pool)
{
	ks_pool_t *p, *n;
	ks_pool_large_t *l;

	for (l = pool->large; l; l=l->next) {
		if (l->alloc) {
			free(l->alloc);
			l->alloc = NULL;
			ks_log_error(KS_LOG_LEVEL_DEBUG, "free one large.\n");
		}
	}

	for (p = pool, n = p->d.next; ;p = n, n = n->d.next) {
		free(p);
		ks_log_error(KS_LOG_LEVEL_DEBUG, "free one pool.\n"); 
		if (n == NULL) {
			break;
		}
	}
}

void
ks_reset_pool(ks_pool_t *pool)
{
	ks_pool_t *p;
	ks_pool_large_t *l;
	
	for (l = pool->large; l; l = l->next) {
		if (l->alloc) {
			free(l->alloc);
			ks_log_error(KS_LOG_LEVEL_DEBUG, "reset one large\n");
		}
	}
	pool->large = NULL;

	for (p = pool; p; p=p->d.next) {
		p->d.last = (u_char *)p + sizeof(ks_pool_t);
	}
	ks_log_error(KS_LOG_LEVEL_DEBUG, "reset one pool\n");
}

void*
ks_palloc(ks_pool_t *pool, size_t size)
{
	u_char *m;
	ks_pool_t *p;

	if (size <= pool->max) {
		p = pool->current;
		do {
			m = ks_align_ptr(p->d.last, KS_ALIGNMENT);
			if ((size_t)(p->d.end - m) >= size) {
				p->d.last = m + size;
				ks_log_error(KS_LOG_LEVEL_DEBUG, "alloc one size.%ld\n", size);
				return m;
			}
			p = p->d.next;
		}while(p);
		ks_log_error(KS_LOG_LEVEL_DEBUG, "alloc one block.size %ld\n", size);
		return ks_palloc_block(pool, size);
	}
	ks_log_error(KS_LOG_LEVEL_DEBUG, "alloc one large.size %ld\n", size);
	return ks_palloc_large(pool, size);
}

void*
ks_pnalloc(ks_pool_t *pool, size_t size)
{
	u_char *m;
	ks_pool_t *p;

	if (size <= pool->max) {
		p = pool->current;
		do {
			m = p->d.last;
			if ((size_t)(p->d.end - m) >= size) {
				p->d.last = m + size;
				return m;
			}
			p = p->d.next;
		}while(p);
		return ks_palloc_block(pool, size);
	}
	return ks_palloc_large(pool, size);
}

static void*
ks_palloc_block(ks_pool_t *pool, size_t size)
{
	u_char *m;
	size_t psize;
	ks_pool_t *p, *new, *current;

	psize = (size_t) (pool->d.end - (u_char *)pool);

	m = ks_memalign(KS_POOL_ALIGNMENT, psize);
	if (m == NULL) {
		return NULL;
	}

	new = (ks_pool_t *)m;
	new->d.end = m + psize;
	new->d.next = NULL;
	new->d.failed = 0;
	
	m += sizeof(ks_pool_t);
	m = ks_align_ptr(m, KS_ALIGNMENT);
	new->d.last = m + size;

	current = pool->current;
	for (p = current; p->d.next; p = p->d.next) {
		if (p->d.failed++ > 4) {
			current = p->d.next;
		}
	}
	p->d.next = new;
	pool->current = current ? current : new;

	return m;
}

static void*
ks_palloc_large(ks_pool_t *pool, size_t size)
{
	void *p;
	uintptr_t n;
	ks_pool_large_t *large;

	p = ks_alloc(size);

	if (p == NULL) {
		return NULL;
	}

	n = 0;
	for (large = pool->large; large; large = large->next) {
		if (large->alloc == NULL) {
			large->alloc = p;
			return p;
		}
		if (n++ > 3) {
			break;
		}
	}

	large = ks_palloc(pool, sizeof(ks_pool_large_t));
	if (large == NULL) {
		free(p);
		return NULL;
	}

	large->alloc = p;
	large->next = pool->large;
	pool->large = large;

	return p;
}

void*
ks_pmemalign(ks_pool_t *pool, size_t size, size_t alignment)
{
	void  *p;
	ks_pool_large_t *large;

	p = ks_memalign(alignment, size);
	if ( p == NULL) {
		return NULL;
	}

	large = ks_palloc(pool, sizeof(ks_pool_large_t));
	if (large == NULL) {
		free(p);
		return NULL;
	}

	large->alloc = p;
	large->next = pool->large;
	pool->large = large;

	return p;
}

intptr_t
ks_pfree(ks_pool_t *pool, void *p)
{
	ks_pool_large_t *l;
	
	for (l = pool->large; l; l= l->next) {
		if (p == l->alloc) {
			free(l->alloc);
			l->alloc = NULL;
			ks_log_error(KS_LOG_LEVEL_DEBUG, "free one larg space.\n");
			return 0;
		}
	}

	return 1;
}

void*
ks_pcalloc(ks_pool_t *pool, size_t size)
{
	void *p;
	p = ks_palloc(pool, size);
	if (p) {
		memset(p, 0, size);
	}

	return p;
}
