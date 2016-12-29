#!/usr/bin/python
import cherrypy
from cherrypy.lib import file_generator
import StringIO
import os
import json
from jinja2 import Environment, FileSystemLoader

class sandcontrol(object):
    def __init__(self, queue):
        self.pqueue = queue
        self.picture = None

    @cherrypy.expose
    def index(self):
        global tmpl
        return tmpl.render()

    @cherrypy.expose
    @cherrypy.tools.json_out()
    def switch(self, id):
        print("button nr {} pressed".format(id))
        return json.dumps({"text" : "button {} ".format(id)})

    @cherrypy.expose
    def pic(self, *args, **kw):
        cherrypy.response.headers['Content-Type'] = "image/png"


        if not self.pqueue.empty():
            try:
                cv2.imwrite('webroot/map.png', self.pqueue.get(block=False))
            except:
                pass

        with open("webroot/map.png") as f:
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
