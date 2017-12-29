# Sandbox

Software for our augmented reality sandbox located in our hackerspace.
Written using libfreenect, python-opencv, SDL2 and python2 & C.

## Dependencies

This project requires a couple of dependencies:

- libfreenect
- python-freenect
- libfreenect2
- pylibfreenect2
- SDL2
- SDL2 Image
- jinja2
- cherrypy
- scipy
- GLew

(we might have forgotten some. If we have, please file an issue so we can
update this list)

## Install dependencies, build & install

Before you run, you need to compile the C code:

```
make
```

Afterwards, launch the program with:
```
./main.py
```

## Keyboard shortcuts

- Escape: terminate the program

- C: enter map offset adjustment mode. Click and drag with mouse to adjust
  (for projection image calibration)

- X: enter map zoom adjustment mode. Click and drag with mouse to adjust
  (for projection image calibration)

- W: reset all Water

## Cron for automatic updates

We use a cronjob for automatic updates.

example at tools/cron

example crontab entry:

* * * * * /bin/bash /[path to git]/sandbox/tools/cron

## License

This project is licensed under the terms of GPLv2+
(GNU General Public License version 2, or (at your
option) any later version). See `COPYING.txt` for details.

