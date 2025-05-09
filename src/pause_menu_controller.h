#pragma once

#include "femenu.h"

struct PauseMenuSystem;
struct PanelQuad;
struct PanelFile;

struct pause_menu_controller : FEMenu {
    FEText *field_2C;
    PanelQuad *field_30[21];

    int field_temp[100];

    PauseMenuSystem *field_214;

    //0x00610AD0
    pause_menu_controller(FEMenuSystem *a2, int a3, int a4);

    //0x0063C710
    void load_ps2_spider(PanelFile *a2);

    //0x0063CC50
    void load_ps2_venom(PanelFile *a2);

    //0x00641D20
    //virtual
    void _Load();

    /* virtual */ void OnActivate();
};

extern void pause_menu_controller_patch();
