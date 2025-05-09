#pragma once

#include "femenu.h"

struct main_menu_keyboard : FEMenu {
    char field_2C[0x11C];

    main_menu_keyboard(FEMenuSystem *a2, int a3, int a4);

    //0x006241E0
    /* virtual */ void OnActivate();

    //0x00613D10
    /* virtual */ void OnDeactivate(FEMenu *a2);
};

extern void main_menu_keyboard_patch();
