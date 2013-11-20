#include "Helper.h"
#include "Lights.h"
#include "objLoader.h"
#include "math.h"

#include <map>
#include <string>

/**

NOTES:
bump: http://www.paulsprojects.net/tutorials/simplebump/simplebump.html
bump mapping: http://nehe.gamedev.net/article/bump_mapping/25006/

**/

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
objLoader *objData;

Lights* lights;

const GLfloat RADIANS_ACCURACY = 0.1f;
GLfloat zRadians = 0.0f;
GLfloat xRadians = 0.0f;	
GLfloat zoom = 7.0f;

std::map< char*, GLuint > textures;

/*
* Preprocessors
*/
// Callbacks
void display();
void myReshape(int w, int h);
void keyboardFunc(unsigned char key, int x, int y);
void specialFunc(int key, int x, int y);

// Application defined
bool loadExternalTexture(char* texture_filename);
void cleanup();
void shutDown();
void init(void);

// Load external textures. // FIXME - use SOIL
bool loadExternalTexture(char* texture_filename)			
{
	BitMapFile* image = Helper::instance().getBMPData("Data/" + std::string(texture_filename));  // @see http://www.amazingtextures.com/

	textures.insert(std::pair<char*, GLuint> (texture_filename, textures.size()));
	glBindTexture(GL_TEXTURE_2D, textures[texture_filename]);

	// Set texture parameters for wrapping and filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Specify an image as the texture to be bound with the currently active texture index.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->sizeX, image->sizeY, 0, 
	            GL_RGB, GL_UNSIGNED_BYTE, image->data);
	
	return true;
}

// @see http://ogldev.atspace.co.uk/www/tutorial32/tutorial32.html
/*
TODO:
splitting into vertexarray by material
solution seen in forum
- this need an appropriate method and data structure
*/
void display()
{
	glPushMatrix();

	// Calculate the camera position using the distance and angles
	float camX = zoom * cos(zRadians); 
	float camY = zoom * sin(xRadians);
	float camZ = zoom;
	// Set the camera position and lookat point
	gluLookAt(camX,camY,camZ,   // Camera position
          0.0, 0.0, 0.0,    // Look at point
          0.0, 1.0, 0.0);   // Up vector

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// @see http://stackoverflow.com/questions/7735203/setting-separate-material-properties-for-different-objects-in-opengl
	// @see http://stackoverflow.com/questions/15451209/how-to-export-vertex-normal-vn-in-obj-file-with-blender-using-particle-system
	for(int i=0; i < objData->faceCount; i++)
	{
		obj_face *o = objData->faceList[i];

		// NOTE: Assume faces are triangulated
		glBegin(GL_TRIANGLE_FAN);
		for(int j=0; j < o->vertex_count; j++)
		{
			// Normals
			glNormal3f(objData->normalList[ o->normal_index[ j ] ]->e[0], 
				objData->normalList[ o->normal_index[ j ] ]->e[1], 
				objData->normalList[ o->normal_index[ j ] ]->e[2]); 

			// Materials
			obj_material *mtl = objData->materialList[ o->material_index ];
			float mat_ambient[] = {mtl->amb[0], mtl->amb[1], mtl->amb[2], 1.0};
			float mat_specular[] = {mtl->spec[0], mtl->spec[1], mtl->spec[2], 1.0};
			float mat_diffuse[] = {mtl->diff[0], mtl->diff[1], mtl->diff[2], 1.0};
			float shininess = mtl->shiny;
			glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
			glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
			glMaterialf(GL_FRONT, GL_SHININESS, shininess);

			// Textures - // blender tutorial for vt missing: http://www.idevgames.com/forums/thread-5344.html
			glBindTexture(GL_TEXTURE_2D, textures[mtl->diff_texture_filename]);
			if (o->texture_index[j] != -1)
				glTexCoord2f(objData->textureList[ o->texture_index[j] ]->e[0],
							objData->textureList[ o->texture_index[j] ]->e[1]);
			
			// Vertices
			glVertex3f(objData->vertexList[ o->vertex_index[j] ]->e[0], 
				objData->vertexList[ o->vertex_index[j] ]->e[1], 
				objData->vertexList[ o->vertex_index[j] ]->e[2]);
		}		
		glEnd();
	}
	glPopMatrix();
	glutSwapBuffers();
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

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 

	// XXX - PROTOTYPING - loading objects
	printf("Loading obj file...\n");
	objData = new objLoader(); //TODO - pass dir to objLoader
	objData->load("Data/cube.obj");
	printf("obj file loaded\n");

	printf("Number of materials: %i\n", objData->materialCount);
	for(int i=0; i<objData->materialCount; i++)
	{
		obj_material *mtl = objData->materialList[i];
		printf("texture: %s\n", mtl->name);
		printf("\t amb_text %s \n", mtl->amb_texture_filename);
		printf("\t diff_text %s \n", mtl->diff_texture_filename);
		printf("\t spec_text %s \n", mtl->spec_texture_filename);
		printf("\t spec_hglt_cmp %s \n", mtl->spec_hghlt_cmp_filename);
		printf("\t alpha_text %s \n", mtl->alpha_texture_filename);
		printf("\t bump_text %s \n", mtl->map_bump_filename);

		loadExternalTexture(mtl->diff_texture_filename);
		printf("\n");
	}
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

	// Creates the lights object.
	// This will allow the application to dynamically set lights in the scene
	lights = new Lights();

	init();
	glutMainLoop();
	return 0;
}