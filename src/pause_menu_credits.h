#pragma once

#include "femenu.h"

#include "fe_game_credits.h"

struct FEMenuSystem;

struct pause_menu_credits : FEMenu {
    fe_game_credits field_2C;
    FEMenuSystem *field_44;

    pause_menu_credits(FEMenuSystem *a2, int a3, int a4);
};
