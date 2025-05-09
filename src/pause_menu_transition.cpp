#include "pause_menu_transition.h"

#include "common.h"
#include "func_wrapper.h"
#include "trace.h"
#include "utility.h"

VALIDATE_SIZE(pause_menu_transition, 0x50u);

pause_menu_transition::pause_menu_transition(FEMenuSystem *a2, int a3, int a4)
    : FEMenu(a2, 0, a3, a4, 0, 0) {
    this->m_vtbl = 0x00893FE8;

    this->field_2C = bit_cast<PauseMenuSystem *>(a2);

    this->field_30 = 0;
    this->field_34 = 0;
    this->field_38 = 0;
    this->field_3C = 0;
    this->field_40 = 0;
    this->field_48 = 21;
}

void pause_menu_transition::set_transition(int a1) {
    this->field_48 = a1;
}

void pause_menu_transition::Update(Float a2) {
    CDECL_CALL(0x0061C680, this, a2);
}

void pause_menu_transition::_Load()
{
    TRACE("pause_menu_transition::Load");
    CDECL_CALL(0x0061C640, this);
}

void pause_menu_transition_patch()
{
    {
        auto address = func_address(&pause_menu_transition::_Load);
        set_vfunc(0x00893FF8, address);
    }
}
