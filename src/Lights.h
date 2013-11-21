#pragma once

#include "OpenGLHeaders.h"

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

/*
* TODO:
*	docs
*/
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
	*
	*/
	inline void reApply(float angle)
	{
		glPushMatrix();
		glRotatef(angle, 0.0f, 1.0f, 0.0f);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glPopMatrix();
	}

	/**
	*
	* TODO: more than one light
	*/
	inline void nextLights()
	{
		glDisable(GL_LIGHT0); // disable current light // FIXME - might disable more than one light
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
		case 0:	
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		break;
		case 1:
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuseLI);
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		break;
		case 2:
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuseR);
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		break;
		case 3:
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuseG);
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		break;
		case 4:
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuseB);
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		break;
		case 5:
			glDisable(GL_LIGHTING);
		break;
		default:
			printf("Lights index undefined\n");
		break;
		}
	}
};