#include "pause_menu_status.h"

#include "common.h"
#include "func_wrapper.h"
#include "memory.h"
#include "utility.h"
#include "trace.h"
#include "panelquad.h"
#include "pausemenusystem.h"
#include "panelfile.h"
#include "cursor.h"
#include "pause_menu_transition.h"
#include "vtbl.h"


#include "sound_instance_id.h"


VALIDATE_SIZE(pause_menu_status, 0x130);

pause_menu_status::pause_menu_status(FEMenuSystem *a2, int a3, int a4)
    : FEMenu(a2, 0, a3, a4, 0, 0) {
    THISCALL(0x0060FF30, this, a2, a3, a4);
}

void pause_menu_status::OnTriangle(int a2)
{

        THISCALL(0x0061CCE0, this, a2);  
    
}

void pause_menu_status::_Load() {
    TRACE("pause_menu_status::Load");

    if constexpr (1)
    {
        auto *v2 = this->field_E8;
        this->field_12C = 0;
        this->field_12D = 0;
        auto *v3 = v2->field_2C;
        this->field_2C[0] = v3->GetPQ("pm_all_back_01");
        this->field_2C[1] = v3->GetPQ("pm_all_back_02");
        this->field_2C[2] = v3->GetPQ("pm_all_back_02a");
        this->field_2C[3] = v3->GetPQ("pm_all_back_02b");
        this->field_2C[4] = v3->GetPQ("pm_all_back_03");
        this->field_2C[5] = v3->GetPQ("pm_all_back_04");
        this->field_2C[6] = v3->GetPQ("pm_all_back_05");
        this->field_2C[7] = v3->GetPQ("pm_all_detail_02");
        this->field_2C[8] = v3->GetPQ("pm_all_detail_03");
        this->field_2C[9] = v3->GetPQ("pm_all_detail_04");
        this->field_2C[10] = v3->GetPQ("pm_all_detail_05");
        this->field_2C[11] = v3->GetPQ("pm_all_detail_06");
        this->field_2C[12] = v3->GetPQ("pm_all_detail_07");
        this->field_2C[13] = v3->GetPQ("pm_all_detail_08");
        this->field_2C[14] = v3->GetPQ("pm_all_detail_09");
        this->field_2C[15] = v3->GetPQ("pm_all_detail_10");
        this->field_2C[16] = v3->GetPQ("pm_all_detail_11");
        this->field_2C[17] = v3->GetPQ("pm_all_box_01");
        this->field_2C[18] = v3->GetPQ("pm_all_box_02");
        this->field_2C[19] = v3->GetPQ("pm_all_box_03");
        this->field_2C[20] = v3->GetPQ("pm_all_icon");
        this->field_2C[21] = v3->GetPQ("pm_status_explanation_box_01");
        this->field_2C[22] = v3->GetPQ("pm_status_explanation_box_02");
        this->field_2C[23] = v3->GetPQ("pm_status_hilite_text");
        this->field_2C[24] = v3->GetPQ("pm_status_hilite_text_01");
        this->field_90[0] = v3->GetPQ("pm_scroll_arrow_down");
        this->field_90[1] = v3->GetPQ("pm_scroll_arrow_up");
        this->field_90[2] = v3->GetPQ("pm_scroll_bar_01");
        this->field_90[3] = v3->GetPQ("pm_scroll_bar_02");
        this->field_90[4] = v3->GetPQ("pm_scroll_spider_icon");
        this->field_A4[0] = v3->GetTextPointer("pm_header_text_AWARDS");
        this->field_A4[1] = v3->GetTextPointer("pm_status_explanation_box_text_BLANK");
        this->field_A4[2] = v3->GetTextPointer("pm_status_text_left_01_BLANK");
        this->field_A4[3] = v3->GetTextPointer("pm_status_text_left_02_BLANK");
        this->field_A4[4] = v3->GetTextPointer("pm_status_text_left_03_BLANK");
        this->field_A4[5] = v3->GetTextPointer("pm_status_text_left_04_BLANK");
        this->field_A4[6] = v3->GetTextPointer("pm_status_text_left_05_BLANK");
        this->field_A4[7] = v3->GetTextPointer("pm_status_text_left_06_BLANK");
        this->field_A4[8] = v3->GetTextPointer("pm_status_text_right_01_BLANK");
        this->field_A4[9] = v3->GetTextPointer("pm_status_text_right_02_BLANK");
        this->field_A4[10] = v3->GetTextPointer("pm_status_text_right_03_BLANK");
        this->field_A4[11] = v3->GetTextPointer("pm_status_text_right_04_BLANK");
        this->field_A4[12] = v3->GetTextPointer("pm_status_text_right_05_BLANK");
        this->field_A4[13] = v3->GetTextPointer("pm_status_text_right_06_BLANK");

        auto *v5 = new PanelQuad {};
        auto *v6 = this->field_2C[21];
        this->field_DC = v5;
        v5->CopyFrom(v6);

        float v34[4] {};
        float v30[4] {};
        this->field_DC->GetPos(v34, v30);

        v30[0] = v30[0] - 50.f;
        v30[1] = v30[1] - 70.f;
        v30[2] = v30[2] - 50.f;
        v30[3] = v30[3] - 70.f;

        this->field_DC->SetPos(v34, v30);

        auto *v10 = new PanelQuad {};
        auto *v11 = this->field_2C[22];
        this->field_E0 = v10;
        v10->CopyFrom(v11);
        this->field_E0->GetPos(v34, v30);

        v30[0] = v30[0] - 50.f;
        v30[1] = v30[1] - 70.f;
        v30[2] = v30[2] - 50.f;
        v30[3] = v30[3] - 70.f;

        this->field_E0->SetPos(v34, v30);
        int v27 = this->field_A4[1]->GetX();
        int v26 = this->field_A4[1]->GetY();

        float a5 = v26 - 45.f;
        float a4 = v27;
        auto *v15 = new FEText {
                static_cast<font_index>(1),
                static_cast<global_text_enum>(292),
                a4,
                a5,
                5,
                static_cast<panel_layer>(1),
                1.0,
                16,
                0,
                color32 {255,200,200,200}};

        auto *v16 = this->field_90[4];
        this->field_E4 = v15;
        this->field_120 = v16->GetCenterY();
        auto *v18 = v3->GetPQ("pm_scroll_spider_icon_end_marker");
        this->field_124 = v18->GetCenterY();
        auto *v19 = this->field_A4[2];
        auto v28 = this->field_2C[23]->GetCenterY();
        auto v20 = v19->GetY();
        auto *v21 = this->field_2C[24];
        auto *v22 = this->field_A4[2];
        this->field_118 = v28 - v20;
        auto v29 = v21->GetCenterY();
        auto v23 = v22->GetY();
        this->field_128 = 0;
        this->field_11C = v29 - v23;
        this->update_selected();
    }
    else
    {


        void(__fastcall* func)(void*, void*) = bit_cast<decltype(func)>(0x0063B890);

        func(this, nullptr);


    }
}

mString *pause_menu_status::get_element_desc(mString *out, int a3)
{
    TRACE("pause_menu_status::get_element_desc");


    return  (mString*) THISCALL(0x00610290, this, out, a3);



}

const mString *pause_menu_status::get_element_value(int index)
{
    TRACE("pause_menu_status::get_element_value");

        return reinterpret_cast<const mString*>(
            THISCALL(0x006101B0, this, index));

}


mString *pause_menu_status::get_element_value_0(mString *out, int index)
{

    return reinterpret_cast<mString *(__fastcall*)(void*, void*, mString *, int)>(0x00610210)(this, nullptr, out, index);

        
}

void pause_menu_status::update_selected()
{
    TRACE("pause_menu_status::update_selected");

    int max_data_row;
    switch (this->field_EC) {
        case 0:  max_data_row = 16; break;
        case 1:  max_data_row = 20; break;
        case 2:  max_data_row = 3;  break;
        default: max_data_row = 0;  break;
    }

    auto clamp_int = [](int v, int lo, int hi) -> int {
        return v < lo ? lo : (v > hi ? hi : v);
    };


    this->field_128 = clamp_int(this->field_128, 0, max_data_row);
    this->field_110 = clamp_int(this->field_110, 0, max_data_row);
    this->field_114 = clamp_int(this->field_114, 0, max_data_row);

    constexpr int kLeftColMin  = 2;
    constexpr int kLeftColMax  = 7;   // <-- restored from 21
    constexpr int kRightColOff = 6;


    if (this->field_104 != kRightColOff) {
        this->field_104 = kRightColOff;
    }

    this->field_FC  = clamp_int(this->field_FC,  kLeftColMin,    kLeftColMax);
    this->field_100 = clamp_int(this->field_100, this->field_FC, kLeftColMax);
    this->field_108 = clamp_int(this->field_108, this->field_FC, this->field_100);
    this->field_10C = clamp_int(this->field_10C, this->field_FC, this->field_100);


    if (this->field_F0 == nullptr ||
        this->field_F4 == nullptr ||
        this->field_F8 == nullptr)
    {
        return;
    }


    void(__fastcall * func)(void*, void*) = bit_cast<decltype(func)>(0x0061CC30);

    func(this, nullptr);
}

void pause_menu_status::Init()
{
     TRACE("pause_menu_status::Init");


        void(__fastcall * func)(void*, void*) = bit_cast<decltype(func)>(0x0062AB20);

        func(this, nullptr);
    
}


void pause_menu_status::OnDown(int a2)
{
    TRACE("pause_menu_status::OnDown");

    if constexpr (1) {

        int count;
        switch (this->field_EC) {
            case 0:  count = 17;
			break;   
            case 1:  count = 21;
			break;   
            case 2:  count = 4;  
			break;   
            default: count = 0;  
			break;
        }
        if (count <= 0) {
            this->update_selected();
            return;
        }


        const int top         = this->field_FC;
        const int bottom      = this->field_100;
        const int window_size = bottom - top + 1;
        const int max_scroll  = (count > window_size) ? (count - window_size) : 0;


        int next_row = this->field_110 + 1;

        if (next_row >= count) {

            next_row         = 0;
            this->field_128  = 0;
            this->field_108  = top;
        }
        else if (next_row >= this->field_128 + window_size) {

            this->field_128  = next_row - (window_size - 1);
            if (this->field_128 > max_scroll) {
                this->field_128 = max_scroll;
            }
            this->field_108  = bottom;
        }
        else {

            this->field_108  = top + (next_row - this->field_128);
        }

        this->field_110 = next_row;

        if (this->field_110 != this->field_114) {
            static string_hash sfx_id_hash{"fe_gs_udscroll"};
            [[maybe_unused]] sound_instance_id id =
                sub_60B960(sfx_id_hash, 1.0f, 1.0f);
        }

        this->update_selected();
    }
    else {

        void(__fastcall * func)(void*, void*, int) = bit_cast<decltype(func)>(0x0061D250);

        func(this, nullptr, a2);
    }
}

void pause_menu_status::OnUp(int a2)
{
    TRACE("pause_menu_status::OnUp");

    if constexpr (1) {
        int count;
        switch (this->field_EC) {
            case 0:  count = 17; break;
            case 1:  count = 21; break;
            case 2:  count = 4;  break;
            default: count = 0;  break;
        }
        if (count <= 0) {
            this->update_selected();
            return;
        }

        const int top         = this->field_FC;
        const int bottom      = this->field_100;
        const int window_size = bottom - top + 1;
        const int max_scroll  = (count > window_size) ? (count - window_size) : 0;


        int prev_row = this->field_110 - 1;

        if (prev_row < 0) {

            prev_row         = count - 1;
            this->field_128  = max_scroll;
            this->field_108  = top + (prev_row - this->field_128);
        }
        else if (prev_row < this->field_128) {

            this->field_128  = prev_row;
            this->field_108  = top;
        }
        else {

            this->field_108  = top + (prev_row - this->field_128);
        }

        this->field_110 = prev_row;

        if (this->field_110 != this->field_114) {
            static string_hash sfx_id_hash{"fe_gs_udscroll"};
            [[maybe_unused]] sound_instance_id id =
                sub_60B960(sfx_id_hash, 1.0f, 1.0f);
        }

        this->update_selected();
    }
    else {

        void(__fastcall * func)(void*, void*, int) = bit_cast<decltype(func)>(0x0061D0F0);

        func(this, nullptr, a2);
    }
}


void pause_menu_status::Down() {
    TRACE("pause_menu_status::Down");
    void (__fastcall *on_down)(pause_menu_status *, void *, int)
        = CAST(on_down, get_vfunc(this->m_vtbl, 0x40));
    on_down(this, nullptr, 0);
}
char pause_menu_status::OnMouseEvent(int msg, int a3, int a4)
{
    TRACE("pause_menu_status::OnMouseEvent");

    constexpr int WM_MOUSEMOVE_ = 512;
    constexpr int WM_LBUTTONUP_ = 514;
    constexpr int WM_RBUTTONUP_ = 517;

    int row_count;
    switch (this->field_EC) {
    case 0:  row_count = 17; break;
    case 1:  row_count = 21; break;
    case 2:  row_count = 4;  break;
    default: row_count = 0;  break;
    }

    const int hit_count = min(row_count + 2, 14);

    Cursor* cur = g_cursor();

    // sub_581CD0: returns the row index under the cursor, or -1 if none.
    auto hit_test = [&]() -> int {
        return reinterpret_cast<int(__cdecl*)(void*, void*, int, int, int, int)>(0x00581CD0)
            (cur, &this->field_A4[0], hit_count, 450, 2, 10);
        };

    // sub_5A0330: true if the cursor is inside the valid menu zone.
    auto in_valid_zone = [&]() -> bool {
        return reinterpret_cast<int(__cdecl*)(void*)>(0x005A0330)(cur) != 0;
        };

    // Vtable helper: pull a vfunc and invoke it with the standard (this, nullptr, 0) signature.
    auto call_vfunc = [&](std::size_t offset) {
        using Fn = void(__fastcall*)(pause_menu_status*, void*, int);
        auto fn = reinterpret_cast<Fn>(get_vfunc(this->m_vtbl, offset));
        fn(this, nullptr, 0);
        };

    switch (msg) {
    case WM_MOUSEMOVE_: {
        if (cur) cur->sub_581C60();
        this->field_10C = this->field_108;

        const int hit = hit_test();

        if (hit == -1) {
            if (in_valid_zone()) {
                if (this->field_12C) {
                    this->field_12C = false;
                    this->update_selected();
                }
            }
            else {
                this->field_12C = true;
                this->field_12D = false;
                this->update_selected();
            }
        }
        else {
            if (this->field_12C) {
                this->field_12C = false;
                this->update_selected();
            }

            const bool unchanged = (this->field_10C == hit);
            this->field_110 = this->field_128 + hit - this->field_FC;
            this->field_108 = hit;

            if (!unchanged) {
                this->update_selected();
            }
        }
        return 0;
    }

    case WM_LBUTTONUP_: {
        const int hit = hit_test();

        if (hit != -1) {
            call_vfunc(0x4C);            // on_cross
            return 0;
        }

        if (!in_valid_zone()) {
            call_vfunc(0x38);            // on_start
            return 0;
        }

        const int needed_rows =
            reinterpret_cast<int(__cdecl*)(void*)>(0x00610180)(this);
        const int window_size = this->field_100 - this->field_FC + 1;
        if (needed_rows <= window_size) return 0;

        const int sw =
            reinterpret_cast<int(__cdecl*)(void*, PanelQuad*, PanelQuad*, PanelQuad*)>(0x00581F00)
            (cur, this->field_90[4], this->field_90[1], this->field_90[0]);
        if (sw == 0) return 0;

        switch (sw - 1) {
        case 0:
            reinterpret_cast<void(__cdecl*)(void*)>(0x0061D880)(this);  // Up()
            break;
        case 2:
            this->Down();
            break;
        default:
            break;
        }
        return 0;
    }

    case WM_RBUTTONUP_: {
        call_vfunc(0x50);                // on_triangle
        return 0;
    }

    default:
        return static_cast<char>(msg - 5);
    }
}

void pause_menu_status_patch()
{
    {
        FUNC_ADDRESS(address, &pause_menu_status::_Load);
    //   set_vfunc(0x008940B0, address);
    }

    {
        FUNC_ADDRESS(address, &pause_menu_status::get_element_desc);
     //   REDIRECT(0x0061CD64, address);
    }
	    {
        FUNC_ADDRESS(address, &pause_menu_status::update_selected);
        REDIRECT(0x0061D235, address);
    }
	
    {
        FUNC_ADDRESS(address, &pause_menu_status::get_element_value_0);
        REDIRECT(0x0061CCDA, address);
    }
	
    {
	FUNC_ADDRESS(address, &pause_menu_status::OnTriangle);
//	set_vfunc(0x008940F0, address);
    }	
    {
	FUNC_ADDRESS(address, &pause_menu_status::Init);
//	set_vfunc(0x008940CC, address);	
    }	
	
    {
	FUNC_ADDRESS(address, &pause_menu_status::OnUp);
	set_vfunc(0x008940DC, address);	
    }
	
    {
	FUNC_ADDRESS(address, &pause_menu_status::OnDown);
	set_vfunc(0x008940E0, address);	
    }
    {
	FUNC_ADDRESS(address, &pause_menu_status::Down);
	//REDIRECT(0x0062AD74, address);	
    }
    {
	FUNC_ADDRESS(address, &pause_menu_status::OnMouseEvent);
	set_vfunc(0x00894140, address);	
    }		
	 
}
