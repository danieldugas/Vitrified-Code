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

re = 1
rh = 0.023
th = 0.47

N = 41

t = np.linspace(0,2*np.pi,N+1)[:-1]
p = np.linspace(-np.pi,np.pi,N+1)[:-1]
r = np.linspace(0,2.,N)

dt = t[1]-t[0]
dp = p[1]-p[0]
dr = r[1]-r[0]

[T,P,R] = np.meshgrid(t,p,r) # get X[p,t,r] for some reason

DV = R*dp * R*np.cos(P)*dt * dr
Matter = np.logical_and(  R > (re-th) , R < (re) ).astype(float)

X = R*np.cos(P) * np.cos(T)
Y = R*np.cos(P) * np.sin(T)
Z = R*np.sin(P)


Sumx = Matter * 0.
Sumy = Matter * 0.
Sumz = Matter * 0.

step = 0
for i in range(N):
    for j in range(N):
        for k in range(N):
            RRinv = 1 / np.sqrt( (X[i,j,k]-X)**2 + (Y[i,j,k]-Y)**2 + (Z[i,j,k]-Z)**2 )
            RRinv[np.isnan(RRinv)] = 0
            RRinv[np.isinf(RRinv)] = 0
            Gravx = (X[i,j,k]-X) * RRinv * RRinv**2 * DV * Matter
            Gravy = (Y[i,j,k]-Y) * RRinv * RRinv**2 * DV * Matter
            Gravz = (Z[i,j,k]-Z) * RRinv * RRinv**2 * DV * Matter
            Sumx[i,j,k] = np.sum(Gravx)
            Sumy[i,j,k] = np.sum(Gravy)
            Sumz[i,j,k] = np.sum(Gravz)

            # Progress
            step += 1
            print str(100*step/N**3) + "%",
            sys.stdout.write('\r')
            sys.stdout.flush()


Gravity = np.sqrt( Sumx**2 + Sumy**2 + Sumz**2 )

# Gravity at Z=0
hf = plt.figure(1)
ha1 = hf.add_subplot(111, projection='3d')
z0 = (N-1)/2
ha1.plot_surface(X[z0,:,:],Y[z0,:,:],Gravity[z0,:,:])


# 3D Scatter
XX =       X.flatten()[np.linspace(0,N**3-1,1001).astype(int)]
YY =       Y.flatten()[np.linspace(0,N**3-1,1001).astype(int)]
ZZ =       Z.flatten()[np.linspace(0,N**3-1,1001).astype(int)]
CC = Gravity.flatten()[np.linspace(0,N**3-1,1001).astype(int)]
ha2 = hf.add_subplot(111, projection='3d')
ha2.scatter(XX,YY,ZZ,c=CC)
