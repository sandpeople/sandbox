
import ctypes
import os

class SandboxSimulation(object):
    def __init__(self):
        self.lib = ctypes.cdll.LoadLibrary(
            os.path.join(os.path.abspath(
                os.path.dirname(__file__)), "libclib.so"))

    def simulate(self, input_depth_image, output_color_image):
        interface_run = self.lib.interface_run
        interface_run.restype = None
        
        # call simulation:
        interface_run(ctypes.c_void_p(input_depth_image.ctypes.data),
            ctypes.c_int(input_depth_image.shape[0]),
            ctypes.c_int(input_depth_image.shape[1]),
            ctypes.c_void_p(output_color_image.ctypes.data))

    def set_height_config(self, height_shift, height_scale):
        set_height_config = self.lib.interface_setHeightConfig
        set_height_config.argtypes = [ctypes.c_double, ctypes.c_double]
        set_height_config.restype = None
        set_height_config(height_shift, height_scale)

    def drag_map(self, x, y):
        interface_mapOffset = self.lib.interface_mapOffset
        interface_mapOffset.argtypes = [ctypes.c_double, ctypes.c_double]
        interface_mapOffset.restype = None
        interface_mapOffset(x, y)

    def reset_map_drag(self):
        interface_resetMapOffset = self.lib.interface_resetMapOffset
        interface_resetMapOffset.restype = None
        interface_resetMapOffset()

    def reset_water(self):
        interface_resetWater = self.lib.interface_resetWater
        interface_resetWater.argtypes = []
        interface_resetWater.restype = None
        interface_resetWater()

    def set_map_zoom(self, zoom):
        interface_zoom = self.lib.interface_setMapZoom
        interface_zoom.argtypes = [ctypes.c_double]
        interface_zoom.restype = None
        interface_zoom(zoom)

    def add_car(self, pos_x, pos_y):
        pass

    def remove_all_cars(self):
        pass

    def spawn_water(self, pos_x, pos_y):
        spawn_water = self.lib.interface_spawnWater
        spawn_water.argtypes = [ctypes.c_double, ctypes.c_double]
        spawn_water.restype = None
        spawn_water(pos_x, pos_y)

    def shutdown(self):
        stop = self.lib.interface_stop
        stop.restype = None
        stop()

