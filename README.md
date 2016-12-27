# Sandbox

Software for our augmented reality sandbox located in our hackerspace.
Written using libfreenect, python-opencv, SDL2 and python2 & C.

## Dependencies

This project requires a couple of dependencies:

Debian:
apt-get install libfreenect-dev libsdl2-image-2.0-0 python-freenect libcv-dev libsdl2-dev libsdl2-image-dev python-opencv python-jinja2 python-pip
(not completely verified) ; pip install -U cherrypy
apt-get install python-scipy
pip install scipy

## Build & run

Before you run, you need to compile the C code:

```
make
```

Afterwards, launch the program with:
```
./main.py
```
(inside the repository)


