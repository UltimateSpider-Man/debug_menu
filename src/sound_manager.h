#pragma once

#include "float.hpp"

#include <cstdint>

struct sound_alias_database;

namespace sound_manager {
    

    extern void set_source_type_volume(unsigned int source_type, Float a2, Float a3);

    extern float get_source_type_volume(unsigned int source_type);

    //0x0050FA50
    extern int fade_sounds_by_type(uint32_t a1, Float a2, Float a3, bool a4);

    //0x00520520
    extern void unpause_all_sounds();


}

extern void sub_54DC10(const char *a1, bool a2);

extern char *sub_50F010();

extern int sub_79A160();

