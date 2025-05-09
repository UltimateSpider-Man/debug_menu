#pragma once

#include "femenu.h"

struct landmarks : FEMenu {
    int field_2C[114];

    //0x006260F0
    landmarks(FEMenuSystem *a2, int a3, int a5);
};
