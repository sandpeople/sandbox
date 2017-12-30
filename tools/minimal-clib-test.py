#!/usr/bin/python3

# Fix import path:
import os
import sys
sys.path = [os.path.join(os.path.abspath(
    os.path.dirname(__file__)), "..")] + sys.path

import clib_interface
import numpy as np
import PIL.Image

def main():
    sandbox_sim = clib_interface.SandboxSimulation()
    im = PIL.Image.open(os.path.join(os.path.abspath(
        os.path.dirname(__file__)), "..", "images",
        "kinect_test_depth_image.png"))
    kinect_1 = clib_interface.SandboxInputConfig(im.size[0], im.size[1])
    sandbox_sim.set_inputs([kinect_1])
    while True:
        sandbox_sim.simulate([np.asarray(im, dtype=np.uint8)],
            columns_rows_swapped=True)

if __name__ == "__main__":
    main()


