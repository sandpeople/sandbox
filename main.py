#!/usr/bin/python
from freenect import sync_get_depth
import cv2.cv as cv
import cv2
import numpy as np
import frame_convert
import sys
from clib_interface import call_clib_sim
import pickle
points=pickle.load(open( "cal.p", "rb" ))
print points

height=80
offset=3.5

run=True
gradient=cv2.imread('gradient.bmp',1)
cv.NamedWindow('Depth')
cv2.setWindowProperty("Depth", cv2.WND_PROP_FULLSCREEN, cv.CV_WINDOW_FULLSCREEN)

def contractions(img, points):
    global done_image
    offsetsarr=[[offset,offset], [640-offset,offset], [offset,480-offset], [640-offset,480-offset]]
    #img.shape = (480, 640)
    #img = offset_points(points)
    #print "\nimg.shape:%s\npoints:%s\noffsetsarr:%s\n" %(img.shape,points,offsetsarr)
    y,x,c = img.shape
    #y -= offset
    #x -= offset
    pts1 = np.float32(points)
    pts2 = np.float32(offsetsarr)
    M = cv2.getPerspectiveTransform(pts1,pts2)
    dst = cv2.warpPerspective(img,M,(x,y))
    return dst

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
    #contractions(img, points)
    call_clib_sim(img, cimg)
    contractions(cimg, points)
    # show resulting image:
    cv2.imshow('Depth', cimg)
   
    # quit if escape is pressed: 
    if cv.WaitKey(10) == ord('1'):
        print "lala 1"

    if cv.WaitKey(10) == 27:
        sys.exit(0)

