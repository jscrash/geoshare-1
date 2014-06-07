#include "gs.h"

/* Connection manager */

main(argc,argv)
int argc;
char **argv;
{



/* Create shared memory region for process communication */

 	shmem_create(REGION_SIZE, &id);


