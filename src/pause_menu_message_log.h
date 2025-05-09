#pragma once

#include "femenu.h"

struct pause_menu_message_log : FEMenu {
    int field_2C[53];

    //0x00611BD0
    pause_menu_message_log(FEMenuSystem *a2, int a3, int a4);

    //virtual
    void _Load();
};

extern void pause_menu_message_log_patch();
