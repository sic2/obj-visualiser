#include "Helper.h"
#include "Lights.h"
#include "Cameras.h"

#include <map>
#include <vector>
#include <string>

/**

NOTES:
bump: http://www.paulsprojects.net/tutorials/simplebump/simplebump.html
bump mapping: http://nehe.gamedev.net/article/bump_mapping/25006/

multitexturing: http://nehe.gamedev.net/tutorial/bump-mapping,_multi-texturing_&_extensions/16009/
	http://www.opengl.org/wiki/Multitexture_with_GLSL
	http://tfpsly.free.fr/english/index.html?url=http://tfpsly.free.fr/english/3d/multitexturing.html
	http://www.lighthouse3d.com/tutorials/glsl-tutorial/multi-texture/

**/

/*
* Key bindings
*/
#define ESC_SIGN 27
#define PLUS_SIGN 43
#define MINUS_SIGN 45
#define SPACE_SIGN 32
#define C_SIGN 67
#define c_SIGN 99
#define L_SIGN 76
#define l_SIGN 108
#define T_SIGN 84
#define t_SIGN 116
#define V_SIGN 86
#define v_SIGN 118

/*
* Perspective view coefficients
*/
#define PERSPECTIVE_ANGLE_VIEW 60.0
#define PERSPECTIVE_NEAR 1.0
#define PERSPECTIVE_FAR 40.0

int width = 500; int height = 500;
const float ZOOM_FACTOR = 0.1;
const GLfloat DEGREES_ACCURACY = 5.0f;
GLfloat theta = 90.0f;
GLfloat phi = 0.0f;	
GLfloat zoom = 3.0f;

// Pointers flags
bool lightsCreated = false;
bool camerasCreated = false;
bool objsDataCreated = false;

Lights* lights;
bool animateMainLight;
GLfloat lightAngle = 0.0f;

Cameras* cameras;

std::vector<objLoader*> objsData;

std::map< char*, GLuint >* textures;
bool enableTextures;

/*
* Preprocessors
*/
// Callbacks
void display();
void myReshape(int w, int h);
void keyboardFunc(unsigned char key, int x, int y);
void specialFunc(int key, int x, int y);
void idleFunc();

// Application defined
void cleanup();
void shutDown(bool success);
void loadObjects(int argc, char** argv);

/**
* TODO - docs
*/
void display()
{
	lights->reApply(lightAngle);
	
	glPushMatrix();
	cameras->moveTo(zoom, theta, phi);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(std::vector< objLoader* >::iterator iter = objsData.begin();
			iter != objsData.end(); ++iter)
	{
		objLoader* objData = *iter;
		// FIXME - needed for myface.obj
		// glScalef(0.5, 0.5, 0.5);
		// glTranslatef(0.0, -3.0, 0.0);
		for(int i=0; i < objData->faceCount; i++)
		{
			obj_face *o = objData->faceList[i];
			for(int j=0; j < o->vertex_count; j++)
			{
				// Normals
				glNormal3f(objData->normalList[ o->normal_index[ j ] ]->e[0], 
						objData->normalList[ o->normal_index[ j ] ]->e[1], 
						objData->normalList[ o->normal_index[ j ] ]->e[2]); 

				// Materials
				obj_material *mtl = objData->materialList[ o->material_index ];
				float mat_ambient[] 	= {mtl->amb[0], mtl->amb[1], mtl->amb[2], 1.0};
				float mat_specular[] 	= {mtl->spec[0], mtl->spec[1], mtl->spec[2], 1.0};
				float mat_diffuse[] 	= {mtl->diff[0], mtl->diff[1], mtl->diff[2], 1.0};
				float shininess = mtl->shiny;
				glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
				glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
				glMaterialf(GL_FRONT, GL_SHININESS, shininess);

				if (enableTextures)
				{	
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, (*textures)[mtl->diff_texture_filename]);
					if (o->texture_index[j] != -1)
						// Flip along y-direction, to compensate CImg behaviour
						glTexCoord2f(objData->textureList[ o->texture_index[j] ]->e[0],
									 1 - objData->textureList[ o->texture_index[j] ]->e[1]);
				}
				else
				{
					glDisable(GL_TEXTURE_2D);
				}

				glBegin(GL_TRIANGLE_FAN); // NOTE: Assume faces are triangulated
				glVertex3f(objData->vertexList[ o->vertex_index[j] ]->e[0], 
						objData->vertexList[ o->vertex_index[j] ]->e[1], 
						objData->vertexList[ o->vertex_index[j] ]->e[2]);
			}		
			glEnd();
		}
	}
	glPopMatrix();
	glutSwapBuffers();
}

/** 
* TODO - docs
*/ 
void loadObjects(int argc, char** argv)
{
	if (argc <= 1)
	{
		printf("No arguments given to render scene\n");
		shutDown(false);
	}

	enableTextures = true;
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // GL_MODULATE allows color and light to be combined with textures.

	textures = new std::map< char*, GLuint >();
	for(int i = 1; i < argc; i++)
	{
		objsData.push_back(Helper::instance().getObjData(argv[i], textures));
	}
	objsDataCreated = true;
}

/**
* Ensures that reshaping the windows keep the model in the correct ratio
*/
void myReshape(int w, int h)
{	
	width = w; height = h;

	glEnable(GL_DEPTH_TEST); // Use depth buffering for hidden surface elimination.

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(PERSPECTIVE_ANGLE_VIEW, width/height, PERSPECTIVE_NEAR, PERSPECTIVE_FAR);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboardFunc(unsigned char key, int x, int y)
{
	switch(key)
	{
	case ESC_SIGN: shutDown(true);
		break;
	case SPACE_SIGN: (animateMainLight = !animateMainLight) ? glutIdleFunc(idleFunc) : glutIdleFunc(NULL);
		break;
	case L_SIGN: case l_SIGN: lights->nextLights();
		break;
	case T_SIGN: case t_SIGN: enableTextures = !enableTextures;
		break;
	case V_SIGN: case v_SIGN: Camera_Location cl = cameras->nextCamera(); zoom = cl.zoom; theta = cl.theta; phi = cl.phi;
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
	case GLUT_KEY_LEFT: phi -= DEGREES_ACCURACY;
		break;
	case GLUT_KEY_RIGHT: phi += DEGREES_ACCURACY; 
		break;
	case GLUT_KEY_UP: theta -= DEGREES_ACCURACY;
		break;
	case GLUT_KEY_DOWN: theta += DEGREES_ACCURACY;
		break;
	default: printf("Unused special key %d pressed\n", key);
		break;
	}
	phi = fmod((double)phi, 360.0); theta = fmod((double)theta, 360.0);
	glutPostRedisplay();
}

// Mouse scrolling snippet from stackoverflow.com
// @see http://stackoverflow.com/questions/14378/using-the-mouse-scrollwheel-in-glut
void mouse(int button, int state, int x, int y)
{
	if ((button == 3) || (button == 4)) // Wheel reports as button 3(scroll up) and button 4(scroll down)
	{
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
		(button == 3) ? zoom -= ZOOM_FACTOR : zoom += ZOOM_FACTOR; // FIXME - set limits
	}
	glutPostRedisplay();
}

void idleFunc()
{
	lightAngle += DEGREES_ACCURACY; 
	glutPostRedisplay();
}

/**
* Delete any allocated resources
*/
void cleanup()
{
	if (lightsCreated)
		delete lights;
	if (camerasCreated)
		delete cameras;
	if (objsDataCreated)
	{
		for(std::vector< objLoader* >::iterator iter = objsData.begin();
			iter != objsData.end(); ++iter)
		{
			delete (*iter);
		}
		delete textures;
	}
}

/**
* Shuts down the application.
* This involves a call to #cleanup() as well.
*/
void shutDown(bool success)
{
	cleanup();
	if (success)	
		exit(EXIT_SUCCESS);
	else
		exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("Shading and Textures");

	// Registering callbacks
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardFunc);
	glutSpecialFunc(specialFunc);
	glutMouseFunc(mouse);

	// Creates the lights object.
	// This will allow the application to dynamically set lights in the scene
	animateMainLight = false;
	lights = new Lights();
	lightsCreated = true;

	cameras = new Cameras();
	camerasCreated = true;

	loadObjects(argc, argv);
	glutMainLoop();
	return 0;
}