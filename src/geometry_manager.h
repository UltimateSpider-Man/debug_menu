#pragma once

#include "func_wrapper.h"
#include "float.hpp"

#include "matrix4x4.h"



namespace geometry_manager {
    enum xform_t
    {
        XFORM_WORLD_TO_VIEW = 0,
        XFORM_VIEW_TO_WORLD = 1,
        XFORM_VIEW_TO_PROJECTION = 2,
        XFORM_PROJECTION_TO_SCREEN = 3,
        XFORM_EFFECTIVE_WORLD_TO_VIEW = 4,
        XFORM_VIEW_TO_SCREEN = 5,
        XFORM_WORLD_TO_PROJECTION = 6,
        XFORM_WORLD_TO_SCREEN = 7,
        NUM_XFORMS = 9
    };
  inline  Var<matrix4x4[NUM_XFORMS]> xforms{ 0x0095FC60 };
inline void enable_scene_analyzer(bool a1)
{
    CDECL_CALL(0x00515730, a1);
}

inline bool is_scene_analyzer_enabled()
{
    return (bool)CDECL_CALL(0x00515720);
}

inline void set_field_of_view(Float fov)
{
    CDECL_CALL(0x0053FFF0);
}

inline matrix4x4& get_xform(xform_t a1) {
    return xforms()[a1];
}


};


