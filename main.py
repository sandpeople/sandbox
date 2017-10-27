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
try:
    map_offset_x = float(parser.get("main", "map_offset_x"))
except configparser.NoOptionError:
    map_offset_x = 0.0
try:
    map_offset_y = float(parser.get("main", "map_offset_y"))
except configparser.NoOptionError:
    map_offset_y = 0.0
try:
    map_zoom = float(parser.get("main", "map_zoom"))
    if map_zoom < 0.00001:
        map_zoom = 1.0
except configparser.NoOptionError:
    map_zoom = 1.0
clib_interface.set_height_config(height_shift, height_scale)
clib_interface.reset_map_drag()
clib_interface.drag_map(map_offset_x, map_offset_y)

def rewrite_config():
    with open("config.ini", "w") as f:
        writer = configparser.ConfigParser()
        writer.add_section("main")
        writer.set("main", "height_shift", height_shift)
        writer.set("main", "height_scale", height_scale)
        writer.set("main", "screen_resolution_x", screen_resolution_x)
        writer.set("main", "screen_resolution_y", screen_resolution_y)
        writer.set("main", "map_offset_x", map_offset_x)
        writer.set("main", "map_offset_y", map_offset_y)
        writer.set("main", "map_zoom", map_zoom)
        writer.write(f)

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
calibration_drag = False
calibration_zoom = False
mouse_dragging = False
mouse_drag_start = None
mouse_drag_reported = None
def mouse_handling(event, x, y, flags, param):
    global mouse_drag_reported
    global mouse_drag_start
    global mouse_dragging
    global calibration_drag
    global map_offset_x
    global map_offset_y
    global map_zoom
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
            if calibration_drag:
                map_offset_x += -float(mouse_drag_report_diff[0] * 0.05)
                map_offset_y += -float(mouse_drag_report_diff[1] * 0.05)
                rewrite_config()
                clib_interface.reset_map_drag()
                clib_interface.drag_map(map_offset_x, map_offset_y)
            elif calibration_zoom:
                drag_dist = \
                    mouse_drag_report_diff[1]
                map_zoom += drag_dist * 0.0001
                clib_interface.set_map_zoom(map_zoom)
                rewrite_config()

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
            cv2.setMouseCallback('Beamer Image', mouse_handling)
        else:
            fullscreen = True
            cv2.destroyAllWindows()
            cv2.namedWindow('Beamer Image', winnormal_const)
            cv2.setWindowProperty("Beamer Image", cv2.WND_PROP_FULLSCREEN,
                fullscreen_const)
            cv2.setMouseCallback('Beamer Image', mouse_handling)
    elif key == 119: # W (reset water)
        print("RESET WATER")
        clib_interface.reset_water() 
    elif key == 99: # C (calibration with dragging)
        if not calibration_drag:
            calibration_zoom = False
            calibration_drag = True
            print("CALIBRATION <<ON>>")
        else:
            calibration_drag = False
            print("CALIBRATION <<OFF>>")
    elif key == 120: # X (calibration with zooming)
        if not calibration_zoom:
            calibration_zoom = True
            calibration_drag = False
            print("ZOOM CALIBRATION <<ON>>")
        else:
            calibration_zoom = False
            print("ZOOM CALIBRATION <<OFF>>")
    elif key > 0 and key < 255:
        print("UNKNOWN KEY: " + str(key))
    

