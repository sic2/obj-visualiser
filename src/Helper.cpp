#include "Helper.h"

#include "OpenGLHeaders.h"

// Std header files
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <fstream>

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

/*
* This function is taken from the book:
* Computer Graphics Through OpenGL: From Theory to Experiments
* by Sumanta Guha
*/
BitMapFile* Helper::getBMPData(std::string filename)
{
   BitMapFile *bmp = new BitMapFile;
   unsigned int size, offset, headerSize;
  
   // Read input file name.
   std::ifstream infile(filename.c_str(), std::ios::binary);
 
   // Get the starting point of the image data.
   infile.seekg(10);
   infile.read((char *) &offset, 4); 
   
   // Get the header size of the bitmap.
   infile.read((char *) &headerSize,4);

   // Get width and height values in the bitmap header.
   infile.seekg(18);
   infile.read( (char *) &bmp->sizeX, 4);
   infile.read( (char *) &bmp->sizeY, 4);

   // Allocate buffer for the image.
   size = bmp->sizeX * bmp->sizeY * 24;
   bmp->data = new unsigned char[size];

   // Read bitmap data.
   infile.seekg(offset);
   infile.read((char *) bmp->data , size);
   
   // Reverse color from bgr to rgb.
   int temp;
   for (unsigned int i = 0; i < size; i += 3)
   { 
      temp = bmp->data[i];
	  bmp->data[i] = bmp->data[i+2];
	  bmp->data[i+2] = temp;
   }

   return bmp;
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
