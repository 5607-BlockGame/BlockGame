#include "glad.h"  //Include order can matter here
#include <SDL.h>
#include <SDL_opengl.h>
#include <cstdio>
#include <iostream>

// Shader macro
#define GLSL(src) "#version 150 core\n" #src

//For Visual Studios
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define GLM_FORCE_RADIANS

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "utils.h"
#include "State.h"
#include "Scene.h"
#include "parse/MapParser.h"

using namespace std;


const float FOV_Y = 3.14f / 4;
const float STRAFE_SPEED = 0.07;
const float LOOK_SPEED = 0.03;
const float ZNEAR = 0.01;
const float ZFAR = 10.0;
const float EXTRA_FACTOR = 3.0f;
const float KEY_DIST = 0.5f;
const float KEY_HEIGHT = -0.1f;

const float JUMP_VEL = 0.07;
const float ACC_G = 0.2f;

const float MOUSE_SENSITIVITY = 0.001;

void handleMouseMove(State &state, const SDL_MouseMotionEvent &event, SDL_Window *window) {
    state.angle += MOUSE_SENSITIVITY * (float) event.xrel;
    state.angle2 += MOUSE_SENSITIVITY * (float) event.yrel;
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    // center mouse
    SDL_WarpMouseInWindow(window, width / 2, height / 2);
    SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);

}

void handleKeyPress(State &state, int code) {
    switch (code) {
        case SDLK_q:
        case SDLK_ESCAPE:
            state.quit = true;
            break;
        case SDLK_f:
            state.fullscreen = !state.fullscreen;
        case SDLK_a:
        case SDLK_d:
            state.movement.sideStrafe = 0;
        case SDLK_s:
        case SDLK_w:
            state.movement.forwardStrafe = 0;
        default:
            break;
    }
}

void handleKeyHold(State &state, int code) {
    auto &movement = state.movement;
    switch (code) {
        case SDLK_a:
            movement.sideStrafe = -1;
            break;
        case SDLK_d:
            movement.sideStrafe = 1;
            break;
        case SDLK_w:
//            movement.forwardStrafe += 1;
            movement.forwardStrafe = 1;
            break;
        case SDLK_s:
            movement.forwardStrafe = -1;
            break;
        case SDLK_SPACE:
            // if on ground
            if (state.camPosition[2] == 0.0) {
                state.movement.velocityY = JUMP_VEL;
            }
            break;
        default:
            break;
    }
}

bool fullscreen = false;
int screen_width = 800;
int screen_height = 600;

char window_title[] = "My OpenGL Program";

float avg_render_time = 0;

int main(int argc, char *argv[]) {

    Map map = MapParser::parseMap("maps/test.txt");

    Utils::SDLInit();

    SDL_ShowCursor(SDL_DISABLE);

    //Create brickCube window (offsetx, offsety, width, height, flags)
    SDL_Window *window = SDL_CreateWindow(window_title, 100, 100, screen_width, screen_height, SDL_WINDOW_OPENGL);

    SDL_SetWindowGrab(window, SDL_TRUE);

    float aspect = static_cast<float>(screen_width) / static_cast<float>(screen_height);

    //Create brickCube context to draw in
    SDL_GLContext context = SDL_GL_CreateContext(window);

    Utils::loadGlad();

    auto woodTexture = Utils::loadBMP("textures/wood.bmp");
    const auto WOOD_TEXTURE_ID = 0;

    auto brickTexture = Utils::loadBMP("textures/brick.bmp");
    const auto BRICK_TEXTURE_ID = 1;

    Model cubeModel = Utils::loadModel("models/cube.txt");
    Model knotModel = Utils::loadModel("models/knot.txt");
    Model teapotModel = Utils::loadModel("models/teapot.txt");


    // combine into one array and change pointers of other models
    Model combined = Model::combine({&cubeModel, &knotModel, &teapotModel});

    //Build brickCube Vertex Array Object (VAO) to store mapping of shader attributse to VBO
    GLuint vao;
    glGenVertexArrays(1, &vao); //Create brickCube VAO
    glBindVertexArray(vao); //Bind the above created VAO to the current context

    //Allocate memory on the graphics card to store geometry (vertex buffer object)
    GLuint vbo[1];
    glGenBuffers(1, vbo);  //Create 1 buffer called vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at brickCube time)
    glBufferData(GL_ARRAY_BUFFER, combined.GetNumberLines() * sizeof(float), combined.data.data(), GL_STATIC_DRAW); //upload vertices to vbo

    auto texturedShader = Utils::InitShader("shaders/textured-Vertex.glsl", "shaders/textured-Fragment.glsl");

    GLint posAttrib = glGetAttribLocation(texturedShader, "position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(posAttrib);

    //GLint colAttrib = glGetAttribLocation(phongShader, "inColor");
    //glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    //glEnableVertexAttribArray(colAttrib);

    GLint normAttrib = glGetAttribLocation(texturedShader, "inNormal");
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (5 * sizeof(float)));
    glEnableVertexAttribArray(normAttrib);

    GLint texAttrib = glGetAttribLocation(texturedShader, "inTexcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));

//    GLint uniView = glGetUniformLocation(texturedShader, "view");
//    GLint uniProj = glGetUniformLocation(texturedShader, "proj");

    glBindVertexArray(0); //Unbind the VAO in case we want to create brickCube new one

    glEnable(GL_DEPTH_TEST);

    TexturedModel brickCube = {
            .model = cubeModel,
            .textureId = BRICK_TEXTURE_ID,
    };

    TexturedModel woodCube = {
            .model = cubeModel,
            .textureId = WOOD_TEXTURE_ID,
    };

    TextureData texturedData = {
            .wallModel = brickCube,
            .keyModel = knotModel,
            .floorModel = woodCube,
            .endModel= teapotModel,
            .doorModel = cubeModel,
    };

    Scene scene(texturedData, map, texturedShader);


    State state{
            .camPosition = scene.GetStartPosition()
    };

    //Event Loop (Loop forever processing each event as fast as possible)
    SDL_Event windowEvent;
    unsigned int t_prev = 0;
    while (state.isRunning()) {
        unsigned int t_start = SDL_GetTicks();

        if (t_prev == 0) {
            t_prev = t_start;
        }

        unsigned int millisPast = t_start - t_prev;
        float secondsPast = millisPast / 1000.F;

        if (!state.onGround() || state.movement.velocityY != 0.0f) {
            state.camPosition[2] += state.movement.velocityY;
            if (state.camPosition[2] < 0.0f) { // so we always hit ground
                state.camPosition[2] = 0.0f;
            }

            state.movement.velocityY -= ACC_G * secondsPast;
        }

        // reset movement
//        state.movement.sideStrafe = 0.0;
//        state.movement.forwardStrafe = 0.0;

        while (SDL_PollEvent(&windowEvent)) {
            switch (windowEvent.type) {
                case SDL_MOUSEMOTION:
                    handleMouseMove(state, windowEvent.motion, window);
                    break;
                case SDL_QUIT:
                    state.quit = true;
                    break;
                case SDL_KEYUP:
                    handleKeyPress(state, windowEvent.key.keysym.sym);
                    break;
                case SDL_KEYDOWN:
                    handleKeyHold(state, windowEvent.key.keysym.sym);
            }

            SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
        }


        auto& movement = state.movement;
        float dX = -STRAFE_SPEED * cos(state.angle) * movement.forwardStrafe + STRAFE_SPEED * sin(state.angle) * movement.sideStrafe;
        float dY = STRAFE_SPEED * sin(state.angle) * movement.forwardStrafe + STRAFE_SPEED * cos(state.angle) * movement.sideStrafe;
        glm::vec3 moveDir(dX, dY, 0.0f);
        glm::vec3 dirExtra(dX * EXTRA_FACTOR, dY * EXTRA_FACTOR, 0.0f);

        glm::vec3 lookDir(-cos(state.angle), sin(state.angle), -sin(state.angle2));
        glm::vec3 dKey(-KEY_DIST * cos(state.angle), KEY_DIST * sin(state.angle), KEY_HEIGHT);

        auto oldPosition = state.camPosition;

        glm::vec3 extraPosition = state.camPosition + dirExtra;
        state.camPosition += moveDir;

        if (dX != 0.0 || dY != 0.0) {

            float x = extraPosition[0];
            float y = extraPosition[1];

            if (scene.IsCollision(x, y)) {
                state.camPosition = oldPosition;
            }
        }

        auto keyPosition = state.camPosition + dKey;
        scene.UpdateGrabbedKeys(keyPosition, state.angle);

        // Clear the screen to default color
        glClearColor(.2f, 0.4f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(texturedShader); //Set the active shader (only one can be used at brickCube time)

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glUniform1i(glGetUniformLocation(texturedShader, "tex0"), WOOD_TEXTURE_ID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, brickTexture);
        glUniform1i(glGetUniformLocation(texturedShader, "tex1"), BRICK_TEXTURE_ID);

        unsigned int t_now = SDL_GetTicks();

        glm::mat4 model = glm::mat4(1);
        GLint modelLoc = glGetUniformLocation(texturedShader, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::vec3 center = state.camPosition + lookDir;
        glm::vec3 up(0.0f, 0.0f, 1.0f);

        // set view matrix
        glm::mat4 view = glm::lookAt(state.camPosition, center, up);

        GLint uniView = glGetUniformLocation(texturedShader, "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

        // Set Perspective ::: zNear zFar
        glm::mat4 proj = glm::perspective(FOV_Y, aspect, ZNEAR, ZFAR);
        GLint uniProj = glGetUniformLocation(texturedShader, "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

        glBindVertexArray(vao);

        scene.Draw();

        SDL_GL_SwapWindow(window); //Double buffering

        unsigned int t_end = SDL_GetTicks();
        char update_title[100];
        unsigned int time_per_frame = t_end - t_start;
        avg_render_time = .98f * avg_render_time + .02f * static_cast<float>(time_per_frame); //Weighted average for smoothing
        sprintf(update_title, "%s [Update: %3.0f ms]\n", window_title, static_cast<double>(avg_render_time));
        SDL_SetWindowTitle(window, update_title);

        t_prev = t_start;
    }


    //Clean Up
    glDeleteProgram(texturedShader);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}

