#!/bin/bash
#PBS -l walltime=00:10:00,nodes=3:ppn=1
#PBS -N mpi_int.1.2
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 3 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=4:ppn=1
#PBS -N mpi_int.1.3
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 4 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=5:ppn=1
#PBS -N mpi_int.1.4
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 5 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=6:ppn=1
#PBS -N mpi_int.1.5
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 6 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=7:ppn=1
#PBS -N mpi_int.1.6
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 7 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=1:ppn=2
#PBS -N mpi_int.1.7
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 2 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=2:ppn=2
#PBS -N mpi_int.1.8
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 4 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=3:ppn=2
#PBS -N mpi_int.1.9
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 6 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=4:ppn=2
#PBS -N mpi_int.1.10
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 8 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=5:ppn=2
#PBS -N mpi_int.1.11
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 10 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=6:ppn=2
#PBS -N mpi_int.1.12
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 12 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=7:ppn=2
#PBS -N mpi_int.1.13
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 14 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=1:ppn=3
#PBS -N mpi_int.1.14
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 3 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=2:ppn=3
#PBS -N mpi_int.1.15
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 6 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=3:ppn=3
#PBS -N mpi_int.1.16
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 9 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=4:ppn=3
#PBS -N mpi_int.1.17
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 12 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=5:ppn=3
#PBS -N mpi_int.1.18
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 15 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=6:ppn=3
#PBS -N mpi_int.1.19
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 18 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=7:ppn=3
#PBS -N mpi_int.1.20
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 21 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=1:ppn=4
#PBS -N mpi_int.1.21
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 4 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=2:ppn=4
#PBS -N mpi_int.1.22
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 8 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=3:ppn=4
#PBS -N mpi_int.1.23
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 12 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=4:ppn=4
#PBS -N mpi_int.1.24
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 16 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=5:ppn=4
#PBS -N mpi_int.1.25
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 20 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=6:ppn=4
#PBS -N mpi_int.1.26
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 24 ./time.exe ./gosperglidergungliderdestruction.cfg

#PBS -l walltime=00:10:00,nodes=7:ppn=4
#PBS -N mpi_int.1.27
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np 28 ./time.exe ./gosperglidergungliderdestruction.cfg
