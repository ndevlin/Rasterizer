# Rasterizer

Written by Nathan Devlin


OVERVIEW

This project rasterizes 3D and 2D obj files. It takes in the obj vertex information, 
along with texture files if relevant, processing them through a json. It then 
builds appropriate matrices to allow for a modifiable camera, and calculates color
fragment information for each pixel to be represented on screen. 

It is built essentially from scratch, and it does not use shaders or the GPU but
rather processes the information in series on the CPU, which may cause it to run
somewhat slowly on some machines. I wrote, in essence, all code except for the
code supporting the GUI; more detail is provided in each file. The broad design was
provided by Adam Malley. 


FILES

The main files of interest are camera.cpp, polygon.cpp, and rasterizer.cpp.
The camera object represents a camera in 3D space and stores information about
the view and perspective matrices, along with allowing for interactive camera
movement. polygon.cpp represents a polygon in 3D space and also has many functions
for interpolating a variety of values within the surface, in addition to tesselating
the polygon. Rasterizer takes in the polygon and triangle information, recalculates
their positions in camera and then screen space, and determines the appropriate
color for each fragment. It then determines the correct fragment to display based
on its z-depth. 


RUNNING THE PROGRAM

Building the project is easiest in the Qt IDE as the GUI was made using Qt.
Or, you can just run the executable Rasterize in the home directory.
Upon opening the program, go to the menu item in the upper left 
"File" -> Load Scene. This will allow you to open an obj file, which is
wrapped in a json, for viewing. Navigate to the "scenes" folder which is
in the home directory of the project. In it there are a number of 3D and
2D scenes that can be viewed. I recommend opening "3D_wahoo.json" which is 
a model of Mario, the most complex object in the scenes folder.
(Note: to render 2D scenes, the boolean variable threeD in rasterizer.cpp 
must be changed to false)


CAMERA CONTROLS

You can use WASD controls to move forward, back, and sideways. Use Q to move
down and E to move up. Use the arrow keys to pan left or right or tilt up or down.
Use Z and X to tilt the camera about its axis clockwise or counter-clockwise.

Thank you for taking the time to check out this project!




