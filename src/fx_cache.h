#pragma once

#include "entity.h"
#include "entity_base_vhandle.h"
#include "float.hpp"
#include "mashable_vector.h"

struct generic_mash_header;
struct cached_special_effect;
struct generic_mash_data_ptrs;

struct fx_cache_ent {
    float field_0;
    int field_4;
};

struct fx_cache {
    int field_0;
    int field_4;
    mashable_vector<fx_cache_ent> field_8;
    fx_cache_ent *field_10;

 
};

