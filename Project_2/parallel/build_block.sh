#!/bin/sh 
rm knn_paral_block
mpicc -O3 -o knn_paral_block knn_paral_block.c
