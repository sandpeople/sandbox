
import ctypes

lib = None
def simulate(input_depth_image, output_color_image):
    global lib
    if lib == None:
        lib = ctypes.cdll.LoadLibrary('./libclib.so')
    interface_run = lib.interface_run
    interface_run.restype = None
    
    # call simulation:
    interface_run(ctypes.c_void_p(input_depth_image.ctypes.data),
        ctypes.c_int(input_depth_image.shape[0]),
        ctypes.c_int(input_depth_image.shape[1]),
        ctypes.c_void_p(output_color_image.ctypes.data))

def set_height_config(height_shift, height_scale):
    global lib
    if lib == None:
        lib = ctypes.cdll.LoadLibrary('./libclib.so')
    set_height_config = lib.interface_setHeightConfig
    set_height_config.argtypes = [ctypes.c_double, ctypes.c_double]
    set_height_config.restype = None
    set_height_config(height_shift, height_scale)

def drag_map(x, y):
    global lib
    if lib == None:
        lib = ctypes.cdll.LoadLibrary('./libclib.so')
    interface_mapOffset = lib.interface_mapOffset
    interface_mapOffset.argtypes = [ctypes.c_double, ctypes.c_double]
    interface_mapOffset.restype = None
    interface_mapOffset(x, y)

def reset_map_drag():
    global lib
    if lib == None:
        lib = ctypes.cdll.LoadLibrary('./libclib.so')
    interface_resetMapOffset = lib.interface_resetMapOffset
    interface_resetMapOffset.restype = None
    interface_resetMapOffset()

def reset_water():
    global lib
    if lib == None:
        lib = ctypes.cdll.LoadLibrary('./libclib.so')
    interface_resetWater = lib.interface_resetWater
    interface_resetWater.argtypes = []
    interface_resetWater.restype = None
    interface_resetWater()

def set_map_zoom(zoom):
    global lib
    if lib == None:
        lib = ctypes.cdll.LoadLibrary('./libclib.so')
    interface_zoom = lib.interface_setMapZoom
    interface_zoom.argtypes = [ctypes.c_double]
    interface_zoom.restype = None
    interface_zoom(zoom)

def add_car(pos_x, pos_y):
    pass

def remove_all_cars():
    pass

def spawn_water(pos_x, pos_y):
    global lib
    if lib == None:
        lib = ctypes.cdll.LoadLibrary('./libclib.so')
    spawn_water = lib.interface_spawnWater
    spawn_water.argtypes = [ctypes.c_double, ctypes.c_double]
    spawn_water.restype = None
    spawn_water(pos_x, pos_y)




