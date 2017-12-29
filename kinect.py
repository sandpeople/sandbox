#/usr/bin/pyhton

from freenect import sync_get_depth
import frame_convert
import sys,os
import numpy as np
import httplib
from PIL import Image

host="127.0.0.1"
port="8080"

def get_depth():
    """ This function obtains the depth image from the kinect, if any is
        connected.
    """
    img = frame_convert.pretty_depth(sync_get_depth()[0])
    return img

def get_image():
    global no_kinect
    # Take kinect image if we have one:
    if not no_kinect:
        img = get_depth()
    else:
        # Apparently, no kinect around. take static test image instead:
        img = Image.open('images/kinect.png')
    return img

def connect():
    # init connection to control
    headers = {"Content-type": "json", "state": "init", "type": "kinect" }
    h = httplib.HTTPConnection(host, port)
    print "sending init packet"
    h.request("GET", "/client/", None, headers)
    print "sent"
    r = h.getresponse()
    headers["tmp_token"]=r.read()
    h.request("GET", "/client/", None, headers)
    r = h.getresponse()
    print r.read()

# check if we have a kinect:
no_kinect = False
try:
    img = get_depth()
except TypeError:
    # nope, no kinect.
    no_kinect = True

img = get_image()
run=True

connect()

while run is True:
    # Take kinect image if we have one:
    img = get_image()

    img.save("webroot/k1.jpg", "JPEG", quality=80, optimize=True, progressive=True)


