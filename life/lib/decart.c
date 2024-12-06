#include "life_lib.h"
#include <mpi.h>

int get_top_left_corner(life_t *l, int coords[2], int dims[2]) {
    return coords[0] * (l->ny / dims[0]) * l->nx + coords[1] * (l->nx / dims[1]);
}

MPI_Comm cart;
MPI_Datatype col_type;
MPI_Datatype row_type;

void _life_init(life_t *l) {
	MPI_Comm_size(MPI_COMM_WORLD, &(l->size));
	MPI_Comm_rank(MPI_COMM_WORLD, &(l->rank));

    int dims[2] = {0, 0};
    MPI_Dims_create(l->size, 2, dims);

    MPI_Type_vector(dims[1], 1, l->nx, MPI_INT, &col_type);
    MPI_Type_commit(&col_type);

    MPI_Type_contiguous(dims[0], MPI_INT, &row_type);
    MPI_Type_commit(&row_type);

    int periods[2] = {1, 1};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart);

    l->start = get_start(l->rank, l->size, l->ny);
    l->stop = l->start + get_rows(l->rank, l->size, l->ny);
}

void _life_free(life_t *) {
    MPI_Type_free(&col_type);
    MPI_Type_free(&row_type);
}

void send_next_row(life_t *l, int src_coords[2], int src_rank, int *dims) {
    int dst_rank;
    int dst_coords[2];

    MPI_Cart_shift(cart, 0, 1, &src_rank, &dst_rank);
    MPI_Cart_coords(cart, dst_rank, 2, dst_coords);
    MPI_Send(
        l->u0 + (get_top_left_corner(l, dst_coords, dims) - l->nx + l->nx * l->ny) % (l->nx * l->ny), 1,
        row_type, dst_rank, TAG, cart);
    MPI_Recv(
        l->u0 + (get_top_left_corner(l, dst_coords, dims) + l->nx) % (l->nx * l->ny), 1,
        row_type, dst_rank, TAG, cart, MPI_STATUS_IGNORE);
}

void send_prev_row(life_t *l, int src_coords[2], int src_rank, int *dims) {
    int dst_rank;
    int dst_coords[2];

    MPI_Cart_shift(cart, 0, -1, &src_rank, &dst_rank);
    MPI_Cart_coords(cart, dst_rank, 2, dst_coords);
    MPI_Recv(
        l->u0 + (get_top_left_corner(l, src_coords, dims) - l->nx + l->nx * l->ny) % (l->nx * l->ny), 1,
        row_type, dst_rank, TAG, cart, MPI_STATUS_IGNORE);
    MPI_Send(
        l->u0 + (get_top_left_corner(l, src_coords, dims) + l->nx) % (l->nx * l->ny), 1,
        row_type, dst_rank, TAG, cart);
}

void send_row(life_t *l, int src_coords[2], int src_rank, int *dims) {
    if (src_coords[0] % 2 == 0) {
        send_next_row(l, src_coords, src_rank, dims);
        send_prev_row(l, src_coords, src_rank, dims);
    } else {
        send_prev_row(l, src_coords, src_rank, dims);
        send_next_row(l, src_coords, src_rank, dims);
    }
}

void send_next_col(life_t *l, int src_coords[2], int src_rank, int *dims) {
    int dst_rank;
    int dst_coords[2];

    MPI_Cart_shift(cart, 1, 1, &src_rank, &dst_rank);
    MPI_Cart_coords(cart, dst_rank, 2, dst_coords);
    MPI_Send(
        l->u0 + (get_top_left_corner(l, dst_coords, dims) - 1 + l->nx * l->ny) % (l->nx * l->ny), 1,
        col_type, src_rank, TAG, cart);
    MPI_Recv(
        l->u0 + (get_top_left_corner(l, dst_coords, dims) + 1) % (l->nx * l->ny), 1,
        col_type, dst_rank, TAG, cart, MPI_STATUS_IGNORE);
}

void send_prev_col(life_t *l, int src_coords[2], int src_rank, int *dims) {
    int dst_rank;
    int dst_coords[2];

    MPI_Cart_shift(cart, 1, -1, &src_rank, &dst_rank);
    MPI_Cart_coords(cart, dst_rank, 2, dst_coords);
    MPI_Recv(
        l->u0 + (get_top_left_corner(l, src_coords, dims) - 1 + l->nx * l->ny) % (l->nx * l->ny), 1,
        col_type, dst_rank, TAG, cart, MPI_STATUS_IGNORE);
    MPI_Send(
        l->u0 + (get_top_left_corner(l, src_coords, dims) + 1 + l->nx * l->ny) % (l->nx * l->ny), 1,
        col_type, src_rank, TAG, cart);
}

void send_col(life_t *l, int src_coords[2], int src_rank, int *dims) {
    if (src_coords[1] % 2 == 0) {
        send_next_col(l, src_coords, src_rank, dims);
        send_prev_col(l, src_coords, src_rank, dims);
    } else {
        send_prev_col(l, src_coords, src_rank, dims);
        send_next_col(l, src_coords, src_rank, dims);
    }
}

void life_get_data(life_t *l) {
    int dims[2];
    int periods[2];
    int coords[2];
    MPI_Cart_get(cart, 2, dims, periods, coords);
    send_row(l, coords, l->rank, dims);
    send_col(l, coords, l->rank, dims);
}

void _life_collect(life_t *l) {
    int el_count = get_rows(l->rank, l->size, l->ny) * l->nx;

    int *counts = l->rank == 0 ? (int*) malloc(l->size * sizeof(int)) : NULL;
    MPI_Gather(&el_count, 1, MPI_INT, counts, 1, MPI_INT, 0, cart);

    int displ;
    MPI_Scan(&el_count, &displ, 1, MPI_INT, MPI_SUM, cart);

    int *displs = l->rank == 0 ? (int*) malloc(l->size * sizeof(int)) : NULL;
    MPI_Gather(&displ, 1, MPI_INT, displs, 1, MPI_INT, 0, cart);
    if (l->rank == 0) {
        for (int i = 0 ;i < l->size; ++i) {
            displs[i] -= counts[i];
        }
    }
    MPI_Gatherv(l->u0 + l->start * l->nx, el_count, MPI_INT,
               ((l->rank == 0) ? l->u1 : NULL),
               counts, displs, MPI_INT,
               0, cart);
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