#!/usr/bin/python
from freenect import sync_get_depth
import cv2.cv as cv
import cv2
import numpy as np
import frame_convert
import sys
from clib_interface import call_clib_sim

height=80
offset=3.5

run=True
gradient=cv2.imread('gradient.bmp',1)
cv.NamedWindow('Depth')
cv2.setWindowProperty("Depth", cv2.WND_PROP_FULLSCREEN, cv.CV_WINDOW_FULLSCREEN)

def get_depth():
    """ This function obtains the depth image from the kinect, if any is
        connected.
    """
    img = frame_convert.pretty_depth(sync_get_depth()[0])
    return img

# check if we have a kinect:
no_kinect = False
try:
    img = get_depth()
except TypeError:
    # nope, no kinect.
    no_kinect = True

def get_image():
    global no_kinect
    # take kinect image if we have one:
    if not no_kinect:
        img = get_depth()
    else:
        # apparently, no kinect around. take static test image instead:
        img = cv2.imread('images/kinect.png',0)
    return img

# create cimg buffer in according format:
img = get_image()
cimg = cv2.cvtColor(img, cv2.COLOR_GRAY2RGB)

while run is True:
    # take kinect image if we have one:
    img = get_image()

    # call C code for simulation:
    call_clib_sim(img, cimg)

    # show resulting image:
    cv2.imshow('Depth', cimg)
   
    # quit if escape is pressed: 
    if cv.WaitKey(10) == 27:
        sys.exit(0)

