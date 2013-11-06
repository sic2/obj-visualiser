#pragma once

#include "OpenGLHeaders.h"

/*
* Light properties
*/
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};  /* Red diffuse light. */
GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location. */
GLfloat light_ambient[] = {1.0, 1.0, 1.0, 1.0}; 
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};

/*
* TODO:
* 	[] moving light
*/
class Lights
{
public:

	/**
	* Initialise the lights
	*/
	inline Lights()
	{
		/* Enable a single OpenGL light. */
		glEnable(GL_LIGHTING);

		// TODO - move light initialisation to specific file
		// having the lights wrapper into an object will 
		// allow me to easily more them around however I like
		// Light - 0
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	}

};