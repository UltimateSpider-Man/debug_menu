#pragma once

#include "generic_interface.h"

#include "float.hpp"
#include "variable.h"
#include "vector3d.h"

#include <list>
#include <vector>

struct actor;
struct resource_key;

template<typename T>
struct bounded_variable {
    T field_0[4];

    void sub_48BFB0(const T &a2);
};

struct damage_interface : generic_interface {
    actor *field_4;
    char field_8[0x1F4];

    bounded_variable<float> field_1FC;
    bounded_variable<float> field_20C;
    bounded_variable<float> field_21C;
    bounded_variable<int> field_22C;

    //0x004DE8A0
    damage_interface(actor *a2);

    //0x004D9BF0
    ~damage_interface();


    //virtual
    void _un_mash(
        generic_mash_header *a2,
        void *a3,
        void *a4,
        generic_mash_data_ptrs *a5);

    //virtual
    void frame_advance(Float a3);

    //0x004D1990

    static inline Var<std::vector<damage_interface *> *> all_damage_interfaces {0x0095A660};

    static inline Var<std::list<damage_interface *> *> found_damageable {0x0095A5EC};
};


