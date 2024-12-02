#include "life_lib.h"
#include <mpi.h>

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
