#!/bin/sh 
rm knn_paral_unblock_svd
mpicc -O3 -o knn_paral_unblock_svd knn_paral_unblock_svd.c
