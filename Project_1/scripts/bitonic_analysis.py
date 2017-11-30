
import os
import time
from subprocess import call
from subprocess import Popen, PIPE
from time import sleep
from math import log
import os

import matplotlib.pyplot as plt

DEVNULL = open(os.devnull, 'wb')

Q = range(12,25 )

#bitonic analysis
path1 = "../Results"
path2 = "{0}/serial_bitonic_results".format(path1)
serialtime = []
for q in Q:
	nofQ.append(q)
	filename = "{0}/bserialresults{1}.txt".format(path2,q)
	f = open(filename,"r")
	f.close()

