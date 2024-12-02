inc = -I/usr/lib/x86_64-linux-gnu/mpich/include
row_lib = row_decomp.c
col_lib = col_decmop.c
lib = life_lib.c common.c

vtk_row:
	mpicc bin/vtk.c $(row_lib) $(lib) $(inc) -o vtk.exe

one_row:
	mpicc bin/one_step.c $(row_lib) $(lib) $(inc) -o one_step.exe

one_col:
	mpicc bin/one_step.c $(col_lib) $(lib) $(inc) -o one_step.exe

