#!/usr/bin/python


# sandbox
# Copyright (C) 2016    CCCFr Sandbox Team
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

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

enable_http=True

pqueue=Queue(maxsize=1)

if enable_http:
    server=server.sandcontrol(queue=pqueue)
    serverd=threading.Thread(target = server.launch_control, args=(pqueue,))
    serverd.daemon = True
    serverd.start()

height_shift = 0
height_scale = 1
screen_resolution_x = 1920
screen_resolution_y = 1080
map_offset_x = 0.0
map_offset_y = 0.0
map_zoom = 1.0

clib_interface.set_height_config(height_shift, height_scale)
clib_interface.reset_map_drag()
clib_interface.drag_map(map_offset_x, map_offset_y)

# Compute proper fullscreen constants for openCV version:
fullscreen_const = None
winnormal_const = None
try:
    fullscreen_const = cv2.cv.CV_WINDOW_FULLSCREEN
    winnormal_const = cv2.cv.CV_WINDOW_NORMAL
except AttributeError:
    fullscreen_const = cv2.WINDOW_FULLSCREEN
    winnormal_const = cv2.WINDOW_NORMAL


run=True
gradient=cv2.imread('gradient.bmp',1)
cv2.namedWindow('Beamer Image', cv2.WINDOW_NORMAL)
cv2.setWindowProperty('Beamer Image', cv2.WND_PROP_FULLSCREEN,
    fullscreen_const)
fullscreen = True

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
    clib_interface.simulate(img, cimg)

    # Ensure right resolution:
    resized = cv2.resize(cimg, (screen_resolution_x, screen_resolution_y), interpolation = cv2.INTER_AREA)

    if not pqueue.full():
        try:
            pqueue.put(resized, block=False)
        except:
            pass
    cv2.imwrite('webroot/map.jpg', resized, [int(cv2.IMWRITE_JPEG_QUALITY), 10])
    cv2.imshow('Beamer Image', resized)
   
    key = (cv2.waitKey(10) % 256)
    if key == 27: # Escape (Quit)
        sys.exit(0)
    elif key == 65480 or key == 102: # F11 / F (toggle fullscreen)
        # Toggle fullscreen:
        if fullscreen:
            fullscreen = False
            cv2.destroyAllWindows()
            cv2.namedWindow('Beamer Image', cv2.WINDOW_AUTOSIZE)
            cv2.setWindowProperty("Beamer Image", cv2.WND_PROP_FULLSCREEN,
                fullscreen_const)
        else:
            fullscreen = True
            cv2.destroyAllWindows()
            cv2.namedWindow('Beamer Image', winnormal_const)
            cv2.setWindowProperty("Beamer Image", cv2.WND_PROP_FULLSCREEN,
                fullscreen_const)
    elif key > 0 and key < 255:
        print("UNKNOWN KEY: " + str(key))
    

