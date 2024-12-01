#include "life_lib.h"
#include <mpi.h>

int main(int argc, char **argv)
{
	life_t l = init(argc, argv, 1);

    l.steps = 1;
	l.save_steps = 1;

    life_step(&l);
    life_save_raw("life.raw", &l);

	cleanup(&l);
	return 0;
}