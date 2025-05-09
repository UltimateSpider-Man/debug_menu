#pragma once

#include "panelanimobject.h"

#include "float.hpp"
#include "mstring.h"
#include "mvector.h"
#include "vector2d.h"
#include "color32.h"

#include "trace.h"

#include <cstdint>

struct from_mash_in_place_constructor;
struct nglTexture;
struct PanelQuadSection;
struct PanelMeshSection;

struct PanelQuad : PanelAnimObject {
    vector2d field_14;
    mVector<PanelQuadSection> pqs;
    PanelMeshSection *pmesh;
    float field_34;
    float field_38;
    mString field_3C;
    int m_vtbl;

    void Init(vector2d* a2, color32* a3, panel_layer a4, Float a5, const char* a6)
    {
        void(__fastcall * func)(void*, void*, vector2d*, color32*, panel_layer, Float, const char*) = CAST(func, get_vfunc(m_vtbl, 0x54));

        func(this, nullptr, a2, a3, a4, a5, a6);
    }
    void SetPos(float a2, float a3, float a4, float a5)
    {
        void(__fastcall * func)(void*, void*, float, float, float, float) = CAST(func, get_vfunc(m_vtbl, 0x94));
        func(this, nullptr, a2, a3, a4, a5);
    }

    void Draw()
    {

                void(__fastcall * func)(void*, void*) = bit_cast<decltype(func)>(0x00616090);
        func(this, nullptr);
    }

    void TurnOn(bool a2)
    {
        void(__fastcall * func)(void*, void*, bool) = CAST(func, get_vfunc(m_vtbl, 0x5C));

        func(this, nullptr, a2);
    }

    void SetColor(color32 a2)
    {
        void(__fastcall * func)(void*, void*, color32) = CAST(func, get_vfunc(m_vtbl, 0x7C));
        func(this, nullptr, a2);
    }

    void SetPos(float* a2, float* a3)
    {
        void(__fastcall * func)(void*, void*, float*, float*) = CAST(func, get_vfunc(m_vtbl, 0x94));
        func(this, nullptr, a2, a3);
    }

    void SetPos(Float a2, Float a3, Float a4, Float a5)
    {
        void(__fastcall * func)(void*, void*, Float, Float, Float, Float) = CAST(func, get_vfunc(m_vtbl, 0x90));
        func(this, nullptr, a2, a3, a4, a5);
    }
    void GetPos(float* a2, float* a3)
    {

        void(__fastcall * func)(void*, void*, float, float) = bit_cast<decltype(func)>(0x006284D0);
        func(this, nullptr, *a2, *a3);
        

        }
    void sub_616710(Float a2, Float a3)
    {
    void(__fastcall* func)(void*, void*, Float, Float) = bit_cast<decltype(func)>(0x00616710);
    func(this, nullptr, a2, a3);
 

    }
    float GetCenterY()
    {
        TRACE("PanelQuad::GetCenterY");

        auto result = this->field_14[1];
        printf("%f", result);
        return result;
    }

    void CopyFrom(const PanelQuad* a2) {

        void(__fastcall * func)(void*, void*, const PanelQuad * ) = bit_cast<decltype(func)>(0x0062E130);
        func(this, nullptr, a2);
    }
    void SetTexture(nglTexture* a2) {

    void(__fastcall* func)(void*, void*, nglTexture*) = bit_cast<decltype(func)>(0x00616290);
   func(this, nullptr, a2);
}
    };

extern void PanelQuad_patch();
