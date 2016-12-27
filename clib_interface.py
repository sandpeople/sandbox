
import ctypes

def sim(input_depth_image, output_color_image):
    lib = ctypes.cdll.LoadLibrary('./libclib.so')
    interface_run = lib.interface_run
    
    # call simulation:
    interface_run(ctypes.c_void_p(input_depth_image.ctypes.data),
        ctypes.c_int(input_depth_image.shape[0]),
        ctypes.c_int(input_depth_image.shape[1]),
        ctypes.c_void_p(output_color_image.ctypes.data))


def add_car(pos_x, pos_y):
    pass

def remove_all_cars():
    pass

def spawn_water(pos_x, pos_y):
    pass



