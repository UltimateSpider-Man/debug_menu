#pragma once

#include "femenu.h"

#include "float.hpp"
#include "string_hash.h"
#include "pausemenusystem.h"
#include "femenusystem.h"

struct FEMenuSystem;
struct FEMenuSystem2;
struct PanelQuad;
struct FEText;




struct pause_menu_root : FEMenu {
    bool field_2C;
    char field_2D;
    int field_30;
    int field_34;
    int field_38;
    PanelQuad *field_3C[9];
    PanelQuad *field_60;
    PanelQuad *field_64;
    PanelQuad *field_68;
    PanelQuad *field_6C;
    PanelQuad *field_70;
    PanelQuad *field_74;
    FEText *field_78[9];
	FEText *field_9C;
    FEText *field_A0;
    FEText *field_A4;
    FEText *field_A8;
    FEMenuSystem *field_AC;
    int field_B0;
    int field_B4;
    float field_B8[4];
    float field_C8[4];
    float field_D8[4];
    float field_E8[4];
	bool field_F8;
    int field_F9;

    //0x0060E590
    pause_menu_root(FEMenuSystem *a2, int a3, int a4);

    //0x0063B2E0
    //virtual
    void _Load();

	
	void OnCross(int a2);


    void OnCross2(int a2);
	


    void sub_62A840();


    void OnActivate();




    void OnDeactivate(FEMenu* a2);


    void Update(Float a2);


    void sub_61C610();




    void set_menu_state(int state);


	


// Helper -- performs the common "transition into submenu" sequence
// shared by the simple top-level cases of OnCross (Status, Options,
// Controller, MessageLog, Credits). Sets the pause_menu_transition
// target state, calls field_AC->MakeActive(1) to kick the transition
// FSM, hides the comic panel if the menu was opened over gameplay,
// and plays the standard FE_PS_ACCEPT sound.
void transition_to_submenu(int target_state);



	
	};
	
	

extern void pause_menu_root_patch();

extern pause_menu_root *& pause_menu_root_ptr;



