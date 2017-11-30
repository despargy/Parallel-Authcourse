
import os
import time
from subprocess import call
from subprocess import Popen, PIPE
from time import sleep
from math import log
import os


DEVNULL = open(os.devnull, 'wb')


Q = range(12, 25)


command = "./bitonic_serial"
path1 = "../Results"
path2 = "{0}/serial_bitonic_results".format(path1)
p1 = Popen(["mkdir",path1], stdout=DEVNULL, stderr=DEVNULL)
p2 = Popen(["mkdir",path2], stdout=DEVNULL,stderr=DEVNULL)
p3 = Popen(["rm","{0}/*".format(path2)], stdout=DEVNULL,stderr=DEVNULL)

for q in Q:
	sumtime1 = 0
	sumtime2 = 0
        count = 0
	filename = "{0}/bserialresults{1}.txt".format(path2,q)
	p4 = Popen(["touch",filename], stdout=DEVNULL,stderr=DEVNULL)
	arguments = "{}".format(q)
	for i in range(0, 10):
		p = Popen([command,arguments], stdout=PIPE)
       		out1 = p.communicate()[0]
		out2 = out1.split()
		method1 = out2[0]
                status1 = out2[7]
                time1 = float(out2[5])
		method2 = out2[8]
                status2 = out2[15]	
                time2 = float(out2[13])
                sumtime1 = sumtime1 + time1
		sumtime2 = sumtime2 + time2
                count = count +1
        time1 = float(sumtime1)/count
	time2 = float(sumtime2)/count
	out3 = "{0} {1} {2} \n{3} {4} {5}".format(method1,time1,status1,method2,time2,status2)
	f = open(filename,"w")
	f.write(out3)
