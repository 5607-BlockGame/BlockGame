//
// Created by Andrew Gazelka on 4/21/21.
//

#pragma once

template <class T>
struct Vec3D {
    T x, y, z;

    Vec3D(T x, T y, T z) : x(x), y(y), z(z) {}
};


template <class T>
struct Vec2D {
    T x,y,z;
};

typedef Vec2D<int> ChunkCoord;
