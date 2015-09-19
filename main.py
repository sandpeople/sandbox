#!/usr/bin/python
from freenect import sync_get_depth
import cv2.cv as cv
import cv2
import numpy as np
import frame_convert
import sys

height=80
offset=3.5

run=True
gradient=cv2.imread('gradient.bmp',1)
cv.NamedWindow('Depth')
cv2.setWindowProperty("Depth", cv2.WND_PROP_FULLSCREEN, cv.CV_WINDOW_FULLSCREEN)

def get_depth():
    """ This ufnction obtains the depth image from the kinect, if any is
        connected.
    """
    img = frame_convert.pretty_depth(sync_get_depth()[0])
    return img

cimg = cv2.cvtColor(img, cv2.COLOR_GRAY2RGB)
while run is True:
    # take kinect image if we have one:
    try:
        img = get_depth()
    except TypeError:
        # apparently, no kinect around. take static test image instead:
        img = cv2.imread('kinect.png',0)
    #print("Array type of incoming image: " + str(img.dtype))
    #print("Array type of outcoming image: " + str(cimg.dtype)


)   #print "gradient:%s img:%s cimg:%s" %(gradient.shape, img.shape, cimg.shape)
    #print "gradient[10,100]: %s wert: %s" %(gradient[10,100], img[10,100])
    for x in range(0, img.shape[0]):
        for y in range(0, img.shape[1]):
            pixel = 255 - img[x,y]
            pixel = ((pixel*offset)+height)%255
            cimg[x,y] = gradient[10, pixel]
    cv2.imshow('Depth', cimg)
    #cv.ShowImage('Depth', img)
    run=True
    if cv.WaitKey(10) == 27:
        sys.exit(0)

