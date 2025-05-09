#pragma once

#include "femenu.h"

struct ltd_edition : FEMenu {
    int field_2C[61];

    //0x00614570
    ltd_edition(FEMenuSystem *a2, int a3, int a4);
};
