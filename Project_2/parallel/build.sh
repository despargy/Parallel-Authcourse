#!/bin/sh 
rm knn_paral
mpicc -O3 -o knn_paral knn_paral.c
 
