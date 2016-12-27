#!/usr/bin/python
import cherrypy
import os
import json
from jinja2 import Environment, FileSystemLoader

class sandcontrol(object):
    def __init__(self, **task_settings):
        self.pqueue = task_settings.get('queue', None)
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
    def pic(self):
        if not pqueue.empty():
            try:
                self.picture = pqueue.get(block=False)
            except:
                pass
        return self.picture

    def launch_control(self):
        start()

def start():
    global tmpl
    CURDIR = os.getcwd()
    staticdir = CURDIR+"/webroot"
    env = Environment(loader=FileSystemLoader(staticdir))
    tmpl = env.get_template('index.html')
    cherrypy.config.update({
        "tools.staticdir.dir" : staticdir,
        "tools.staticdir.on" : True
        })
    cherrypy.quickstart(sandcontrol())
