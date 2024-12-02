#include "life_lib.h"
#include <mpi.h>

void _life_init(life_t *l) {
	MPI_Comm_size(MPI_COMM_WORLD, &(l->size));
	MPI_Comm_rank(MPI_COMM_WORLD, &(l->rank));
    l->start = get_start(l->rank, l->size, l->ny);
    l->stop = l->start + get_rows(l->rank, l->size, l->ny);
}

void _life_free(life_t *) {}

void send_prev(life_t *l) {
	int num_of_prev_rows = get_rows(dec(l->rank, l->size), l->size, l->ny);
	MPI_Sendrecv(
		l->u0 + l->start * l->nx, l->nx * get_rows(l->rank, l->size, l->ny),
		MPI_INT, dec(l->rank, l->size), TAG,
		l->u0 + (l->start - num_of_prev_rows + l->ny) % l->ny * l->nx, l->nx * num_of_prev_rows,
		MPI_INT, dec(l->rank, l->size), TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void send_next(life_t *l) {
	MPI_Sendrecv(
		l->u0 + l->start * l->nx, l->nx * get_rows(l->rank, l->size, l->ny), 
		MPI_INT, inc(l->rank, l->size), TAG,
		l->u0 + (l->start + get_rows(l->rank, l->size, l->ny)) % l->ny * l->nx,
		l->nx * get_rows(inc(l->rank, l->size), l->size, l->ny),
		MPI_INT, inc(l->rank, l->size), TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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
	if (l->rank == 0) {
		for (int i = 1; i < l->size; ++i) {
			MPI_Recv(l->u0 + i * (l->ny / l->size) * l->nx, get_rows(i, l->size, l->ny) * l->nx,
					MPI_INT, i, SAVE_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	} else {
        MPI_Send(l->u0 + l->start * l->nx, get_rows(l->rank, l->size, l->ny) * l->nx, MPI_INT, 0, SAVE_TAG, MPI_COMM_WORLD);
    }
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