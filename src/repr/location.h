//
// Created by Andrew Gazelka on 4/21/21.
//

#pragma once

#include <ostream>
#include <glm/vec3.hpp>
#include <boost/functional/hash.hpp>

template<
        typename T, //real type
        typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
>
struct Vec3D {
    Vec3D<int>(Vec3D<long> vec3D) : x(vec3D.x), y(vec3D.y), z(vec3D.z) {}

    Vec3D<double>(glm::vec3 vec3D) : x(vec3D[0]), y(vec3D[1]), z(vec3D[2]) {}

    operator glm::vec3() const {
        return {x, y, z};
    }


    T x, y, z;

    Vec3D(T x, T y, T z) : x(x), y(y), z(z) {}


    Vec3D<T> operator>>(const int amount) const {
        return {x >> amount, y >> amount, z >> amount};
    }

    Vec3D<T> operator+(const Vec3D<T> &other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    bool operator==(const Vec3D &rhs) const {
        return x == rhs.x &&
               y == rhs.y &&
               z == rhs.z;
    }

    bool operator!=(const Vec3D &rhs) const {
        return !(rhs == *this);
    }

};


inline glm::vec3 to_glm(Vec3D<double> vec3D) {
    return glm::vec3(vec3D.x, vec3D.y, vec3D.z);
}


template<class T>
struct Vec2D {
    T x, y;

    Vec2D(T x, T y) : x(x), y(y) {}

    bool operator==(const Vec2D &rhs) const {
        return x == rhs.x &&
               y == rhs.y;
    }

    bool operator!=(const Vec2D &rhs) const {
        return !(rhs == *this);
    }
};

namespace std {

    template<typename T>
    struct hash<Vec2D<T>> {
        std::size_t operator()(Vec2D<T> const &v) const {
            std::size_t seed = 0;
            boost::hash_combine(seed, v.x);
            boost::hash_combine(seed, v.y);
            return seed;
        }
    };

}


typedef Vec2D<int> ChunkCoord;
typedef Vec3D<double> PlayerLoc;
typedef Vec3D<long> BlockLocation;
typedef Vec3D<double> Vec3;
