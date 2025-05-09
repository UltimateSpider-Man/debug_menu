#pragma once

#include "femenu.h"

#include "float.hpp"

struct FEMenuSystem;
struct PanelQuad;
struct FEText;

enum class PauseMenuState : int {
    State0 = 0,
    State1,
    State2,
    State3,
    State4,
    State5,
    State6,
    State7,
    State8,
    State9
};

struct pause_menu_root : FEMenu {
    bool field_2C;
    char field_2D;
    int field_30;
    int field_34;
    int field_38;
    PanelQuad* field_3C[9];
    PanelQuad* field_60;
    PanelQuad* field_64;
    PanelQuad* field_68;
    PanelQuad* field_6C;
    PanelQuad* field_70;
    PanelQuad* field_74;
    FEText* field_78[9];
    FEText* field_9C;
    FEText* field_A0;
    FEText* field_A4;
    FEText* field_A8;
    FEMenuSystem* field_AC;
    int field_B0;
    int field_B4;
    float field_B8[4];
    float field_C8[4];
    float field_D8[4];
    float field_E8[4];
    bool field_F8;
    int field_FC;

     inline   void OnCross(int a1, int a2, int a3);

        // Assuming these are member variables
        bool byte_965C21{ false };
        PauseMenuState state;
        bool flag1{ false };
        bool flag2{ false };
        bool flag3{ false };

        // Assuming these are member variables with complex types
        void* complex_ptr1;
        void* complex_ptr2;

        // Function pointer types
        using FuncPtr1 = void (*)(void*, int);
        using FuncPtr2 = void (*)(float**, uint32_t, float, float);

        // Assuming these are global functions
     inline   void sub_5C5920();
        void sub_5BAF30();
        void sub_5C58D0();
        void sub_5A6D70();
        void sub_401D40(bool);
        void sub_61C610();
        void sub_61C520();
        void sub_5B0D70(void*);
        bool sub_5BAF50(void*);

        // Modern C++ version of sub_60B960
        void sub_62A840();

    // 0x0060E590
    pause_menu_root(FEMenuSystem* a2, int a3, int a4);

    // 0x0063B2E0
    // virtual
   inline void _Load();

    // 0x00641BF0
   inline void update_switching_heroes();

    // 0x0061BD00
    // virtual
   inline void OnUp(int a2);

    // 0x0061BE10
    // virtual
   inline void OnDown(int a2);

    // 0x006490A0
    // virtual
   inline void Update(Float a2);
};

extern void pause_menu_root_patch();
