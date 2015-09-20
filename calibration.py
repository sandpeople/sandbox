import cv2
import cv2.cv as cv
from freenect import sync_get_depth
import frame_convert
import numpy as np
from skimage import exposure
 

# initialize the list of reference points and boolean indicating
# whether cropping is being performed or not
reference_points = []
cropping = False
done_image=None
fontFace = cv2.FONT_HERSHEY_SCRIPT_SIMPLEX;
fontScale = 2;
thickness = 3;
baseline=0;
textSize = cv2.getTextSize("1", fontFace,fontScale, thickness);
baseline += thickness;

offset=100
offsets=((offset,offset), (640-offset,offset), (offset,480-offset), (640-offset,480-offset))

def weirdyze_ratio(points):
    for i in range(0,3):
        point=[]
        point.append(points[i][1])
        point.append(points[i][0])
        points[i]=point
    return points

def contractions(img, points):
    global done_image
    #img.shape = (480, 640)
    x,y = img.shape

    pts1 = np.float32([points[0],points[1],points[2],points[3]])
    pts2 = np.float32([[0,0],[0,y],[x,0],[x, y]])
    M = cv2.getPerspectiveTransform(pts1,pts2)
    dst = cv2.warpPerspective(img,M,(y,x))
    done_image=dst

def get_depth():
    return frame_convert.pretty_depth(sync_get_depth()[0])

def get_image():
    image = get_depth()
    N = 4
    arr = np.array(image,dtype=np.float) / N
    for i in range(1,N):
        image=get_depth() # - img_avgi
        imarr=np.array(image,dtype=np.float)
        arr=arr+imarr/N
    # Round values in array and cast as 8-bit integer
    arr=np.array(np.round(arr),dtype=np.uint8)
    img=exposure.rescale_intensity(arr, in_range=(160,190))
    return img

done=False
points=None
def click_and_crop(event, x, y, flags, param):
    # grab references to the global variables
    global reference_points, cropping, done, points
 
    # if the left mouse button was clicked, record the starting
    # (x, y) coordinates and indicate that cropping is being
    # performed
    if event == cv2.EVENT_LBUTTONDOWN:
        if cropping == False:
            reference_points.append((x, y))
        else:
            reference_points.append((x, y))
            done=True
 
    # check to see if the left mouse button was released
    elif event == cv2.EVENT_LBUTTONUP:
        # record the ending (x, y) coordinates and indicate that
        # the cropping operation is finished
        if cropping == False:
            reference_points.append((x, y))
        cropping = True
 
        # draw a rectangle around the region of interest
        #cv2.rectangle(image, reference_points[0], reference_points[1], (0, 255, 0), 2)
        cv2.line(image,reference_points[0],reference_points[1],(255,0,0),5)
        if done == True:
            reference_points.append((x, y))
            points = reference_points
            contractions(image, weirdyze_ratio(points))
            done=False
            cropping=False
        cv2.imshow("image", get_image()) 

image=get_image()
cv2.namedWindow("image")
cv2.setMouseCallback("image", click_and_crop)
 
# keep looping until the 'q' key is pressed
while True:
    # display the image and wait for a keypress
    if isinstance(done_image, np.ndarray):
        break
    image=get_image()
    if len(reference_points) >= 2:
        cv2.line(image,reference_points[0],reference_points[1],(255,0,0),5)
    if len(reference_points) >= 4:
        cv2.line(image,reference_points[2],reference_points[3],(255,0,0),5)
    cv2.circle(image,offsets[0], 10, (230), -1)
    cv2.putText(image, "1", offsets[0], fontFace, fontScale,(offset,offset), thickness, 8);
    cv2.circle(image,offsets[1], 10, (230), -1)
    cv2.putText(image, "2", offsets[1], fontFace, fontScale,(offset,offset), thickness, 8);
    cv2.circle(image,offsets[2], 10, (230), -1)
    cv2.putText(image, "3", offsets[2], fontFace, fontScale,(offset,offset), thickness, 8);
    cv2.circle(image,offsets[3], 10, (230), -1)
    cv2.putText(image, "4", offsets[3], fontFace, fontScale,(offset,offset), thickness, 8);
    cv2.imshow("image", image)
    key = cv2.waitKey(1) & 0xFF   
 
    # if the 'c' key is pressed, break from the loop
    if key == ord("c"):
        break

if len(points) == 4:
    while 1:
        cv2.imshow("image", done_image)
        cv.WaitKey(10)
        contractions(get_image(), points)
 
# close all open windows
cv2.destroyAllWindows()
