

#pragma once

#include "wds.h"
#include "mission_manager.h"
#include "resource_key.h"
#include "func_wrapper.h"
#include "region.h"

#include "string_hash.h"
#include "subdivision_node_obb_base.h"

#include <cassert>
#include <cstdio>

struct region;





struct proximity_map;

#pragma once


#include "vector3d.h"

#include <cstdint>

struct region;
struct subdivision_node;

inline constexpr auto MAX_REGIONS_TO_FIND_FROM_POINT = 4;





struct subdivision_node_obb_base;



struct region2 {
    char field_0[0x2C];
    region_mash_info* mash_info;
    void* region_entities;
    int field_34;
    int field_38;
    int field_3C;
    int field_40;
    int field_44;
    int field_48;
    int field_4C;
    int flags;
    uint8_t unk1[0x50];
    vector3d field_A4;
    float field_B0[3];
    float field_BC;
    uint8_t unk3[0x4];
    uint8_t variants;
    uint8_t unk4[0x6B];
    subdivision_node_obb_base* obb;


    bool is_loaded() const {
        return (this->flags & 0x10) != 0;
    }


};




struct terrain
{
    region** regions;
    int field_4;
    int field_8;
    int total_regions;
    int strips;
    int total_strips;
    float field_18[3];
    region2** regions2;
    proximity_map* region_map;


    region* get_region(int idx)
    {
        assert(idx >= 0);
        assert(idx < total_regions);

        return this->regions[idx];
    }


    int get_num_regions() const
    {
        return this->total_regions;
    }

    void update_region_pack_info()
    {
        void(__fastcall * func)(void*) = bit_cast<decltype(func)>(0x0054F380);
        func(this);
    }

    region* find_region(string_hash a2)
    {

        region* (__fastcall * func)(void*, int, string_hash) = bit_cast<decltype(func)>(0x00534920);
        return func(this, 0, a2);
    }



    region* find_region2(const vector3d& a2, const region2* a3)
    {
        


            return (region*)THISCALL(0x0052DFF0, this, &a2, a3);


    }

    region* find_region(const vector3d& a2, const region* a3)
    {
        region* (__fastcall * func)(void*, int, const vector3d*, const region*) = bit_cast<decltype(func)>(0x0052DFF0);
        return func(this, 0, &a2, a3);
    }




    void show_obbs()
    {

    }

    void unload_district_immediate(int a2)
    {
        auto func = bit_cast<fastcall_call>(0x005571B0);
        func(this, 0, a2);
    }

    void unload_all_districts_immediate()
    {
        void(__fastcall * func)(void*) = (decltype(func))0x00557350;
        func(this);
    }

    void set_district_variant(int a2, int variant, bool a4)
    {
        void(__fastcall * func)(void*, void*, int, int, bool) =
            bit_cast<decltype(func)>(0x00557480);

        func(this, nullptr, a2, variant, a4);
    }

    void unlock_district(int a2) {
        void(__fastcall * func)(void*, void*, int) =
            bit_cast<decltype(func)>(0x005148A0);

        func(this, nullptr, a2);
    }

};




