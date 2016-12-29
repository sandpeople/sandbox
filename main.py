#!/usr/bin/python
from freenect import sync_get_depth
import ConfigParser as configparser
import cv2 
import numpy as np
import frame_convert
import sys
import clib_interface
import server
import os
import pickle
import threading
from Queue import Queue
if not os.path.exists("cal.p"):
    os.system("cp cal.p.default cal.p")
if not os.path.exists("config.ini"):
    os.system("cp config.ini.default config.ini")
points=pickle.load(open( "cal.p", "rb" ))
print points

enable_http=True

pqueue=Queue(maxsize=1)

if enable_http:
    server=server.sandcontrol(queue=pqueue)
    serverd=threading.Thread(target = server.launch_control, args=(pqueue,))
    serverd.daemon = True
    serverd.start()

# Various options:
parser = configparser.ConfigParser()
parser.read("config.ini")
try:
    height_shift = float(parser.get("main", "height_shift"))
except configparser.NoOptionError:
    height_shift = 0
height_scale = float(parser.get("main", "height_scale"))
offset=3.5
screen_resolution_x = int(parser.get("main", "screen_resolution_x"))
screen_resolution_y = int(parser.get("main", "screen_resolution_y"))
clib_interface.set_height_config(height_shift, height_scale)

# Compute proper fullscreen constants for openCV version:
fullscreen_const = None
winnormal_const = None
try:
    fullscreen_const = cv2.cv.CV_WINDOW_FULLSCREEN
    winnormal_const = cv2.cv.CV_WINDOW_NORMAL
except AttributeError:
    fullscreen_const = cv2.WINDOW_FULLSCREEN
    winnormal_const = cv2.WINDOW_NORMAL

# Handle mouse events:
calibration = False
mouse_dragging = False
mouse_drag_start = None
mouse_drag_reported = None
def mouse_handling(event, x, y, flags, param):
    global mouse_drag_reported
    global mouse_drag_start
    global mouse_dragging
    global calibration
    if event == cv2.EVENT_LBUTTONDOWN:
        mouse_dragging = True
        mouse_drag_start = (x, y)
        mouse_drag_reported = (0, 0)
    elif (event == cv2.EVENT_MOUSEMOVE or \
            event == cv2.EVENT_LBUTTONUP) and mouse_dragging:
        if event == cv2.EVENT_LBUTTONUP:
            mouse_dragging = False
        mouse_drag_vector = (x - mouse_drag_start[0],
            y - mouse_drag_start[1])
        mouse_drag_report_diff = (
            mouse_drag_vector[0] - mouse_drag_reported[0],
            mouse_drag_vector[1] - mouse_drag_reported[1]);
        mouse_drag_reported = mouse_drag_vector
        if mouse_drag_report_diff[0] != 0 or \
                mouse_drag_report_diff[1] != 0:
            if calibration:
                clib_interface.drag_map(
                    -float(mouse_drag_report_diff[0] * 0.05),
                    -float(mouse_drag_report_diff[1] * 0.05))

run=True
gradient=cv2.imread('gradient.bmp',1)
cv2.namedWindow('Beamer Image', cv2.WINDOW_NORMAL)
cv2.setWindowProperty('Beamer Image', cv2.WND_PROP_FULLSCREEN,
    fullscreen_const)
cv2.setMouseCallback('Beamer Image', mouse_handling)
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
    # Take kinect image if we have one:
    if not no_kinect:
        img = get_depth()
    else:
        # Apparently, no kinect around. take static test image instead:
        img = cv2.imread('images/kinect.png', 0)
    return img

# Create cimg buffer in according format:
img = get_image()
cimg = cv2.cvtColor(img, cv2.COLOR_GRAY2RGB)

while run is True:
    # Take kinect image if we have one:
    img = get_image()

    # Call C code for simulation:
    #contractions(img, points)
    clib_interface.simulate(img, cimg)
    contractions(cimg, points)

    # Show resulting image:
    resized = cv2.resize(cimg, (screen_resolution_x, screen_resolution_y), interpolation = cv2.INTER_AREA)
    if not pqueue.full():
        try:
            pqueue.put(resized, block=False)
        except:
            pass
    cv2.imwrite('webroot/map.jpg', resized, [int(cv2.IMWRITE_JPEG_QUALITY), 10])
    cv2.imshow('Beamer Image', resized)
   
    key = cv2.waitKey(10)

    if key == 27:
        # Quit if escape is pressed:
        sys.exit(0)
    elif key == 65480 or key == 102:
        # Toggle fullscreen:
        if fullscreen:
            fullscreen = False
            cv2.destroyAllWindows()
            cv2.namedWindow('Beamer Image', cv2.WINDOW_AUTOSIZE)
            cv2.setWindowProperty("Beamer Image", cv2.WND_PROP_FULLSCREEN,
                fullscreen_const)
            cv2.setMouseCallback('Beamer Image', mouse_handling)
        else:
            fullscreen = True
            cv2.destroyAllWindows()
            cv2.namedWindow('Beamer Image', winnormal_const)
            cv2.setWindowProperty("Beamer Image", cv2.WND_PROP_FULLSCREEN,
                fullscreen_const)
            cv2.setMouseCallback('Beamer Image', mouse_handling)
    elif key == 99:
        if not calibration:
            calibration = True
            print("CALIBRATION <<ON>>")
        else:
            calibration = False
            print("CALIBRATION <<OFF>>")
    elif key >= 0:
        print("UNKNOWN KEY: " + str(key))
    

