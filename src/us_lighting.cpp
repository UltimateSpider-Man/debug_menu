#include "us_lighting.h"

#include "func_wrapper.h"


#include "variables.h"


#if 0
static string_hash usl_skydome_names[4] {
                                        int(to_hash("sky_day")),
                                        int(to_hash("sky_night")),
                                        int(to_hash("sky_rainy")),
                                        int(to_hash("sky_sunset")),
                                        };
#endif

void us_lighting_switch_time_of_day(int a1)
{

        CDECL_CALL(0x00408790, a1);
    }


