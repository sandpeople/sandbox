
#include <stdio.h>

void cfun(const void* depth_array, int xsize, int ysize,
        void* outdatav) {
    const double * indata = (double *) indatav;
    double * outdata = (double *) outdatav;
    int i;
    print("got data.\n");
    for (i = 0; i < rowcount * colcount; ++i) {
        outdata[i] = indata[i] * 2;
    }
    print("processing complete\n");
}

