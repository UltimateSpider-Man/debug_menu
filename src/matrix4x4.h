


#pragma once

#include "vector4d.h"

#include "vector3d.h"



struct matrix4x4 {
    vector4d arr[4];




    auto& operator[](int idx) {
        return arr[idx];
    }

    const auto& operator[](int idx) const {
        return arr[idx];
    }


    friend vector3d operator*(const matrix4x4& a2, const vector3d& a3) {
        vector3d result;
        result[0] = a2[0][0] * a3[0] + a2[1][0] * a3[1] + a2[2][0] * a3[2] + a2[3][0];
        result[1] = a2[0][1] * a3[0] + a2[1][1] * a3[1] + a2[2][1] * a3[2] + a2[3][1];
        result[2] = a2[0][2] * a3[0] + a2[1][2] * a3[1] + a2[2][2] * a3[2] + a2[3][2];

        return result;
    }
    friend matrix4x4 operator*(const matrix4x4& a2, const matrix4x4& a3) {
        matrix4x4 result;
        CDECL_CALL(0x00587EB0, &result, a2, &a3);
    }




};
