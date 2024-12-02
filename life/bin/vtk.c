#include "../lib/life_lib.h"
#include <mpi.h>

int main(int argc, char **argv)
{
	life_t l = init(argc, argv, 0);

	int i;
	char buf[100];
	for (i = 0; i < l.steps; i++) {
		if (i % l.save_steps == 0) {
			sprintf(buf, "vtk/life_%06d.vtk", i);
			printf("Saving step %d to '%s'.\n", i, buf);
			life_save_vtk(buf, &l);
		}
		life_step(&l);
	}

	cleanup(&l);
	MPI_Finalize();
	return 0;
}