#include "Lights.h"

#include <stdio.h>
#include <cstdlib>

/*
* Key bindings
*/
#define ESC_SIGN 27
#define PLUS_SIGN 43
#define MINUS_SIGN 45
#define C_SIGN 67
#define c_SIGN 99
#define SPACE_SIGN 32

/*
* Perspective view coefficients
*/
#define PERSPECTIVE_ANGLE_VIEW 40.0
#define PERSPECTIVE_NEAR 1.0
#define PERSPECTIVE_FAR 10.0

// Window dimensions
int width = 500; int height = 500;

Lights* lights;

// User Control fields
const GLfloat RADIANS_ACCURACY = 0.5f;
GLfloat zRadians = 0.0f;
GLfloat xRadians = 0.0f;	
GLfloat zoom = 7.0f;

/*
* BOX Data
* 
* TODO : read data from file
*/
GLfloat n[6][3] = {  /* Normals for the 6 faces of a cube. */
  {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
  {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0} };
GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
  {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
  {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };
GLfloat v[8][3];  /* Will be filled in with X,Y,Z vertexes. */

/*
* Preprocessors
*/
// Callbacks
void display();
void myReshape(int w, int h);
void keyboardFunc(unsigned char key, int x, int y);
void specialFunc(int key, int x, int y);

// Application defined
void cleanup();
void shutDown();
void init(void);

/* ----------- *
 *  Functions  *
 *  ---------- */ 

void display()
{
	glPushMatrix();

	// FIXME: @see http://www.gamedev.net/topic/592015-rotating-around-point-using-glulookat/
	gluLookAt(zRadians, xRadians, zoom,  
			0.0, 0.0, 0.0,
			0.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	* Create a box
	* 
	* TODO - render all objects of the scene
	*/
	/* Adjust cube position to be asthetic angle. */
	glTranslatef(0.0, 0.0, -1.0);
	glRotatef(60, 1.0, 0.0, 0.0);
	glRotatef(-20, 0.0, 0.0, 1.0);

	for (int i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
		glNormal3fv(&n[i][0]);
		glVertex3fv(&v[faces[i][0]][0]);
		glVertex3fv(&v[faces[i][1]][0]);
		glVertex3fv(&v[faces[i][2]][0]);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}

	glPopMatrix();

	// Flush and swap buffers
	glutSwapBuffers();
}

/**
* Ensures that reshaping the windows keep the model in the correct ratio
*/
void myReshape(int w, int h)
{	
	width = w; height = h;
}

void keyboardFunc(unsigned char key, int x, int y)
{
	switch(key)
	{
	case ESC_SIGN: shutDown();
		break;
	default: printf("key %d unknown \n", key);
		break;
	} // end switch
	glutPostRedisplay();
}

void specialFunc(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_LEFT: zRadians -= RADIANS_ACCURACY;
		break;
	case GLUT_KEY_RIGHT: zRadians += RADIANS_ACCURACY; 
		break;
	case GLUT_KEY_UP: xRadians -= RADIANS_ACCURACY;
		break;
	case GLUT_KEY_DOWN: xRadians += RADIANS_ACCURACY;
		break;
	default: printf("Unused special key %d pressed\n", key);
		break;
	}
	glutPostRedisplay();
}

/**
* Delete any allocated resources
*/
void cleanup()
{
	delete lights;
}

/**
* Shuts down the application.
* This involves a call to #cleanup() as well.
*/
void shutDown()
{
	cleanup();
	exit(EXIT_SUCCESS);
}

void init(void)
{
	/* Setup cube vertex data. */
	// TODO - automatically create this association for a given object
	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;

	/* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);

	/* Setup the view of the cube. */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(PERSPECTIVE_ANGLE_VIEW, width/height, PERSPECTIVE_NEAR, PERSPECTIVE_FAR);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

int main(int argc, char **argv)
{	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("Shading and Texture");

	// Registering callbacks
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardFunc);
	glutSpecialFunc(specialFunc);

	// Creates the lights object.
	// This will allow the application to dynamically set lights in the scene
	lights = new Lights();

	init();
	glutMainLoop();
	return 0;
}