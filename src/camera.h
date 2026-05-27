#pragma once

#include "actor.h"

struct vector3d;
struct string_hash;
struct entity;
struct mic;

struct camera : actor {
    mic *field_C0;
    float field_C4;
    float field_C8;


    void * operator new(size_t size);


    //0x0057EFD0
    //virtual
    void sync(camera &a2);

    //virtual
    void set_fov(Float fov);


    float get_fov();


    float get_far_plane_factor();



    bool is_externally_controlled() const;

    


    //virtual
    bool _is_a_camera() const;

};

inline Var<camera*> g_camera_link {0x0095C720};

struct region;

extern vector3d collide_with_world(
        camera *,
        const vector3d &a3,
        float a2,
        const vector3d &arg10,
        region *reg);


