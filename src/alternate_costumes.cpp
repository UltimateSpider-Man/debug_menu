#include "alternate_costumes.h"







#include "func_wrapper.h"
#include "game.h"



#include "panelfile.h"
#include "resource_manager.h"
#include "script_manager.h"


#include "variables.h"


VALIDATE_SIZE(ptr_PanelQuad, 12u);
VALIDATE_SIZE(ptr1_PanelQuad, 8u);

VALIDATE_SIZE(alternate_costumes, 0x16C);
VALIDATE_OFFSET(alternate_costumes, field_C4, 0xC4);

static Var<char *[1]> off_937798 { 0x00937798 };

alternate_costumes::alternate_costumes(FEMenuSystem *a2, int a3, int a4)
    : FEMenu(a2, 0, a3, a4, 8, 0) {
    CDECL_CALL(0x00625D70, this, a2, a3, a4);
}

void alternate_costumes::Init() {
    CDECL_CALL(0x00646790, this);
}

void alternate_costumes::sub_640BC0(int a2) {
    CDECL_CALL(0x00640BC0, this, a2);
}

void alternate_costumes::set_info() {
    CDECL_CALL(0x00614B60, this);
}

void alternate_costumes::sub_640740(int idx) {

        CDECL_CALL(0x00640740, this, idx);
    
}

void alternate_costumes::sub_640510(int idx) {
    CDECL_CALL(0x00640510, this, idx);
}

bool alternate_costumes::sub_614AC0(int a1) {
    mString v4{"gv_unlockable_costumes"};

 return (bool) CDECL_CALL(0x00614AC0, a1);
    
}

void alternate_costumes::onActivate() {
    printf("alternate_costumes::onActivate:");

 
        CDECL_CALL(0x00642340, this);
    
}

void alternate_costumes::OnDeactivate(FEMenu *a2) {
    printf("alternate_costumes::OnDeactivate:");
    CDECL_CALL(0x00614A30, this, a2);
}

void alternate_costumes::OnTriangle(int a2) {
    printf("alternate_costumes::OnTriangle:");
    CDECL_CALL(0x00640BC0, this, a2);
}

void alternate_costumes::sub_614C60(int a2) {

        CDECL_CALL(0x00614C60, this, a2);
    
}

void alternate_costumes::update_selected(int a2)
{
    if constexpr (0)
	{
        for (auto &p : this->field_C4) {
            auto v7 = (float) (a2 * this->field_10C);

            auto vtbl = bit_cast<fastcall_call(*)[1]>(p.field_0->m_vtbl);
            void (__fastcall *field_C0)(void *, void *, Float, Float) = CAST(field_C0, (*vtbl)[48]);

            field_C0(p.field_0, nullptr, 0.0, v7);
        }

        int v6;

        auto v5 = this->field_114;
        if (a2 >= 0) {
            v6 = (v5 + 5) % 6;
        } else {
            v6 = (v5 + 4) % 6;
        }

        auto v8 = (float) -(a2 * this->field_110);

        auto *panel = this->field_C4[v6].field_0;

        auto vtbl = bit_cast<fastcall_call(*)[1]>(panel->m_vtbl);
        void (__fastcall *field_C0)(void *, void *, Float, Float) = CAST(field_C0, (*vtbl)[48]);
        field_C0(panel, nullptr, 0.0, v8);

        this->sub_640740(this->field_118);
        this->sub_640510(this->field_114);
        this->sub_614C60(a2);
        this->set_info();
        this->field_118 = this->field_114;
    } else {
        CDECL_CALL(0x00640840, this, a2);
    }
}

void alternate_costumes::OnUp(int a2) {
    printf("alternate_costumes::OnUp:");
    CDECL_CALL(0x00640950, this, a2);
}

void alternate_costumes::OnDown(int a2) {
    printf("alternate_costumes::OnDown:");
    CDECL_CALL(0x00640A00, this, a2);
}

void alternate_costumes_patch() {
    {
        auto address = func_address(&alternate_costumes::update_selected);
        REDIRECT(0x0064097A, address);
        REDIRECT(0x00640A34, address);
    }

    {
        auto address = func_address(&alternate_costumes::sub_640740);
        REDIRECT(0x0064090A, address);
    }

    {
        auto address = func_address(&alternate_costumes::sub_640510);
        REDIRECT(0x006425BD, address);
        REDIRECT(0x00640918, address);
    }

    {
        auto address = func_address(&alternate_costumes::sub_640BC0);
        set_vfunc(0x00894B78, address);
    }

    auto addr = 0x00894B6C;

    {
        auto address = func_address(&alternate_costumes::onActivate);
        set_vfunc(addr, address);
    }

    addr += 0x4;

    {
        auto address = func_address(&alternate_costumes::OnDeactivate);
        set_vfunc(addr, address);
    }

    addr += 0x8;

    {
        auto address = func_address(&alternate_costumes::OnTriangle);
        set_vfunc(addr, address);
    }

    addr += 0x4;

    {
        auto address = func_address(&alternate_costumes::OnUp);
        set_vfunc(addr, address);
    }

    addr += 0x4;

    {
        auto address = func_address(&alternate_costumes::OnDown);
        set_vfunc(addr, address);
    }
}
