#!/usr/bin/python
import cherrypy
from cherrypy.lib import file_generator
import StringIO
import os
import json
from jinja2 import Environment, FileSystemLoader
import clib_interface
#
height = 300
width  = 400

class sandcontrol(object):
    def __init__(self, queue):
        self.pqueue = queue
        self.picture = None

    @cherrypy.expose
    def index(self):
        global tmpl
        return tmpl.render()

    @cherrypy.expose
    def switch(self, item, pos):
        x = pos['x']
        y = pos['y']

        print(item, pos, x,y)
        if "wat-on" is item:
           clib_interface.spawn_water(height/2,width/2)
        else:
           print("button nr {} pressed".format(id))
           return json.dumps({"text" : "button {} ".format(id)})

    @cherrypy.expose
    def pic(self, *args, **kw):
        cherrypy.response.headers['Content-Type'] = "image/jpeg"
        if not self.pqueue.empty():
            try:
                cv2.imwrite('webroot/map.jpg', self.pqueue.get(block=False), [int(cv2.IMWRITE_JPEG_QUALITY), 10])
            except:
                pass

        with open("webroot/map.jpg") as f:
            return f.read()


    def launch_control(self, queue):
        start(queue)

def start(queue):
    global tmpl
    CURDIR = os.getcwd()
    staticdir = CURDIR+"/webroot"
    env = Environment(loader=FileSystemLoader(staticdir))
    tmpl = env.get_template('index.html')
    cherrypy.config.update({
        "tools.staticdir.dir" : staticdir,
        "tools.staticdir.on" : True
        })
    cherrypy.server.socket_host = "0.0.0.0"
    cherrypy.quickstart(sandcontrol(queue))
