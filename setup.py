#!/usr/bin/env python
from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize
import re
import numpy as np

# Only use Cython if it is available, else just use the pre-generated files
from Cython.Distutils import build_ext
source_ext = '.pyx'
cmdclass = {'build_ext': build_ext}


ext_modules = [Extension("freenect", ["freenect" + source_ext],
                         libraries=['usb-1.0', 'freenect', 'freenect_sync'],
                         runtime_library_dirs=['/usr/local/lib',
                                               '/usr/local/lib64',
                                               '/usr/lib/'],
                         extra_compile_args=['-fPIC', '-I', '../../include/',
                                             '-I', '/usr/include/libusb-1.0/',
                                             '-I', '/usr/local/include/libusb-1.0',
                                             '-I', '/usr/local/include',
                                             '-I', '../c_sync/',
                                             '-I', np.get_include()]),
               cythonize("main.pyx")]
setup(name='freenect',
      cmdclass=cmdclass,
      ext_modules=ext_modules)
