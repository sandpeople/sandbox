#!/usr/bin/python
import cherrypy
import os
import json

class sandcontrol(object):
    @cherrypy.expose
    def index(self):
        return """<html>
<head>
<script src="http://code.jquery.com/jquery-2.1.4.min.js" type="text/javascript"></script>
<script type="text/javascript">
$(document).ready(function() {
    $("button").click(function() {
        $.ajax({
            url: "switch",
            type: "POST",
            data: {id: $(this).attr('id')},
            success: function(response) {
                alert(response);
                $("#test").html(response);
            } 
        });
    });
});
</script>
</head>
<body>
    <h1>Sand Control</h1>
    <button id=1>Button 1</button> </br>
    <button id=2>Button 2</button> </br>
    <button id=3>Button 3</button> </br>
    <button id=4>Button 4</button>

    <div id=test></div>

</body>
</html>"""

    @cherrypy.expose
    @cherrypy.tools.json_out()
    def switch(self, id):
        print("button nr {} pressed".format(id))
        return json.dumps({"text" : "button {} ".format(id)})

class launch_control():
    def start():
        CURDIR = os.getcwd()
        cherrypy.config.update({
            "tools.staticdir.dir" : CURDIR,
            "tools.staticdir.on" : True
            })
        cherrypy.quickstart(sandcontrol())
