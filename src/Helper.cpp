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

// Load external textures.
bool Helper::loadExternalTexture(char* texture_filename, std::map< char*, GLuint > textures)			
{
	typedef unsigned char uchar;
	// FIXME - do not hard code location
	CImg<uchar> src((std::string("Data/") + std::string(texture_filename)).c_str()); // DEBUG - 
	//src.display();

	// Reorder pixels
	// @see http://sourceforge.net/p/cimg/discussion/334630/thread/38f46281/
	CImg<uchar> texture(src,false);
	uchar* ptrs1 = src.data(0,0,0,0);
	uchar* ptrs2 = src.data(0,0,0,1);
	uchar* ptrs3 = src.data(0,0,0,2);

	uchar* ptrd = texture.data();
	printf("spectrum %d \n", texture.spectrum());
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

 	textures.insert(std::pair<char*, GLuint> (texture_filename, textures.size()));
 	glBindTexture(GL_TEXTURE_2D, textures[texture_filename]);

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
