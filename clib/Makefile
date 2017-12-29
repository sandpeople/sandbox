all:
	rm -f vmath.o
	g++ -O3 -g -fPIC -Wall -Wextra -DGLM_HAS_CXX11_STL=0 -c -o vmath.o vmath.cpp
	gcc -O3 -g -fPIC -std=c99 -Wall -Wextra -Wno-unused-parameter -shared -o ../libclib.so fluid.c images.c interface.c multiimgrotator.c particle.c random.c simulation.c topology.c transform.c vmath.o -lSDL2 -lSDL2_image -lGLEW
