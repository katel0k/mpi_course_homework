#include "life_lib.h"

int inc(int rank, int size) {
	return (rank + 1) % size;
}

int dec(int rank, int size) {
	return (rank - 1 + size) % size;
}

int get_rows(int rank, int size, int n) {
    return rank == size - 1 ? n - (n / size) * rank : n / size;
}

int get_start(int rank, int size, int n) {
	return n / size * rank;
}