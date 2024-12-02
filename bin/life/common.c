#include "life_lib.h"
#include <mpi.h>

void _life_init(life_t *l) {
	MPI_Comm_size(MPI_COMM_WORLD, &(l->size));
	MPI_Comm_rank(MPI_COMM_WORLD, &(l->rank));
    l->start = get_start(l->rank, l->size, l->ny);
    l->stop = l->start + get_rows(l->rank, l->size, l->ny);
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

	int i, j, r;
	while ((r = fscanf(fd, "%d %d\n", &i, &j)) != EOF) {
		l->u0[ind(i, j)] = 1;
	}
	fclose(fd);

	_life_init(l);
}

void life_init_from_raw(const char *path, life_t *l) {
	FILE *fd = fopen(path, "r");
	assert(fd);
	assert(fscanf(fd, "%d %d ", &l->nx, &l->ny));

	l->u0 = (int*)calloc(l->nx * l->ny, sizeof(int));
	l->u1 = (int*)calloc(l->nx * l->ny, sizeof(int));

	char cell;
	int i = 0;
	while (fscanf(fd, "%c", &cell) != EOF && i < l->nx * l->ny) {
		l->u0[i++] = cell == '1';
	}
	fclose(fd);
	_life_init(l);
}

void life_free(life_t *l)
{
	free(l->u0);
	free(l->u1);
	l->nx = l->ny = 0;
}

life_t init(int argc, char **argv, int is_raw) {
    MPI_Init(&argc, &argv);
	if (argc != 2) {
		printf("Usage: %s input file.\n", argv[0]);
		exit(0);
	}
	life_t l;
	if (is_raw) {
		life_init_from_raw(argv[1], &l);
	} else {
		life_init(argv[1], &l);
	}
    return l;
}

void cleanup(life_t *l) {
    life_free(l);
	MPI_Finalize();
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
            MPI_Recv(l->u0 + i * (l->ny / l->size) * l->nx, get_rows(i, l->size, l->ny) * l->nx,
                    MPI_INT, i, SAVE_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        for (int i2 = 0; i2 < l->ny; i2++) {
            for (int i1 = 0; i1 < l->nx; i1++) {
                fprintf(f, "%d\n", l->u0[ind(i1, i2)]);
            }
        }
        fclose(f);
    }
    else {
        MPI_Send(l->u0 + l->start * l->nx, get_rows(l->rank, l->size, l->ny) * l->nx, MPI_INT, 0, SAVE_TAG, MPI_COMM_WORLD);
    }
}

void life_save_raw(const char *path, life_t *l) {
	if (l->rank == 0) {
        FILE *f;
        f = fopen(path, "w");
        assert(f);
		fprintf(f, "%d %d ", l->nx, l->ny);
        for (int i = 1; i < l->size; ++i) {
            MPI_Recv(l->u0 + i * (l->ny / l->size) * l->nx, get_rows(i, l->size, l->ny) * l->nx,
                    MPI_INT, i, SAVE_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
		for (int i = 0; i < l->nx * l->ny; ++i) {
			fprintf(f, "%d", l->u0[i]);
		}
        fclose(f);
    }
    else {
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