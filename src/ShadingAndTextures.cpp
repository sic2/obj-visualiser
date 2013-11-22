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
* Picking constants
*/
#define PICK_TOLLERANCE 5
#define PICK_BUFFER_SIZE 128
#define NO_HITS 0
#define HITS_OFFSET 3

/*
* Perspective view coefficients
*/
#define PERSPECTIVE_ANGLE_VIEW 60.0
#define PERSPECTIVE_NEAR 1.0
#define PERSPECTIVE_FAR 40.0

#define FPS_CONST 30.0

int width = 500; int height = 500;
const float ZOOM_FACTOR = 0.1;
const GLfloat DEGREES_ACCURACY = 5.0f;
GLfloat theta = 90.0f;
GLfloat phi = 0.0f;	
GLfloat zoom = 3.0f;

// Picking
unsigned int pickBuffer[PICK_BUFFER_SIZE];

// Pointers flags
bool lightsCreated = false;
bool camerasCreated = false;
bool objsDataCreated = false;

// Lights and animation
Lights* lights;
bool animateMainLight;
GLfloat lightAngle = 0.0f;
unsigned int FPS;
// Cameras
Cameras* cameras;
// Loaded Objects
std::vector<objLoader*> objsData;
// Textures
std::map< char*, GLuint >* textures;
bool enableTextures;

// Id-objects mapping (needed for picking) 
// id -> (face_ID, object)
std::map< int, std::pair<int, objLoader*> > mapFaceToObject;
unsigned int nextId = 0;

/*
* Preprocessors
*/
// Callbacks
void display();
void myReshape(int w, int h);
void keyboardFunc(unsigned char key, int x, int y);
void specialFunc(int key, int x, int y);
void Timer(int value);

// Application defined
void cleanup();
void shutDown(bool success);
void loadObjects(int argc, char** argv);
void processPicks(GLint hits, GLuint buffer[], int x, int y);
void picking(int x, int y);

/**
* Display all objects in the scene.
* iterate over each object and apply appropriate material/texture
*
* At each frame lights and cameras are updated, if necessary
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
		printf("facecount is %d\n", (*iter)->faceCount);
		for(int i = 0; i < (*iter)->faceCount; ++i)
		{
			obj_face *o = (*iter)->faceList[i];

			obj_material *mtl = (*iter)->materialList[ o->material_index ];
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
			}
			else
			{
				glDisable(GL_TEXTURE_2D);
			}

			glPushName(nextId);
			mapFaceToObject.insert(std::make_pair(nextId, std::make_pair(i, (*iter))));
			nextId++;
			glBegin(GL_TRIANGLE_FAN); // NOTE: Assume faces are triangulated
			for(int j = 0; j < o->vertex_count; ++j)
			{
				if (o->normal_index[ j ] == -1)
				{
					printf("Normal vector/s missing\n");
					shutDown(false);
				}
				obj_vector *normalList = (*iter)->normalList[ o->normal_index[ j ] ];
				glNormal3f(normalList->e[0], 
						normalList->e[1], 
						normalList->e[2]); 

				if (o->texture_index[j] != -1 && enableTextures)
					// Flip along y-direction, to compensate CImg behaviour
					glTexCoord2f((*iter)->textureList[ o->texture_index[j] ]->e[0],
								 1 - (*iter)->textureList[ o->texture_index[j] ]->e[1]);
				
				obj_vector *vertexList = (*iter)->vertexList[ o->vertex_index[ j ] ];
				glVertex3f(vertexList->e[0], 
						vertexList->e[1], 
						vertexList->e[2]);
			}		
			glEnd();

			glPopName();
		}
	}
	glPopMatrix();
	glutSwapBuffers();
}

/** 
* Loads any object specified by the user
* @param char* argv[] is the array of object (string) to load
* if not object filename is passed, then the program shuts down
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

void Timer(int value) 
{
	if (FPS > 0.0)
	{
		lightAngle += DEGREES_ACCURACY; 
		glutPostRedisplay();     
		glutTimerFunc(FPS, Timer, 0);
	}
}

void keyboardFunc(unsigned char key, int x, int y)
{
	switch(key)
	{
	case ESC_SIGN: shutDown(true);
		break;
	case SPACE_SIGN: 
		if(animateMainLight = !animateMainLight) 
		{
			FPS = FPS_CONST;
			glutTimerFunc(0, Timer, 1);
		}
		else 
		{
			FPS = 0.0f;
		}
		break;
	case L_SIGN: case l_SIGN: lights->nextLights();
		break;
	case T_SIGN: case t_SIGN: enableTextures = !enableTextures;
		break;
	case V_SIGN: case v_SIGN: {Camera_Location cl = cameras->nextCamera(); zoom = cl.zoom; theta = cl.theta; phi = cl.phi;}
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
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{	
		picking(x, y);
	}
	else if ((button == 3) || (button == 4)) // Wheel reports as button 3(scroll up) and button 4(scroll down)
	{
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
		(button == 3) ? zoom -= ZOOM_FACTOR : zoom += ZOOM_FACTOR;
	}
	glutPostRedisplay();
}

/*  
 * Note: this is the exact same method used in practical 2.
 *
 * Note: This method would not work for heights < 0,
 * 		unless index is changed to GLint*
 */
void processPicks(GLint hits, GLuint buffer[], int x, int y)
{
	GLuint* index = (GLuint *)buffer;
	unsigned int maxHeightHit = 0;
	for (unsigned int i = 0; i < (unsigned int) hits && hits > (int) NO_HITS ; ++i)
	{
		index += (int) HITS_OFFSET; // skip zmin and zmax
		if (*index > maxHeightHit) maxHeightHit = *index;
		printf("hit at %d \n", *index);
		index++; // next hit 
	}
	 if (hits > (int) NO_HITS) 
	 	printf("picked face %d\n", mapFaceToObject[maxHeightHit].first);
}

/*
* Note: this is the exact same method used in practical 2.
*/
void picking(int x, int y)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glSelectBuffer(PICK_BUFFER_SIZE, pickBuffer);
	glRenderMode(GL_SELECT);

	glInitNames();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluPickMatrix((GLdouble) x, (GLdouble) (viewport[3] - y), PICK_TOLLERANCE, PICK_TOLLERANCE, viewport);
	gluPerspective(PERSPECTIVE_ANGLE_VIEW, width/height, PERSPECTIVE_NEAR, PERSPECTIVE_FAR);
	display();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glFlush();

	processPicks(glRenderMode(GL_RENDER), pickBuffer, x, y);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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