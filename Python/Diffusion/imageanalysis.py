## Interferometer Image Data Extraction
## 
## Written by Daniel Dugas
## last modified 2016.01.08

## How to use this script:
## 1. Copy all measurement images into [script_parent_directory]/to_analyse
## 2. Run script
## 3. Follow command-line instructions

from scipy.misc import imread
import numpy as np
import os

# set up plotting
import matplotlib.pyplot as plt
plt.close("all")
plt.ion()

# flags
show_fourier_transform = False
enable_orthogonal_projection = False
max_files = 1000
max_plots = 10

# Physical parameters
# contrast factor ( pi (g/l mm)-1 )
try:
    eta 
except NameError:
    eta = 0.51 * np.pi
# concentration (mg/mL)
try:
    C
except NameError:
    C = 2.5
# Cell thickness (mm)
try:
    L
except NameError:
    L = 10.0
# Radians to pixel value
# 2pi [radians] = 255 [px value]
rads_by_pxval = 2.*np.pi / 255.


# list image files
filenames = os.listdir("./to_analyse/")
nfiles = min(max_files,len(filenames))
if nfiles == 0:
    print "error: no files found"

# image positioning and options
keys = raw_input("Reuse previous positioning values? ([Enter]=yes, p=no)")

image = imread("to_analyse/"+filenames[0], flatten=True)
sizex,sizey = image.shape
plt.figure(1)
plt.cla()
plt.imshow(image)

if keys == 'p':
    # User input for cut_x
    print "Click desired cut position"
    click = plt.ginput()
    cut_x = click[0][0].astype(int)
    print "cut_x = " + str(cut_x)
    plt.axvline(cut_x, color='r')
    # User input for membrane
    print "Click top of membrane"
    click = plt.ginput()
    membrane_top = click[0][1].astype(int)
    print "membrane_top = " + str(membrane_top)
    plt.axhline(membrane_top, color='k')
    print "Click bottom of membrane"
    click = plt.ginput()
    membrane_bot = click[0][1].astype(int)
    print "membrane_bot = " + str(membrane_bot)
    plt.axhline(membrane_bot, color='k')
    membrane = range(membrane_top,membrane_bot)
    opaque_xy = (cut_x,membrane_top/2+membrane_bot/2)
    # User input for scale
    print "Click left edge of cell"
    click = plt.ginput()
    cell_left = click[0][0].astype(int)
    plt.axvline(cell_left, color='k')
    print "Click right edge of cell"
    click = plt.ginput()
    cell_right = click[0][0].astype(int)
    plt.axvline(cell_right, color='k')
    cell_width_px = cell_right - cell_left
    print "cell_width_px = " + str(cell_width_px)
    cell_width_mm = 10.0
    px_by_mm = cell_width_px / cell_width_mm

try:
    crop
except NameError:
    crop = [0, 240] # vertical portion of the image that should be analysed

nsamples = 1 # number of cuts per image
try:
    unwrapping_delta
except NameError:
    unwrapping_delta = 100 # bound on 'allowed' intensity variation

# pre-allocations
cut_evo  = np.zeros((nfiles,crop[1]-crop[0]))
time_evo = np.zeros(nfiles)

for i in range(nfiles):
    filename = filenames[i]
    # extract data from image
    image = imread("to_analyse/"+filename, flatten=True)
    sizex,sizey = image.shape
    time = int( filename.rsplit('.',1)[0] )
    time_evo[i] = time

    # re-scale image value ( 0 to value of opaque pixels )
    image -= image[opaque_xy[1],opaque_xy[0]]

    # make cuts and average
    cut = np.zeros((nsamples, sizex))
    for j in range(nsamples):
        cut[j][:] = image[:, cut_x +j ]

    avgcut = np.average(cut, axis=0)

    # display image data and cuts
#     if (i < max_plots):
#         plt.subplot(max_plots,2,1+2*i)
#         plt.imshow(image)
#         plt.subplot(max_plots,2,2*(i+1))
#         plt.step(range(len(avgcut)), avgcut)

    # crop and correct cut
    finalcut = avgcut[crop[0]:crop[1]]
    cut_evo[i][crop[0]:crop[1]] = finalcut

# sort the evolutions
sort_indx = time_evo.argsort()
time_evo = time_evo[sort_indx]
cut_evo = cut_evo[sort_indx.T,:]


# frequency domain
if show_fourier_transform:
    plt.figure()
    fourier_evo = np.zeros((nfiles,120))
    for i in range(nfiles):
        cut = cut_evo[i][:]
        n = len(cut)
        frq = np.arange(n/2)
        fourier = np.fft.fft(cut)/n
        fourier = abs(fourier[range(n/2)])
        fourier_evo[i][:] = fourier
        plt.plot(frq, fourier_evo.T)



## PART 2 ##

initial_concentration = np.zeros(cut_evo[0].shape)
initial_concentration[membrane] = 0.5
initial_concentration[membrane[-1]:] = 1

# Unwrapping
unwrap = np.copy(cut_evo)
diff = np.diff(unwrap)
spikes = abs(diff) > unwrapping_delta
not_spikes = np.logical_not(spikes)
corrective = diff
corrective[not_spikes] = 0
corrective = np.cumsum(corrective, axis=1)

unwrap[:,1:] = unwrap[:,1:] - corrective

# Levelling
levelled = unwrap
upset = np.tile(unwrap[:,0] ,[len(levelled[0]),1] ).T
levelled -= upset

# Inversing
inversed = levelled
wrongway = np.tile((
    inversed[:,0] > inversed[:,-1]
    ), [len(inversed[0]),1] ).T
inversed[wrongway] = -inversed[wrongway]

# Remove Background Interferometer Distortion
undistorted = inversed
    # linear approach to distortion - not very accurate
# distortion_rate = np.tile(
#         (undistorted[:,-1] - undistorted[:,-20] )/ 20 * 0.8, 
#         [len(undistorted[0]),1] ).T
# distortion = distortion_rate * np.tile(
#         range(0-membrane[-1],len(undistorted[0])-membrane[-1]),
#         [len(undistorted[:,0]),1] )
# distortion[:,:membrane[-1]] = 0
    # measurement approach to distortion
# background = undistorted[0, :] - initial_concentration
# distortion = np.tile( background, [len(undistorted[:,0]),1] )
# undistorted -= distortion

# Scaling
scaled = undistorted
scaled = scaled * rads_by_pxval * (1 / eta / L )

# Splice Levelling
spliced = scaled
upset = np.tile( spliced[:,-1] - spliced[-1,-1], [len(scaled[0]),1] ).T
upset[:,:membrane[0]] = 0
upset[:,membrane] = upset[:,membrane]/2
spliced -= upset



final = spliced


## PART 3 ##

# visual comparison
plt.figure()
plt.subplot(2,1,1)
plt.plot(cut_evo[-5:].T)
plt.subplot(2,1,2)
plt.plot(final[-5:].T)

# 3D
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm

if enable_orthogonal_projection:
    from mpl_toolkits.mplot3d import proj3d
    def orthogonal_proj(zfront, zback):
        a = (zfront+zback)/(zfront-zback)
        b = -2*(zfront*zback)/(zfront-zback)
        return np.array([[1,0,0,0],
                            [0,1,0,0],
                            [0,0,a,b],
                            [0,0,0,zback]])
    proj3d.persp_transformation = orthogonal_proj

pixel_range = range(crop[0],crop[1])
mm_range = pixel_range / px_by_mm
X, Y = np.meshgrid(time_evo, mm_range)
hf = plt.figure()
ha = hf.add_subplot(111, projection='3d')
ha.plot_surface(X.T[:,pixel_range], Y.T[:,pixel_range], final[:,pixel_range], 
        cmap=cm.coolwarm, linewidth=0,
        cstride = 1 ,
        antialiased=True)

# Write to file
axes_and_data = np.concatenate((time_evo[:,None], final),axis=1)
axes_and_data = np.concatenate((np.append(0,mm_range)[None,:], axes_and_data),axis=0)
np.savetxt('out.txt', axes_and_data, delimiter=',')
print "Data written to out.txt"
