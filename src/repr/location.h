//
// Created by Andrew Gazelka on 4/21/21.
//

#pragma once

template <class T>
struct Vec3D {
    T x, y, z;
};


template <class T>
struct Vec2D {
    T x,y,z;
};

typedef Vec2D<int> ChunkCoord;
