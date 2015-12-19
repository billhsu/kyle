# Addons #

## ofxFft ##
Wraps FFTW (includes Windows library) and KISS FFT (a BSD-licensed, platform independent FFT library).

## ofxCvHaarFinder ##
Wraps OpenCV's [haar detection](http://en.wikipedia.org/wiki/Haar-like_features) algorithm, generally used for face tracking.

## ofxDisplayList ##
Wraps OpenGL's [display lists](http://www.songho.ca/opengl/gl_displaylist.html), allowing for dl.begin(), dl.end() and dl.draw(), and handling destruction and copying of the display list reference.

## ofxColor ##
Implements some arithmetic operators on colors, comparison operators for sorting routines, represents HSV and RGB simultaneously (automatically converting between them), handles different color ranges, and handles equals operators for hex and string inputs, allowing for ofxColor myColor = "0x5f3759"; or ofxColor myColor = 0x5f3759;

## ofxKmeans ##
Wraps OpenCV's [k-means clustering](http://en.wikipedia.org/wiki/k-means), clustering vectors of 2d ofPoints and colors from images. Multi-pass adaptive color clustering is also implemented, so that only max-k must be specified. Requires ofxColor.

## ofxSvg ##
Implements a subset of the [SVG specification](http://www.w3.org/TR/SVG/). Loads .svg files, efficiently draws them with caching. Allows a single set of paths to be drawn multiple times with variations using drawPlain(). Requires ofxColor and ofxSvg.

# Demos #
## Barnes Hut ##
Implements [barnes-hut](http://en.wikipedia.org/wiki/Barnes-Hut_simulation) gravity approximation simulation, allowing tens of thousands of particles interacting gravitationally to be simulated in realtime.

## Quadtree Particle System ##
Implements [quadtrees](http://en.wikipedia.org/wiki/Quadtree) for efficient region lookup in sparse, unbounded data.

## Binned Particle System ##
Uses binning/spatial hashing for efficient region lookup in dense, bounded data. Runs faster than quadtrees. Includes optional fast inverse sqrt as a compile time flag.