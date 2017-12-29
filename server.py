#!/usr/bin/python
import cherrypy
from cherrypy.lib import file_generator
import StringIO
import os
import json
from jinja2 import Environment, FileSystemLoader
import clib_interface
from time import sleep, time

height = 300
width  = 400

class sandcontrol(object):
    def __init__(self, pqueue, cqueue, kinects, beamer, uqueue):
        self.pqueue = pqueue
        self.cqueue = cqueue
        self.uqueue = uqueue
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
                self.picture = self.pqueue.get(block=False)
            except:
                pass

        if not self.picture:
            with open("webroot/map.jpg") as f:
                self.picture = f.read()
        return self.picture

    def find_client(self, id, token, type, search="id"):
        print "searching for id:%s token:%s type:%s search:%s" %(id, token, type, search)
        if not self.uqueue.empty():
            self.kinects, self.beamer = self.uqueue.get()
        if type == "kinect":
            liste = self.kinects
        elif type == "beamer":
            liste = self.beamer
        else:
            raise
        print "in liste %s" %liste
        if search == "id":
            for client in liste:
                if str(client.id) == str(id):
                    if str(client.token) == str(token):
                        return client
                    else:
                        return False
        elif search == "token":
            for client in liste:
                if client.tmp_token == token:
                    return client 
        return None

    @cherrypy.expose
    def client(self):
        state=cherrypy.request.headers.get("State", None)
        type=cherrypy.request.headers.get("Type", None)
        id=cherrypy.request.headers.get("Id", -1)
        if state == "init":
            print "client connecting"
            tToken=cherrypy.request.headers.get("Tmp-Token")
            if tToken:
                client=self.find_client(id, tToken, type, search="token")
                if client:
                    print "client %s connected" %client.id
                    return "%s|#|%s" %(client.token, client.id)
                else:
                    print "no such client"
                    return
            data={}
            data["state"] = state
            data["type"] = type
            data["tmp_token"] = str(time())
            if cherrypy.request.method == "POST":
                data["body"] = cherrypy.request.body.read()
            self.cqueue.put(data)
            return data["tmp_token"]
        elif state == "connected":
            print "connected client"
            client = self.find_client(id, cherrypy.request.headers.get("token", ""), type)
            if client == False:
                print "wrong token"
                return "403"
            elif client == None:
                print "client not found"
                return "404"
            print "client %s" %client.id
            # if posting client wants to send pictures
            if cherrypy.request.method == "POST" and type == "kinect":
                print "receive image"
                print cherrypy.request.process_request_body
                try:
                    tmp=cherrypy.request.body.read()
                    client.ingressq.put(tmp, block=False)
                except Queue.Full:
                    # send "Too Many Requests" to signal client to throttle
                    return "429"
                except:
                    raise
                return 200

            elif type == "beamer":
                try:
                    img = client.egressq.get(block=False)
                except Queue.Empty:
                    # send "Too Many Requests" to signal client to throttle
                    return "429"
                except:
                    raise
                return img
            return "404" 
            
        elif state == "disconnect":
            return "401"
 
        else:
            return "404"

    def launch_control(self, pqueue, cqueue, kinects, beamer, uqueue):
        start(pqueue, cqueue, kinects, beamer, uqueue)

def start(pqueue, cqueue, kinects, beamer, uqueue):
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
    cherrypy.quickstart(sandcontrol(pqueue, cqueue, kinects, beamer, uqueue))

