
#pragma once

#include "float.hpp"
#include "mstring.h"
#include "variable.h"




#include <cmath>


#include <cstdio>


inline constexpr auto LARGE_EPSILON = 0.0099999998;
inline constexpr auto EPSILON = 0.000099999997;
inline constexpr float SAMPLING_PERIOD = 0.033433333f - EPSILON;
inline constexpr float DURATION_EPSILON = 0.00050000002f;
inline constexpr auto MAX_ALLOWED_POSITION_LENGTH_SQUARED = 1.0e10;

struct euler_direction;
struct vector4d;

struct vector3d
{
    float x, y, z;

    vector3d() : x(0), y(0), z(0) {}

    explicit vector3d(float f) : x(f), y(f), z(f) {}

    constexpr vector3d(float a1, float a2, float a3) : x(a1), y(a2), z(a3) {}

    vector3d(const vector3d& v) : x(v[0]), y(v[1]), z(v[2]) {
    }


    const float& operator[](int idx) const {
        return (&x)[idx];
    }

    float& operator[](int idx) {
        return (&x)[idx];
    }
    bool operator!=(const vector3d& a2) const
    {
        return !(*this == a2);
    }
    bool operator==(const vector3d& v) const   {
        return !(*this == v);
    };





 inline   void operator+=(const vector3d& v) {
        *this = *this + v;
    }

 inline   vector3d operator+(float f) const {
        vector3d result = (*this);
        result[0] = result[0] + f;
        result[1] = result[1] + f;
        result[2] = result[2] + f;

        return result;
    }

  inline  vector3d operator-(float f) const {
        vector3d result = (*this);
        result[0] = result[0] - f;
        result[1] = result[1] - f;
        result[2] = result[2] - f;

        return result;
    }

   inline vector3d operator+(const vector3d& v) const {
        vector3d result;

        result[0] = x + v[0];
        result[1] = y + v[1];
        result[2] = z + v[2];

        return result;
    }

  inline  vector3d operator-() const {
        return ((*this) * -1.f);
    }

 inline   vector3d& operator-=(const vector3d& vec3) {
        x -= vec3[0];
        y -= vec3[1];
        z -= vec3[2];

        return (*this);
    }

   inline vector3d operator-(const vector3d& vec3) const {
        vector3d result;

        result[0] = x - vec3[0];
        result[1] = y - vec3[1];
        result[2] = z - vec3[2];

        return result;
    }

 inline   vector3d operator*(float a1) const {
        vector3d result;
        result[0] = x * a1;
        result[1] = y * a1;
        result[2] = z * a1;

        return result;
    }

  inline  friend vector3d operator*(float a1, const vector3d& a2) {
        vector3d result = a2 * a1;

        return result;
    }



  inline  void operator*=(float a1) {
        x *= a1;
        y *= a1;
        z *= a1;
    }

   inline vector3d operator/(float a1) const {
        vector3d result;
        result[0] = x * (1.0 / a1);
        result[1] = y * (1.0 / a1);
        result[2] = z * (1.0 / a1);

        return result;
    }
 /* inline vector3d sub_501B20(const matrix4x4& a2, const vector3d& a3)
    {
        vector3d result;
        CDECL_CALL(0x00501B20, &result, &a2, &a3);
        return result;
    }*/
    void operator/=(float a1) {
        auto v1 = (1.0 / a1);
        x *= v1;
        y *= v1;
        z *= v1;
    }
    Float length2() const {
#ifndef USE_GLM
        return (this->x * this->x + this->y * this->y + this->z * this->z);
#else
        return glm::length2(glm::vec3{ this->x, this->y, this->z });
#endif
    }
    void normalize() {
#ifndef USE_GLM
        float length2 = this->length2();

        if (length2 > 9.9999994e-11) {
            float v3 = 1.0f / sqrt(length2);
            this->x *= v3;
            this->y *= v3;
            this->z *= v3;
        }
#else
        glm::vec3 vec = glm::vec3{ this->x, this->y, this->z };

        vec = glm::fastNormalize(vec);

        this->x = vec.x;
        this->y = vec.y;
        this->z = vec.z;
#endif
    }
};







inline float dot(const vector3d& a1, const vector3d& a2) {
#ifndef USE_GLM
    return a1[2] * a2[2] + a1[1] * a2[1] + a1[0] * a2[0];
#else
    return glm::dot(glm::vec3{ a1[0], a1[1], a1[2] }, glm::vec3{ a2[0], a2[1], a2[2] });
#endif
}


const vector3d ZEROVEC{ 0, 0, 0 };

