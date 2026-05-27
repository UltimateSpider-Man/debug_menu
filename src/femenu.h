#pragma once

#include "float.hpp"
#include "fetext.h"
#include "float.hpp"

#include <cstdint>

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
    FEMenu(FEMenuSystem* a2, uint32_t a3, int a4, int a5, int16_t a6, int16_t a7);

    void* operator new(size_t size);




        void Load();



        void Init();


    void OnActivate(); 

    void OnDeactivate(FEMenu* a2);

    void OnSelect(int a2);


    void OnStart(int a2);


    void OnUp(int a2);


    void OnDown(int a2);


    void OnLeft(int a2);


    void OnRight(int a2);


    void OnCross(int a2);


    void OnTriangle(int a2);


    void OnSquare(int a2);


    void OnCircle(int a2);


    void OnL1(int a2);


    void OnR1(int a2);


    void OnL2(int a2);


    void OnR2(int a2);


    void OnAnyButtonPress(int a2, int a3); 


    void OnButtonRelease(int a2, int a3); 



    void SetVis(int a2); 

    

    void Up();


    void Down();


    void ButtonHeldAction();

    
};

extern void sub_582A30();

extern void sub_5A6D70();

extern void FEMenu_patch();
