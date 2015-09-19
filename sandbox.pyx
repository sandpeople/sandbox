
import numpy as np
cimport numpy as np

cdef calculate_depth_gradient(np.ndarray* kinect_depth_image, np.ndarray* output_array):
    cdef int x,y
	for x in range(0,kinect_depth_image.shape[0]):
        for y in range(0,kinect_depth_image.shape[1]):
            pixel=255-img[x,y]
            pixel=((pixel*offset)+height)%255
            cimg[x,y]=gradient[10,pixel]
    return 
