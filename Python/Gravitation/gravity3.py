import sys
import math
import random
import numpy as np
np.seterr(all = 'ignore')
import matplotlib.pyplot as plt
from matplotlib import cm
from mpl_toolkits.mplot3d import Axes3D
plt.ion() # plots don't hang execution

plt.close("all")

## PART I
## Compute Solution of gravity field for single circle of radius rm
# Actual field is rho/rm * G
# x, z expressed in multiples of rm
N = 501

x = np.linspace(-10.,10.,N)
z = np.linspace(-10.,10.,N)
t = np.linspace( 0.,np.pi,10*N)

dt = t[1] - t[0]

[X, Z, T] = np.meshgrid(x, z, t)

from scipy import interpolate
GX = 2* np.sum( ( -X + np.cos(T) ) / ( ( np.cos(T)- X )**2 + ( np.sin(T) )**2 + Z**2 )**(3/2) * dt ,
        axis=2)
GZ = 2* np.sum( ( -Z ) / ( ( np.cos(T)- X )**2 + ( np.sin(T) )**2 + Z**2 )**(3/2) * dt ,
        axis=2)

XX = X[:,:,0]
ZZ = Z[:,:,0]

try:
    gx
except NameError:
    gx = interpolate.interp2d(XX, ZZ, GX, kind='linear')
    gz = interpolate.interp2d(XX, ZZ, GZ, kind='linear')
# to obtain the values: field = rho*dr*dz/rm * gx(X/rm, Z/rm)

plt.quiver(XX,ZZ,GX,GZ)
plt.figure()
plt.contour(XX,ZZ,np.sqrt(GX**2+GZ**2))

## PART II
## Assemble field solutions using circular symmetry around axis z
# Accurate as long as : - rotational symmetry applies
#                       - xy circles have large enough radius

re = 1
rh = 0.023
th = 0.47

N = 101

x = np.linspace(-2.,2.,N)
z = np.linspace(-2.,2.,N)

dx = x[1] - x[0]
dz = z[1] - z[0]

[X, Z] = np.meshgrid(x, z)

R = np.sqrt( X**2 + Z**2 )
Density = np.logical_and(  R > (re-th) , R < (re) ).astype(float)

Sumx = Density * 0.
Sumz = Density * 0.

step = 0
for i in range(N):
    for j in range(N):
        if Density[i,j] != 0:
            rm = R[i,j]
            scaledgx = gx(x/rm, z/rm)
            scaledgz = gz(x/rm, z/rm)
            scaledgx[np.isnan(scaledgx)] = 0
            scaledgz[np.isnan(scaledgz)] = 0
            Sumx += Density[i,j]*dx*dz/rm * scaledgx
            Sumz += Density[i,j]*dx*dz/rm * scaledgz

        # Progress
        step += 1
        print str(100*step/N**2) + "%",
        sys.stdout.write('\r')
        sys.stdout.flush()


Gravity = np.sqrt( Sumx**2 + Sumz**2 )

plt.figure()
plt.quiver(X,Z,Sumx,Sumz)
