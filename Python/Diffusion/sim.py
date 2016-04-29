# Diffusion Simulation Through Porous Medium
# Python 2.7
# Daniel Dugas

# Currently
# Naive Scheme for discrete space differentiation
# Simple spatial grid

import math
import random
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
np.seterr(all = 'ignore')
plt.ion() # plots don't hang execution

plt.close("all")

## Simulation Parameters
num_steps = 30
num_particles = 100
max_velocity = 1
min_dt = np.inf

## flags
enable_variable_dt = False
enable_velocity_culling = False

## Generate N Particles
skew_initial_particle_distribution = 0
N = num_particles
# positions
X = np.random.rand(N)
Y = np.random.rand(N)
# velocities
VX = np.random.rand(N) - 0.5
VY = np.random.rand(N) - 0.5
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
Time = np.zeros((num_steps+1,1))
# Probes
X0_probe = np.zeros((num_steps+1,1))
X0_probe[0] = X[0]

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

    # Cull excessive velocities
    if enable_velocity_culling:
        Vnorm = np.sqrt( VX**2 + VY**2 )
        excss = Vnorm>max_velocity
        VX[excss] /= Vnorm[excss]/max_velocity
        VY[excss] /= Vnorm[excss]/max_velocity

    # find distance between every particle R
    # stored as inverse of R
    # to fix div by 0 at main diagonal
    DX = np.tile(X,(N,1)) - np.tile(X,(N,1)).T
    DY = np.tile(Y,(N,1)) - np.tile(Y,(N,1)).T
    Rinv = ( DX**2 + DY**2 )**(-0.5) 
    Rinv[np.eye(N)==1] = 0

    # calculate largest allowable dt ( time to first encounter /2)
    if enable_variable_dt:
        r_min = Rinv.max()**-1
        v_max = Vnorm.max()
        dt = min( 0.25* r_min/v_max , min_dt )
    else:
        dt = 1

    # update position
    X += VX *dt
    Y += VY *dt

    # update velocity
    F = Rinv**2
    VX += (-DX * Rinv * F).sum(axis=1)/Mass *dt
    VY += (-DY * Rinv * F).sum(axis=1)/Mass *dt

    # update time-vector
    Time[step] = Time[step-1] + dt
    
    # fill value probes
    X0_probe[step] = X[0]
        
    # display results
    subplot_counter += 1
    plt.subplot(nyplots,nxplots,subplot_counter)
    plt.scatter(X, Y, s=20, c=Species, cmap=cm.Greys)
    plt.axis(viewaxis)

plt.subplot(nyplots,nxplots,nyplots*nxplots-1)
plt.plot(Time, X0_probe, 'ro')
