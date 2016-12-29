# Sandbox

Software for our augmented reality sandbox located in our hackerspace.
Written using libfreenect, python-opencv, SDL2 and python2 & C.

## Dependencies

This project requires a couple of dependencies:

- libfreenect, python-freenect
- SDL2
- SDL2 Image
- CV 2+, python-opencv
- jinja2
- cherrypy
- scipy
- GLew

(we might have forgotten some. If we have, please file an issue so we can
update this list)

## Build & run

Before you run, you need to compile the C code:

```
make
```

Afterwards, launch the program with:
```
./main.py
```

## Cron

example at tools/cron

example crontab entry:

* * * * * /bin/bash /[path to git]/sandbox/tools/cron

## License

This project is licensed under the terms of GPLv2+
(GNU General Public License version 2, or (at your
option) any later version). See `COPYING.txt` for details.

