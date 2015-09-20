from freenect import sync_get_depth
import sys
# sys.path.append('~/libfreenect/wrappers/python/')

import cv2
import cv2.cv as cv
import frame_convert
import numpy as np

def get_depth():
    return frame_convert.pretty_depth(sync_get_depth()[0])

def manip_img(img):
    img=img*2
    return img

cv.NamedWindow('debug')
cv2.setWindowProperty("debug", cv2.WND_PROP_FULLSCREEN, cv.CV_WINDOW_FULLSCREEN)
img=get_depth()
img=manip_img(img)
img_avg=img
for i in range(0,20):
    img=get_depth()
    img=manip_img(img)
    img_avg=img_avg+img
img_avg = img_avg / 20

while 1:
    img_tmp=get_depth() - img_avg
    for i in range(0,5):
        img = get_depth() - img_avg  #cv2.imread('kinect.png',0)  #get_depth()
        img = manip_img(img)
        img_tmp=img+img_tmp
    img=img_tmp/5
    #img = cv2.medianBlur(img,5)
    #print img.dtype
    #img = cv2.Laplacian(img,cv2.CV_8S)
    #print img.dtype
    #gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    
    #cv2.equalizeHist(img,img)
    #circles = None
    circles = cv2.HoughCircles(img,cv.CV_HOUGH_GRADIENT,1,20,param1=50,param2=30,minRadius=1,maxRadius=100)
    #cv2.imwrite('kinect.png',img)
    #print("frame")

    if circles is None:
        cv2.imshow('debug',img)
        #cv2.destroyAllWindows()
    else:
        circles = np.uint16(np.around(circles))
        for i in circles[0,:]:
            # draw the outer circle
            cv2.circle(img,(i[0],i[1]),i[2],(0,255,0),2)
            # draw the center of the circle
            cv2.circle(img,(i[0],i[1]),2,(0,0,255),3)
        cv2.imshow('debug',img)
    if cv.WaitKey(1) == 120:
        break
