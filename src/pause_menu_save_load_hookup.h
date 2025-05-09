#pragma once

struct pause_menu_save_load_hookup {
    int field_0[9];

    bool field_24;

    //0x0062C970
    void SaveFile(int a2);

    //0x0062C9B0
    void LoadFile(int a2);

    //0x0062BF90
    void set_current_state(int a2);
};
