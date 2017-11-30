
import os
import time
from subprocess import call
from subprocess import Popen, PIPE
from time import sleep
from math import log
import os


DEVNULL = open(os.devnull, 'wb')


Q = range(12, 25)

path1 = "../Results"
path2 = "{0}/serial_qsort_results".format(path1)

for q in Q:
        filename = "{0}/qsortserialresults{1}.txt".format(path2,q)

