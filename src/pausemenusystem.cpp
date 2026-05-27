#include "pausemenusystem.h"

#include "comic_panels.h"
#include "common.h"
#include "game.h"



#include "debug_menu.h"

#include <cassert>
#include "femenusystem.h"
#include "cursor.h"
#include "memoryunitmanager.h"
#include "pause_menu_save_load_display.h"

VALIDATE_SIZE(PauseMenuSystem, 0x10u);

PauseMenuSystem *& pause_menu_system_ptr = var<PauseMenuSystem*>(0x00937B1C);

typedef int(__fastcall* unpause_ptr)(void*);
unpause_ptr unpause = (unpause_ptr)0x0053A880;


void PauseMenuSystem::MakeActive(int a2) {


    void(__fastcall * func)(void*, void*,int) = bit_cast<decltype(func)>(0x0060B610);

    func(this, nullptr, a2);
}


void PauseMenuSystem::OnButtonPress(int a2, int a3) {
    if (this->field_38 < 0) return;
    const int back_btn = 13;
    if (a2 == 1
        && (this->field_38 == 1 || this->field_38 == 2)
        && a3 == back_btn
        && os_developer_options::instance->get_flag("ENABLE_ZOOM_MAP"))
    {
        this->Deactivate();
        debug_menu::hide();
        return;
    }
    void(__fastcall * func)(void*, void*, int, int) = bit_cast<decltype(func)>(0x006187D0);

    func(this, nullptr, a2, a3);
}


void PauseMenuSystem::Deactivate()
{
  /* if (this->m_index >= 0)
    {
        this->MakeActive(0);
        unpause(g_game_ptr());
        comic_panels::game_play_panel()->field_67 = this->field_38;
    }*/

    void(__fastcall * func)(void*, void*) = bit_cast<decltype(func)>(0x0060BEE0);

    func(this, nullptr);
}

void PauseMenuSystem::Activate(int a2, bool a3)
{
    if (this->field_38 < 0)
    {
        this->MakeActive(a2);

        if (a3)
            g_game_ptr->pause();
        pause_menu_save_load_display* v4 = this->field_34;
        this->field_38 = static_cast<uint8_t>(comic_panels::game_play_panel()->field_67);

        MemoryUnitManager::RegisterInsertRemoveObserver(
            reinterpret_cast<MemoryUnitManager::InsertRemoveObserver*>(v4->field_2C));

        Cursor* cur = g_cursor();
        if (cur != nullptr) {
            cur->field_120 = 0;
            cur->field_114 = 0;
        }


        void(__fastcall * func)(void*, void*, int, int) = bit_cast<decltype(func)>(0x0060BE90);

        func(this, nullptr, a2, a3);
    }
}



void PauseMenuSystem_patch() {

    {
        FUNC_ADDRESS(address, &PauseMenuSystem::OnButtonPress);
       // set_vfunc(0x00893E38, address);
    }

    {
    FUNC_ADDRESS(address, &PauseMenuSystem::Activate);
 //   REDIRECT(0x00673136, address);
  //  REDIRECT(0x00673338, address);
  //  REDIRECT(0x006734D5, address);
  //  REDIRECT(0x006737D5, address);

}

}


