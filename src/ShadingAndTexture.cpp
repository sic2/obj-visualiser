#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <sstream>


/*
* Key bindings
*/
#define ESC_SIGN 27
#define PLUS_SIGN 43
#define MINUS_SIGN 45
#define C_SIGN 67
#define c_SIGN 99
#define TWO_SIGN 50
#define THREE_SIGN 51
#define SPACE_SIGN 32
#define OPEN_ANGULAR_SIGN 60
#define CLOSE_ANGULAR_SIGN 62

const float X_MIN = -1.0f;
const float X_MAX = 1.0f;
const float Y_MIN = -1.0f;
const float Y_MAX = 1.0f;
const float PERSPECTIVE_ANGLE_OF_VIEW = 90.0f;
const float PERSPECTIVE_NEAR = 0.0f;
const float PERSPECTIVE_FAR = 1.0f;

// Window dimensions
int width = 500; int height = 500;

// View settings
bool useOrthoProj = true;

/*
* Preprocessors
*/
// Callbacks
void display();
void myReshape(int w, int h);
void keyboardFunc(unsigned char key, int x, int y);
void specialFunc(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void menu(int);

// Projections
void orthoProj(int w, int h);
void perspectiveProj(int w, int h);

// Application defined
void setup(int w, int h);
void cleanup();
void shutDown();
void processPicks(GLint hits, GLuint buffer[], int x, int y);
void picking(int x, int y);

/* ----------- *
 *  Functions  *
 *  ---------- */ 

void display()
{
	glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT);
	// TOOD
	glPopMatrix();

	glFlush();
}

/**
* Set the parameters for the ortho-projection
*/
void orthoProj(int w, int h)
{
	if (w <= h)
	{
		gluOrtho2D(X_MIN, X_MAX, 
			Y_MIN * (GLfloat) h / (GLfloat) w, Y_MAX * (GLfloat) h / (GLfloat) w);
	}
	else
	{
		gluOrtho2D(X_MIN * (GLfloat) w / (GLfloat) h, X_MAX * (GLfloat) w / (GLfloat) h, 
			Y_MIN, Y_MAX);
	}
}

/**
* Set the perspective projection
*
* Note: this works only on Linux - Lab machines
*/
void perspectiveProj(int w, int h)
{
	gluPerspective(PERSPECTIVE_ANGLE_OF_VIEW, w / h, PERSPECTIVE_NEAR, PERSPECTIVE_FAR); 
}

/**
* Ensures that reshaping the windows keep the model in the correct ratio
*/
void myReshape(int w, int h)
{	
	width = w; height = h;
	setup(w, h);
}

void keyboardFunc(unsigned char key, int x, int y)
{
	// TODO
	glutPostRedisplay();
}

void specialFunc(int key, int x, int y)
{
	// TODO
	glutPostRedisplay();
}

void setup(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
   	glLoadIdentity();
	if (useOrthoProj)
	{	
		orthoProj(w, h);
	}
	else
	{
		perspectiveProj(w, h);
	}
	glMatrixMode(GL_MODELVIEW);
}

/**
* Delete any allocated resources
*/
void cleanup()
{
	// TODO
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

int main(int argc, char **argv)
{	
	glutInit(&argc, argv);
	glutInitWindowSize(width, height);
	glutCreateWindow("Shading and Texture");

	// Registering callbacks
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardFunc);
	glutSpecialFunc(specialFunc);

	glClearColor(0.0,0.0,0.0,1.0);
	glColor3f(1.0,1.0,1.0);

	glutMainLoop();
	return 0;
}
