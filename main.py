#!/usr/bin/python
from freenect import sync_get_depth
import cv2.cv as cv
import cv2
import numpy as np
import frame_convert

height=80
offset=3.5

run=True
gradient=cv2.imread('gradient.bmp',1)
cv.NamedWindow('Depth')

def get_depth():
    img=frame_convert.pretty_depth(sync_get_depth()[0])
    return img

while run is True:
    img = get_depth() #cv2.imread('kinect.png',0) #get_depth()
    cimg = cv2.cvtColor(img,cv2.COLOR_GRAY2RGB)
    #print "gradient:%s img:%s cimg:%s" %(gradient.shape, img.shape, cimg.shape)
    #print "gradient[10,100]: %s wert: %s" %(gradient[10,100], img[10,100])
    for x in range(0,img.shape[0]):
        for y in range(0,img.shape[1]):
            pixel=255-img[x,y]
            pixel=((pixel*offset)+height)%255
            cimg[x,y]=gradient[10,pixel]
    cv2.imshow('Depth', cimg)
    #cv.ShowImage('Depth', img)
    run=True
    if cv.WaitKey(10) == 27:
        sys.exit(0)

