#!/bin/bash
#PBS -N mpi-UN-n=4:p=4
#PBS -q pdlab
#PBS -j oe
#PBS -l nodes=4:ppn=4

module load mpi
cd $PBS_O_WORKDIR

mpiexec -np $PBS_NP ./knn_paral_unblock 30 10000 784
