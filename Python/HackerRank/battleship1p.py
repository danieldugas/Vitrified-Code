## Receive board ##
###################
import sys
input_data = []
for line in sys.stdin:
  input_data.append(line)
N = int(input_data[0])
board = input_data[1:]

## Load State ##
################
# Read from state files
#!/usr/bin/python
pm_filename = "pm.csv"
lm_filename = "lm.csv"
sl_filename = "sl.csv"
try:
  with open( pm_filename ) as f:
    pm_file_lines = f.readlines()
  with open( lm_filename ) as f:
    lm_file_lines = f.readlines()
  with open( sl_filename ) as f:
    sl_file_lines = f.readlines()
  is_first_execution = False
except:
  is_first_execution = True
# Instantiate state variables
if is_first_execution:
  print >> sys.stderr, "Initialising possibility matrix"
  import itertools
  rows = range(N)
  columns = range(10)
  possible_moves = list(itertools.product(rows, columns))
  last_move = (0,0)
  spotlight = []
else:
  possible_moves = [(int(line.strip().split(' ')[0]),int(line.strip().split(' ')[1])) for line in pm_file_lines]
  last_move = (int(lm_file_lines[0].strip().split(' ')[0]), int(lm_file_lines[0].strip().split(' ')[1]))
  spotlight = [(int(line.strip().split(' ')[0]),int(line.strip().split(' ')[1])) for line in sl_file_lines]

## Process board data ##
########################
# If a hit is registered, shine a 3x3 'spotlight' onto the hit area.
# This spotlight will be prioritized for the next attacks
if board[last_move[0]][last_move[1]] == 'h':
   print >> sys.stderr, "HIT!"
   
   x_moves = [-1,0,1]
   y_moves = [-1,0,1]
   import itertools
   beam_moves = list(itertools.product(x_moves, y_moves))
   beam_moves.remove((0,0))
   for (x_move, y_move) in beam_moves:
      spot = (last_move[0]+x_move, last_move[1]+y_move)
      if not spot in spotlight:
        if spot in possible_moves:
          spotlight.append(spot)
    
      
## Decision Making ##
#####################
if len(spotlight) > 0:
  (r,c) = spotlight.pop(0)
  possible_moves.remove((r,c))
else:
  import random
  index_to_pop = random.randint(0,len(possible_moves)-1)
  (r,c) = possible_moves.pop(index_to_pop)

## Output ##
############
print r,
print c

## Save State ##
################
with open( pm_filename, "w") as f:
    for item in possible_moves:
       f.write("%s %s\n" % item)
with open( lm_filename, "w") as f:
    f.write("%s %s\n" % (r, c))
with open( sl_filename, "w") as f:
    for item in spotlight:
       f.write("%s %s\n" % item)

