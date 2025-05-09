#pragma once

#include <cstdint>
#include <cassert>
#include <cstring>

#include "common.h"

#include "float.hpp"
#include "fixedstring32.h"
#include "os_developer_options.h"

namespace MemoryUnitManager
{
    struct Container {
        int empty[144];
        int field_240;
        int field_244;
        char field_248[64];
    };
}

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

void init();

game_data_meat();

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

   game_settings()
        : field_4 { 0 }
    {
        if constexpr (1) {
            this->m_vtbl = 0x0088B234;

            this->field_4BF = false;
            this->field_4C0 = false;
            this->field_4C1 = false;
            this->field_4C2 = false;
            this->field_4BF = false;

            [[maybe_unused]] auto v4 = os_developer_options::instance()->get_string(os_developer_options::strings_t::SKU);

            strncpy(this->field_4A8, "", 12u);
            this->field_4A8[11] = '\0';

            this->field_4B8 = 0;
            this->m_slot_num = 0;
            this->field_4C8 = 0;
        } else {
            CDECL_CALL(0x0057BF50, this);
        }
    }




    // 0x005799E0
    void init_script_buffer();

    // 0x005737A0
    int get_game_size();

    // 0x0057C1B0
    int soft_save(uint32_t soft_save_type);

    void update_miles_crawled_venom(Float a2);

    void update_miles_crawled_spidey(Float a2);

    // 0x0057EAB0
    void start_new_game();

    // 0x005802D0
    void frame_advance(Float a2);

    // 0x00579A60
    void export_game_options();

    // 0x00579A10
    void export_game_settings();



        static void soft_load(uint32_t soft_save_type)
    {
        void(__fastcall * func)(void*, void*, int) = bit_cast<decltype(func)>(0x0057C1E0);
        func(0, nullptr, soft_save_type);
    }

    static void save(int slot_num)
    {
        void(__fastcall * func)(void*, void*, int) = bit_cast<decltype(func)>(0x0057D460);
        func(0, nullptr, slot_num);
    }

 
    // 0x00579B90
    void collect_game_settings();

    void sub_579990();

    // 0x00579BF0
    void collect_game_options();

    char* get_buffer(int a2);

    // 0x00579790
    void reset_container(bool a2);

    // 0x00579970
   int load()
    {
       void(__fastcall * func)(void*, void*) = (decltype(func))0x00579970;
       func(this, nullptr);



    }

    // 0x0057F410
     void load_game(int slot_num)
    {
        void(__fastcall * func)(void*, void*, int) = (decltype(func))0x0057F410;
        func(this, nullptr, slot_num);
    }

    void load_most_recent_game();

    // 0x00573700
    void set_script_buffer_size();

    // 0x005739E0
    void update_miles_web_zipping(Float a2);

    // 0x00573A60
    void update_miles_run_venom(Float a2);

    // 0x00573A00
    void update_miles_run_spidey(Float a2);

    // 0x00573AA0
    void update_web_fluid_used(Float a2);




};

VALIDATE_SIZE(game_settings, 0x4CCu);






