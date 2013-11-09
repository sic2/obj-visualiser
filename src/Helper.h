#pragma once

#include <string>
#include <cstdarg>

// Struct of bitmap file.
struct BitMapFile
{
   int sizeX;
   int sizeY;
   unsigned char *data;
};

/*
* Use singleton pattern as described in
* Professional C++ - second edition. Gregorie, Solter and Kleper
*/
class Helper
{
        
public:
    static Helper& instance();
     
    /**
    * Display a given text in the glut window.
    * @param x, y 	position of the text (from left)
    * @param fmt 	text to display. The char i is substituted by 
    *				any integer in the sequence '...' if following '%'
    *				i.e. "This is a test %i", 10
    *				prints: 
    *					This is a test 10
    * Note: the color of the text is fixed to white
    * 		the dimension of the text is fixed to 9x15 pixels
    */
    void displayText(float x, float y, const char *fmt, ...);

    /*
    * Routine to read a bitmap file.
    * Works only for uncompressed bmp files of 24-bit color.
    *
    * @param fileName (.bmp) to be loaded
    * @return bitMapFile {width, height, data*}
    */
    BitMapFile* getBMPData(std::string filename);
        
protected:
    static Helper sInstance;
        
private:
    Helper() {}
    virtual ~Helper() {}

    void writeBitmapString(void *font, std::string str);
    std::string integerToString(int value);
};