#!/usr/bin/python
from freenect import sync_get_depth
import cv2.cv as cv
import cv2
import numpy as np
import frame_convert

run=True
gradient=cv2.imread('gradient.bmp',1)
cv.NamedWindow('Depth')

def get_depth():
    img=frame_convert.pretty_depth(sync_get_depth()[0])
    return img

while run is True:
    img = cv2.imread('kinect.png',0) #get_depth()
    cimg = cv2.cvtColor(img,cv2.COLOR_GRAY2RGB)
    print "gradient:%s img:%s cimg:%s" %(gradient.shape, img.shape, cimg.shape)
    print "gradient[10,100]: %s wert: %s" %(gradient[10,100], img[10,100])
    for x in range(0,img.shape[0]):
        for y in range(0,img.shape[1]):
            pass
            cimg[x,y]=gradient[10,256-img[x,y]]
    cv2.imshow('Depth', cimg)
    #cv.ShowImage('Depth', img)
    run=True
    if cv.WaitKey(10) == 27:
        break
