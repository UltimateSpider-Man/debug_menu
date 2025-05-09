#pragma once

#include "float.hpp"
#include "fetext.h"

#include <cstdint>

struct FEMenu;
struct FEMultiLineText;
struct FEText;

struct FEMenuEntry {
    std::intptr_t m_vtbl;
    FEMenu *field_4;
    int field_8;
    int field_C;
    FEText *field_10;
    bool field_14;
    bool field_15;
    bool field_16;
    bool field_17;

    FEMenuEntry(FEMenu *a1, FEText *a2, bool a3);

    //0x00629650
    FEMenuEntry(global_text_enum a3, FEMenu *arg4, bool a11, font_index a2, int a6);

    /* virtual */ inline void Update(Float a2)
    {
    }

    //0x006497D0
    /* virtual */inline bool GetDisable();

    /* virtual */ inline void SetPos(Float a2, Float a3);

    //0x0060A840
  inline  void CommonConstructor(FEText *a2, FEMenu *a3);

   inline void SetText(global_text_enum a2);

    struct string {
        int field_0[2];

        char *guts;
        void *field_C;
    };

  inline  void SetTextNoLocalize(string a2);

    /* virtual */inline void Highlight(bool a2, bool a3);
};

extern void FEMenuEntry_patch();
