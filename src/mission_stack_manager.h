#pragma once

#include "resource_pack_slot.h"
#include "variable.h"

struct mString;
struct resource_pack_group;

struct mission_stack_manager {
    int pack_loads_or_unloads_pending;
    int field_4;
    bool loading_started;
    bool unloading_started;
    bool field_A;
    bool field_B;

void pop_mission_pack(const mString& a2, const mString& a3);

void pop_mission_pack_immediate(const mString& a1, const mString& a2);


bool waiting_for_push_or_pop();



    //0x0096851C


    static inline Var<mission_stack_manager*> s_inst { 0x0096851C };


};

