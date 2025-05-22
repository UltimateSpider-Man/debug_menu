#include "conglom.h"

#include "entity.h"

#include "common.h"
#include "femanager.h"
#include "func_wrapper.h"
#include "game.h"
#include "geometry_manager.h"

#include "ngl.h"

#ifdef TARGET_XBOX
#include "mashable_interface.h"
#endif

#include "memory.h"
#include "po.h"
#include "os_developer_options.h"

#include "region.h"

#include "skeleton_interface.h"


#include "trace.h"
#include "utility.h"
#include "variables.h"
#include "variant_interface.h"
#include "vector2di.h"
#include "vtbl.h"

#include "vector3d.h"
#include <cmath>

VALIDATE_SIZE(conglomerate, 0x130);

static constexpr auto MAX_BONES = 64u;
static constexpr auto MAX_MEMBERS = 16u;
static constexpr auto MAX_BONES_AND_MEMBERS = MAX_MEMBERS + MAX_BONES;




void conglomerate::create_parentage_tree()
{
    TRACE("conglomerate::create_parentage_tree");

    CDECL_CALL(0x004E55E0, this);
}

void conglomerate::create_variant_ifc()
{
    auto *mem = mem_alloc(sizeof(variant_interface));
    this->m_variant_interface = new (mem) variant_interface {this};
}


void conglomerate::sub_4D0E00()
{
    CDECL_CALL(0x004D0E00, this);
}


entity_base* conglomerate::get_bone(const string_hash& a2, bool a3)
{

        return (entity_base*)CDECL_CALL(0x004CCC90, this, &a2, a3);
    
}

vector3d* conglomerate::get_colgeom_center()
{


    vector3d*(__fastcall * func)(void*, void*) = (decltype(func))0x00509F10;
    func(this, nullptr);
    return (vector3d*)0;
}


bool conglomerate::render_complex_shadow(Float camera_distance)
{
    assert(this->is_flagged(EFLAG_MISC_CAST_SHADOW) && this->is_flagged(EFLAG_MISC_HIRES_SHADOW));

    assert(camera_distance <= 50.0f);

    if constexpr (0)
    {}
    else
    {
        bool (__fastcall *func)(void *, void *edx, Float camera_distance) = CAST(func, 0x004E5300);
        return func(this, nullptr, camera_distance);
    }
}

void conglomerate::render_simple_shadow(Float arg0, Float arg4)
{
    assert(this->is_flagged(EFLAG_MISC_CAST_SHADOW)
            && this->is_flagged(EFLAG_GRAPHICS_VISIBLE));

    if constexpr (0)
    {
    }
    else
    {


        void(__fastcall * func)(void*, void*, Float, Float) = (decltype(func))0x004E4D80;
        func(this, nullptr, arg0, arg4);
    }
}





bool conglomerate::has_tentacle_ifc()
{
    bool (__fastcall *func)(void *) = CAST(func, get_vfunc(m_vtbl, 0x294));
    return func(this);
}

bool conglomerate::has_variant_ifc()
{
    bool (__fastcall *func)(void *) = CAST(func, get_vfunc(m_vtbl, 0x29C));
    return func(this);
}

variant_interface *conglomerate::variant_ifc()
{
    variant_interface* (__fastcall *func)(void *) = CAST(func, get_vfunc(m_vtbl, 0x2A0));
    return func(this);
}

void conglomerate::_render(Float a2)
{
    TRACE("conglomerate::render");


                void(__fastcall * func)(void*, void*, Float) = (decltype(func))0x004F9930;
        func(this, nullptr,a2);

}

float conglomerate::get_colgeom_radius()
{

        void(__fastcall * func)(void*, void*) = (decltype(func))0x004D2670;
    func(this, nullptr);


         return (float)0;

    
}

void conglomerate::_un_mash(generic_mash_header* a2, void* a3, generic_mash_data_ptrs* a4)
{


    void(__fastcall * func)(void*, void* ,generic_mash_header*, void*, generic_mash_data_ptrs*) = (decltype(func))0x004FC830;
    func(this, nullptr,a2,a3,a4);
}

bool render_drop_shadow(math::MatClass<4, 3>& a1, Float a2, Float a3, bool a4)
{
    TRACE("render_drop_shadow");

    return (bool)CDECL_CALL(0x0059F910, &a1, a2, a3, a4);


}



void conglomerate_patch()
{

    REDIRECT(0x004E52D4, render_drop_shadow);


    {
        auto address = func_address(&conglomerate::_render);
       set_vfunc(0x0088487C, address);
    }

    {
        {
            auto address = func_address(&conglomerate::_un_mash);
            set_vfunc(0x00884834, address);
        }

    return;

    auto address = func_address(&conglomerate::get_colgeom_radius);
    set_vfunc(0x00884924, address);

    {
        auto address = func_address(&conglomerate::get_colgeom_center);
        //set_vfunc(0x00884928, address);
    }
    }
 }
