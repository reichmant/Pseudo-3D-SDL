# Pseudo 3D Rendering Engine in SDL2
This is a rendering engine comparable to that used in the 1922 video game by id software, *Wolfenstein 3D*. It implements a similar raycasting algorithm, but uses C and SDL2.

## Getting Started
### Prerequisites
You'll need to install SDL2, SDL2_ttf and GCC for the architecture of the target machine. SDL2_ttf is only neccessary for displaying the FPS in the window - you can remove this from the "includes" if you don't intend to use this option
### Compiling
If you intend to build for your local machine, simply running 'make' will work. Similarly, if you modify the makefile to point to your SDL2 and GCC paths, you can compile for Raspberry Pi 3 by typing 
```
make pi
```
.
## Usage
The compiled executable can be run directly. There are a few command line arguments:
```
-showfps # // If 1, FPS is displayed in the window. If 2, it's printed to terminal
-h # // The rendering window's height
-w # // The rendering window's width
-limitfps # // Stops rendering until the end of the second after this many frames are rendered. Typically do 30 or 60. If unspecified, it's 999.
```

## Controls

```
W - Move forward the direction you are facing
S - Move backward (relative to the direction you are facing)
Left Arrow - Turn to the left
Right Arrow - Turn to the right
Shift - "Sprint" - increases movement (but not turning) speed.
Q - Quit
Alt-F4, X in the corner of the window, Ctrl-C in terminal - Also quit.
```

## Credits
Raycasting Algorithm heavily adapted from Lode Vandevenne’s [Computer Graphics tutorial](http://lodev.org/cgtutor/raycasting.html)
Used for educational purposes only.

This project is currently compliant with GPL 3, from what I can tell.
