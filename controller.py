#!/usr/bin/python

import os,sys
from pylib import clients
import clib_interface
import server
import threading
from PIL import Image
import Queue
from time import sleep, time

# set client lists
kinects=[]
beamer=[]

# start webserver
## init picture_queue
pqueue=Queue.Queue(maxsize=1)
## init command_queue
cqueue=Queue.Queue()
## init client update queue
uqueue=Queue.LifoQueue(maxsize=1)
server=server.sandcontrol(pqueue, cqueue, kinects, beamer, uqueue)
serverd=threading.Thread(target = server.launch_control, args=(pqueue, cqueue, kinects, beamer, uqueue))
serverd.daemon = True
serverd.start()

# set simulation defaults
height_shift = 0
height_scale = 1
screen_resolution_x = 1920
screen_resolution_y = 1080
map_offset_x = 0.0
map_offset_y = 0.0
map_zoom = 1.0

# initiate simulation
#sandbox_sim = clib_interface.SandboxSimulation()
#sandbox_sim.set_height_config(height_shift, height_scale)
#sandbox_sim.reset_map_drag()
#sandbox_sim.drag_map(map_offset_x, map_offset_y)

# prepare target pic
cimg = Image.new("L", (screen_resolution_x,screen_resolution_y), color=0)

while True:

    # handle client requests
    if not cqueue.empty():
        data=cqueue.get(block=False)
        if data["state"] == "init":
            if data["type"] == "kinect":
                print "new kinect"
                new_kinect=clients.kinect_client(id=len(kinects), data=data)
                kinects.append(new_kinect)
                try:
                    uqueue.put((kinects, beamer), block=False)
                except Queue.Full:
                    pass
            elif request["type"] == "beamer":
                print "new beamer"
                new_beamer=clients.beamer_client(id=len(beamer), data=data)
                beamer.append(new_beamer)
                try:
                    uqueue.put((kinects, beamer), block=False)
                except Queue.Full:
                    pass
    else:
        #sandbox_sim.simulate(img, cimg)
        sleep(1)
