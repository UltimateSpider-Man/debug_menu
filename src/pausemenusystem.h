#pragma once


#include "femanager.h"
#include "variable.h"

struct PanelFile;
struct menu_nav_bar;
struct pause_menu_save_load_display;

struct PauseMenuSystem  {
    PanelFile *field_2C;
    menu_nav_bar *field_30;
    pause_menu_save_load_display *field_34;
    int field_38;

    //0x0060BEE0
    void Deactivate();

    void MakeActive(int a2);

    void OnButtonPress(int a2, int a3);
    

    void Activate(int a2, bool a3);

};


extern void PauseMenuSystem_patch();

extern PauseMenuSystem *& pause_menu_system_ptr;