
def call_clib_sim(
indata = numpy.ones((5,6), dtype=numpy.double)
outdata = numpy.zeros((5,6), dtype=numpy.double)
lib = ctypes.cdll.LoadLibrary('./ctest.so')
fun = lib.cfun
# Here comes the fool part.
fun(ctypes.c_void_p(indata.ctypes.data), ctypes.c_void_p(outdata.ctypes.data))

print 'indata: %s' % indata
print 'outdata: %s' % outdata

