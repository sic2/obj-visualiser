#pragma once

#include "OpenGLHeaders.h"

#include <stdio.h>
#include <math.h>

#define NUMBER_CAMERA_LOCATIONS 7
#define FRONT_VIEW_CAMERA 0
#define TOP_VIEW_CAMERA 1
#define RIGHT_SIDE_VIEW_CAMERA 2
#define LEFT_SIDE_VIEW_CAMERA 3
#define BACK_VIEW_CAMERA 4
#define FAR_VIEW_CAMERA 5
#define NEAR_VIEW_CAMERA 6

#define STD_ZOOM 4

/**
*
*/
typedef struct Camera_Location
{
	float zoom;
	float theta;
	float phi;
} Camera_Location;

static const float DEG_TO_RAD = 0.0174532;

class Cameras
{
public:
	inline Cameras()
	{
		_index = 0;
	}

	virtual ~Cameras() {}

	inline void moveTo(float zoom, float theta, float phi)
	{
		// @see http://homepages.ius.edu/RWISMAN/b481/html/notes/FlyAround.htm
		float camX = zoom * sin(theta * DEG_TO_RAD) * sin(phi * DEG_TO_RAD);
	  	float camY = zoom * cos(theta * DEG_TO_RAD);
	   	float camZ = zoom * sin(theta * DEG_TO_RAD) * cos(phi * DEG_TO_RAD);
		// Reduce theta slightly to obtain another point on the same longitude line on the sphere.
		GLfloat dt=1.0;
		GLfloat eyeXtemp = zoom * sin(theta * DEG_TO_RAD - dt) * sin(phi * DEG_TO_RAD);
		GLfloat eyeYtemp = zoom * cos(theta * DEG_TO_RAD - dt);
		GLfloat eyeZtemp = zoom * sin(theta * DEG_TO_RAD - dt) * cos(phi * DEG_TO_RAD);
		// Connect these two points to obtain the camera's up vector.
		float upX = eyeXtemp - camX;
		float upY = eyeYtemp - camY;
		float upZ = eyeZtemp - camZ;
		// Set the camera position and lookat point
		gluLookAt(camX,camY,camZ,   // Camera position
			  	0.0, 0.0, 0.0,    // Look at point
			  	upX, upY, upZ);   // Up vector
	}

	inline Camera_Location nextCamera()
	{
		_index++;
		_index = _index % NUMBER_CAMERA_LOCATIONS;
		return getCameraLocation();
	}

private:
	int _index;

	inline Camera_Location getCameraLocation()
	{
		Camera_Location cl;
		switch(_index)
		{
		case FRONT_VIEW_CAMERA: cl.zoom = STD_ZOOM; cl.theta = 90.0; cl.phi = 0.0;
		break;
		case TOP_VIEW_CAMERA: cl.zoom = STD_ZOOM; cl.theta = 45.0; cl.phi = 0.0;
		break;
		case RIGHT_SIDE_VIEW_CAMERA: cl.zoom = STD_ZOOM; cl.theta = 90.0; cl.phi = -90.0;
		break;
		case LEFT_SIDE_VIEW_CAMERA: cl.zoom = STD_ZOOM; cl.theta = 90.0; cl.phi = 90.0;
		break;
		case BACK_VIEW_CAMERA: cl.zoom = STD_ZOOM; cl.theta = 90.0; cl.phi = 180.0;
		break;
		case FAR_VIEW_CAMERA: cl.zoom = 10.0; cl.theta = 90.0; cl.phi = 0.0;
		break;
		case NEAR_VIEW_CAMERA: cl.zoom = 2.0; cl.theta = 90.0; cl.phi = 0.0;
		break;
		default:
			printf("Undefined camera picked \n");
		break;
		}
		return cl;
	}
};