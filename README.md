PhiSimulator
============

A simulator of classical mechanics problems.


Dependencies
------------

* vlc
* imagemagick
* avconv


Usage
-----

Set the input in input/bodies.txt and input/environment.txt, compile and execute simulator.cpp.

bodies.txt describes the bodies to be simulated, environment.txt specifies the property of the simulation universe.
This is the standard to be followed for the input:

* 	bodies.txt : 

	1. line = number of bodies
	
	Following lines specify each body's property:
	
	Body's description:

	1. line	= mass radius fixed

	2. line	= x-pos y-pos

	3. line	= x-vel y-vel
		
	
	Where:
	*	x-pos, y-pos and x-vel,y-vel describe the initial position and velocity of the body
	
	*	radius is the radius of the body as shown in the final output video
	
	*	fixed has value 0 or 1: 1 means the body is fixed in his position, 0 means it is not fixed.

		
* 	environment.txt :
	
	1.	line = G-value
	
	2. 	line = dt
	
	3.	line = ImageCapturingFrequency VideoLength
			
