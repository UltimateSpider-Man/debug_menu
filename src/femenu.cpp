#include "femenu.h"

#include "common.h"
#include "femenusystem.h"
#include "fetext.h"
#include "input_mgr.h"
#include "memory.h"
#include "utility.h"
#include "variables.h"
#include "vtbl.h"

VALIDATE_SIZE(FEMenu, 44u);

FEMenu::FEMenu(FEMenuSystem* a2, uint32_t a3, int a4, int a5, int16_t a6, int16_t a7)
{
    this->m_vtbl = 0x00893C88;

    this->field_4 = CAST(this->field_4, operator new(4 * a3));
    for (uint32_t i = 0; i < a3; ++i) {
        this->field_4[i] = nullptr;
    }

    this->field_28 = static_cast<decltype(this->field_28)>(a7 & 0xFDFF);
    this->field_C = a4;
    this->field_10 = a5;
    this->field_20 = 0;
    this->highlighted = -1;
    this->field_8 = a2;
    this->num_entries = static_cast<decltype(this->num_entries)>(a3);
    this->field_14 = 28;
    this->field_18 = 0;
    this->field_26 = a6;
    if (a2 != nullptr) {
        this->field_2B = a2->GetDefaultColorScheme();
    }
    else {
        this->field_2B = 0;
    }
}

void *FEMenu::operator new(size_t size)
{
    auto *mem = mem_alloc(size);
    return mem;
}



void FEMenu::Load()
{
    void (__fastcall *func)(void *) = CAST(func, get_vfunc(m_vtbl, 0x10));
    func(this);
}


void FEMenu::Init()
{
    void (__fastcall *func)(void *) = CAST(func, get_vfunc(m_vtbl, 0xC));

    func(this);
}

void FEMenu::OnActivate() {
    void (__fastcall *func)(void *) = CAST(func, get_vfunc(m_vtbl, 0x2C));

    func(this);
}

void FEMenu::OnDeactivate(FEMenu *a2) {
    void (__fastcall *func)(FEMenu *, void *, FEMenu *) = CAST(func, get_vfunc(m_vtbl, 0x30));
    func(this, nullptr, a2);
}

void FEMenu::OnSelect(int a2)
{
    void (__fastcall *func)(FEMenu *, void *, int) = CAST(func, get_vfunc(m_vtbl, 0x34));
    func(this, nullptr, a2);
}

void FEMenu::OnStart(int a2)
{
    void (__fastcall *func)(FEMenu *, void *, int) = CAST(func, get_vfunc(m_vtbl, 0x38));
    func(this, nullptr, a2);
}

void FEMenu::OnUp(int a2)
{
    void (__fastcall *func)(FEMenu *, void *, int) = CAST(func, get_vfunc(m_vtbl, 0x3C));
    func(this, nullptr, a2);
}

void FEMenu::OnDown(int a2)
{
    void (__fastcall *func)(FEMenu *, void *, int) = CAST(func, get_vfunc(m_vtbl, 0x40));
    func(this, nullptr, a2);
}

void FEMenu::OnLeft(int a2)
{
    void (__fastcall *func)(FEMenu *, void *, int) = CAST(func, get_vfunc(m_vtbl, 0x44));
    func(this, nullptr, a2);
}

void FEMenu::OnRight(int a2)
{
    void (__fastcall *func)(FEMenu *, void *, int) = CAST(func, get_vfunc(m_vtbl, 0x48));
    func(this, nullptr, a2);
}

void FEMenu::OnCross(int a2)
{
    void (__fastcall *func)(FEMenu *, void *, int) = CAST(func, get_vfunc(m_vtbl, 0x4C));
    func(this, nullptr, a2);
}

void FEMenu::OnTriangle(int a2)
{
    void (__fastcall *func)(FEMenu *, void *, int) = CAST(func, get_vfunc(m_vtbl, 0x50));

    func(this, nullptr, a2);
}

void FEMenu::OnSquare(int a2)
{
    void (__fastcall *func)(FEMenu *, void *, int) = CAST(func, get_vfunc(m_vtbl, 0x54));

    func(this, nullptr, a2);
}

void FEMenu::OnCircle(int a2)
{
    void (__fastcall *func)(FEMenu *, void *, int) = CAST(func, get_vfunc(m_vtbl, 0x58));

    func(this, nullptr, a2);
}

void FEMenu::OnL1(int a2)
{
    void (__fastcall *func)(FEMenu *, void *, int) = CAST(func, get_vfunc(m_vtbl, 0x5C));
    func(this, nullptr, a2);
}

void FEMenu::OnR1(int a2)
{
    void (__fastcall *func)(FEMenu *, void *, int) = CAST(func, get_vfunc(m_vtbl, 0x60));
    func(this, nullptr, a2);
}

void FEMenu::OnL2(int a2)
{
    void (__fastcall *func)(FEMenu *, void *, int) = CAST(func, get_vfunc(m_vtbl, 0x64));
    func(this, nullptr, a2);
}

void FEMenu::OnR2(int a2)
{
    void (__fastcall *func)(FEMenu *, void *, int) = CAST(func, get_vfunc(m_vtbl, 0x68));
    func(this, nullptr, a2);
}

void FEMenu::OnAnyButtonPress(int a2, int a3) {
    printf("OnAnyButtonPress:");

    if ((128 & this->field_28) != 0 && (a3 == 4 || a3 == 8 || a3 == 16 || a3 == 32) &&
        a2 == input_mgr::instance()->field_58 - 1000000) {
        this->field_2A = a3;
        this->field_1C = 0.30000001;
    }
}

void FEMenu::OnButtonRelease(int a2, int a3) {
    if ((128 & this->field_28) != 0 && (a3 == 4 || a3 == 8 || a3 == 16 || a3 == 32) &&
        a2 == input_mgr::instance()->field_58 - 1000000 && a3 == this->field_2A) {
        this->field_2A = -1;
    }
}


void FEMenu::SetVis(int a2) {
    printf("SetVis(): 0x%08X", m_vtbl);

  
        THISCALL(0x0060AE00, this, a2);
    
}

void FEMenu::Up() {
    void (__fastcall *func)(void *) = CAST(func, get_vfunc(m_vtbl, 0xA4));

    func(this);
}

void FEMenu::Down() {
    void (__fastcall *func)(void *) = CAST(func, get_vfunc(m_vtbl, 0xA8));

    func(this);
}

void FEMenu::ButtonHeldAction() {
    if ((128 & this->field_28) != 0) {
        auto v1 = this->field_2A;
        if (v1 == 4) {
            this->Up();
        } else if (v1 == 8) {
            this->Down();
        }
    }
}

void sub_582A30() {
    SetEvent(hEvent());
    WaitForSingleObject(hObject(), 0xFFFFFFFF);
    CloseHandle(hObject());
    CloseHandle(hEvent());
    hObject() = nullptr;
    hEvent() = nullptr;
}

void sub_5A6D70() {
    printf("sub_5A6D70");

    CDECL_CALL(0x005A6D70);
}

void FEMenu_patch() {

    return;
    {
        REDIRECT(0x00630B61, sub_5A6D70);
        REDIRECT(0x00635CA0, sub_5A6D70);
    }



    if constexpr (0) {


        {
            FUNC_ADDRESS(address, &FEMenu::SetVis);
            SET_JUMP(0x0060AE00, address);
        }


        {
            FUNC_ADDRESS(address, &FEMenu::OnAnyButtonPress);
            set_vfunc(0x0089426C, address);
        }
    }
}
