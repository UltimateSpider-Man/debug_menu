#pragma once

#include "femenu.h"

struct FEMenuSystem;

struct pause_menu_options_display : FEMenu {
    int field_2C[94];

    //0x0061D970
    pause_menu_options_display(FEMenuSystem *a2, int a3, int a4);

    //0x0063BE80
    //virtual
    void _Load();
};

extern void pause_menu_options_display_patch();
