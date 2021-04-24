#pragma once

#include <vector>
#include <string>
#include "glad.h"  //Include order can matter here
#include <SDL.h>
#include <SDL_opengl.h>
//#include <vec3.hpp>
//#include <type_ptr.hpp>
#include <ostream>
#include <iostream>


struct Model {
    unsigned int startVertices = 0;
    std::vector<float> data;

    [[nodiscard]] unsigned int GetNumberLines() const{
        return data.size();
    }

    [[nodiscard]] unsigned int GetNumberVertices() const{
        return GetNumberLines() >> 3; // divide by 8
    }

    static Model combine(std::initializer_list<Model*> models);

    void draw() const{
        glDrawArrays(GL_TRIANGLES, startVertices, GetNumberVertices()); //(Primitive Type, Start Vertex, Num Verticies)
    }

    friend std::ostream &operator<<(std::ostream &os, const Model &model);
};

namespace Utils {
    Model loadModel(const std::string &name);

    void loadGlad();

    void SDLInit();

    GLuint InitShader(const char *vShaderFileName, const char *fShaderFileName);

// Create a NULL-terminated string by reading the provided file
    char *readShaderSource(const char *shaderFile);

    void drawGeometry(unsigned int shaderProgram, const Model& model1, const Model& model2, float colR, float colG, float colB);

    unsigned int loadBMP(const std::string &filePath);

}


