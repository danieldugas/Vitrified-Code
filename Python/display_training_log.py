from __future__ import print_function

import numpy as np

## Handle Command-line arguments
import sys
argv = sys.argv[:]
script_path = argv.pop(0)
try:
  log_path = argv.pop(0)
except IndexError:
  log_path = "/tmp/training_log.csv"

log = np.loadtxt(log_path)

# ASCII display
x = log[:,0]
y = log[:,1]
l = log[:,2]

import subprocess
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
