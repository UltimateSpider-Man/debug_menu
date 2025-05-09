#include "variant_interface.h"



#include "common.h"
#include "func_wrapper.h"
#include "memory.h"
#include "ngl.h"

#include "entity.h"

#include "bit.h"

#include "conglom.h"

#include "debug_menu.h"

#include "trace.h"
#include "utility.h"

VALIDATE_SIZE(variant_interface, 0x54);



static variant_info* get_random_variant()
{
    if constexpr (0)
    {
    }
    else
    {
        return (variant_info*)CDECL_CALL(0x004CAD00);
    }
}


mashable_vector<variant_info>* variant_interface::get_variants(variant_interface*)
{
    variant_interface *v2 = {
    };

    return &v2->variants;
}


void variant_interface::apply_variant(string_hash a2)
{

    TRACE("variant_interface::apply_variant");

    if constexpr (0)
    {
    }
    else
    {
    void(__fastcall * func)(void*, void*, string_hash) = (decltype(func))0x0048AC20;
    func(this, nullptr, a2);
    }
}

void variant_interface::apply_variant(variant_info *info)
{
    TRACE("variant_interface::apply_variant");

    if constexpr (0)
    {}
    else
    {
        CDECL_CALL(0x004DB110, this, info);
    }
}




// Assuming dword_14ED6A4 and dword_14ED6A0 are some global flags or constants


void variant_interface::destroy_ifl_frames()
{
    auto **v1 = this->field_2C;
    int v2 = 3;
    do
    {
        auto *v3 = *v1;
        if ( v3 != nullptr )
        {
            mem_dealloc(v3, 1);

        }

        *v1++ = nullptr;
        --v2;
    }
    while ( v2 != 0 );
}






