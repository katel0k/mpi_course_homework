#include "life_lib.h"
#include <mpi.h>

MPI_Datatype column_type;

void _life_init(life_t *l) {
	MPI_Comm_size(MPI_COMM_WORLD, &(l->size));
	MPI_Comm_rank(MPI_COMM_WORLD, &(l->rank));
    MPI_Type_vector(l->ny, 1, l->nx, MPI_INT, &column_type);
    MPI_Type_commit(&column_type);
    l->start = get_start(l->rank, l->size, l->nx);
    l->stop = l->start + get_rows(l->rank, l->size, l->nx);
}

void _life_free(life_t *) {
    MPI_Type_free(&column_type);
}

void send_next(life_t *l) {
    MPI_Send(l->u0 + dec(l->stop, l->nx), 1,
		column_type, inc(l->rank, l->size), TAG, MPI_COMM_WORLD);

    MPI_Recv(l->u0 + inc(l->stop, l->nx), 1,
        column_type, inc(l->rank, l->size), TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void send_prev(life_t *l) {
    MPI_Recv(l->u0 + dec(l->start, l->nx), 1,
		column_type, dec(l->rank, l->size), TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Send(l->u0 + l->start, 1,
        column_type, dec(l->rank, l->size), TAG, MPI_COMM_WORLD);
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
    MPI_Datatype column_span_type;
    if (l->rank == 0) {
		for (int i = 1; i < l->size; ++i) {
            MPI_Type_vector(l->ny, get_rows(i, l->size, l->nx), l->nx, MPI_INT, &column_span_type);
            MPI_Type_commit(&column_span_type);
            MPI_Recv(l->u0 + i * (l->nx / l->size), 1,
                    column_span_type, i, SAVE_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Type_free(&column_span_type);
		}
    } else {
        MPI_Type_vector(l->ny, get_rows(l->rank, l->size, l->nx), l->nx, MPI_INT, &column_span_type);
        MPI_Type_commit(&column_span_type);
        MPI_Send(l->u0 + l->start, 1,
                column_span_type, 0, SAVE_TAG, MPI_COMM_WORLD);
        MPI_Type_free(&column_span_type);
    }
}

void life_step(life_t *l)
{
	life_get_data(l);

	for (int y = 0; y < l->ny; y++) {
		for (int x = l->start; x < l->stop; x++) {
			int n = 0;
			n += l->u0[ind(x+1, y)];
			n += l->u0[ind(x+1, y+1)];
			n += l->u0[ind(x,   y+1)];
			n += l->u0[ind(x-1, y)];
			n += l->u0[ind(x-1, y-1)];
			n += l->u0[ind(x,   y-1)];
			n += l->u0[ind(x-1, y+1)];
			n += l->u0[ind(x+1, y-1)];
			l->u1[ind(x,y)] = 0;
			if (n == 3 && l->u0[ind(x,y)] == 0) {
				l->u1[ind(x,y)] = 1;
			}
			if ((n == 3 || n == 2) && l->u0[ind(x,y)] == 1) {
				l->u1[ind(x,y)] = 1;
			}
		}
	}
	int *tmp = l->u0;
	l->u0 = l->u1;
	l->u1 = tmp;
}