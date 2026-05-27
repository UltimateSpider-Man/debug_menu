#pragma once



#include "float.hpp"
#include "mstring.h"
#include "mvector.h"
#include "vector2d.h"
#include "color32.h"

#include "fetext.h"
#include <cstdint>

struct from_mash_in_place_constructor;
struct nglTexture;
struct PanelQuadSection;
struct PanelMeshSection;

struct PanelQuad {
    vector2d field_14;
    mVector<PanelQuadSection> pqs;
    PanelMeshSection *pmesh;
    float field_34;
    float field_38;
    mString field_3C;


    PanelQuad();

    PanelQuad(from_mash_in_place_constructor* a2); 

    void SetPos(float a2, float a3);


    PanelQuad* __fastcall PanelQuad_constructor(PanelQuad* self, int, from_mash_in_place_constructor* a2);


    ~PanelQuad(); 

    void _destruct_mashed_class();

    void TurnOn(bool a2);

    void GetPos(float* a2, float* a3);




    int _get_mash_sizeof();


    vector2d GetMax(); 

    vector2d GetMin(); 

    void sub_616710(Float a2, Float a3); 

    void SetTexture(nglTexture* a2); 


    void Draw();



    //0x006284D0
    //virtual 
    void SetPos(float* a2, float* a3);



    float GetCenterX();


    float GetCenterY();


    void CopyFrom(const PanelQuad* a2);

};

extern void PanelQuad_patch();;
