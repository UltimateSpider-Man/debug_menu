#include "sound_manager.h"

#include "common.h"
#include "func_wrapper.h"

#include "trace.h"
#include "utility.h"
#include "variable.h"
#include "variables.h"
#include <assert.h>

static constexpr int SM_MAX_SOURCE_TYPES = 8;

static Var<bool> s_sound_manager_initialized{0x0095C829};

struct sound_volume {
    float field_0;
    int field_4[7];
};

VALIDATE_SIZE(sound_volume, 0x20);

static Var<sound_volume[8]> s_volumes_by_type{0x0095C9A8};



float sound_manager::get_source_type_volume(unsigned int source_type) {
    assert(s_sound_manager_initialized());
    assert(source_type < SM_MAX_SOURCE_TYPES);

    return s_volumes_by_type()[source_type].field_0;
}

void sound_manager::set_source_type_volume(unsigned int source_type, Float a2, Float a3)
{
    if constexpr (0) {
    } else {
        CDECL_CALL(0x0050FC50, source_type, a2, a3);
    }
}

void sound_manager::unpause_all_sounds()
{
    if constexpr (1) {
    } else {
        CDECL_CALL(0x00520520);
    }
}

int sound_manager::fade_sounds_by_type(uint32_t a1, Float a2, Float a3, bool a4)
{
    if constexpr (1) {
    }
    else {
        int(__cdecl * func)(uint32_t a1, Float a2, Float a3, bool a4) = CAST(func, 0x0050FA50);
        return func(a1, a2, a3, a4);

        
    }

    return 0;
}



