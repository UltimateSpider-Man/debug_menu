#include "panelquad.h"

#include "common.h"
#include "func_wrapper.h"
#include "ngl.h"

#include "trace.h"
#include "vtbl.h"



VALIDATE_SIZE(PanelQuad, 0x38);

PanelQuad::PanelQuad() {
    THISCALL(0x00637E80, this);
}

PanelQuad::PanelQuad(from_mash_in_place_constructor *a2) {
    THISCALL(0x00637F00, this, a2);
}

void PanelQuad::SetPos(float a2, float a3)
{
  THISCALL(0x00608F70, this, a2, a3);
}

PanelQuad * __fastcall PanelQuad_constructor(PanelQuad *self, int, from_mash_in_place_constructor *a2)
{
    TRACE("PanelQuad::PanelQuad");
    return (PanelQuad*) THISCALL(0x00637F00, self, a2);
}

PanelQuad::~PanelQuad() {
    THISCALL(0x0043F7F0, this);
}

void PanelQuad::_destruct_mashed_class()
{
    TRACE("PanelQuad::_destruct_mashed_class");

    THISCALL(0x00633A30, this);
}



int PanelQuad::_get_mash_sizeof()
{
#ifdef TARGET_XBOX
    return 0x48;
#else
    return 0x4C;
#endif

}

vector2d PanelQuad::GetMax() {
    vector2d result;
    THISCALL(0x00616990, this, &result);

    return result;
}

vector2d PanelQuad::GetMin() {
    vector2d result;
    THISCALL(0x006168C0, this, &result);

    return result;
}

void PanelQuad::sub_616710(Float a2, Float a3) {
    THISCALL(0x00616710, this, a2, a3);
}

void PanelQuad::SetTexture(nglTexture *a2) {
    THISCALL(0x00616290, this, a2);
}

void PanelQuad::Draw()
{
    TRACE("PanelQuad::Draw");

    THISCALL(0x00616090, this);
}


//0x006284D0
//virtual 
void SetPos(float* a2, float* a3)
{
    TRACE("PanelQuad::SetPos");

    THISCALL(0x006284D0, &a2, &a3);
}


float PanelQuad::GetCenterX()
{
    TRACE("PanelQuad::GetCenterX");

    auto result = this->field_14[0];
    printf("%f", result);
    return result;
	
	THISCALL(0x0043F770,this);
}

float PanelQuad::GetCenterY()
{
    TRACE("PanelQuad::GetCenterY");

    auto result = this->field_14[1];
    printf("%f", result);
    return result;
}

void PanelQuad::CopyFrom(const PanelQuad *a2) {
    THISCALL(0x0062E130, this, a2);
}

void PanelQuad::TurnOn(bool a2) {
    void(__fastcall * func)(void*, void*, bool) = CAST(func, get_vfunc(m_vtbl, 0x5C));

    func(this, nullptr, a2);
}

void PanelQuad::GetPos(float* a2, float* a3)
{
    TRACE("PanelQuad::GetPos");


    void (__fastcall * func)(void*, void*, float*, float*) = bit_cast<decltype(func)>(0x006166A0);

    func(this, nullptr, a2, a3);

    return (void)func;

}


void PanelQuad::SetPos(float* a2, float* a3)
{
    TRACE("PanelQuad::GetPos");


    void(__fastcall * func)(void*, void*, float*, float*) = bit_cast<decltype(func)>(0x006284D0);

    func(this, nullptr, a2, a3);

    return (void)func;

}



void PanelQuad_patch()
{
    {
        FUNC_ADDRESS(address, &PanelQuad::Draw);
        set_vfunc(0x0087B9E8, address);
        set_vfunc(0x0087BAB8, address);

        REDIRECT(0x00615F99, nglListAddMesh);
    }

    {
        FUNC_ADDRESS(address, &PanelQuad::GetCenterX);
        SET_JUMP(0x0043F770, address);
    }

    {
        FUNC_ADDRESS(address, &PanelQuad::GetCenterY);
        SET_JUMP(0x0043F780, address);
    }



    {
        REDIRECT(0x00429C61, PanelQuad_constructor);
    }

    {
        FUNC_ADDRESS(address, &PanelQuad::_destruct_mashed_class);
        set_vfunc(0x0087B990, address);
    }


    {
        FUNC_ADDRESS(address, &PanelQuad::_get_mash_sizeof);
        set_vfunc(0x0087B9DC, address);
    }
}
