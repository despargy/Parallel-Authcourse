
import os
import time
from subprocess import call
from subprocess import Popen, PIPE
from time import sleep
from math import log
import os

import matplotlib.pyplot as plt

DEVNULL = open(os.devnull, 'wb')

processors = [1, 2, 4, 8, 16, 32, 64, 128, 256]
Q = range(12,25 )

#openMP analysis
path1 = "../Results"
path2 = "{0}/openMP_parallel_results".format(path1)
for n_proc in processors:
	imptime = []
	rectime = []
        for q in Q:
                filename = "{0}/openMPresults_{1}_{2}.txt".format(path2,n_proc,q)
                f = open(filename,"r")
                lines = f.read().splitlines()
                f.close()
                imptime.append(lines[0].split()[1])
                rectime.append(lines[1].split()[1])
	plt.figure(1)
        plt.plot(Q, imptime, linewidth=2.0, label="proc={}".format(n_proc) )
        plt.figure(2)
        plt.plot(Q, rectime, linewidth=2.0, label="proc={}".format(n_proc) )

path3 = "{0}/serial_qsort_results".format(path1)
qtime = []
for q in Q:
        filename = "{0}/qsortserialresults{1}.txt".format(path3,q)
        f = open(filename,"r")
        lines = f.read().splitlines()
        f.close()
        qtime.append(lines[0].split()[0])


plt.figure(1)
plt.xlabel('q: when N=2^q')
plt.ylabel('time')
plt.title('Imperative OpenMP method + Qsort')
plt.plot(Q, qtime, 'r^',label="Qsort" )
plt.plot(Q, qtime , 'r', linewidth=3.0)
plt.legend(loc='upper left')
plt.figure(2)
plt.xlabel('q: when N=2^q')
plt.ylabel('time')
plt.title('Recursive OpenMP method + Qsort')
plt.plot(Q, qtime, 'r^', label="Qsort" )
plt.plot(Q, qtime , 'r', linewidth=3.0)
plt.legend(loc='upper left')
plt.show()

