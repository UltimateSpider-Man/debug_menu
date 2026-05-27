#pragma once
#include "float.hpp"

#include <cmath>

struct vector3d
{
    float x, y, z;

    // Element access. Both overloads return `float` by value so MSVC's
    // overload resolution doesn't get confused by the return-type
    // mismatch (returning `const float&` vs `float`) that the previous
    // version had — MSVC was picking the wrong overload in a few
    // call-sites and producing C2440 "no conversion from vector3d to
    // int" errors. For write access, use `.x` / `.y` / `.z` directly.
    float operator[](int i) const {
        return (&x)[i];
    }

    Float length2() const {
#ifndef USE_GLM
        return (this->x * this->x + this->y * this->y + this->z * this->z);
#else
        return glm::length2(glm::vec3{ this->x, this->y, this->z });
#endif
    }

    float length() const {
        return std::sqrt(this->length2());
    }

    // Component-wise arithmetic — the engine code uses `v * 0.5f`, `a + b` etc.
    vector3d operator*(float s) const { return vector3d{ x * s, y * s, z * s }; }
    vector3d operator+(const vector3d& o) const { return vector3d{ x + o.x, y + o.y, z + o.z }; }
    vector3d operator-(const vector3d& o) const { return vector3d{ x - o.x, y - o.y, z - o.z }; }
    vector3d& operator+=(const vector3d& o) { x += o.x; y += o.y; z += o.z; return *this; }
    vector3d& operator-=(const vector3d& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    vector3d& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }


};

const vector3d ZEROVEC{0, 0, 0};


