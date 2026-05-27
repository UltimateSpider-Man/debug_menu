#include "pause_menu_root.h"


#include "comic_panels.h"
#include "common.h"
#include "entity_base.h"
#include "fe_health_widget.h"
#include "fetext.h"
#include "femanager.h"
#include "femenusystem.h"
#include "igofrontend.h"
#include "mstring.h"
#include "panelquad.h"
#include "pausemenusystem.h"
#include "panelfile.h"
#include "utility.h"
#include "trace.h"
#include "vtbl.h"
#include "wds.h"
#include "movie_manager.h"



#include "game.h"




#include "mission_manager.h"

#include "variables.h"

#include "sound_instance_id.h"



#include "script_object.h"

#include "variables.h"

#include "wds.h"
#include "pause_menu_transition.h"


VALIDATE_SIZE(pause_menu_root, 0x100u);



pause_menu_root::pause_menu_root(FEMenuSystem* a2, int a3, int a4) : FEMenu(a2, 0, a3, a4, 8, 0) {
    this->field_AC = a2;

    this->field_A0 = nullptr;
    this->field_B0 = 0;
    this->field_B4 = 0;
    this->field_30 = 0;
    this->field_2C = 0;
    this->field_2D = 0;
}






void pause_menu_root::_Load()
{
    TRACE("pause_menu_root::Load");


    if constexpr (0)
    {

        auto *v2 = bit_cast<PauseMenuSystem *>(this->field_AC)->field_2C;

        this->field_3C[0] = v2->GetPQ("pm_splash_back_01a");
        this->field_3C[1] = v2->GetPQ("pm_splash_back_01b");
        this->field_3C[2] = v2->GetPQ("pm_splash_back_02a");
        this->field_3C[3] = v2->GetPQ("pm_splash_back_02b");
        this->field_3C[4] = v2->GetPQ("pm_splash_back_03a");
        this->field_3C[5] = v2->GetPQ("pm_splash_back_03b");
        this->field_3C[6] = v2->GetPQ("pm_splash_back_stub_01");
        this->field_3C[7] = v2->GetPQ("pm_splash_back_stub_02");
        this->field_3C[8] = v2->GetPQ("pm_splash_icon");



        this->field_68 = v2->GetPQ("pm_splash_dialog_box_01");
        this->field_6C = v2->GetPQ("pm_splash_dialog_box_02");
        this->field_60 = v2->GetPQ("pm_splash_hilite_text");
        this->field_64 = v2->GetPQ("pm_splash_hilite_text_01");
        this->field_70 = v2->GetPQ("pm_splash_back_04");
        this->field_74 = v2->GetPQ("pm_splash_back_venom");
    


        this->field_78[8] = v2->GetTextPointer("pm_splash_text_09");
        this->field_78[0] = v2->GetTextPointer("pm_splash_text_01");
        this->field_78[1] = v2->GetTextPointer("pm_splash_text_02");
        this->field_78[2] = v2->GetTextPointer("pm_splash_text_03");
        this->field_78[3] = v2->GetTextPointer("pm_splash_text_04");
        this->field_78[4] = v2->GetTextPointer("pm_splash_text_05");
        this->field_78[5] = v2->GetTextPointer("pm_splash_text_06");
        this->field_78[6] = v2->GetTextPointer("pm_splash_text_07");
        this->field_78[7] = v2->GetTextPointer("pm_splash_text_08");


        this->field_9C = v2->GetTextPointer("pm_splash_text_GAMEPAUSED");
        this->field_A0 = v2->GetTextPointer("pm_splash_dialog_box_text_BODY");
        this->field_A4 = v2->GetTextPointer("pm_splash_dialog_box_text_NOWAY");

        this->field_A8 = v2->GetTextPointer("pm_splash_dialog_box_text_OKAY");

        

        for (auto i = 0u; i < 9u; ++i)
        {
            this->field_3C[i]->TurnOn(true);
        }

        this->field_78[0]->SetShown(true);
        this->field_78[0]->SetNoFlash(color32{ 255, 230, 208, 63 });
        this->field_78[0]->SetScale(1.2, 1.2);

        for (auto i = 0u; i < 8u; ++i) 
        {
            auto *v6 = this->field_78[i + 1];
            v6->SetShown(true);
            v6->SetNoFlash(color32{ 255, 200, 114, 56 });
        }

        this->field_68->TurnOn(1);
        this->field_6C->TurnOn(1);
        this->field_60->TurnOn(1);
        this->field_64->TurnOn(1);
        this->field_9C->SetShown(true);
        this->field_9C->SetText(static_cast<global_text_enum>(253));
        this->field_9C->SetNoFlash(color32{ 255, 200, 200, 200 });
        this->field_A0->SetShown(1);
        this->field_A0->SetText(static_cast<global_text_enum>(271));
        this->field_A0->SetNoFlash(color32{ 255, 200, 200, 200 });
        this->field_A4->SetShown(1);
        this->field_A4->SetText(static_cast<global_text_enum>(254));
        this->field_A4->SetNoFlash(color32{ 255, 200, 114, 56 });
        this->field_A8->SetShown(1);
        this->field_A8->SetText(static_cast<global_text_enum>(255));
        this->field_A8->SetNoFlash(color32{ 255, 200, 114, 56 });
 
        this->field_78[7]->SetText(static_cast<global_text_enum>(265));
		this->field_78[8]->SetText(static_cast<global_text_enum>(263));
        this->field_78[0]->SetText(static_cast<global_text_enum>(275));
        this->field_78[1]->SetText(static_cast<global_text_enum>(91));
        this->field_78[2]->SetText(static_cast<global_text_enum>(92));
        this->field_78[3]->SetText(static_cast<global_text_enum>(260));
        this->field_78[4]->SetText(static_cast<global_text_enum>(258));
        this->field_78[5]->SetText(static_cast<global_text_enum>(259));
        this->field_78[6]->SetText(static_cast<global_text_enum>(273));
        
        auto v8 = this->field_78[0]->GetX();
        auto v9 = this->field_78[0]->GetY();
        this->field_60->GetPos(this->field_B8, this->field_C8);
        this->field_64->GetPos(this->field_D8, this->field_E8);

        for (auto i = 0u; i < 4u; ++i)
        {
            this->field_B8[i] = this->field_B8[i] - v8;
            this->field_C8[i] = this->field_C8[i] - v9;
            this->field_D8[i] = this->field_D8[i] - v8;
            this->field_E8[i] = this->field_E8[i] - v9;
        }

        this->field_F8 = false;
    }
    else
    {


        void(__fastcall * func)(void*, void*) = bit_cast<decltype(func)>(0x0063B2E0);

        func(this, nullptr);
        }
            
     }
        






void pause_menu_root::sub_61C610()
{
     CDECL_CALL(0x0061C610);
}

void sub_648F40() {
    CDECL_CALL(0x00648F40);
}

void pause_menu_root::OnDeactivate(FEMenu *a2)
{
    this->field_28 &= ~0x80;
}

void pause_menu_root::Update(Float a2) {

        THISCALL(0x006490A0, this, a2);
    
}

void  sub_582AD0()
{

CDECL_CALL(0x00582AD0);
  
}

void pause_menu_root::sub_62A840()
{
    CDECL_CALL(0x0062A840);
}

void pause_menu_root::OnActivate()
{

  
  THISCALL(0x006302D0, this);


}



// Helper to set menu state
void pause_menu_root::set_menu_state(int state) {
    // Original bytecode:
    //   *(_DWORD *)(*(_DWORD *)(*(_DWORD *)(this->field_AC + 4) + 4) + 72) = state;
    // i.e. field_AC->field_4[1] is the pause_menu_transition instance
    // installed by PauseMenuSystem's constructor, and byte offset 72
    // (0x48) inside that struct is field_48 -- the transition FSM's
    // target state.
    //
    // The previous body, `menu_data[18];`, was a no-op (read with no
    // assignment); set_menu_state(21) calls from handle_skip_confirmation
    // / handle_hero_toggle silently did nothing.
    auto *transition = bit_cast<pause_menu_transition *>(this->field_AC->field_4[1]);
    transition->field_48 = state;
}

// Helper -- the common trailing sequence for the simple "open a
// submenu" cases of OnCross (Status / Options / Controller /
// MessageLog / Credits). The original inlines this block six times
// with only `target_state` varying:
//
//   field_AC->field_4[1]->field_48 = target_state;   // transition target
//   field_AC->MakeActive(1);                         // slot 1 = transition FSM
//   if (!pause_menu_system->field_38)                // wasn't visible before pause
//       game_play_panel->field_67 = 0;               // ...so hide comic panel
//   FEManager::PlayInterfaceSound("fe_ps_accept", 1, 1);
void pause_menu_root::transition_to_submenu(int target_state) {
    this->set_menu_state(target_state);

    auto *menu_sys = bit_cast<PauseMenuSystem *>(this->field_AC);
    menu_sys->MakeActive(1);

    if (!menu_sys->field_38) {
        comic_panels::game_play_panel()->field_67 = 0;
    }

    static string_hash sfx_id_hash{"FE_PS_ACCEPT"};
    [[maybe_unused]] sound_instance_id id = sub_60B960(sfx_id_hash, 1.0f, 1.0f);
}



// Custom pause-menu Awards page entry point. See pause_menu_root.h
// for the full behavior comment. Bound to case 5 of OnCross.
//
// Step by step:
//   1. Bail safely if the pause menu hasn't fully constructed.
//   2. Re-Init the pause_menu_awards data block so all 17 award
//      labels + 17 descriptions + the "ACQUIRED" / "NOT ACQUIRED"
//      strings are fresh from the localized string table.
//   3. Force pause_menu_status into AWARDS sub-mode (field_EC = 0)
//      and reset the highlighted row (field_128 = 0).
//   4. Log the full 17-award state via sp_log -- useful for tracing
//      and proves the C++ data flow is working before any rendering.
//   5. Call update_selected to repopulate the on-screen row FETexts
//      now, so the first frame after the transition already shows
//      awards content rather than whatever the status menu was last
//      rendering.
//   6. Transition to slot 3 with the standard tail (MakeActive(1),
//      hide comic panel, FE_PS_ACCEPT sound) via transition_to_submenu.


void pause_menu_root::OnCross2(int a2)
{
    int type = this->field_B0;

    static string_hash sfx_id_hash{ "FE_PS_ACCEPT" };
    if (type == 7)

        int func = 0x00630460;
        void(__fastcall * func)(void*, void*) = bit_cast<decltype(func)>(0x00630460);

    func(this, nullptr);




}

void pause_menu_root::OnCross(int a2)
{
    int type = this->field_B0;
	  
static string_hash sfx_id_hash{"FE_PS_ACCEPT"};
	                if (type == 0)
                this->transition_to_submenu(3);  
				    if (type == 1)
		        this->transition_to_submenu(1); 			
				    if (type == 2)
		        this->transition_to_submenu(2);  
				    if (type == 3)
		        this->transition_to_submenu(4);  
				    if (type == 4)
		        this->transition_to_submenu(5);  
				    if (type == 5)
			    this->transition_to_submenu(6);  
			        if (type == 6)
                this->transition_to_submenu(7);  
					if (type == 7)
                OnCross2(7);

					if (type == 8)
			    this->field_AC->MakeDeactive(5);
			[[maybe_unused]] sound_instance_id id = sub_60B960(sfx_id_hash, 1.0, 1.0);
						 
						 


}






void pause_menu_root_patch() {

    {
        FUNC_ADDRESS(address, &pause_menu_root::_Load);
        set_vfunc(0x00893F48, address);
    }

    {
        FUNC_ADDRESS(address, &pause_menu_root::OnCross);
     //   set_vfunc(0x00893F84, address);
    }


    return;


	

}


pause_menu_root *& pause_menu_root_ptr = var<pause_menu_root*>(0x0965C21);
