#pragma once

#include "femenu.h"

struct FEMenuSystem;
struct PanelQuad;
struct menu_nav_bar;

struct unlockables_menu : FEMenu {
    FEMenuSystem *field_2C;
    int field_30;
    menu_nav_bar *field_34;
    PanelQuad *field_38[14];
    int field_70[36];

    //0x00614020
    unlockables_menu(FEMenuSystem *a2, int a3, int a4);

    //0x00614110
    //virtual
    void _Load();

    //0x0062DB20
    //virtual
    void OnCross(int a2);

    //0x006253C0
    //virtual
    void OnTriangle(int a2);

    //0x0062D510
    //virtual
    void OnActivate();
};

extern void unlockables_menu_patch();
