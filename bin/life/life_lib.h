#pragma once

#ifndef LIFE_LIB_H
#define LIFE_LIB_H

#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define ind(i, j) (((i + l->nx) % l->nx) + ((j + l->ny) % l->ny) * (l->nx))
#define TAG 99

typedef struct {
	int nx, ny;
	int *u0;
	int *u1;
	int steps;
	int save_steps;

	/* MPI */
	int start, stop;
	int rank;
	int size;
} life_t;

life_t init(int argc, char** argv);
void cleanup(life_t *l);
void life_init(const char *path, life_t *l);
void life_free(life_t *l);
void life_step(life_t *l);
void life_save_vtk(const char *path, life_t *l);

#endif