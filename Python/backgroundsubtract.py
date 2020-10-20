from __future__ import print_function
import cv2 as cv
import argparse
parser = argparse.ArgumentParser(description='This program shows how to use background subtraction methods provided by \
                                              OpenCV. You can process both videos and images.')
parser.add_argument('--input', type=str, help='Path to a video or a sequence of image.', default='vtest.avi')
parser.add_argument('--algo', type=str, help='Background subtraction method (KNN, MOG2).', default='MOG2')
args = parser.parse_args()
if args.algo == 'MOG2':
    backSub = cv.createBackgroundSubtractorMOG2()
else:
    backSub = cv.createBackgroundSubtractorKNN()
capture = cv.VideoCapture(cv.samples.findFileOrKeep(args.input))
if not capture.isOpened:
    print('Unable to open: ' + args.input)
    exit(0)

composite = None
i = 0
while True:
    ret, frame = capture.read()
    if frame is None:
        break
    
    fgMask = backSub.apply(frame)
    fgMask[fgMask > 0] = 255
    
    masked = cv.bitwise_and(frame, frame, mask = fgMask)

    # only apply every tenth frame
    if i % 10 != 0:
        i += 1
        continue
    # allow algo to warmup
    if i <= 40:
        i += 1
        continue
    
    if composite is None:
        addMask = fgMask
        addFrame = cv.bitwise_and(frame, frame, mask=addMask)
        composite = addFrame
        composite_mask = fgMask
    else:
#         addMask = cv.bitwise_and(fgMask, cv.bitwise_not(composite_mask))
        addMask = fgMask
        addFrame = cv.bitwise_and(frame, frame, mask=addMask)

        # remove newly added from composite
        composite_mask = cv.bitwise_and(composite_mask, cv.bitwise_not(fgMask))
        composite = cv.bitwise_and(composite, composite, mask=composite_mask)
#         composite = cv.bitwise_or(addFrame, composite)

        # add newly added
        composite = addFrame + composite
        composite_mask = cv.bitwise_or(addMask, composite_mask)

    lastframe = frame

    
    cv.rectangle(frame, (10, 2), (100,20), (255,255,255), -1)
    cv.putText(frame, str(capture.get(cv.CAP_PROP_POS_FRAMES)), (15, 15),
               cv.FONT_HERSHEY_SIMPLEX, 0.5 , (0,0,0))
    
    
#     cv.imshow('Frame', frame)
    cv.imshow('FG Mask', addMask)
    cv.imshow('composite', composite_mask)
#     cv.imshow('FG Mask', composite_mask)
    
    keyboard = cv.waitKey(30)
    if keyboard == 'q' or keyboard == 27:
        break
    i += 1

# add missing info from background
addMask = cv.bitwise_not(composite_mask)
addFrame = cv.bitwise_and(lastframe, lastframe, mask=addMask)
composite = addFrame + composite
cv.imshow('composite', composite)
keyboard = cv.waitKey(30)
