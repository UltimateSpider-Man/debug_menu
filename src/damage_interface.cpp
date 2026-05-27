#include "damage_interface.h"

#include "actor.h"
#include "common.h"
#include "func_wrapper.h"
#include "resource_key.h"
#include "trace.h"
#include "utility.h"
#include "vtbl.h"

VALIDATE_SIZE(damage_interface, 0x23Cu);

template<>
void bounded_variable<float>::sub_48BFB0(const float &a2)
{
    this->field_0[0] = a2;
    if (a2 > this->field_0[2]) {
        this->field_0[0] = this->field_0[2];
    }

    if (this->field_0[0] < this->field_0[1]) {
        this->field_0[0] = this->field_0[1];
    }
}

damage_interface::damage_interface(actor *a2)
{
    THISCALL(0x004DE8A0, this, a2);
}

damage_interface::~damage_interface()
{
    THISCALL(0x004D9BF0, this);
}





void damage_interface::_un_mash(
        generic_mash_header *header,
        void *a3,
        void *a4,
        generic_mash_data_ptrs *a5)
{
    TRACE("damage_interface::un_mash");

    if constexpr (0)
    {}
    else
    {
        THISCALL(0x004D9E20, this, header, a3, a4, a5);
    }
}

void damage_interface::frame_advance(Float a3)
{
    TRACE("damage_interface::frame_advance");

    THISCALL(0x004EC4A0, this, a3);
}


