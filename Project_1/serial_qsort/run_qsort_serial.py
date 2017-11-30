
import os
import time
from subprocess import call
from subprocess import Popen, PIPE
from time import sleep
from math import log
import os


DEVNULL = open(os.devnull, 'wb')


Q = range(12, 25)


command = "./qsort_serial"
path1 = "../Results"
path2 = "{0}/serial_qsort_results".format(path1)
p1 = Popen(["mkdir",path1], stdout=DEVNULL, stderr=DEVNULL)
p2 = Popen(["mkdir",path2], stdout=DEVNULL,stderr=DEVNULL)
p3 = Popen(["rm","{0}/*".format(path2)], stdout=DEVNULL,stderr=DEVNULL)

for q in Q:
	sumtime = 0
        count = 0
	filename = "{0}/qsortserialresults{1}.txt".format(path2,q)
	p4 = Popen(["touch",filename], stdout=DEVNULL,stderr=DEVNULL)
	arguments = "{}".format(q)
	for i in range(0, 10):
		p = Popen([command,arguments], stdout=PIPE)
       		out1 = p.communicate()[0]
		out2 = out1.split()
                status = out2[6]
                time = float(out2[4])
                sumtime = sumtime + time
                count = count +1
        time = float(sumtime)/count
	out3 = "{0} {1}".format(time,status)
	f = open(filename,"w")
	f.write(out3)
