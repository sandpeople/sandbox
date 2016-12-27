#!/usr/bin/python
from freenect import sync_get_depth
import cv2 as cv
import cv2
import numpy as np
import frame_convert
import sys
import clib_interface
import server
import pickle
import threading
from Queue import Queue
points=pickle.load(open( "cal.p", "rb" ))
print points

webapi_queue=Queue()
server=server.sandcontrol()
serverd=threading.Thread(target = server.launch_control)
serverd.daemon = True
serverd.start()

height=80
offset=3.5

screen_resolution_x = 1280
screen_resolution_y = 1024

run=True
gradient=cv2.imread('gradient.bmp',1)
cv2.namedWindow('Depth', cv2.WINDOW_NORMAL)
cv2.setWindowProperty("Depth", cv2.WND_PROP_FULLSCREEN,
    cv2.WINDOW_FULLSCREEN)
fullscreen = True

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
    # Take kinect image if we have one:
    img = get_image()

    # Call C code for simulation:
    #contractions(img, points)
    clib_interface.sim(img, cimg)
    contractions(cimg, points)

    # Show resulting image:
    resized = cv2.resize(cimg, (screen_resolution_x, screen_resolution_y), interpolation = cv2.INTER_AREA)
    cv2.imshow('Depth', resized)
   
    key = cv.waitKey(10)

    if key == 27:
        # Quit if escape is pressed:
        sys.exit(0)
    elif key == 65480 or key == 102:
        # Toggle fullscreen:
        if fullscreen:
            fullscreen = False
            cv2.destroyAllWindows()
            cv2.namedWindow('Depth', cv2.WINDOW_AUTOSIZE)
            cv2.setWindowProperty("Depth", cv2.WND_PROP_FULLSCREEN,
                cv.WINDOW_FULLSCREEN)
        else:
            fullscreen = True
            cv2.destroyAllWindows()
            cv2.namedWindow('Depth', cv2.WINDOW_NORMAL)
            cv2.setWindowProperty("Depth", cv2.WND_PROP_FULLSCREEN,
                cv.WINDOW_FULLSCREEN)
    elif key >= 0:
        print("UNKNOWN KEY: " + str(key))
    

