#pragma once

#include "variable.h"

#include "float.hpp"
#include "fixedstring.h"



inline constexpr auto SB_TYPE_LEVEL_COMMON = 8;
inline constexpr auto SB_TYPE_HERO = 9;
inline constexpr auto SB_TYPE_MOVIE = 10;
inline constexpr auto SB_TYPE_MISSION = 11;

inline constexpr auto SB_STATE_EMPTY = 0;
inline constexpr auto SB_STATE_LOADING = 1;
inline constexpr auto SB_STATE_LOADED = 2;

struct sound_bank_slot {
    fixedstring<8> field_0;
    int m_state;
    int field_24;
    int field_30;
    int field_34;
    void load(const char* a1, const char* a2, bool a4, int a5)
    {
        TRACE("sound_bank_slot::load");
        // TRACE(("sound_bank_slot " + std::string {a1} + " " + std::string {a2}).c_str());
        CDECL_CALL(0x0054CC30, this ,a1, a2, a4, a5);

        // assert(0);
    }
};



Var<sound_bank_slot[12]> s_sound_bank_slots { 0x009601A8 };