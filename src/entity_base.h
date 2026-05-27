#pragma once

#include "common.h"
#include "entity_base_vhandle.h"
#include "func_wrapper.h"
#include "string_hash.h"
#include "utility.h"
#include "vtbl.h"

#include "damage_interface.h"

#include <cstdint>

struct po;

struct conglomerate;
struct motion_effect_struct;
struct sound_and_pfx_interface;
struct physical_interface;

struct entity_base {
    int m_vtbl;
    uint32_t field_4;
    uint32_t field_8;
    po* my_rel_po;
    string_hash field_10;
    po* my_abs_po;
    motion_effect_struct* field_18;
    entity_base_vhandle my_handle;
    entity_base* m_parent;
    entity_base* m_child;
    entity_base* field_28;
    int16_t proximity_map_cell_reference_count;
    uint8_t m_timer;
    void* adopted_children;
    conglomerate* my_conglom_root;
    sound_and_pfx_interface* my_sound_and_pfx_interface;
    int16_t field_3C;
    int16_t field_3E;
    int8_t field_40;
    int8_t field_41;
    int8_t rel_po_idx;
    int8_t proximity_map_reference_count;

    bool is_flagged(unsigned int a2) const
    {
        return (a2 & this->field_4) != 0;
    }

    bool has_damage_ifc() {
        return false;
    }

    void update_abs_po(bool a2) {

        constexpr auto n_bytes = 512;
        entity_base** ents = nullptr;


        THISCALL(0x004DB590, this, a2);
    }

    bool is_ext_flagged(uint32_t a2) const {
        return (a2 & this->field_8) != 0;
    }


    damage_interface* damage_ifc() {
        return (damage_interface*)field_4;
    }

    auto get_id() const {
        return this->field_10;
    }

    bool has_physical_ifc() {
        if constexpr (1) {
            bool(__fastcall * func)(entity_base*) = CAST(func, get_vfunc(m_vtbl, 0x124));
            return func(this);

        }
        else {
            return false;
        }
    }

    physical_interface* physical_ifc() {
        //assert(0 && "Accessing an invalid interface");
        //return nullptr;

        physical_interface* (__fastcall * func)(entity_base*) = CAST(func, get_vfunc(m_vtbl, 0x128));
        return func(this);
    }
};

VALIDATE_SIZE(entity_base, 0x44u);

