#include "fe_menu_nav_bar.h"

#include "common.h"
#include "func_wrapper.h"
#include "trace.h"
#include "utility.h"
#include <cassert>

VALIDATE_SIZE(menu_nav_bar, 0x2Cu);

menu_nav_bar::menu_nav_bar() : field_4() {
    this->text_box = nullptr;
    this->background_a = nullptr;
    this->field_1C = nullptr;
    this->field_20 = nullptr;
    this->field_24 = nullptr;
}

void menu_nav_bar::Load()
{
    TRACE("menu_nav_bar::Load");
    assert(text_box != nullptr && background_a != nullptr);

    CDECL_CALL(0x00612080, this);
}

void menu_nav_bar::AddButtons(menu_nav_bar::button_type a2,
                              menu_nav_bar::button_type a3,
                              global_text_enum a4) {
    CDECL_CALL(0x006121C0, this, a2, a3, a4);
}

void menu_nav_bar::Reformat() {
    CDECL_CALL(0x006122B0, this);
}

void menu_nav_bar_patch()
{
    {
        auto address = func_address(&menu_nav_bar::Load);
        //REDIRECT(0x006431A4, address);
    }
}
