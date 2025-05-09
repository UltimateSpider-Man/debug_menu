#pragma once

#include "float.hpp"
//#include "ngl/shaders/us_pcuv_shader.h"

#include <cstdint>
#include <vector>

struct fe_mini_map_dot;
struct PanelQuad;
struct PanelAnimFile;
struct PanelFile;
struct matrix4x4;

struct fe_mini_map_widget {
    std::intptr_t m_vtbl{0x00895A00};
    std::vector<fe_mini_map_dot *> field_364;
    PanelFile *mini_map_icons;
    PanelQuad *map_icon_spidey;
    PanelQuad *map_icon_others;
    PanelQuad *minimap_ring;
    PanelFile *mini_map_frame;
    PanelQuad *map_frame_black;
    PanelQuad *map_frame_white;
    PanelQuad *map_frame_white_stub;
    PanelQuad *compass_base;
    PanelQuad *compass_arrow;
    PanelQuad *map_frame_map_placeholder;
    PanelAnimFile *field_3A0;
    void *field_3A4;
    char field_3A8;
    float field_3AC;
    int field_3B0;
    int field_3B4;

    //0x006343C0
    fe_mini_map_widget();

    //virtual
    ~fe_mini_map_widget();

    //0x00619690
    void PrepareRegions();

    //0x00638C30
    void RenderMeshes(matrix4x4 *a2, float &a4);

    //0x0063AEC0
  void UpdatePOIs(matrix4x4 *a2, Float a3, Float a4, Float a5, Float a6, Float a7);

    //0x006432F0
    //virtual
    void Init();

    //0x00641990
    //virtual
    void Draw();

    //0x00641810
    //virtual
    void Update(Float a2);
};

extern void fe_mini_map_widget_patch();
