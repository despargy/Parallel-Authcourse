#!/bin/sh 
rm knn_paral_block_svd
mpicc -O3 -o knn_paral_block_svd knn_paral_block_svd.c
