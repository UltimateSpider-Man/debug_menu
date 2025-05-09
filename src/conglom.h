#pragma once

#include "actor.h"
#include "mashable_vector.h"
#include "variable.h"

#include "skeleton_interface.h"

struct animation_interface;
struct skeleton_interface;
struct script_data_interface;
struct tentacle_interface;
struct decal_data_interface;
struct variant_interface;
struct als_res_data;
struct light_manager;
struct light_source;

namespace als {
struct animation_logic_system;
}

struct conglomerate : actor {
    mashable_vector<entity_base *> members;
    mashable_vector<entity_base *> skin_bones;
    mashable_vector<po> member_abs_po;
    mashable_vector<po> all_rel_po;
    mashable_vector<po> all_model_po;
    mashable_vector<int8_t> field_E8;
    mashable_vector<int8_t> field_F0;
    light_manager *field_F8;
    _std::list<actor *> *field_FC;
    _std::list<light_source *> *field_100;
    _std::list<void *> *field_104;
    float field_108;
    float field_10C;
    uint32_t field_110;
    als_res_data *field_114;
    skeleton_interface skeleton_ifc;
    animation_interface *m_animation_ifc;
    script_data_interface *m_script_data_ifc;
    tentacle_interface *m_tentacle_interface;
    decal_data_interface *my_decal_data_interface;
    variant_interface *m_variant_interface;
    variant_interface *variant_ifc;
    


};

inline Var<actor *> global_transfer_variable_the_conglom {0x00959570};


