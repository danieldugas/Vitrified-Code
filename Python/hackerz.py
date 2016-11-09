from __future__ import print_function

import numpy as np

## Handle Command-line arguments
import sys
argv = sys.argv[:]
script_path = argv.pop(0)

log = list(np.random.normal(size=(100)))

# ASCII display
x = range(len(log))
y = log
import subprocess

numbers = [np.random.normal(size=(34,3)) for _ in range(10)]
for i in range(100000):
  log.append(np.random.normal())
  log.pop(0)

  for line in log:
    print(line)

  gnuplot = subprocess.Popen(["/usr/bin/gnuplot"],
                             stdin=subprocess.PIPE,
                             stdout=subprocess.PIPE)
  gnuplot.stdin.write("set term dumb 79 25\n".encode('utf-8'))
  gnuplot.stdin.write("plot '-' using 1:2 title 'Line1' with linespoints \n".encode('utf-8'))
  for i,j in zip(x,y):
    gnuplot.stdin.write((str(i) + " " + str(j)).encode('utf-8'))
    gnuplot.stdin.write("e\n".encode('utf-8'))
    gnuplot.stdin.flush()

  for line in str(gnuplot.communicate()[0]).split("\\n"):
    print(line)

  import time
  time.sleep(0.1)
