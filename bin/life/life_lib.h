#pragma once

#ifndef LIFE_LIB_H
#define LIFE_LIB_H

#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

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

life_t init(int argc, char** argv, int is_raw);
void cleanup(life_t *l);
void life_init(const char *path, life_t *l);
void life_init_from_raw(const char *path, life_t *l);
void life_free(life_t *l);
void life_step(life_t *l);
void life_save_vtk(const char *path, life_t *l);
void life_save_raw(const char *path, life_t *l);

#endif