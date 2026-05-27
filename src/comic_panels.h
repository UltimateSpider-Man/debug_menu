#pragma once

#include <cstdint>
#include "variable.h"
#include "matrix4x4.h"

struct cut_scene_player;
struct vector3d;
struct actor;
struct entity;
struct signaller;
struct nglTexture;



namespace comic_panels {

struct panel;





struct panel {
    std::intptr_t m_vtbl;
    matrix4x4 field_4;
    cut_scene_player *field_4C;
    float field_50;
    int field_54;
    float field_58;
    bool field_5C;
    char field_64;
    char field_65;
    char field_66;
    bool field_67;
    bool field_68;


};

struct panel_component_camera {
    int field_18;
    int field_1C;
    bool field_20;
    float field_24;
    int field_28;
    int field_2C;
    int field_30;
    char field_48;

};

struct panel_component_base {
    int m_vtbl;
    float field_8;
    bool field_C;

};




extern Var<panel *> game_play_panel;



} // namespace comic_panels

