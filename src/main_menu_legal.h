#pragma once

#include "femenu.h"

struct FrontEndMenuSystem;

struct main_menu_legal : FEMenu {
    FrontEndMenuSystem *field_2C;
    float field_30;
    FEMenuSystem *field_34;

    main_menu_legal(FEMenuSystem *a2, int a3, int a4);
};
