#include <stdio.h>
#include <stdlib.h>
#include "ks_pool.h"

int
main(int argc, char **argv)
{
	ks_pool_t *pool=NULL;
	pool = ks_pool_create(1024);
	void *pa = ks_palloc(pool, 10);
	void *pb = ks_palloc(pool, 10);
	void *pc = ks_palloc(pool, 1004);
	void *pd = ks_palloc(pool, 10);
	ks_reset_pool(pool);
	pa = ks_palloc(pool, 1024);
	ks_pool_destroy(pool);
	printf("Good\n");
	return 0;
}
