# Diffusion Simulation Through Porous Medium
# Python 2.7
# Daniel Dugas

# Currently
# Implements walls

import sys
import math
import random
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.patches import Rectangle
np.seterr(all = 'ignore')
plt.ion() # plots don't hang execution

plt.close("all")


## Physical parameters
D = 0.0000001 # diffusion coefficient

## Simulation Parameters
num_steps = 20000
num_particles = 1000
max_dt = 1

## flags
enable_variable_dt = True
enable_velocity_culling = False
enable_velocity_alert = True
skew_initial_particle_distribution = False


## Define Experiment Boundaries
## Cells spill over boundary ( by 1 cellwidth )
## Walls exist on the edge of cells
## _|_|_|_|_|_|_|_
## _|_|_|_|_|_|_|_
## _|_|_|_|_|_|_|_
##  | | | | | | | 
# Generate Cells ( NX x NY )
# 2 ghost cells ( 1 on each side )
NX = 200+2
NY = 100+2
viewaxis = [-0.5,2.5,-0.5,1.5]
boundaxis= [0.,2.,0.,1.]
boundwdth = boundaxis[1]-boundaxis[0]
boundhght = boundaxis[3]-boundaxis[2]
cellwdth  = boundwdth/(NX-2)
cellhght  = boundhght/(NY-2)
# a cell position is that of its center
Cell = np.zeros((NX,NY))
CellPosX = np.tile(
        np.linspace(boundaxis[0]-0.5*cellwdth, boundaxis[1]+0.5*cellwdth, NX)
        , (NY,1) ).T
CellPosY = np.tile(
        np.linspace(boundaxis[2]-0.5*cellhght, boundaxis[3]+0.5*cellhght, NY)
        , (NX,1) )
# Generate Walls 
# WallX[0,0] is the right-wall of Cell[0,0]
# WallY[0,0] is the top-wall of Cell[0,0]
WallX = np.zeros((NX,NY)) # vertical walls 
WallY = np.zeros((NX,NY)) # horizontal walls
# Fill in Wall boundary cells and walls
Cell[  CellPosX<boundaxis[0] ] = 1
Cell[  CellPosX>boundaxis[1] ] = 1
Cell[  CellPosY<boundaxis[2] ] = 1
Cell[  CellPosY>boundaxis[3] ] = 1
WallX[  0, : ] = 1
WallX[ -2, : ] = 1
WallY[  :, 0 ] = 1
WallY[  :,-2 ] = 1

## Create membrane
WallX[ 101, ::10 ] = 1
WallX[ 103, ::15 ] = 1
WallX[ 105, ::10 ] = 1


## Generate N Particles
N = num_particles
# positions
X = np.random.rand(N)/2 + 0.25
Y = np.random.rand(N)/2 + 0.25
# velocities
VX = ( np.random.rand(N) - 0.5 ) * cellwdth
VY = ( np.random.rand(N) - 0.5 ) * cellhght
VX[:] = 0
VY[:] = 0
# species and mass
Species = (np.random.rand(N)>=0.5).astype(int) # water or protein
Mass = np.ones(N)
Mass[Species==1] = 1
Mass[Species==0] = 1
# skewing
if skew_initial_particle_distribution:
    X = X + 0.5*Species
    Y = Y + 0.5*Species


## Pre-simulation plot
fig1 = plt.figure()
nyplots = 4
nxplots = 4
plt.subplot(nyplots,nxplots,1)
plt.pcolor(CellPosX, CellPosY, -WallX, cmap=cm.gray)
plt.scatter(X, Y, s=20, c=Species, cmap=cm.Greys)
plt.axis(viewaxis)
#rectangle
ca = plt.gca()
ca.add_patch(Rectangle((0,0),2,1,
              alpha=1, facecolor='none'))
subplot_counter = 1


## Pre-define arrays
time = np.zeros((num_steps+1,1))
alerts = np.zeros((num_steps))
InCells = np.zeros((num_particles,2))
DVX = VX * 0
DVY = VY * 0
# Probes
x0_probe = np.zeros((num_steps))
dt_probe = np.zeros((num_steps))


## Iteration
for step in range(1,num_steps+1):



    # calculate largest allowable dt ( time to first encounter /2)
    if enable_variable_dt:
        dt = min( cellwdth / abs(VX).max() , cellhght / abs(VY).max() )
        dt = min( dt, max_dt )
        dt_probe[step-1] = dt
    else:
        dt = 1

    # Alert excessive velocities
    max_vx = cellwdth/dt
    max_vy = cellhght/dt
    if enable_velocity_alert:
        alerts[step-1] = sum( 
                 ( abs(VX) > max_vx ).astype(int)
               + ( abs(VY) > max_vy ).astype(int) )

    # Cull excessive velocities
    if enable_velocity_culling:
        VX[ abs(VX) > max_vx ] = max_vx * np.sign(VX[ abs(VX) > max_vx ])
        VY[ abs(VY) > max_vy ] = max_vy * np.sign(VY[ abs(VY) > max_vy ])

    ## Assign particles to cells
    InCells[:,0] = np.floor( (NX-2)*X/boundwdth ) + 1
    InCells[:,1] = np.floor( (NY-2)*Y/boundhght ) + 1
    InCells = InCells.astype(int)

    # calculate force felt by each particle
    for particle in range(num_particles):
        # find neighbouring particles
        incell = InCells[particle]
        right = [ 1, 0]
        left  = [-1, 0]
        up    = [ 0, 1]
        down  = [ 0,-1]
        NeighbourCells = np.vstack( (
                           incell,
                           incell + right,
                           incell + left ,
                           incell + up   ,
                           incell + down ,
                           incell + right + up  ,
                           incell + right + down,
                           incell + left  + up  ,
                           incell + left  + down ) )

        InCells1D = InCells[:,0] + InCells[:,1] * NX
        NeighbourCells1D = NeighbourCells[:,0] + NX * NeighbourCells[:,1]

        Neighbours = np.where( (InCells1D[:,None] == NeighbourCells1D).any(axis=1) )[0]

        # remove particle from list of its neighbours
        Neighbours = np.delete(Neighbours, np.where(Neighbours==particle))

        # move to miniature 9-cell system
        MiniX = X[Neighbours]
        MiniY = Y[Neighbours]
        MiniN = len(Neighbours)

        # within mini system:
        # find distance between every particle R
        # stored as inverse of R
        DX = MiniX - X[particle]
        DY = MiniY - Y[particle]
        Rinv = ( DX**2 + DY**2 )**(-0.5) 
# TODO: check if distance too small?

        # find forces exerted by neighbours
        F = D * Rinv**2
        DVX[particle] = (-DX * Rinv * F).sum()/Mass[particle] *dt
        DVY[particle] = (-DY * Rinv * F).sum()/Mass[particle] *dt

        # check wether particle will run out of its cell
        cellcenterx = CellPosX[ incell[0], incell[1] ]
        offcenterx  = X[particle] - cellcenterx
        movedx = abs( VX[particle]*dt + offcenterx ) > cellwdth/2 
        movedleft = np.sign(VX[particle]) < 0

        cellcentery = CellPosY[ incell[0], incell[1] ]
        offcentery  = Y[particle] - cellcentery
        movedy = abs( VY[particle]*dt + offcentery ) > cellhght/2 
        moveddown = np.sign(VY[particle]) < 0

        # bounce back
        wallinthewayx = WallX[ incell[0] - int(movedleft), incell[1] ] == 1
        if movedx and wallinthewayx :
            DVX[particle] = -VX[particle]
            VX[particle] = 0

        wallinthewayy = WallY[ incell[0], incell[1] - int(moveddown) ] == 1
        if movedy and wallinthewayy :
            DVY[particle] = -VY[particle]
            VY[particle] = 0

    # update position
    x0_probe[step-1] = X[0]
    X += VX *dt
    Y += VY *dt

    # update velocity
    VX += DVX
    VY += DVY

    # update time-vector
    time[step] = time[step-1] + dt
    
    # progress
    if np.mod(step*100,num_steps) == 0:
        print str(100*step/num_steps) + "%",
        sys.stdout.write('\r')
        sys.stdout.flush()
    # display results
    if np.mod(step*10,num_steps) == 0:
        subplot_counter += 1
        plt.subplot(nyplots,nxplots,subplot_counter)
        plt.pcolor(CellPosX, CellPosY, -WallX, cmap=cm.gray)
        plt.scatter(X, Y, s=20, c=Species, cmap=cm.Greys)
        plt.axis(viewaxis)
        #rectangle
        ca = plt.gca()
        ca.add_patch(Rectangle((0,0),2,1,
                      alpha=1, facecolor='none'))

# trim last time value
time = time[:-1]

plt.figure()
plt.subplot(2,1,1)
plt.plot(time, x0_probe, 'ro')
plt.title('x position of single particle')
plt.subplot(2,1,2)
plt.plot(time, dt_probe)
plt.title('dt')
