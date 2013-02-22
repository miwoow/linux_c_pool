#include "ks_alloc.h"

void *
ks_alloc(size_t size)
{
	void *p;

	p = malloc(size);
	if (p == NULL) 
		printf("Error: malloc\n");

	return p;
}

void *
ks_calloc(size_t size)
{
	void *p;

	p = ks_alloc(size);
	if (p)
		memset(p, 0, size);
	
	return p;
}

void *
ks_memalign(size_t alignment, size_t size)
{
	void *p;
	int err;

	err = posix_memalign(&p, alignment, size);
	if (err) {
		printf("Error: posix_memalign\n");
		p = NULL;
	}

	return p;
}
