# Diffusion Simulation Through Porous Medium
# Python 2.7
# Daniel Dugas

# Currently
# Cell-grid calculates interparticle effects only at short-distance

import math
import random
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
np.seterr(all = 'ignore')
plt.ion() # plots don't hang execution

plt.close("all")

## Physical parameters
D = 0.0000001 # diffusion coefficient

## Simulation Parameters
num_steps = 30
num_particles = 100
max_velocity = 1
min_dt = np.inf

## flags
enable_variable_dt = False
enable_velocity_culling = False
enable_velocity_alert = True
skew_initial_particle_distribution = False

## Generate N Particles
N = num_particles
# positions
X = np.random.rand(N)/10 + 0.5
Y = np.random.rand(N)/10 + 0.5
# velocities
VX = np.random.rand(N) - 0.5
VY = np.random.rand(N) - 0.5
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

## Pre-define arrays
time = np.zeros((num_steps+1,1))
alerts = np.zeros((num_steps+1))
DVX = VX * 0
DVY = VY * 0
# Probes
x0_probe = np.zeros((num_steps+1,1))
x0_probe[0] = X[0]

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
viewaxis = [-0.5,1.5,-0.5,1.5]
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
Wall = np.zeros((NX-1,NY-1))
WallPosX = np.tile(
        np.linspace(boundaxis[0], boundaxis[1], NX-1)
        , (NY-1,1) ).T
WallPosY = np.tile(
        np.linspace(boundaxis[2], boundaxis[3], NY-1)
        , (NX-1,1) )
# Fill in Wall boundary cells and walls
Cell[CellPosX<boundaxis[0]] = 1
Cell[CellPosX>boundaxis[1]] = 1
Cell[CellPosY<boundaxis[2]] = 1
Cell[CellPosY>boundaxis[3]] = 1
Wall[WallPosX==boundaxis[0]] = 1
Wall[WallPosX==boundaxis[1]] = 1
Wall[WallPosY==boundaxis[2]] = 1
Wall[WallPosY==boundaxis[3]] = 1

# Create Index Masks ( to access array subsets easily )
NOWALLS = (Cell!=1)

## Assign particles to cells
NearCells = np.zeros((num_particles,2))
NearCells[:,0] = np.floor( (NX-2)*X/boundwdth ) + 1
NearCells[:,1] = np.floor( (NY-2)*Y/boundhght ) + 1

## Pre-simulation plot
fig1 = plt.figure()
nyplots = 6
nxplots = 7
plt.subplot(nyplots,nxplots,1)
plt.scatter(X, Y, s=20, c=Species, cmap=cm.Greys)
plt.axis(viewaxis)
subplot_counter = 1

## Iteration
for step in range(1,num_steps+1):
    if np.mod(step*10,num_steps) == 0:
        print str(100*step/num_steps) + "%"

    # Cull excessive velocities
    if enable_velocity_culling:
        Vnorm = np.sqrt( VX**2 + VY**2 )
        excss = Vnorm>max_velocity
        VX[excss] /= Vnorm[excss]/max_velocity
        VY[excss] /= Vnorm[excss]/max_velocity

    # calculate largest allowable dt ( time to first encounter /2)
    if enable_variable_dt:
        r_min = Rinv.max()**-1
        v_max = Vnorm.max()
        dt = min( 0.25* r_min/v_max , min_dt )
    else:
        dt = 1

    # Alert excessive velocities
    if enable_velocity_alert:
        alerts[step] = sum( 
                 ( VX > cellwdth/dt ).astype(int)
               + ( VY > cellhght/dt ).astype(int) )

    # calculate force felt by each particle
    for particle in range(num_particles):
        # find neighbouring particles
        right = [ 1, 0]
        left  = [-1, 0]
        up    = [ 0, 1]
        down  = [ 0,-1]
        NeighbourCells = np.vstack( (
                           NearCells[particle],
                           NearCells[particle] + right,
                           NearCells[particle] + left ,
                           NearCells[particle] + up   ,
                           NearCells[particle] + down ,
                           NearCells[particle] + right + up  ,
                           NearCells[particle] + right + down,
                           NearCells[particle] + left  + up  ,
                           NearCells[particle] + left  + down ) )
        NearCells1D = NearCells[:,0] + NearCells[:,1] * NX
        NeighbourCells1D = NeighbourCells[:,0] + NX * NeighbourCells[:,1]
        Neighbours = np.where( (NearCells1D[:,None] == NeighbourCells1D).any(axis=1) )[0]
        Neighbours = np.delete(Neighbours, np.where(Neighbours==particle))

        # find forces exerted by neighbours
        # move to miniature 9-cell system
        MiniX = X[Neighbours]
        MiniY = Y[Neighbours]
        MiniN = len(Neighbours)
        # find distance between every particle R
        # stored as inverse of R
        # to fix div by 0 at main diagonal
        DX = MiniX - X[particle]
        DY = MiniY - Y[particle]
        Rinv = ( DX**2 + DY**2 )**(-0.5) 
# TODO: check if distance too small?

        F = D * Rinv**2
        DVX[particle] = (-DX * Rinv * F).sum()/Mass[particle] *dt
        DVY[particle] = (-DY * Rinv * F).sum()/Mass[particle] *dt



    # update position
    X += VX *dt
    Y += VY *dt

    # update velocity
    VX += DVX
    VY += DVY

    # update time-vector
    time[step] = time[step-1] + dt
    
    # fill value probes
    x0_probe[step] = X[0]
        
    # display results
    subplot_counter += 1
    plt.subplot(nyplots,nxplots,subplot_counter)
    plt.scatter(X, Y, s=20, c=Species, cmap=cm.Greys)
    plt.axis(viewaxis)

plt.subplot(nyplots,nxplots,nyplots*nxplots-1)
plt.plot(time, x0_probe, 'ro')
