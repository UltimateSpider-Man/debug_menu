#pragma once

#include "float.hpp"
#include "input_mgr.h"
#include "utility.h"

#include "femenuentry.h"

#include <cstdint>
#include "vtbl.h"

struct FEMenuSystem;
struct FEText;
struct FEMenuEntry;



struct FEMenu {
    std::intptr_t m_vtbl;
    FEMenuEntry **field_4;
    FEMenuSystem *field_8;
    int field_C;
    int field_10;
    int field_14;
    int field_18;
    float field_1C;
    int16_t field_20;
    int16_t highlighted;
    int16_t num_entries;
    int16_t field_26;
    int16_t field_28;
    char field_2A;
    char field_2B;

    //0x0060AA90
    FEMenu(FEMenuSystem *a2, uint32_t a3, int a4, int a5, int16_t a6, int16_t a7)    {
        void (__fastcall *func)(void*, void*, uint32_t, int , int,int16_t,int16_t) = (decltype(func)) 0x0060AA90;
        func(this,a2,a3,a4,a5,a6,a7);
    }

    //0x00618610
    //virtual
    void AddEntry(int a2, FEText *a3, bool a4)  {

        void (__fastcall *func)(void*,int, void*, bool) = (decltype(func)) 0x0060AA90;
        func(this,a2,a3,a4);
    }


    void Init()
    {
        void(__fastcall * func)(void*) = CAST(func, get_vfunc(m_vtbl, 0xC));
        func(this);
    }

    void Load()
    {
        void(__fastcall * func)(void*) = CAST(func, get_vfunc(m_vtbl, 0x10));
        func(this);
    }

    void Draw()
    {
        void(__fastcall * func)(void*) = CAST(func, get_vfunc(m_vtbl, 0x18));
        func(this);
    }

    void OnActivate()
    {
        void(__fastcall * func)(void*) = CAST(func, get_vfunc(m_vtbl, 0x2C));
        func(this);
    }

    void Up()
    {
        void(__fastcall * func)(void*) = CAST(func, get_vfunc(m_vtbl, 0xA4));

        func(this);
    }

    void Down()
    {
        void(__fastcall * func)(void*) = CAST(func, get_vfunc(m_vtbl, 0xA8));

        func(this);
    }

    void OnDeactivate(FEMenu* a2)
    {
        void(__fastcall * func)(FEMenu*, void*, FEMenu*) = CAST(func, get_vfunc(m_vtbl, 0x30));
        func(this, nullptr, a2);
    }

    void ButtonHeldAction()
    {
        if ((128 & this->field_28) != 0) {
            auto v1 = this->field_2A;
            if (v1 == 4) {
                this->Up();
            } else if (v1 == 8) {
                this->Down();
            }
        }
    }

  inline  void Update(Float a2)
    {
        if ((128 & this->field_28) != 0 && this->field_2A != -1) {
            this->field_1C = this->field_1C - a2;
            if (this->field_1C <= 0.0f) {
                this->ButtonHeldAction();
                auto v4 = this->field_28;
                this->field_1C = 0.1;
                if ((v4 & 0x100) != 0) {
                    this->field_1C = 0.010000001;
                }
            }
        }

        for (auto i = 0; i < this->num_entries; ++i) {
            auto* v6 = this->field_4[i];
            v6->Update(a2);
        }
    }

};

inline void sub_582A30() {

        CDECL_CALL(0x00582A30);
    }

inline void sub_5A6D70() {

        CDECL_CALL(0x005A6D70);
       
    }

extern void FEMenu_patch();
