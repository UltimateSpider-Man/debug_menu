#pragma once

#include <cstdint>

#include "vector3d.h"
#include "vector4d.h"

#include "func_wrapper.h"

struct matrix4x4 {
    vector4d arr[4];

    auto& operator[](uint32_t idx) {
        return arr[idx];
    }



};

inline vector3d sub_501B20(const matrix4x4& a2, const vector3d& a3)
{
    vector3d result;
    CDECL_CALL(0x00501B20, &result, &a2, &a3);
    return result;
}