#include "Helper.h"
#include "Lights.h"
#include "objLoader.h"

/*
* Perspective view coefficients
*/
#define PERSPECTIVE_ANGLE_VIEW 40.0
#define PERSPECTIVE_NEAR 1.0
#define PERSPECTIVE_FAR 10.0

// Window dimensions
int width = 500; int height = 500;
objLoader *objData;

Lights* lights;

void keyboardFunc(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 27: exit(EXIT_SUCCESS);
		break;
	default: printf("key %d unknown \n", key);
		break;
	} // end switch
	glutPostRedisplay();
}

const GLfloat RADIANS_ACCURACY = 0.5f;
GLfloat zRadians = 0.0f;
GLfloat xRadians = 0.0f;	
GLfloat zoom = 7.0f;


void display()
{
	glPushMatrix();

	gluLookAt(zRadians, xRadians, zoom,  
			0.0, 0.0, 0.0,
			0.0, 1.0, 0.0);

	// TODO - could set new light position here (see red book #5): glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate a texture.
	// glBindTexture(GL_TEXTURE_2D, texture[0]); // TODO - do not hardcode id 

	// @see http://stackoverflow.com/questions/7735203/setting-separate-material-properties-for-different-objects-in-opengl
	// @see http://stackoverflow.com/questions/15451209/how-to-export-vertex-normal-vn-in-obj-file-with-blender-using-particle-system
	for(int i=0; i < objData->faceCount; i++)
	{
		obj_face *o = objData->faceList[i];

		// TODO - use switch
		if (o->vertex_count == 2)
		{
			glBegin(GL_LINES);
		}
		if (o->vertex_count == 3)
		{
			glBegin(GL_TRIANGLES);
		}
		else if (o->vertex_count == 4)
		{
			glBegin(GL_QUADS);
		}
		
		glNormal3f(objData->normalList[i]->e[0], objData->normalList[i]->e[1], objData->normalList[i]->e[2]); 
		for(int j=0; j < o->vertex_count && j < 4; j++) // FIXME - assume 4 vertices
		{
			obj_material *mtl = objData->materialList[o->material_index];

			float mat_ambient[] = {mtl->amb[0], mtl->amb[1], mtl->amb[2], 1.0};
			float mat_specular[] = {mtl->spec[0], mtl->spec[1], mtl->spec[2], 1.0};
			float mat_diffuse[] = {mtl->diff[0], mtl->diff[1], mtl->diff[2], 1.0};
			float shininess = mtl->shiny;
			glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
			glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
			glMaterialf(GL_FRONT, GL_SHININESS, shininess);

			glVertex3f(objData->vertexList[ o->vertex_index[j] ]->e[0], 
				objData->vertexList[ o->vertex_index[j] ]->e[1], 
				objData->vertexList[ o->vertex_index[j] ]->e[2]);
		}		
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


void init(void)
{
	/* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);

	/* Setup the view of the cube. */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(PERSPECTIVE_ANGLE_VIEW, width/height, PERSPECTIVE_NEAR, PERSPECTIVE_FAR);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Turn on OpenGL texturing.
	//glEnable(GL_TEXTURE_2D);
	// Specify how texture values combine with current surface color values.
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 

	// PROTOTYPING
	objData = new objLoader();
	objData->load("Data/sphere.obj");
}

int main(int argc, char **argv)
{	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("Test");

	// Registering callbacks
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardFunc);

	// Creates the lights object.
	// This will allow the application to dynamically set lights in the scene
	lights = new Lights();

	init();
	glutMainLoop();
	return 0;
}