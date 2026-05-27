#pragma once

#include "femenu.h"

struct pause_menu_save_load_hookup;

struct pause_menu_save_load_display : FEMenu {
    pause_menu_save_load_hookup *field_2C;

    int field_30[49];

    //0x0062B820
    pause_menu_save_load_display(FEMenuSystem *a2, int a3, int a4);

    //0x0063D080
    //virtual
    void _Load();
};

extern void pause_menu_save_load_display_patch();
