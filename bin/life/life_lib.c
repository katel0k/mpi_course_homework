#include "life_lib.h"
#include <mpi.h>

int get_num_rows_for_proc(int rank, int size, int n) {
    return rank == size - 1 ? n - n / n * rank : n / size;
}

life_t init(int argc, char **argv) {
    MPI_Init(&argc, &argv);
	if (argc != 2) {
		printf("Usage: %s input file.\n", argv[0]);
		exit(0);
	}
	life_t l;
	life_init(argv[1], &l);
    return l;
}

void cleanup(life_t *l) {
    life_free(&l);
	MPI_Finalize();
}

/**
 * Загрузить входную конфигурацию.
 * Формат файла, число шагов, как часто сохранять, размер поля, затем идут координаты заполненых клеток:
 * steps
 * save_steps
 * nx ny
 * i1 j2
 * i2 j2
 */
void life_init(const char *path, life_t *l)
{
	FILE *fd = fopen(path, "r");
	assert(fd);
	assert(fscanf(fd, "%d\n", &l->steps));
	assert(fscanf(fd, "%d\n", &l->save_steps));
	assert(fscanf(fd, "%d %d\n", &l->nx, &l->ny));

	l->u0 = (int*)calloc(l->nx * l->ny, sizeof(int));
	l->u1 = (int*)calloc(l->nx * l->ny, sizeof(int));

	int i, j, r, cnt;
	cnt = 0;
	while ((r = fscanf(fd, "%d %d\n", &i, &j)) != EOF) {
		l->u0[ind(i, j)] = 1;
		cnt++;
	}

    if (l->rank == 0) {
        printf("Steps %d, save every %d step.\n", l->steps, l->save_steps);
        printf("Field size: %dx%d\n", l->nx, l->ny);
        printf("Loaded %d life cells.\n", cnt);
    }
	fclose(fd);

	MPI_Comm_size(MPI_COMM_WORLD, &(l->size));
	MPI_Comm_rank(MPI_COMM_WORLD, &(l->rank));
    int rows_per_proc = l->ny / l->size;
    l->start = rows_per_proc * l->rank;
    l->stop = l->start + rows_per_proc;
    if (l->rank == l->size - 1) l->stop = l->ny;
}

void life_free(life_t *l)
{
	free(l->u0);
	free(l->u1);
	l->nx = l->ny = 0;
}

void life_save_vtk(const char *path, life_t *l)
{
    if (l->rank == 0) {
        FILE *f;
        f = fopen(path, "w");
        assert(f);
        fprintf(f, "# vtk DataFile Version 3.0\n");
        fprintf(f, "Created by write_to_vtk2d\n");
        fprintf(f, "ASCII\n");
        fprintf(f, "DATASET STRUCTURED_POINTS\n");
        fprintf(f, "DIMENSIONS %d %d 1\n", l->nx+1, l->ny+1);
        fprintf(f, "SPACING %d %d 0.0\n", 1, 1);
        fprintf(f, "ORIGIN %d %d 0.0\n", 0, 0);
        fprintf(f, "CELL_DATA %d\n", l->nx * l->ny);

        fprintf(f, "SCALARS life int 1\n");
        fprintf(f, "LOOKUP_TABLE life_table\n");
        for (int i = 1; i < l->size; ++i) {
            MPI_Recv(l->u0 + i * l->nx, get_num_rows_for_proc(i, l->size, l->ny) * l->nx,
                    MPI_INT, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        for (int i2 = 0; i2 < l->ny; i2++) {
            for (int i1 = 0; i1 < l->nx; i1++) {
                fprintf(f, "%d\n", l->u0[ind(i1, i2)]);
            }
        }
        fclose(f);
    }
    else {
        MPI_Send(l->u0 + l->start * l->nx, (l->stop - l->start) * l->nx, MPI_INT, 0, TAG, MPI_COMM_WORLD);
    }
}

void life_step(life_t *l)
{
    int rows_per_proc = l->ny / l->size;
	int i, j;

    printf("%d %d %d\n", l->rank, l->start, l->stop * l->nx % (l->ny * l->nx));
    MPI_Sendrecv(l->u0 + l->start * l->nx, l->nx, MPI_INT, (l->rank - 1 + l->size) % l->size, TAG,
                l->u0 + l->stop * l->nx % (l->ny * l->nx), l->nx, MPI_INT, (l->rank + 1) % l->size, TAG,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Sendrecv(l->u0 + l->stop * l->nx % (l->ny * l->nx), l->nx, MPI_INT, (l->rank + 1) % l->size, TAG,
                l->u0 + (l->start - 1 + rows_per_proc) * l->nx, l->nx, MPI_INT, (l->rank - 1 + l->size) % l->size, TAG,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	for (j = l->start; j < l->stop; j++) {
		for (i = 0; i < l->nx; i++) {
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
	int *tmp;
	tmp = l->u0;
	l->u0 = l->u1;
	l->u1 = tmp;
}