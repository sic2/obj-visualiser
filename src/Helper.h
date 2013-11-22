#pragma once

#include "OpenGLHeaders.h"

#include "objLoader.h"

#include <string>
#include <cstdarg>
#include <map>
#include <vector>

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

    /**
    * Load an .obj file and return an object as defined by objLoader.h
    * @param [IN] filename of texture
    * @param [OUT] textures map used by environment
    * @param [OUT] materials vector used by environment
    * @return loaded object
    */
    objLoader* getObjData(char* obj_filename, std::map< char*, GLuint > *textures, std::vector<obj_material*> *materials);
        
protected:
    static Helper sInstance;
        
private:
    Helper() {}
    virtual ~Helper() {}

    void writeBitmapString(void *font, std::string str);
    std::string integerToString(int value);

    /*
    * Load texture from file
    * @param [IN] filename of texture
    * @param [OUT] textures map used by environment
    * @return True if texture could be loaded successfully
    */
    bool loadExternalTexture(char* texture_filename, std::map< char*, GLuint > *textures);

};