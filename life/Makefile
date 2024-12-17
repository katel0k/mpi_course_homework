inc = -I/usr/lib/x86_64-linux-gnu/mpich/include
row_lib = lib/row_decomp.c
col_lib = lib/col_decomp.c
scat_lib = lib/scat.c
dec_lib = lib/decart.c
lib = lib/life_lib.c lib/common.c

vtk_row:
	mpicc -g bin/vtk.c $(row_lib) $(lib) $(inc) -o vtk.exe

one_row:
	mpicc -g bin/one_step.c $(row_lib) $(lib) $(inc) -o build/one_step.exe

one_col:
	mpicc -g bin/one_step.c $(col_lib) $(lib) $(inc) -o build/one_step.exe

one_scat:
	mpicc -g bin/one_step.c $(scat_lib) $(lib) $(inc) -o build/one_step.exe

one_dec:
	mpicc -g bin/one_step.c $(dec_lib) $(lib) $(inc) -o build/one_step.exe -lm

time_dec:
	mpicc -g bin/time.c $(dec_lib) $(lib) $(inc) -o build/time.exe -lm