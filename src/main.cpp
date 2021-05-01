#include "glad.h"  //Include order can matter here
#include <SDL.h>
#include <cstdio>
#include <generator/SimpleGenerator.h>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shader macro
#define GLSL(src) "#version 150 core\n" #src

//For Visual Studios
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define GLM_FORCE_RADIANS

//#include "gtc/matrix_transform.hpp"
//#include "gtc/type_ptr.hpp"


#include "utils.h"
#include "State.h"
#include "vis/Scene.h"

using namespace std;


const float FOV_Y = 3.14f / 4;
const float STRAFE_SPEED = 3.0f;
const float ZNEAR = 1.0;
const float ZFAR = 100.0;
const float EXTRA_FACTOR = 3.0f;
const float KEY_DIST = 0.5f;
const float KEY_HEIGHT = -0.1f;

const float JUMP_VEL = 0.07;
const float FLY_VEL = 2.0;
const float ACC_G = 0.2f;

const float MOUSE_SENSITIVITY = 0.001;

void handleMouseMove(State &state, const SDL_MouseMotionEvent &event, SDL_Window *window) {
    state.angle += MOUSE_SENSITIVITY * (float) event.xrel;
    state.angle2 += MOUSE_SENSITIVITY * (float) event.yrel;
    if(state.angle2 >= M_PI_2) state.angle2 = M_PI_2 - 0.0001;
    if(state.angle2 <= -M_PI_2) state.angle2 = -M_PI_2 - 0.0001;
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
            break;
        case SDLK_a:
        case SDLK_d:
            state.movement.sideStrafe = 0;
            break;
        case SDLK_w:
        case SDLK_s:
            state.movement.forwardStrafe = 0;
            break;
        case SDLK_SPACE:
        case SDLK_LSHIFT:
            state.movement.velocityZ = 0;
        default:
            break;
    }
}

void handleKeyHold(State &state, int code) {
    auto &movement = state.movement;
    switch (code) {
        case SDLK_a:
//            movement.sideStrafe -= 1;
            movement.sideStrafe = -1;
            break;
        case SDLK_d:
            movement.sideStrafe = 1;
            break;
        case SDLK_w:
            movement.forwardStrafe = 1;
            break;
        case SDLK_s:
            movement.forwardStrafe = -1;
            break;
        case SDLK_SPACE:
            state.movement.velocityZ = FLY_VEL;
            // if on ground
//            if (state.camPosition[2] == 0.0) { TODO: re-implement
//                state.movement.velocityZ = JUMP_VEL;
//            }
            break;
        case SDLK_LSHIFT:
            state.movement.velocityZ = -FLY_VEL;
            break;
        default:
            break;
    }
}

bool fullscreen = true;
//int screen_width = 1000;
//int screen_height = 600;

char window_title[] = "My OpenGL Program";

float avg_render_time = 0;

int main(int argc, char *argv[]) {


    Utils::SDLInit();

    SDL_ShowCursor(SDL_DISABLE);

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    auto screen_width = displayMode.w;
    auto screen_height = displayMode.h;

    SDL_Window *window = SDL_CreateWindow(window_title, 100, 100, screen_width, screen_height, SDL_WINDOW_FULLSCREEN_DESKTOP);

    SDL_SetWindowGrab(window, SDL_TRUE);

    float aspect = static_cast<float>(screen_width) / static_cast<float>(screen_height);

    //Create brickCube context to draw in
    SDL_GLContext context = SDL_GL_CreateContext(window);

    Utils::loadGlad();
//
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,16);

    auto woodTexture = Utils::loadBMP("textures/wood.bmp", false);
    const auto WOOD_TEXTURE_ID = 0;

    auto brickTexture = Utils::loadBMP("textures/brick.bmp", false);
    const auto BRICK_TEXTURE_ID = 1;

    auto crossHairTexture = Utils::loadBMP("textures/crosshair.bmp", true);
    const auto CROSSHAIR_TEXTURE_ID = 2;

    Model cubeModel = Utils::loadModel("models/cube.txt");

    // combine into one array and change pointers of other models
    Model combined = Model::combine({&cubeModel});

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

    // Crosshair data
    CrossHair crossHair;

    GLuint vao2d;
    glGenVertexArrays(1, &vao2d);
    glBindVertexArray(vao2d); // Bind the above created VAO to the current context

    // Allocate memory on the graphics card to store geometry (vertex buffer object)
    GLuint vbo2d[1];
    glGenBuffers(1, vbo2d);  // Create 1 buffer called vbo2d
    glBindBuffer(GL_ARRAY_BUFFER, vbo2d[0]); // Set the vbo2d as the active array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(crossHair.vertices), crossHair.vertices, GL_STATIC_DRAW); //upload vertices to vbo2d

    auto textured2dShader = Utils::InitShader("shaders/vertex2D.glsl", "shaders/fragment2D.glsl");

    GLint vertexAttrib = glGetAttribLocation(textured2dShader, "vertex");
    glEnableVertexAttribArray(vertexAttrib);
    glVertexAttribPointer(vertexAttrib, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

//    glEnable(GL_MULTISAMPLE); // multiple samples

    SimpleGenerator generator;

    World world(generator);

    Scene scene(world, texturedShader, textured2dShader, cubeModel, crossHair);


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
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    state.isMining = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    state.isMining = false;
                    break;
            }

            SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
        }


        auto &movement = state.movement;

        glm::vec3 lookDirGlm(1.0,0.0,0.0);
        glm::vec3 moveDirGlm(STRAFE_SPEED * movement.forwardStrafe, -STRAFE_SPEED * movement.sideStrafe,movement.velocityZ);


        glm::mat4 rotMat(1);
        rotMat = glm::rotate(rotMat, -state.angle, glm::vec3(0,0,1));
        moveDirGlm = glm::vec3(rotMat * glm::vec4(moveDirGlm, 1.0));

        rotMat = glm::rotate(rotMat, state.angle2, glm::vec3(0,1,0));

        lookDirGlm = glm::vec3(rotMat * glm::vec4(lookDirGlm, 1.0));

        Vec3 lookDir = lookDirGlm;
        Vec3 moveDir = moveDirGlm;
        Vec3 dirExtra = moveDir * EXTRA_FACTOR;

        auto oldPosition = state.camPosition;
        Vec3 extraPosition = state.camPosition + dirExtra;

        if (moveDir.hasMagnitude()) {
            if (scene.IsCollision(extraPosition)) {
                state.camPosition = oldPosition;
            } else{
                state.camPosition += moveDir;
            }
        }

        // Animate hand if mining block
        if (state.isMining) {
            if (state.handRotation <= -20.0 * M_PI / 180.0) state.handRotation = 20.0 * M_PI / 180.0;
            else state.handRotation -= 3.0 * M_PI / 180.0;
        } else {
            state.handRotation = -20.0 * M_PI / 180.0;
        }

        // Clear the screen to default color
        glClearColor(.2f, 0.4f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(texturedShader); //Set the active shader (only one can be used at brickCube time)
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
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

        Vec3 center = state.camPosition + lookDir;
        Vec3 up(0.0f, 0.0f, 1.0f);

        // set view matrix
        glm::mat4 view = glm::lookAt(to_glm(state.camPosition), to_glm(center), to_glm(up));

        GLint uniView = glGetUniformLocation(texturedShader, "view");
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

        // Set Perspective ::: zNear zFar frustum
        glm::mat4 proj = glm::perspective(FOV_Y, aspect, ZNEAR, ZFAR);
        GLint uniProj = glGetUniformLocation(texturedShader, "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

        glBindVertexArray(vao);

        scene.Draw(state.camPosition);
        scene.DrawPlayer(state.camPosition, lookDir, state.handRotation, view);

        // Switch to 2D to render crosshair
        glUseProgram(textured2dShader); //Set the active shader
        glDisable(GL_DEPTH_TEST);

        // Tutorial: https://learnopengl.com/Advanced-OpenGL/Blending
        glEnable(GL_BLEND);     // Enable  blending because texture has transparency
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Tutorial: https://learnopengl.com/In-Practice/2D-Game/Rendering-Sprites
        glBindVertexArray(vao2d);
        glBindBuffer(GL_ARRAY_BUFFER, vbo2d[0]);

        glm::mat4 proj2D = glm::ortho(0.0f, (float) screen_width, (float) screen_height, 0.0f, -1.0f, 1.0f);
        GLint uniProj2D = glGetUniformLocation(textured2dShader, "projection");
        glUniformMatrix4fv(uniProj2D, 1, GL_FALSE, glm::value_ptr(proj2D));

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, crossHairTexture);

        scene.DrawCrossHair(screen_width/2, screen_height/2);

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
    glDeleteProgram(textured2dShader);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, vbo2d);
    glDeleteVertexArrays(1, &vao2d);

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}

