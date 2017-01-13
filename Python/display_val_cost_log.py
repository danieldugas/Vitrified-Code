from __future__ import print_function

import numpy as np

## Handle Command-line arguments
import sys
argv = sys.argv[:]
script_path = argv.pop(0)
LOG_SCALE = False
log_path = "autoencoder-marmot/val_cost_log.txt"
while len(argv) > 0:
  arg = argv.pop(0)
  if arg == "--logscale": LOG_SCALE = True
  else: log_path = arg

log = np.loadtxt(log_path)

# ASCII display
x = range(len(log))
Y = log

import subprocess
gnuplot = subprocess.Popen(["/usr/bin/gnuplot"],
                           stdin=subprocess.PIPE,
                           stdout=subprocess.PIPE)
gnuplot.stdin.write("set term dumb 79 15\n".encode('utf-8'))
for n, y in enumerate(Y.T):
    if LOG_SCALE: gnuplot.stdin.write("set logscale y\n".encode('utf-8'))
    gnuplot.stdin.write(("plot \""+log_path+"\" using "+str(n+1)+" with lines\n").encode('utf-8'))
    gnuplot.stdin.flush()

for line in str(gnuplot.communicate()[0]).split("\\n"):
  print(line)
