# Shading And Texture

	DESCRIPTION
======================================

# How to compile and run the program:

Compile:

	$ make

To turn on optimisation (-O2)

	$ make release

Run:

	$ ./shadingAndTexture Data/cube.obj (+ or other data sets)

In one command:

	$ make release && ./shadingAndTexture Data/cube.obj (+ or other data sets)

# TODO

	[] pick face and cycle material/textures
	[] multi-texturing
	[] bump texturing

# Known bugs

	[] Zoom limits not set

# Demo

	LINK


# Requirements

	gcc compiler, compliant to c++03

## Tested targets
	i686-apple-darwin10-gcc-4.2.1 on Mac OS X 10.6.8
	i686-apple-darwin11-llvm-gcc-4.2 on Mac OS X 10.8.5
	gcc-4.4.7 (Red Hat 4.4.7-3) on Scientific Linux - Kernel Linux 2.6.32

# Third party resources

The makefile is a modified version of the one provided by [Michael Safyan](https://sites.google.com/site/michaelsafyan/software-engineering/how-to-write-a-makefile)

Wavefront .obj loader (slightly modified version): 
[objLoader](http://www.kixor.net/dev/objloader/) 

Image library (C/C++):
[CImg](http://cimg.sourceforge.net/)