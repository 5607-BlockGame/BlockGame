#include <fstream>
#include <boost/format.hpp>
#include "utils.h"

namespace Utils {

    Model loadModel(const std::string &name) {
        std::ifstream file;
        file.open(name);

        if (!file.is_open()) {
            const auto msg = boost::format{"File %1% is not open for reading"} % name;
            throw std::invalid_argument(msg.str());
        }

        unsigned long numLines = 0;
        file >> numLines;
        std::vector<float> modelData(numLines, 0);

        for (unsigned int i = 0; i < numLines; i++) {
            file >> modelData[i];
        }

        file.close();

        return {
                .data = modelData
        };
    }

    void loadGlad() {
        if (gladLoadGLLoader(SDL_GL_GetProcAddress)) {
            printf("\nOpenGL loaded\n");
            printf("Vendor:   %s\n", glGetString(GL_VENDOR));
            printf("Renderer: %s\n", glGetString(GL_RENDERER));
            printf("Version:  %s\n\n", glGetString(GL_VERSION));
        } else {
            throw std::logic_error("ERROR: Failed to initialize OpenGL context.");
        }
    }

    void SDLInit() {
        SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)

        //Ask SDL to get a recent version of OpenGL (3.2 or greater)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    }

    GLuint InitShader(const char *vShaderFileName, const char *fShaderFileName) {
        GLuint vertex_shader, fragment_shader;
        GLchar *vs_text, *fs_text;
        GLuint program;

        // check GLSL version
        printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

        // Create shader handlers
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        // Read source code from shader files
        vs_text = readShaderSource(vShaderFileName);
        fs_text = readShaderSource(fShaderFileName);

        // error check
        if (vs_text == nullptr) {
            printf("Failed to read from vertex shader file %s\n", vShaderFileName);
            exit(1);
        }

        if (fs_text == nullptr) {
            printf("Failed to read from fragent shader file %s\n", fShaderFileName);
            exit(1);
        }

        // Load Vertex Shader
        const char *vv = vs_text;
        glShaderSource(vertex_shader, 1, &vv, nullptr);  //Read source
        glCompileShader(vertex_shader); // Compile shaders

        // Check for errors
        GLint  compiled;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            throw std::logic_error("Vertex shader failed to compile:\n");
        }

        // Load Fragment Shader
        const char *ff = fs_text;
        glShaderSource(fragment_shader, 1, &ff, nullptr);
        glCompileShader(fragment_shader);
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);

        //Check for Errors
        if (!compiled) {
            throw std::logic_error("Fragment shader failed to compile");
        }

        // Create the program
        program = glCreateProgram();

        // Attach shaders to program
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);

        // Link and set program to use
        glLinkProgram(program);

        return program;
    }

    char *readShaderSource(const char *shaderFile) {
        FILE *fp;
        long length;
        char *buffer;

        // open the file containing the text of the shader code
        fp = fopen(shaderFile, "r");

        // check for errors in opening the file
        if (fp == nullptr) {
            auto msg = boost::format{"can't open shader source file %1%"} % shaderFile;
            throw std::invalid_argument(msg.str());
        }

        // determine the file size
        fseek(fp, 0, SEEK_END); // move position indicator to the end of the file;
        length = ftell(fp);  // return the value of the current position

        // allocate a buffer with the indicated number of bytes, plus one
        buffer = new char[length + 1];

        // read the appropriate number of bytes from the file
        fseek(fp, 0, SEEK_SET);  // move position indicator to the startVertices of the file
        fread(buffer, 1, length, fp); // read all of the bytes

        // append a NULL character to indicate the end of the string
        buffer[length] = '\0';

        // close the file
        fclose(fp);

        // return the string
        return buffer;
    }

    unsigned int loadBMP(const std::string &filePath, bool transparent) {
        //// Allocate Texture 0 (Wood) ///////
        SDL_Surface *surface = SDL_LoadBMP(filePath.c_str());
        if (surface == nullptr) { //If it failed, print the error
            throw std::invalid_argument("could not load bmp");
        }
        GLuint tex;
        glGenTextures(1, &tex);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        //What to do outside 0-1 range
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        //Load the texture into memory
        if (transparent) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGR, GL_UNSIGNED_BYTE, surface->pixels);
        }

        glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture

        SDL_FreeSurface(surface);

        return tex;
    }

}

Model Model::combine(std::initializer_list<Model *> models) {

    std::vector<float> data;

    // second pass create textures array
    int i = 0;
    int iStart = 0;
    for (auto &model: models) {
        for (int j = 0; j < model->GetNumberLines(); ++j) {
            data.push_back(model->data[j]);
            i++;
        }
        model->startVertices = iStart >> 3;
        iStart = i;
    }

    return {
            .data = data
    };

}

std::ostream &operator<<(std::ostream &os, const Model &model) {
    os << "numLines: " << model.GetNumberLines() << " numVertices: " << model.GetNumberVertices() << " startVertices: " << model.startVertices;
    return os;
}
