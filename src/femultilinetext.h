#pragma once

#include "fetext.h"

#include "color32.h"
#include "float.hpp"
#include "global_text_enum.h"

#include "func_wrapper.h"
#include <cassert>

struct MultiLineString;
struct mash_info_struct;

struct FEMultiLineText : FEText {
    color32 field_68;
    float field_6C;
    float field_70;
    float field_74;
    float field_78;
    int field_7C;
    int field_80;
    int line_avail_num;
    MultiLineString *lines;
    int field_8C;
    int field_90;
    int field_94;
    int field_98;
    bool field_9C;
    bool field_9D;
    bool field_9E;



    void SetNumLines(int n) {
        TRACE("FEMultiLineText::SetNumLines", std::to_string(n).c_str());

        assert(n != 0);

        CDECL_CALL(0x00617F30, this, n);
    }

    //0x0060A3E0
    [[nodiscard]] static mString ReplaceEndlines(mString a2);
};

void FEMultiLineText_patch();
