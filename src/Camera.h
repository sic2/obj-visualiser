#pragma once

#include "OpenGLHeaders.h"

#include <math.h>

class Camera
{
	// TODO
public:
	static inline void moveTo(float zoom, float theta, float phi)
	{
		// @see http://homepages.ius.edu/RWISMAN/b481/html/notes/FlyAround.htm
		float camX = zoom * sin(theta*0.0174532) * sin(phi*0.0174532);
	  	float camY = zoom * cos(theta*0.0174532);
	   	float camZ = zoom * sin(theta*0.0174532) * cos(phi*0.0174532);
		// Reduce theta slightly to obtain another point on the same longitude line on the sphere.
		GLfloat dt=1.0;
		GLfloat eyeXtemp = zoom * sin(theta*0.0174532-dt) * sin(phi*0.0174532);
		GLfloat eyeYtemp = zoom * cos(theta*0.0174532-dt);
		GLfloat eyeZtemp = zoom * sin(theta*0.0174532-dt) * cos(phi*0.0174532);
		// Connect these two points to obtain the camera's up vector.
		float upX=eyeXtemp-camX;
		float upY=eyeYtemp-camY;
		float upZ=eyeZtemp-camZ;
		// Set the camera position and lookat point
		gluLookAt(camX,camY,camZ,   // Camera position
			  0.0, 0.0, 0.0,    // Look at point
			  upX, upY, upZ);   // Up vector
	}
};