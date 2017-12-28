
import ctypes
import os

class SandboxInputConfig(object):
    def __init__(self, size_x, size_y, height_shift, height_scale):
        self.w = size_x
        self.h = size_y
        self.height_shift = height_shift
        self.height_scale = height_scale
        self.world_pos_x = 0.0
        self.world_pos_y = 0.0
        self.world_pos_z = 0.0
        self.world_width = 1.0
        self.world_height = 1.0
        self.world_rotation_x = 0.0
        self.world_rotation_y = 0.0
        self.world_rotation_z = 0.0

class SandboxOutputConfig(object):
    def __init__(self, output_w, output_h):
        self.w = output_w
        self.h = output_h
        self.world_pos_x = 0.0
        self.world_pos_y = 1.0
        self.world_pos_z = 1.0
        self.ground_plane_world_distance = 1.0
        self.ground_plane_world_width = 1.0
        self.ground_plane_world_height = 1.0

class SandboxSimulation(object):
    def __init__(self):
        self.lib = ctypes.cdll.LoadLibrary(
            os.path.join(os.path.abspath(
                os.path.dirname(__file__)), "libclib.so"))
        self._inputs = []
        self._outputs = []
        self.interface_run = self.lib.interface_run
        self.interface_run.argtypes = []
        self.interface_run.restype = None
        self.interface_setInputImg = self.lib.interface_setInputImg
        self.interface_setInputImg.argtypes = [
            ctypes.c_int, ctypes.c_void_p]
        self.interface_setInputImg.restype = None

    def simulate(self, input_depth_images):
        if len(input_depth_images) != len(self._inputs):
            raise ValueError("the provided amount of depth images is " +
                str(len(input_depth_images)) + ", but the amount of " +
                "inputs is currently " + str(len(self._inputs)) +
                ". use set_input_config() to configure your inputs " +
                "accordingly before calling this")

        # Set input images:
        index = -1
        for input_config in self._inputs:
            index += 1
            assert(input_config.w == input_depth_images[index].shape[0])
            assert(input_config.h == input_depth_images[index].shape[1])
            self.interface_setInputImg(index,
                ctypes.c_void_p(input_depth_images[index].ctypes.data))

        # Call simulation:
        self.interface_run()

        # Get outputs:
        for output_config in self._outputs:
            pass

    def set_input_config(self, inputs):
        class InputConfigStruct(Structure):
            _fields_ = [("w", ctypes.c_size_t),
                        ("h", ctypes.c_size_t),
                        ("world_x", ctypes.c_double),
                        ("world_y", ctypes.c_double),
                        ("world_z", ctypes.c_double),
                        ("size_w", ctypes.c_double),
                        ("size_h", ctypes.c_double),
                        ("rotation_x", ctypes.c_double),
                        ("rotation_y", ctypes.c_double),
                        ("rotation_z", ctypes.c_double),
                        ("height_shift", ctypes.c_double),
                        ("height_scale", ctypes.c_double)]

        set_input_amount = self.lib.interface_setInputAmount
        set_input_amount.argtypes = [ctypes.c_int]
        set_input_amount.restype = None
        set_input_amount(len(inputs))
        index = -1
        for input_config in self._inputs:
            index += 1
            config = InputConfigStruct()
            config.w = input_config.w
            config.h = input_config.h
            config.height_scale = input_config.height_scale
            config.height_shift = input_config.height_shift
            config.world_x = input_config.world_pos_x
            config.world_y = input_config.world_pos_y
            config.world_z = input_config.world_pos_z
            config.size_w = input_config.world_width
            config.size_h = input_config.world_height
            config.height_shift = input_config.height_shift
            config.height_scale = input_config.height_scale
            set_config = self.lib.interface_setInputConfig
            set_config.argtypes = [ctypes.c_int,
                ctypes.POINTER(InputConfigStruct)]
            set_config.restype = None
            set_config(index, ctypes.POINTER(config))

    def set_output_config(self, outputs):
        pass

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

