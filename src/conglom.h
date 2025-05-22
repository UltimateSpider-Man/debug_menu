#pragma once

#include "actor.h"
#include "mashable_vector.h"
#include "variable.h"
#include "parse_generic_mash.h"

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
    mashable_vector<entity_base*> members;
    mashable_vector<entity_base*> skin_bones;
    mashable_vector<po> member_abs_po;
    mashable_vector<po> all_rel_po;
    mashable_vector<po> all_model_po;
    mashable_vector<int8_t> field_E8;
    mashable_vector<int8_t> field_F0;
    light_manager* field_F8;
    _std::list<actor*>* field_FC;
    _std::list<light_source*>* field_100;
    int field_104;
    float field_108;
    float field_10C;
    uint32_t field_110;
    als_res_data* field_114;
    skeleton_interface* skeleton_ifc;
    animation_interface* field_11C;
    script_data_interface* script_data_ifc;
    tentacle_interface* field_124;
    decal_data_interface* my_decal_data_interface;
    variant_interface* m_variant_interface;

void create_parentage_tree();


    void create_variant_ifc();


    void sub_4D0E00();


    bool render_complex_shadow(Float camera_distance);


    void render_simple_shadow(Float arg0, Float arg4);


    bool has_tentacle_ifc();


    bool has_variant_ifc();


    void _render(Float a2);

    float get_colgeom_radius();


    vector3d* get_colgeom_center();


    void _un_mash(generic_mash_header* a2, void* a3, generic_mash_data_ptrs* a4);

    variant_interface* variant_ifc();


        bool is_cloned_conglomerate() const
    {
        return (this->field_110 & 0x40) != 0;
    }


    entity_base* get_bone(const string_hash& a2, bool a3);




};

inline Var<actor*> global_transfer_variable_the_conglom { 0x00959570 };

extern void conglomerate_patch();


