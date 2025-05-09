#include "debug_render.h"
#include "app.h"
#include "camera.h"
#include "color.h"
#include "common.h"

#include "femanager.h"
#include "fixedstring.h"
#include "func_wrapper.h"
#include "game.h"
#include "geometry_manager.h"
#include "line_info.h"

#include "ngl.h"

#include "os_developer_options.h"

#include "trace.h"
#include "variables.h"
#include "vector2di.h"
#include "vector3d.h"
#include "vtbl.h"
#include "wds_render_manager.h"



#include <array>
#include <cassert>
#include <cmath>

VALIDATE_SIZE(debug_line, 0x20);

VALIDATE_SIZE(debug_lines_t, 0x2808);

debug_lines_t debug_lines{};



Var<int[51]> min_values{0x009617A0};

Var<int[51]> max_values{0x009227E8};

Var<int[34]> debug_render_items = {0x00960D30};

int debug_render_get_ival(debug_render_items_e item) {
    assert(item < DEBUG_RENDER_ITEMS_COUNT);

    return debug_render_items()[item];
}

void debug_render_set_ival(debug_render_items_e a1, int a2)
{
    auto max = a2;
    if ( a2 > debug_render_get_max(a1) )
    {
        max = debug_render_get_max(a1);
    }

    auto min = debug_render_get_min(a1);
    if ( max >= min )
    {
        debug_render_items()[a1] = max;
    }
    else
    {
        debug_render_items()[a1] = min;
    }
}

bool debug_render_get_bval(debug_render_items_e item) {
    assert(item < DEBUG_RENDER_ITEMS_COUNT);

    return debug_render_items()[item] != 0;
}


int debug_render_get_min(debug_render_items_e item)
{
    assert(item < DEBUG_RENDER_ITEMS_COUNT);

    return min_values()[item];
}

int debug_render_get_max(debug_render_items_e item)
{
    assert(item < DEBUG_RENDER_ITEMS_COUNT);
    return max_values()[item];
}

//std::vector<vector3d> s_debug_hemisphere_pos{};

nglMesh *s_debug_hemisphere{nullptr};
nglMesh *s_debug_cylinder{nullptr};
nglMesh *s_debug_box{nullptr};
nglMesh *s_debug_disc{nullptr};



static color32 stru_1589F90[] = {color32{0, 255, 0, 128},
                                 color32{255, 255, 0, 128},
                                 color32{0, 255, 255, 128},
                                 color32{255, 255, 255, 128},
                                 color32{255, 0, 255, 128},
                                 color32{255, 128, 128, 128},
                                 color32{80, 80, 255, 128},
                                 color32{255, 80, 80, 128}};

