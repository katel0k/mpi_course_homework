#include "life_lib.h"
#include <mpi.h>

void _life_init(life_t *l) {
	MPI_Comm_size(MPI_COMM_WORLD, &(l->size));
	MPI_Comm_rank(MPI_COMM_WORLD, &(l->rank));
    l->start = get_start(l->rank, l->size, l->ny);
    l->stop = l->start + get_rows(l->rank, l->size, l->ny);
}

void _life_free(life_t *) {}

void send_next(life_t *l) {
	MPI_Send(
		l->u0 + dec(l->stop, l->nx) * l->nx, l->nx,
		MPI_INT, inc(l->rank, l->size), TAG, MPI_COMM_WORLD);
	MPI_Recv(
		l->u0 + inc(l->stop, l->nx) * l->nx, l->nx,
		MPI_INT, inc(l->rank, l->size), TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void send_prev(life_t *l) {
	MPI_Recv(
		l->u0 + dec(l->start, l->nx) * l->nx, l->nx,
		MPI_INT, dec(l->rank, l->size), TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Send(
		l->u0 + l->start * l->nx, l->nx, 
		MPI_INT, dec(l->rank, l->size), TAG, MPI_COMM_WORLD);
}

void life_get_data(life_t *l) {
	if (l->size <= 2) {
		send_next(l);
	} else {
		if (l->rank % 2 == 0) {
			send_next(l);
			send_prev(l);
		} else {
			send_prev(l);
			send_next(l);
		}
	}
}

void _life_collect(life_t *l) {
    int el_count = get_rows(l->rank, l->size, l->ny) * l->nx;

    int *counts = l->rank == 0 ? (int*) malloc(l->size * sizeof(int)) : NULL;
    MPI_Gather(&el_count, 1, MPI_INT, counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int displ;
    MPI_Scan(&el_count, &displ, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    int *displs = l->rank == 0 ? (int*) malloc(l->size * sizeof(int)) : NULL;
    MPI_Gather(&displ, 1, MPI_INT, displs, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (l->rank == 0) {
        for (int i = 0 ;i < l->size; ++i) {
            displs[i] -= counts[i];
        }
    }
    MPI_Gatherv(l->u0 + l->start * l->nx, el_count, MPI_INT,
               ((l->rank == 0) ? l->u1 : NULL),
               counts, displs, MPI_INT,
               0, MPI_COMM_WORLD);
    int *tmp = l->u0;
	l->u0 = l->u1;
	l->u1 = tmp;
    free(displs);
    free(counts);
}

void life_step(life_t *l)
{
	life_get_data(l);

	for (int j = l->start; j < l->stop; j++) {
		for (int i = 0; i < l->nx; i++) {
			int n = 0;
			n += l->u0[ind(i+1, j)];
			n += l->u0[ind(i+1, j+1)];
			n += l->u0[ind(i,   j+1)];
			n += l->u0[ind(i-1, j)];
			n += l->u0[ind(i-1, j-1)];
			n += l->u0[ind(i,   j-1)];
			n += l->u0[ind(i-1, j+1)];
			n += l->u0[ind(i+1, j-1)];
			l->u1[ind(i,j)] = 0;
			if (n == 3 && l->u0[ind(i,j)] == 0) {
				l->u1[ind(i,j)] = 1;
			}
			if ((n == 3 || n == 2) && l->u0[ind(i,j)] == 1) {
				l->u1[ind(i,j)] = 1;
			}
		}
	}
	int *tmp = l->u0;
	l->u0 = l->u1;
	l->u1 = tmp;
}