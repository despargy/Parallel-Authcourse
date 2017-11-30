


import os
import time
from subprocess import call
from subprocess import Popen, PIPE
from time import sleep
from math import log
import os

DEVNULL = open(os.devnull, 'wb')



arg1 = "../serial_bitonic/"
arg2 = "run_bitonic_serial.py"

os.chdir(arg1)
p1 = Popen(["python",arg2], stdout=DEVNULL, stderr=DEVNULL)
p1.wait()
arg1 = "../serial_qsort"
arg2 = "run_qsort_serial.py"
os.chdir(arg1)
p2 = Popen(["python",arg2], stdout=DEVNULL, stderr=DEVNULL)
p2.wait()
arg1 = "../openMP"
arg2 = "run_openMP_parallel.py"
os.chdir(arg1)
p3 = Popen(["python",arg2], stdout=DEVNULL, stderr=DEVNULL)
p3.wait()
arg1 = "../CilkPlus"
arg2 = "run_cilkplus_parallel.py"
os.chdir(arg1)
p4 = Popen(["python",arg2], stdout=DEVNULL, stderr=DEVNULL)
p4.wait()
arg1 = "../pthreads"
arg2 = "run_pthreads_parallel.py"
os.chdir(arg1)
p5 = Popen(["python",arg2], stdout=DEVNULL, stderr=DEVNULL)
p5.wait()


