#include "igozoomoutmap.h"

#include "common.h"
#include "func_wrapper.h"
#include "game.h"
#include "string_hash.h"
#include "variable.h"
#include "wds.h"

#include "utility.h"
#include "debug_menu.h"




IGOZoomOutMap::IGOZoomOutMap() {
    THISCALL(0x006489A0, this);
}


void IGOZoomOutMap::sub_64A250()
{
    THISCALL(0x0064A250, this);
}


void  IGOZoomOutMap::sub_64A270()
{
    THISCALL(0x0064A270, this);
}

void IGOZoomOutMap::UpdateInScene()
{
    if ( this->field_5C5 )
    {
        for ( int i = 0; i < this->field_5B4; ++i )
        {
            if ( this->field_5B8 == this->field_0[i].field_14 ) {
                this->field_0[i].field_0.UpdateInScene();
            }
        }
    }
}

void IGOZoomOutMap::DoneZoomingBack() {
    g_game_ptr()->enable_marky_cam(false, false, -1000.0, 0.0);
    g_game_ptr()->field_15E = false;
   // g_game_ptr()->unpause();
}

bool IGOZoomOutMap::sub_55F320() {
    return this->field_5C4 || this->field_5C3;
}

void IGOZoomOutMap::sub_638AD0(int a2, int a3, int a4) {
    THISCALL(0x00638AD0, this, a2, a3, a4);
}

void IGOZoomOutMap::Update(Float a2) {
    THISCALL(0x0063A760, this, a2);
}



void IGOZoomPOI::UpdateInScene()
{
    THISCALL(0x0062A160, this);
}

void IGOZoomOutMap::OnSelectPress() {
    // === ENABLE_ZOOM_MAP gate. ===
    if (!os_developer_options::instance->get_flag("ENABLE_ZOOM_MAP")) {

        return;
    }
    else {

        void(__fastcall * func)(void*, void*) = bit_cast<decltype(func)>(0x0000638570);

        func(this, nullptr);
    }

    debug_menu::hide();

}



void IGOZoomOutMap_patch() {

    {
        FUNC_ADDRESS(address, &IGOZoomOutMap::OnSelectPress);
        REDIRECT(0x00638714, address);
        REDIRECT(0x00638A29, address);
        REDIRECT(0x00638B39, address);
    }


}