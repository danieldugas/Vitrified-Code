from scipy.misc import imread
import numpy as np
import os

# set up plotting
import matplotlib.pyplot as plt
plt.close("all")
plt.ion()
plt.figure()

# flags
max_files = 1000
nsamples = 1

# image positioning and options
try:
    cut_x # does it exist in the current namespace
except NameError:
    cut_x = 155 # Default x-coordinate of the cut
try:
    unwrapping_delta
except NameError:
    unwrapping_delta = 100 # bound on 'allowed' intensity variation

# list image files
filenames = os.listdir("./to_analyse/")
nfiles = min(max_files,len(filenames))
if nfiles == 0:
    print "error: no files found"

# sort filenames
time_evo = np.zeros(nfiles)
for i in range(nfiles):
    filename = filenames[i]
    time_evo[i] = int( filename.rsplit('.',1)[0] )

sort_indx = time_evo.argsort()
filenames = np.array(filenames)[sort_indx]

# User input for cut_x
image = imread("to_analyse/"+filenames[0], flatten=True)
sizex,sizey = image.shape
plt.figure(1)
plt.cla()
plt.imshow(image)
print "Click desired cut position"
click = plt.ginput()
cut_x = click[0][0]
plt.axvline(cut_x, color='r')

print "Review images to be analysed. 'exit' to cancel"
print "Deleted images can be recovered in ./to_delete"
for i in range(nfiles):
    filename = filenames[i]

    # extract data from image
    image = imread("to_analyse/"+filename, flatten=True)
    sizex,sizey = image.shape
    time = int( filename.rsplit('.',1)[0] )

    # make cuts and average
    cut = np.zeros((nsamples, sizex))
    for j in range(nsamples):
        cut[j][:] = image[:, cut_x +j ]

    avgcut = np.average(cut, axis=0)

    # Unwrap
    # Parameters
    # Unwrapping
    unwrap = np.copy(avgcut)
    diff = np.diff(unwrap)
    spikes = abs(diff) > unwrapping_delta
    not_spikes = np.logical_not(spikes)
    corrective = diff
    corrective[not_spikes] = 0
    corrective = np.cumsum(corrective)
    unwrap[1:] = unwrap[1:] - corrective
    # Levelling
    levelled = unwrap
    reference = unwrap[0]
    upset = np.tile(reference,[len(levelled)])
    levelled -= upset
    # Inversing
    inversed = levelled
    wrongway = np.tile((
        inversed[0] > inversed[-1]
        ), [len(inversed)])
    inversed[wrongway] = -inversed[wrongway]
    # Scaling
#     scaled = inversed
#     scalefactor = np.tile( scaled[-1], [len(scaled)])
#     scaled = scaled / scalefactor

    final = inversed

    # display image data and cuts
    plt.figure(1)
    plt.cla()
    plt.imshow(image)
    plt.axvline(cut_x, color='r')
    plt.xlabel("time=" + str(time))
    plt.figure(2)
    plt.subplot(2,1,1)
    plt.cla()
    plt.step(range(len(avgcut)), avgcut)
    plt.subplot(2,1,2)
    plt.cla()
    plt.step(range(len(final)), final)

    # Get user input
    keys = raw_input("Delete image? (d=delete, Default=no) ")
    if keys == 'd':
        os.rename("./to_analyse/"+filename, "./to_delete/"+filename)
        print filename + " deleted."
    if keys == 'exit':
        break
    
plt.close("all")
