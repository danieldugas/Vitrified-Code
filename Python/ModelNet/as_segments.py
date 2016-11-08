import numpy as np

def dotoff_to_seg(path):
  obj = []
  with open(path, 'r') as file:
    assert file.readline().strip() == "OFF"
    nvertices = int(file.readline().split(' ')[0])
    for i in range(nvertices):
      obj.append( [float(val) for val in file.readline().strip().split(' ')] )
  return np.array(obj)

import os
import sys
walk_dir = "/home/daniel/Desktop/ModelNet10"

paths = [subsubroot+'/'+file_ for root, subdirs, _ in os.walk(walk_dir)
                              for subdir in subdirs
                              for subroot, subsubdirs, _ in os.walk(root+'/'+subdir)
                              for subsubdir in subsubdirs
                              for subsubroot, _, files in os.walk(subroot+'/'+subsubdir)
                              for file_ in files]
paths = [path for path in paths if path.split('.')[-1] == "off"]

segments = []
classes = []
for i, path in enumerate(paths):
  segments.append(dotoff_to_seg(path))
  classes.append(path.split('/')[-1].split('_')[0])
  print(i)
  print(len(paths))
