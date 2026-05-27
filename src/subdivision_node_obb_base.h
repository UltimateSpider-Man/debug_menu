#pragma once

#include <cstdint>

#include "vector3d.h"
#include "vector4d.h"
#include "float.hpp"
#include "variable.h"

struct vector3d;

struct subdivision_node_obb_base {
    int field_0;
    vector3d field_4;
    int field_10;
    uint16_t field_14;
    uint16_t field_16;
    vector3d field_18;
    float field_24[3];
    float field_30[3];
    vector3d field_3C;


    void get_vertices(vector3d* out) const
    {
        THISCALL(0x00513100, this, out);
    }

    bool sub_564E80(vector4d& a2, vector4d& a3, vector4d& a4, vector4d& a5) const
    {

            return (bool)THISCALL(0x00564E80, this, &a2, &a3, &a4, &a5);
        
    }


    bool point_inside_or_on(const vector3d& a2) const
    {

       
            return (bool)THISCALL(0x0052BD30, this, &a2);
        
    }

    static inline Var<int> visit_key{0x0095C894};
};


