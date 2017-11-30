
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

#pthreads analysis
path1 = "../Results"
path2 = "{0}/pthreads_parallel_results".format(path1)
for n_proc in processors:
	rectime = []
	for q in Q:
                filename = "{0}/pthreadsresults_{1}_{2}.txt".format(path2,n_proc,q)
                f = open(filename,"r")
                lines = f.read().splitlines()
                f.close()
                rectime.append(lines[0].split()[1])
	plt.plot(Q, rectime,linewidth=2.0, label="proc={}".format(n_proc) )

path3 = "{0}/serial_qsort_results".format(path1)
qtime = []
for q in Q:
        filename = "{0}/qsortserialresults{1}.txt".format(path3,q)
        f = open(filename,"r")
        lines = f.read().splitlines()
        f.close()
        qtime.append(lines[0].split()[0])
#plt.plot(Q, qtime, 'y^')
plt.xlabel('q : where N=2^q')
plt.ylabel('time')
plt.title('Recursive Pthreads method + Qsort')
plt.plot(Q, qtime, 'r^', label="Qsort" )
plt.plot(Q, qtime , 'r', linewidth=3.0)
plt.legend(loc='upper left')
plt.show()

