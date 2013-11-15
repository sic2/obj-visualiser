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

GLuint	texture[1];	// FIXME
// Load external textures.
bool loadExternalTexture(char* texture_filename)			
{
	// Local storage for bmp image data.
	BitMapFile *image[1];
	// Load the texture.
	image[0] = Helper::instance().getBMPData("Data/" + std::string(texture_filename));  // @see http://www.amazingtextures.com/

	// Activate texture index texture[0]. 
	glBindTexture(GL_TEXTURE_2D, texture[0]); 

	// Set texture parameters for wrapping.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Specify an image as the texture to be bound with the currently active texture index.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0, 
	            GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);

	printf("Texture %s loaded and set \n", texture_filename);
	return true;
}

void display()
{
	glPushMatrix();

	gluLookAt(zRadians, xRadians, zoom,  
			0.0, 0.0, 0.0,
			0.0, 1.0, 0.0);

	// TODO - could set new light position here (see red book #5): glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate a texture.
	glBindTexture(GL_TEXTURE_2D, texture[0]); // TODO - do not hardcode id 

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
			// glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
			// glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
			// glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
			// glMaterialf(GL_FRONT, GL_SHININESS, shininess);

			// blender tutorial for vt missing: http://www.idevgames.com/forums/thread-5344.html
			glTexCoord2f(objData->textureList[ j ]->e[0], objData->textureList[ j ]->e[1]);
			//printf("%f %f \n", objData->textureList[ j ]->e[0], objData->textureList[ j ]->e[1]);
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
	glEnable(GL_TEXTURE_2D);
	// Specify how texture values combine with current surface color values.
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 

	// PROTOTYPING
	printf("Loading obj file...\n");
	objData = new objLoader(); //TODO - pass dir to objLoader
	objData->load("Data/cube.obj");
	printf("obj file loaded\n");

	printf("Number of materials: %i\n", objData->materialCount);
	for(int i=0; i<objData->materialCount; i++)
	{
		obj_material *mtl = objData->materialList[i];
		printf(" texture: %s\n", mtl->texture_filename);
		loadExternalTexture(mtl->texture_filename);
		printf("\n");
	}

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