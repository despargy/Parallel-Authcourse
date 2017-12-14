#!/bin/sh 
rm knn_paral_unblock
mpicc -O3 -o knn_paral_unblock knn_paral_unblock.c
