shebang = f'''#!/bin/bash'''
job_template = (
'''
#PBS -l walltime=00:10:00,nodes={node}:ppn={ppn}
#PBS -N mpi_int.1.{proc}
#PBS -q batch
cd $PBS_O_WORKDIR
mpiexec --hostfile $PBS_NODEFILE -np {np} ./time.exe -- ./gosperglidergungliderdestruction.cfg
'''
)
with open('job.sh', 'w') as f:
    f.write(shebang)
    for proc in range(2, 28):
        ppn = proc // 7 + 1
        node = proc % 7 + 1
        np = ppn * node
        f.write(job_template.format(proc=proc, node=node, ppn=ppn, np=np))
