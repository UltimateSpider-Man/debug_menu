#include "camera.h"

#include "common.h"
#include "entity.h"
#include "func_wrapper.h"
#include "geometry_manager.h"
#include "memory.h"

#include "trace.h"
#include "utility.h"
#include "vector3d.h"
#include "vtbl.h"

#include <cmath>

VALIDATE_SIZE(camera, 0xCCu);


void * camera::operator new(size_t size)
{
    return mem_alloc(size);
}

void camera::sync(camera &a2) {
    if constexpr (1) {
        void (__fastcall *func)(camera *, void *, camera *) = CAST(func, get_vfunc(m_vtbl, 0x294));

        func(this, nullptr, &a2);

    } else {
        THISCALL(0x0057EFD0, this, &a2);
    }
}

void camera::set_fov(Float fov)
{
    void (__fastcall *func)(void *, void *, Float) = CAST(func, get_vfunc(m_vtbl, 0x29C));
    func(this, nullptr, fov);
}

float camera::get_fov() {
    float (__fastcall *func)(void *) = CAST(func, get_vfunc(m_vtbl, 0x2A0));
    return func(this);
}

float camera::get_far_plane_factor() {
    float (__fastcall *func)(void *) = CAST(func, get_vfunc(m_vtbl, 0x2A8));
    return func(this);
}


bool camera::is_externally_controlled() const {
    return this->is_flagged(0x400000);
}




bool camera::_is_a_camera() const
{
    TRACE("camera::is_a_camera");

    return true;
}



