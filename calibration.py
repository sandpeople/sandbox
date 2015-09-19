from freenect import sync_get_depth
import sys
sys.path.append('~/libfreenect/wrappers/python/')

import cv2
import cv2.cv as cv
import frame_convert
import numpy as np

def get_depth():
    return frame_convert.pretty_depth(sync_get_depth()[0])

while 1:
    img = get_depth() #cv2.imread('kinect.png',0)  #get_depth()
    #img = cv2.medianBlur(img,5)
    #gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    circles = cv2.HoughCircles(img,cv.CV_HOUGH_GRADIENT,1,20,param1=50,param2=30,minRadius=0,maxRadius=200)
    #cv2.imwrite('kinect.png',img)

    if circles is None:
        cv2.imshow('no detected circles',img)
        #cv2.destroyAllWindows()
        continue

    circles = np.uint16(np.around(circles))
    for i in circles[0,:]:
        # draw the outer circle
        cv2.circle(img,(i[0],i[1]),i[2],(0,255,0),2)
        # draw the center of the circle
        cv2.circle(img,(i[0],i[1]),2,(0,0,255),3)

    cv2.imshow('detected circles',img)
    if cv.WaitKey(500) == 120:
        break
