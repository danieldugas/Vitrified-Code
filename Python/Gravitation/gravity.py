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

N = 20

x = np.linspace(-2.,2.,N)
y = np.linspace(-2.,2.,N)
z = np.linspace(-2.,2.,N)

[X,Y,Z] = np.meshgrid(x,y,z)

R = np.sqrt(X**2 + Y**2 + Z**2)
Matter = np.logical_and(  R > (re-th) , R < (re) ).astype(float)

Sumx = Matter * 0.
Sumy = Matter * 0.
Sumz = Matter * 0.

for i in range(N):
    for j in range(N):
        for k in range(N):
            RRinv = 1 / np.sqrt( (x[i]-X)**2 + (y[j]-Y)**2 + (z[k]-Z)**2 )
            RRinv[np.isnan(RRinv)] = 0
            RRinv[np.isinf(RRinv)] = 0
            Gravx = (x[i]-X) * RRinv * RRinv**2
            Gravy = (y[j]-Y) * RRinv * RRinv**2
            Gravz = (z[k]-Z) * RRinv * RRinv**2
            Sumx[i,j,k] = np.sum(Gravx)
            Sumy[i,j,k] = np.sum(Gravy)
            Sumz[i,j,k] = np.sum(Gravz)


Gravity = np.sqrt( Sumx**2 + Sumy**2 + Sumz**2 )

XX, YY = np.meshgrid(x, y)
hf = plt.figure()
for i in range(1,11):
    ha = hf.add_subplot(340+i, projection='3d')
    ha.plot_surface(XX, YY, Gravity[:,:,2*(i-1)], 
            cmap=cm.coolwarm, linewidth=0,
            cstride = 1 ,
            antialiased=False)
    ha.contour(XX, YY, Gravity[:,:,i], 
            cmap=cm.coolwarm, linewidth=0,
            cstride = 1 ,
            antialiased=False)
