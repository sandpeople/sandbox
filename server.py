#!/usr/bin/python
import cherrypy
from cherrypy.lib import file_generator
import StringIO
import os
import json
from jinja2 import Environment, FileSystemLoader
import clib_interface

height = 300
width  = 400

class sandcontrol(object):
    def __init__(self, pqueue, cqueue, kinects, beamer):
        self.pqueue = pqueue
        self.cqueue = cqueue
        self.kinects = kinects
        self.beamer = beamer
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
        with open("debug", "w") as f:
            json.dump((item, pos, x, y), f)
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
                self.picture = None
            except:
                pass

        if not self.picture:
            with open("webroot/map.jpg") as f:
                self.picture = f.read()
        return self.picture

    def find_client(self, id, token, type, search="id"):
        if type == "kinect":
            liste = kinects
        elif type == "beamer":
            liste = beamer
        else
            raise

        if search == "id":
            for client in self.liste:
                if client.id == id:
                    if client.token == token:
                        return client
                    else:
                        return False
        elif search == "token":
            for client in self.liste:
                if client.tmp_token == token:
                    return client 
        return None

    @cherrypy.expose
    def client(self):
        state=cherrypy.request.headers.get("state", None)
        type=cherrypy.request.headers.get("type", None)
        id=cherrypy.request.headers.get("id", -1)
        if state == "init":
            tmp_token=cherrypy.request.headers.get("tmp_token")
            if tmp_token:
                client=self.find_client(id, tmp_token, type, search="token")
                if client:
                    return client.token
            data={}
            data["state"] = state
            data["type"] = type
            data["tmp_token"] = 
            if request.method == "POST":
                data["body"] = cherrypy.request.body.read()
            cqueue.put(data)
        elif state == "connected":
            client = self.find_client(id, cherrypy.request.headers.get("token", ""), type)
            if client == False:
                return 403
            elif client == None:
                return 404

            # if posting client wants to send pictures
            if request.method == "POST" and type == "kinect":
                try:
                    client.ingressq.put(cherrypy.request.body.read(), block=False)
                except Queue.Full:
                    # send "Too Many Requests" to signal client to throttle
                    return 429
                except:
                    raise
                return 200

            elif type == "beamer":
                try:
                    img = client.egressq.get(block=False)
                except Queue.Empty:
                    # send "Too Many Requests" to signal client to throttle
                    return 429
                except:
                    raise
                return img
                
            
        elif state == "disconnect":
            return 401
 
        else:
            return 404

    def launch_control(self, pqueue, cqueue, kinects, beamer):
        start(pqueue, cqueue, kinects, beamer)

def start(pqueue, cqueue, kinects, beamer):
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
    cherrypy.quickstart(sandcontrol(pqueue, cqueue, kinects, beamer))

