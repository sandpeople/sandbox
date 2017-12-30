PHONY=all
OS=$(shell lsb_release -si)

ifeq ($(OS),Debian)
	INSTALL_BIN=apt
endif

ifeq ($(OS),Fedora)
	INSTALL_BIN=yum
endif

all: libfreenect libfreenect2 build

build: build-essentials build-clib

build-clib:
	cd clib && make

build-essentials:
ifeq ($(OS),Debian)
	sudo $(INSTALL_BIN) install \
		build-essential \
		cmake \
		pkg-config \
		python-pip
endif
ifeq ($(OS),Fedora)
	sudo $(INSTALL_BIN) install \
		make \
		automake \
		gcc \
		gcc-c++ \
		kernel-devel \
		build-essential \
		cmake \
		pkg-config \
		python-pip
endif

clean:
	rm -rf libfreenect2

install:
	echo

libfreenect: depends
	sudo $(INSTALL_BIN) install freenect python-freenect libfreenect-dev

libfreenect2: depends build-essentials
	test -d libfreenect2 || git clone https://github.com/OpenKinect/libfreenect2.git
	cd libfreenect2 && cmake -DCMAKE_INSTALL_PREFIX=/usr/local/
	cd libfreenect2 && make
	cd libfreenect2 && sudo make install
	cd libfreenect2 && sudo make install-openni2
	pip install pylibfreenect2

depends:
	sudo $(INSTALL_BIN) install \
		i965-va-driver \
		libglfw3-dev \
		libglew-dev \
		libglfw3 \
		libffi-dev \
		libjpeg-dev \
		libopenni2-dev \
		libsdl2-dev \
		libsdl2-image-dev \
		libusb-1.0-0-dev \
		libusb-1.0-0 \
		libva-dev \
		libva-drm1 \
		libva-egl1 \
		libva-glx1 \
		libva-tpi1 \
		libva-wayland1 \
		libva-x11-1 \
		libva1 \
    	ocl-icd-libopencl1 \
    	ocl-icd-opencl-dev \
    	openni2-utils \
    	vainfo

	pip install CherryPy SciPy Jinja2
