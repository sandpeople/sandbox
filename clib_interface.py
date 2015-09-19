
import ctypes

def call_clib_sim(input_depth_image, output_color_image):
    #indata = numpy.ones((5,6), dtype=numpy.double)
    #outdata = numpy.zeros((5,6), dtype=numpy.double)
    lib = ctypes.cdll.LoadLibrary('./clib.so')
    run_simulation = lib.run_simulation
    
    # call simulation:
    run_simulation(ctypes.c_void_p(input_depth_image.ctypes.data),
        ctypes.c_int(input_depth_image.shape[0]),
        ctypes.c_int(input_depth_image.shape[1]),
        ctypes.c_void_p(output_color_image.ctypes.data))

    #print 'indata: %s' % indata
    #print 'outdata: %s' % outdata

