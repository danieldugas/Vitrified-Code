# Neural Network Exercise
# Python 2.7
# Daniel Dugas

import math
import random
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
np.seterr(all = 'ignore')
plt.ion() # plots don't hang execution

plt.close("all")

# Create a data set
# N points, of positions X and Y and color C.
N = 10
X = np.random.rand(N)
Y = np.random.rand(N)
C = (np.random.rand(N)>=0.5).astype(int)
X = X #+ 0.5*C
Y = Y #+ 0.5*C


fig1 = plt.figure()
plt.subplot(2,7,1)
plt.scatter(X, Y, s=20, c=C, cmap=cm.Greys)


# sigmoid transfer function
# IMPORTANT: when using the logit (sigmoid) transfer function for the output layer make sure y values are scaled from 0 to 1
# if you use the tanh for the output then you should scale between -1 and 1
# we will use sigmoid for the output layer and tanh for the hidden layer
def sigmoid(x):
    return 1 / (1 + np.exp(-x))

# derivative of sigmoid
def dsigmoid(y):
    return y * (1.0 - y)


# parameters for the neural network
# the three layers are i, j, k, respectively the input, hidden, and output layers.
isize = 2
jsize = 20
ksize = 1

jWeights = np.random.randn(jsize,isize) * 2 - 1
kWeights = np.random.randn(ksize,jsize) * 2 - 1

jBiases = np.zeros(jsize)
kBiases = np.zeros(ksize)


# verification
resolution = 100
visual_learning = np.zeros((resolution, resolution))
for ix in range(0,resolution):
        for iy in range(0,resolution):
                inputs = np.array([ix*1.0/resolution,iy*1.0/resolution])
                iOut = inputs 
                jIn = jWeights.dot(iOut) + jBiases
                jOut = sigmoid(jIn) 
                kIn = kWeights.dot(jOut) + kBiases
                kOut = sigmoid(kIn) 
                outputs = kOut
                visual_learning[ix][iy] = outputs
plt.subplot(2,7,2)
plt.pcolor(visual_learning.T, cmap=cm.Greys)

figure_number = 2
# iteration
passes = 10000
for s in range(0, passes):
        pass_error = 0
        kWeightChange = 0
        jWeightChange = 0
        kBiasChange = 0
        jBiasChange = 0
        for n in range(0, N):
                inputs = np.array([X[n],Y[n]])
                # forward
                iOut = inputs

                jIn = jWeights.dot(iOut) + jBiases
                jOut = sigmoid(jIn)

                kIn = kWeights.dot(jOut) + kBiases
                kOut = sigmoid(kIn)

                outputs = kOut

                # backward
                error = (outputs[0]-C[n])**2
                pass_error = pass_error + error
                
                if s % (passes/10) == 0:
                    print 'x={0:.1f} y={1:.1f} : {2} {3} -> {4}'.format(
                        X[n], Y[n], outputs[0], C[n], error)

                kDelta = kOut * (1 - kOut) * (kOut - C[n])

                jDelta = jOut * (1 - jOut) * (kDelta.dot(kWeights))

                eta = 10
                kWeightChange += -eta * np.outer(kDelta, jOut) / N
                jWeightChange += -eta * np.outer(jDelta, iOut) / N

                kBiasChange += -eta * kDelta / N
                jBiasChange += -eta * jDelta / N

        # Update Weights
        kWeights = kWeights + kWeightChange
        jWeights = jWeights + jWeightChange

        kBiases = kBiases + kBiasChange
        jBiases = jBiases + jBiasChange

        if s % (passes/10) == 0:
                print pass_error
                print jWeights
                figure_number += 1
                # verification
                resolution = 100
                visual_learning = np.zeros((resolution, resolution))
                for ix in range(0,resolution):
                        for iy in range(0,resolution):
                                inputs = np.array([ix*1.0/resolution,iy*1.0/resolution])
                                iOut = inputs 
                                jIn = jWeights.dot(iOut) + jBiases
                                jOut = sigmoid(jIn) 
                                kIn = kWeights.dot(jOut) + kBiases
                                kOut = sigmoid(kIn) 
                                outputs = kOut
                                visual_learning[ix][iy] = outputs
                plt.subplot(2,7,figure_number)
                plt.pcolor(visual_learning.T, cmap=cm.Greys)
