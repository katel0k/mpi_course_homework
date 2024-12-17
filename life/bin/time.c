// https://stackoverflow.com/a/26769672
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

#include "../lib/life_lib.h"
#include <mpi.h>
#include <time.h>


int main(int argc, char **argv)
{
	life_t l = init(argc, argv, 0);

	struct timespec tms;
	if (clock_gettime(CLOCK_REALTIME, &tms)) {
		perror("Failed to get time");
		MPI_Finalize();
		return 1;
	}
	int64_t start = tms.tv_sec * 1000000;
	start += tms.tv_nsec / 1000;

	int i;
	for (i = 0; i < l.steps; i++) {
		life_step(&l);
	}

	if (clock_gettime(CLOCK_REALTIME, &tms)) {
		perror("Failed to get time");
		MPI_Finalize();
		return 1;
	}
	int64_t end = tms.tv_sec * 1000000;
	end += tms.tv_nsec / 1000;
	if (l.rank == 0) {
		printf("%ld\n", end - start);
	}

    life_save_vtk("./life.vtk", &l);
	cleanup(&l);
	return 0;
}