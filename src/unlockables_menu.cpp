#include "unlockables_menu.h"

#include "common.h"
#include "func_wrapper.h"
#include "log.h"
#include "utility.h"
#include "trace.h"
#include "panelquad.h"

#include <cassert>

VALIDATE_SIZE(unlockables_menu, 0x100u);

unlockables_menu::unlockables_menu(FEMenuSystem *a2, int a3, int a4)
    : FEMenu::FEMenu(a2, 0, a3, a4, 8, 0) {
    CDECL_CALL(0x00614020, this, a2, a3, a4);
}

void unlockables_menu::OnCross(int a2) {
    printf("unlockables_menu::OnCross():");

    CDECL_CALL(0x0062DB20, this, a2);
}

void unlockables_menu::OnTriangle(int a2) {
    printf("unlockables_menu::OnTriangle():");

    CDECL_CALL(0x006253C0, this, a2);
}

void unlockables_menu::OnActivate() {
    printf("unlockables_menu::OnActivate():");

    CDECL_CALL(0x0062D510, this);
}

void unlockables_menu::_Load()
{
    TRACE("unlockables_menu::Load");

    if constexpr (1)
    {
        for (auto i = 0u; i < 14u; ++i)
        {
            sp_log("%d", i);
            auto *quad = this->field_38[i];
            assert(quad != nullptr);

            quad->TurnOn(true);
        }
    }
    else
    {
        CDECL_CALL(0x00614110, this);
    }
}

void unlockables_menu_patch() {

    {
        auto address = func_address(&unlockables_menu::_Load);
        set_vfunc(0x00894918, address);
    }
    return;

    {
        auto address = func_address(&unlockables_menu::OnCross);
        set_vfunc(0x00894954, address);
    }

    {
        auto address = func_address(&unlockables_menu::OnTriangle);
        //set_vfunc(0x00894958, address);
    }

    {
        auto address = func_address(&unlockables_menu::OnActivate);
        set_vfunc(0x00894934, address);
    }
}
