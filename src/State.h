#pragma once

#include <repr/location.h>
#include <glm/vec3.hpp>

enum class Look {
    NONE,
    LEFT,
    RIGHT
};

enum class Strafe {
    NONE,
    FORWARD,
    BACKWARD
};


struct Movement {
    float sideStrafe = 0.0;
    float forwardStrafe =0.0;
    float velocityY = 0.0;
    static Movement Default(){
        Movement movement;
        return movement;
    }
};



struct State {
    bool quit = false;
    bool fullscreen = false;
    Vec3 camPosition;
    float angle;
    float angle2;
    Movement movement;

    [[nodiscard]] inline bool isRunning() const{
        return !quit;
    }
};

