#include "Helper.h"

// Std header files
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <fstream>

#include "CImg.h"

using namespace cimg_library;

#define WHITE_COLOR 1.0f
#define DEBUG 0

Helper Helper::sInstance;

Helper& Helper::instance()
{
	return sInstance;
}

void Helper::displayText(float x, float y, const char *fmt, ...)
{
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	float color = WHITE_COLOR;
	glColor3f(color, color, color);
	glRasterPos2f(x, y);

	std::string textToDisplay;

	va_list args;
	va_start(args, fmt);
	while (*fmt != '\0')
	{
		const char* tmp = fmt;
		++tmp;
		if (*fmt == '%' && *tmp == 'i')
		{			
			int value = va_arg(args, int);
			textToDisplay += integerToString(value);
			++fmt;
		}
		else
		{
			textToDisplay += *fmt;
		}
		++fmt;
	}
	va_end(args);

	writeBitmapString(GLUT_BITMAP_9_BY_15, textToDisplay.c_str());
	glPopAttrib();
}

objLoader* Helper::getObjData(char* obj_filename, std::map< char*, GLuint > *textures, std::vector<obj_material*> *materials)
{
	objLoader* objData = new objLoader();
	objData->load(obj_filename);

	printf("\nobj: %s - Number of materials: %i\n", obj_filename, objData->materialCount);
	for(int i=0; i<objData->materialCount; i++)
	{
		obj_material *mtl = objData->materialList[i];
		if (DEBUG)
		{
			printf("texture: %s\n", mtl->name);
			printf("\t amb_text %s \n", mtl->amb_texture_filename);
			printf("\t diff_text %s \n", mtl->diff_texture_filename);
			printf("\t spec_text %s \n", mtl->spec_texture_filename);
			printf("\t spec_hglt_cmp %s \n", mtl->spec_hghlt_cmp_filename);
			printf("\t alpha_text %s \n", mtl->alpha_texture_filename);
			printf("\t bump_text %s \n", mtl->map_bump_filename);
		}
		if (std::string(mtl->diff_texture_filename).size() != 0)
		{
			materials->push_back(mtl);
			Helper::instance().loadExternalTexture(mtl->diff_texture_filename, textures);
		}
	}

	return objData;
}

/*
 * PRIVATE METHODS
 */

/*
* This function is taken from the book:
* Computer Graphics Through OpenGL: From Theory to Experiments
* by Sumanta Guha
*/
void Helper::writeBitmapString(void *font, std::string str)
{  
	int len = str.length() + 1;
	char *cstr = new char[len];
	strcpy(cstr, str.c_str());
	for(int i = 0; i < len && cstr[i] != '\0'; i++)
	{
		glutBitmapCharacter(font, cstr[i]);
	}
	delete [] cstr;
}

std::string Helper::integerToString(int value)
{
	std::ostringstream convert; 
	convert << value;
	return convert.str();
}

// Load external textures.
bool Helper::loadExternalTexture(char* texture_filename, std::map< char*, GLuint > *textures)			
{
	typedef unsigned char uchar;
	// XXX - assume all textures are under director Data/
	CImg<uchar> src((std::string("Data/") + std::string(texture_filename)).c_str());

	// Reorder pixels
	// Idea got from official forum, but enhanced to support BW textures 
	// @see http://sourceforge.net/p/cimg/discussion/334630/thread/38f46281/
	CImg<uchar> texture(src,false);
	uchar* ptrs1 = src.data(0,0,0,0);
	uchar* ptrs2 = src.data(0,0,0,1);
	uchar* ptrs3 = src.data(0,0,0,2);

	uchar* ptrd = texture.data();
	const unsigned int size = src.width() * src.height() * src.spectrum();
	for (unsigned int i = 0; i < size; i += src.spectrum()) 
	{
		ptrd[i] = *ptrs1; ptrs1++; 
		if (src.spectrum() > 1)
		{
			ptrd[i + 1] = *ptrs2; ptrs2++;
			ptrd[i + 2] = *ptrs3; ptrs3++;
		}
	}

	GLuint texture_ID;
	glGenTextures(1, &texture_ID);
 	textures->insert(std::pair<char*, GLuint> (texture_filename, texture_ID));
 	glBindTexture(GL_TEXTURE_2D, (*textures)[texture_filename]);

 	// Set texture parameters for wrapping and filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Specify an image as the texture to be bound with the currently active texture index.
	if (texture.spectrum() == 1) // Black and White
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width(), texture.height(), 0, 
	         GL_LUMINANCE, GL_UNSIGNED_BYTE, ptrd);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width(), texture.height(), 0, 
			GL_RGB, GL_UNSIGNED_BYTE, ptrd);
	return true;
}