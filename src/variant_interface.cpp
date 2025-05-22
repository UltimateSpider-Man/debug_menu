#include "variant_interface.h"

#include "common.h"
#include "func_wrapper.h"
#include "trace.h"
#include "utility.h"

VALIDATE_SIZE(variant_interface, 0x58);

variant_interface::variant_interface(conglomerate *a2)
{
    this->m_vtbl = 0x008835A0;
    this->field_4 = a2;
    this->field_8 = true;
}

variant_info *variant_interface::get_random_variant()
{
    if constexpr (0)
    {
    }
    else
    {
        return (variant_info*)CDECL_CALL(0x004CAD00, this);
    }
}

void variant_interface::apply_variant(string_hash a2)
{
    TRACE("variant_interface::apply_variant");

    if constexpr (0)
    {}
    else
    {
        void (__fastcall * func)(void*, void*, string_hash) = (decltype(func))0x004E0920;
        func(this, nullptr,a2);
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

void variant_interface_patch()
{
    {
        void (variant_interface::* apply_variant)(string_hash) = &variant_interface::apply_variant;
        auto address = func_address( apply_variant);
        REDIRECT(0x0066F402, address);
        REDIRECT(0x0066F372, address);
    }
}