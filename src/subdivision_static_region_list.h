

#pragma once

#include "variable.h"
#include "vector3d.h"

#include <vector>
#include "trace.h"
#include "func_wrapper.h"
#include "scratchpad_stack.h"

struct region;
struct subdivision_node;
struct subdivision_visitor;

struct stack_allocator;
struct proximity_map_construction_leaf;

struct traverse_test {
    int field_0;
    vector3d field_4;
    float field_10;
};


struct region_mirror_data {
    region *field_0;
    vector3d field_4;
    vector3d field_10;

    region_mirror_data(region *);
};

struct static_region_list_methods {
    std::intptr_t m_vtbl;

   static void term()
    {
        if constexpr (1) {
            scratchpad_stack::pop(scratchpad(), 0x40);
            scratchpad() = nullptr;
        }
        else {
            CDECL_CALL(0x00513FF0);
        }
    }

static void init()
{
    TRACE("static_region_list_methods::init");

    if constexpr (1) {
        scratchpad() = static_cast<int *>(scratchpad_stack::alloc(0x40));
        for ( int i = 0; i < 16; ++i ) {
            scratchpad()[i] = 0;
        }
    } else {
        CDECL_CALL(0x00513F90);
    }
}



    static inline Var<int *> scratchpad{0x0095C8B8};

    static inline Var<region_mirror_data *> mirror {0x0095C8BC};
};




