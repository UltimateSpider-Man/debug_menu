#pragma once

#include "femenu.h"

#include "comic_panels.h"

struct FEMenuSystem;
struct menu_nav_bar;
struct PanelQuad;
struct entity;
struct nglTexture;
struct FEMultiLineText;

struct character_viewer : FEMenu {
    struct internal {
        PanelQuad *field_0;
        int field_4;
        bool field_8;
    };

    struct internal2 {
        std::intptr_t m_vtbl;
    };

    struct internal1 {
        int field_0;
        PanelQuad *field_4;
    };

    FEMenuSystem *field_2C;
    int field_30;
    menu_nav_bar *field_34;
    int field_38;
    int field_3C;
    int field_40;
    int field_44;
    int field_48;
    int field_4C;
    int field_50;
    int field_54;
    int field_58;
    int field_5C;
    int field_60;
    int field_64;
    int field_68;
    int field_6C;
    int field_70;
    int field_74;
    int field_78;
    int field_7C;
    int field_80;
    int field_84;
    PanelQuad *field_88;
    nglTexture *field_8C[5];
    int field_A0[1];
    nglTexture *field_A4;

    FEMultiLineText *field_A8;
    int field_AC;
    internal field_B0[40];
    int field_290;
    int field_294;
    int field_298;
    int field_29C;
    int field_2A0;
    internal1 field_2A4;
    int empty1[11];

    comic_panels::panel *field_2D8;
    entity *field_2DC;
    float field_2E0[1];
    int empty[3];
    float field_2F0;
    int field_2F4;
    char field_2F8;

    int field_2FC;

    //0x00625670
    character_viewer(FEMenuSystem *a2, int a3, int a4);

    //0x006142B0
    void set_info();

    //0x00640070
    void update_selected(int a2);

    void sub_63FC90(int a2);

    void sub_63FEC0(int idx);

    void sub_6143B0(int a2);

    //0x00641EE0
    //virtual
    void OnActivate();

    //0x00614260
    //virtual
    void OnDeactivate(FEMenu *a2);

    //0x00646000
    //virtual
    void Init();

    //0x00640180
    //virtual
    void OnUp(int a2);

    //0x00640230
    //virtual
    void OnDown(int a2);
};

extern void character_viewer_patch();
