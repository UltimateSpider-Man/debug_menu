#pragma once

#include <cstdint>

#include "common.h"
#include "fixedstring32.h"

#include "damage_interface.h"
#include "func_wrapper.h"
#include "game.h"
#include "memory.h"
#include "mission_manager.h"
#include "mstring.h"
#include "os_developer_options.h"
#include "resource_key.h"
#include "settings.h"
#include "sound_manager.h"
#include "trace.h"
#include "utility.h"
#include "variables.h"
#include "wds.h"
#include "float.hpp"

#include "memoryunitmanager.h"


#include "log.h"
#include <direct.h>
#include <string.h>

#include <cassert>





#pragma once

#include <cstdint>

#include "variable.h"




struct game_data_essentials {
    short field_0;
    short field_2;
    short field_4;
    short field_6;
    short field_8;
    short field_A;
    int field_C;
    int field_10;
    char field_14[25];
    char field_2E[12];
};

VALIDATE_SIZE(game_data_essentials, 0x3C);

struct game_data_meat {
    int m_hero_points;
    int field_4;
    int m_upg_impact_web_pts;
    int m_upg_hero_meter_pts;
    int field_10;
    int field_14;
    int field_18;
    int m_hero_meter_level_1;
    int m_hero_meter_level_2;
    int m_hero_meter_level_3;
    int m_cur_hero_meter_lvl;
    int m_cur_hero_meter_pts;
    bool m_opt_score_display;
    bool field_31;
    int field_34;
    float field_38;
    float field_3C;
    int field_40;
    bool m_mini_map_enabled;
    bool m_enable_web_shot;
    bool field_46;
    bool field_47;
    bool m_web_gloves;
    bool m_adv_web_gloves;
    bool m_enable_impact_web;
    bool m_adv_impact_web;
    bool m_web_cowboy;
    bool field_4D;
    bool field_4E;
    bool field_4F;
    bool field_50;
    bool field_51;
    bool field_52;
    int m_difficulty;
    float m_run_sensitivity;
    bool field_5C;
    bool field_5D;
    bool m_show_style_points;
    float m_hero_health;
    int m_hero_type;
    int m_swing_speed;
    bool field_6C;
    bool field_6D;
    bool field_6E;
    bool field_6F;
    bool field_70;
    bool field_71;
    bool field_72;
    bool field_73;
    bool field_74;
    bool field_75;
    bool field_76;
    bool field_77;
    bool field_78;
    bool field_79;
    bool field_7A;
    bool field_7B;
    bool field_7C;
    bool field_7D;
    bool field_7E;
    int field_80;
    int field_84;
    int field_88;
    int field_8C;
    int field_90;
    float field_94;
    float field_98;
    float field_9C;
    float m_miles_web_zipping;
    float m_web_fluid_used;
    int field_A8;
    int field_AC;
    int field_B0;
    int field_B4;
    int field_B8;
    float field_BC;
    float field_C0;
    float field_C4;
    int field_C8;
    int field_CC;
    int field_D0;
    int field_D4;
    int field_D8;
    int field_DC;
    int field_E0;
    int field_E4;
    int field_E8;
    int field_EC;
    int field_F0;
    int field_F4;
    int field_F8;
    int field_FC;
    int field_100;
    int field_104;
    int field_108;
    int field_10C;
    int field_110;
    fixedstring32 field_114;
    fixedstring32 field_134;
};

struct game_settings {
    std::intptr_t m_vtbl;
    MemoryUnitManager::Container field_4;
    game_data_essentials field_28C[3]{};
    game_data_meat field_340;
    char *field_494;
    char *field_498;
    char *field_49C[3];
    char field_4A8[12];
    int field_4B4;
    int field_4B8;
    bool field_4BC[3];
    bool field_4BF;
    bool field_4C0;
    bool field_4C1;
    bool field_4C2;
    int m_slot_num;
    int field_4C8;











    void sub_5288B0(void* Memory)
    {
        CDECL_CALL(0x005288B0, Memory);
    }



    void Callback(MemoryUnitManager::eOperation a2) {
        THISCALL(0x0057C0B0, this, a2);
    }

    void init_script_buffer()
    {

            THISCALL(0x005799E0, this);
        
    }

    void update_miles_crawled_venom(Float a2)
    {
        this->field_340.field_C0 += a2 * 0.0006213712;
    }

    void update_miles_crawled_spidey(Float a2)
    {
        this->field_340.field_98 += a2 * 0.0006213712;
    }

    void start_new_game() {

        TRACE("game_settings::start_new_game");

        void(__fastcall * func)(const void*, void* edx) = CAST(func, 0x0057EAB0);
        return func(this, nullptr);
    }

    void frame_advance(Float a2)
    {

            THISCALL(0x005802D0, this, a2);

    }

    void export_game_options()
    {
        TRACE("game_settings::export_game_options");

        void(__fastcall * func)(const void*, void* edx) = CAST(func, 0x0057C1E0);
        return func(this, nullptr);
    }

    void export_game_settings()
    {

        TRACE("game_settings::export_game_settings");

        void(__fastcall * func)(const void*, void* edx) = CAST(func, 0x00579A10);
        return func(this, nullptr);
    }

    static constexpr auto NUM_SOFT_SAVE_BUFFERS = 2;

    void soft_load(uint32_t soft_save_type) {

            THISCALL(0x0057C1E0, this, soft_save_type);

    }

    void update_miles_run_venom(Float a2) {
        this->field_340.field_BC += a2 * 0.0006213712f;
    }

    void update_miles_web_zipping(Float a2) {
        this->field_340.m_miles_web_zipping += a2 * 0.0006213712f;
    }

    void update_miles_run_spidey(Float a2) {
        this->field_340.field_94 += a2 * 0.0006213712f;
    }



    void update_web_fluid_used(Float a2) {
        this->field_340.m_web_fluid_used += a2;
    }

    void reset_container(bool a2)
    {

        TRACE("game_settings::reset_container");

        void(__fastcall * func)(const void*, void* edx, bool) = CAST(func, 0x00579970);
        return func(this, nullptr, a2);
    }

    int load()
    {


        TRACE("game_settings::load");


        int(__fastcall * func)(const void*, void* edx) = CAST(func, 0x00579970);
        return func(this, nullptr);
    }

    void load_game(int slot_num)
    {


        TRACE("game_settings::load_game");

        void(__fastcall * func)(const void*, void* edx, int) = CAST(func, 0x0057F410);
        return func(this, nullptr, slot_num);

    }



    void load_most_recent_game()
    {


        TRACE("game_settings::load_most_recent_game");

        void(__fastcall * func)(const void*, void* edx) = CAST(func, 0x0057F580);
        return func(this, nullptr);
    }

    int* GetSystemDate(int* out) {


        TRACE("game_settings::GetSystemDate");

        int*(__fastcall * func)(const void*, void* edx, int*) = CAST(func, 0x00573510);
        return func(this, nullptr, out);
    }

    void collect_game_settings()
    {


            TRACE("game_settings::collect_game_settings");

            void(__fastcall * func)(const void*, void* edx) = CAST(func, 0x00579B90);
            return func(this, nullptr);
        
    }

    void sub_579990()
    {
        TRACE("game_settings::sub_579990");

        void(__fastcall * func)(const void*, void* edx) = CAST(func, 0x00579990);
        return func(this, nullptr);
        
    }

    void collect_game_options() {
        TRACE("game_settings::collect_game_options");

        void(__fastcall * func)(const void*, void* edx) = CAST(func, 0x00579BF0);
        return func(this, nullptr);
    }

    char* get_buffer(int a2) {
        return this->field_49C[a2];
    }

    int soft_save(uint32_t soft_save_type)
    {
       


            {
                TRACE("game_settings::soft_save");

                int(__fastcall * func)(const void*, void* edx, uint32_t) = CAST(func, 0x0057C1B0);
                return func(this, nullptr, soft_save_type);
            }

        }




    void save(int slot_num)
    {


            {
                TRACE("game_settings::save");

                void(__fastcall * func)(const void*, void* edx, int) = CAST(func, 0x0057D460);
                return func(this, nullptr, slot_num);
            }
        
    }


    bool get_num(const resource_key& att, float& a3, bool a4) const
    {
        TRACE("game_settings::set_num");

            bool(__fastcall * func)(const void*, void* edx, const resource_key&, float&, bool) = CAST(func, 0x00575930);
            return func(this, nullptr, att, a3, a4);
        }


        bool set_num(const resource_key & att, Float a3)
        {
            TRACE("game_settings::set_num");


                bool(__fastcall * func)(const void*, void* edx, const resource_key*, Float) = CAST(func, 0x00573AE0);
                return func(this, nullptr, &att, a3);
            }

    bool set_str(const resource_key& a2, const mString& a3) {
        TRACE("game_settings::set_str");


        bool(__fastcall * func)(const void*, void* edx, const resource_key&, const mString&) = CAST(func, 0x00579CE0);
        return func(this, nullptr, a2, a3);
    }

    bool get_str(const resource_key& a2, mString& a3) const {
        TRACE("game_settings::get_str");

            bool(__fastcall * func)(const void*, void* edx, const resource_key&, mString&) = CAST(func, 0x00579F40);
            return func(this, nullptr, a2, a3);
        }

};

VALIDATE_SIZE(game_settings, 0x4CCu);
