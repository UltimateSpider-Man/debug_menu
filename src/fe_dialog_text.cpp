#include "fe_dialog_text.h"

#include "common.h"
#include "femenusystem.h"
#include "fetext.h"
#include "femultilinetext.h"
#include "func_wrapper.h"
#include "panelfile.h"
#include "trace.h"
#include "utility.h"
#include "vtbl.h"

#include <cassert>
#include <cstdio>

VALIDATE_SIZE(fe_dialog_text, 0x10C);
VALIDATE_OFFSET(fe_dialog_text, field_78, 0x78);
VALIDATE_OFFSET(fe_dialog_text, field_9C, 0x9C);

fe_dialog_text::fe_dialog_text(FEMenuSystem *a2, int a3, int a4) : FEMenu(a2, 0, a3, a4, 0, 0) {
    CDECL_CALL(0x0060D570, this, a2, a3, a4);
}

void fe_dialog_text::set_text(string a1) {
    //sp_log("fe_dialog_text::set_text: %s", a1.data);
    CDECL_CALL(0x0060D960, this, a1);
}

void fe_dialog_text::_Load() {
    TRACE("fe_dialog_text::Load");

    if constexpr (0)
    {
        assert(panel == nullptr && "Dialog text widget already loaded.");

        auto *v2 = PanelFile::UnmashPanelFile("text_box_big",panel_layer{1});
        this->panel = v2;
        this->field_90 = this->panel->GetAnimationPointer(0);
        this->field_94 = this->panel->GetAnimationPointer(1);
        this->field_30 = v2->GetPQ("tb_big_back_01");
        this->field_34 = v2->GetPQ("tb_big_back_02");
        this->field_38 = v2->GetPQ("tb_detail_01");
        this->field_3C = v2->GetPQ("tb_big_gradient_m_completed");
        this->field_40 = v2->GetPQ("tb_big_gradient_m_failed");
        this->field_44 = v2->GetPQ("tb_big_gradient_reward");
        this->field_48 = v2->GetPQ("tb_big_gradient_hints");
        this->field_4C = v2->GetPQ("tb_big_gradient_generic_text");
        this->field_50 = v2->GetPQ("tb_text_link_left_hilite");
        this->field_54 = v2->GetPQ("tb_text_link_center_hilite");
        this->field_58 = v2->GetPQ("tb_text_link_right_hilite");
        this->field_5C = v2->GetPQ("tb_scroll_arrow_down");
        this->field_60 = v2->GetPQ("tb_scroll_arrow_up");
        this->field_64 = v2->GetPQ("tb_scroll_bar_01");
        this->field_68 = v2->GetPQ("tb_scroll_bar_02");
        this->field_6C = v2->GetPQ("tb_scroll_spider_icon");
        this->field_70 = v2->GetPQ("tb_scroll_spider_icon_end_marker");
        this->field_74 = v2->GetPQ("tb_body_text_scroll_template");
        this->field_78 = v2->GetTextPointer("tb_header_text_MISSIONCOMPLETED");
        this->field_7C = v2->GetTextPointer("tb_body_text_generic_EMPTY");
        this->field_80 = v2->GetTextPointer("tb_body_text_scroll_EMPTY");
        this->field_84 = v2->GetTextPointer("tb_text_link_left_BLANK");
        this->field_88 = v2->GetTextPointer("tb_text_link_right_BLANK");
        this->field_8C = v2->GetTextPointer("tb_text_link_center_BLANK");

        {
            mString v69 {""};
            FEText::string text;
            std::memcpy(&text, &v69, sizeof(v69));

            this->field_78->SetTextNoLocalize(text);

            this->field_7C->SetTextNoLocalize(text);

            this->field_80->SetTextNoLocalize(text);
        }
        
        bit_cast<FEMultiLineText *>(this->field_80)->SetNumLines(15);
        this->field_84->SetText(global_text_enum{ 148 });
        this->field_88->SetText(global_text_enum{ 149 });
        this->field_8C->SetText(global_text_enum{ 150 });

        this->field_78->SetNoFlash(color32 {255,200,200,200});
        this->field_7C->SetNoFlash(color32 { 255,200,200,200 });
        this->field_80->SetNoFlash(color32 { 255,200,200,200 });
        this->field_84->SetNoFlash(color32 {255,230,208 ,63});
        this->field_88->SetNoFlash(color32 {255,200,114,56});
        this->field_8C->SetNoFlash(color32 { 255,230,208 ,63 });
     
        this->field_8C->SetScale(1.2, 1.2);
        this->field_9C = 0;
        this->field_A0 = false;
        this->field_A1 = false;
        this->field_6C->GetPos(this->field_A8, this->field_B8);
        this->field_70->GetPos(this->field_A8, this->field_C8);

        std::memcpy(this->field_D8, this->field_B8, sizeof(this->field_B8));
        
        this->field_A2 = false;
        this->field_A3 = false;
        this->field_A4 = 0;
        
        this->field_E8 = this->field_80->GetY();

        float v71[4];
        float v72[4];
        this->field_74->GetPos(v71, v72);
        this->field_EC[0] = v71[0];
        this->field_EC[1] = v71[3];
        this->field_F4[0] = v72[0];
        this->field_F4[1] = v72[3];
    }
    else
    {
        CDECL_CALL(0x00643C90, this);
    }
}

void fe_dialog_text::set_title(string a2) {
    //sp_log("set_title: %d", a2.m_size);

    if constexpr (1) {
        void (__fastcall *SetTextNoLocalize)(void *, void *, string) = CAST(SetTextNoLocalize, get_vfunc(this->field_78->m_vtbl, 0x8C));

        SetTextNoLocalize(this->field_78, nullptr, a2);
    } else {
        CDECL_CALL(0x0060DB30, this, a2);
    }
}

void fe_dialog_text_patch() {

    {
        auto address = func_address(&fe_dialog_text::_Load);
        set_vfunc(0x00893E88, address);
    }

    return;
    {
        auto address = func_address(&fe_dialog_text::set_text);
        REDIRECT(0x0067335A, address);
    }

    {
        auto address = func_address(&fe_dialog_text::set_title);
        REDIRECT(0x00673379, address);
    }
}
