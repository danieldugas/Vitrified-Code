import os

# flags
max_files = 1000

# list image files
filenames = os.listdir("./to_rename/")
nfiles = min(max_files,len(filenames))
if nfiles == 0:
    print "error: no files found"

    # Get user input
print "Which rule would you like to apply to the files"
print "1. Insert text"
print ""
mode = raw_input("your selection: ")
text = raw_input("text to insert: ")
for i in range(nfiles):
    filename = filenames[i]

    # separate file name and extension
    [name,ext] = filename.rsplit('.',1) 

    newfilename = name+text+'.'+ext

    os.rename("./to_rename/"+filename, "./to_rename/"+newfilename)
    print filename + " -> " + newfilename
