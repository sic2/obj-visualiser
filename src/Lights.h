#pragma once

#include "OpenGLHeaders.h"

#include <stdio.h>

#define NUMBER_LIGHTS 6

/*
* Light properties
*/
// Normal lighting
GLfloat light_position[] = {0.0, 0.0, 10.0, 0.0}; 
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0}; 
GLfloat light_ambient[] = {1.0, 1.0, 1.0, 1.0}; 
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};

// Less diffuse intensity
GLfloat light_diffuseLI[] = {0.01, 0.01, 0.01, 1.0};
// Blue light
GLfloat light_diffuseB[] = {0.0, 0.0, 1.0, 1.0};
// Green light
GLfloat light_diffuseG[] = {0.0, 1.0, 0.0, 1.0};
// Red light 
GLfloat light_diffuseR[] = {1.0, 0.0, 0.0, 1.0};

GLfloat light_position1[] = {0.0, 10.0, 0.0, 0.0}; 
GLfloat light_specular1[] = {1.0, 0.0, 1.0, 1.0};

class Lights
{
public:

	/**
	* Initialise the lights
	*/
	inline Lights()
	{
		_index = 0;

		/* Enable a single OpenGL light. */
		glEnable(GL_LIGHTING);
		// Light - 0
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	}

	/**
	* Rotate lights by a given angle and reApply the light
	*/
	inline void reApply(float angle)
	{
		glPushMatrix();
		glRotatef(angle, 0.0f, 1.0f, 0.0f);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		if (_index == 5)
			glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
		glPopMatrix();
	}

	/**
	* Activate the next light/s
	*/
	inline void nextLights()
	{
		glDisable(GL_LIGHT0); // disable current light
		if (_index == 5)
			glDisable(GL_LIGHT1);

		_index++;
		_index = _index % NUMBER_LIGHTS;
		activateNextLights();
	}

private:
	int _index;

	inline void activateNextLights()
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		switch(_index)
		{
		case 0:	 // Normal light
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		break;
		case 1: // Low Intensity
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuseLI);
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		break;
		case 2: // RED
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuseR);
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		break;
		case 3: // GREEN
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuseG);
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		break;
		case 4: // BLUE
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuseB);
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		break;
		case 5: // TWO LIGHTS
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

			glEnable(GL_LIGHT1);
			glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
			glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
		break;
		case 6: // NO LIGHT
			glDisable(GL_LIGHTING);
		break;
		default:
			printf("Lights index undefined\n");
		break;
		}
	}
};