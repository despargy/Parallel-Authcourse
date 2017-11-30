
import os
import time
from subprocess import call
from subprocess import Popen, PIPE
from time import sleep
from math import log
import os


DEVNULL = open(os.devnull, 'wb')

processors = [1, 2, 4, 8, 16, 32, 64, 128, 256]
Q = range(12, 25)


command = "./pthreads_parallel"
path1 = "../Results"
path2 = "{0}/pthreads_parallel_results".format(path1)
p1 = Popen(["mkdir",path1], stdout=DEVNULL, stderr=DEVNULL)
p2 = Popen(["mkdir",path2], stdout=DEVNULL,stderr=DEVNULL)
p3 = Popen(["rm","{0}/*".format(path2)], stdout=DEVNULL,stderr=DEVNULL)

for n_proc in processors:
	for q in Q:
		sumtime = 0
		count = 0
		filename = "{0}/pthreadsresults_{1}_{2}.txt".format(path2,n_proc,q)
		p4 = Popen(["touch",filename], stdout=DEVNULL,stderr=DEVNULL)
		arg1 = "{0}".format(q)
		arg2 = "{0}".format(n_proc)
		for i in range(0, 10):
			p = Popen([command,arg1,arg2], stdout=PIPE)
      			out1 = p.communicate()[0]
			out2 = out1.split()
			method = out2[0]
			status = out2[7]
			time = float(out2[5])
			sumtime = sumtime + time
			count = count +1
		time = float(sumtime)/count
		out3 = "{0} {1} {2}".format(method,time,status)
		f = open(filename,"w")
		f.write(out3)
