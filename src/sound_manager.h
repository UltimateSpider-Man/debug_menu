#pragma once

#include "float.hpp"

#include "sound_bank_slot.h"

#include <cstdint>

struct sound_alias_database;

static constexpr int SM_MAX_SOURCE_TYPES = 8;

static Var<bool> s_sound_manager_initialized{0x0095C829};


struct sound_volume {
    float field_0;
    int field_4[7];
};


static Var<sound_volume[8]> s_volumes_by_type{0x0095C9A8};


namespace sound_manager {

    char* sub_50F010()
{
    if constexpr (1) {
        int curr_char = strlen(g_scene_name()) - 1;
        if (curr_char > 0) {
            while (g_scene_name()[curr_char] != '\\') {
                if (--curr_char <= 0) {
                    goto LABEL_4;
                }
            }
            return &g_scene_name()[curr_char + 1];
        }
    LABEL_4:
        if (g_scene_name()[curr_char] == '\\') {
            return &g_scene_name()[curr_char + 1];
        }

        return &g_scene_name()[curr_char];
    } else {
        return (char*)CDECL_CALL(0x0050F010);
    }
}

void load_hero_sound_bank(const char *a1, bool a2)
{
    assert(s_sound_bank_slots()[SB_TYPE_LEVEL_COMMON].get_state() == SB_STATE_LOADED);

    char *v11 = sub_50F010();

    s_sound_bank_slots()[SB_TYPE_HERO].load(v11, a1, a2, 0);
}

}

