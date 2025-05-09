#include "character_viewer.h"

#include "camera.h"

#include "fe_menu_nav_bar.h"
#include "comic_panels.h"
#include "common.h"
//#include "cut_scene_player.h"
#include "entity.h"

#include "femenusystem.h"
#include "fetext.h"

#include "func_wrapper.h"
#include "game.h"
#include "marky_camera.h"
#include "mission_stack_manager.h"
#include "mstring.h"

#include "panelquad.h"
#include "resource_manager.h"
//#include "return_address.h"
#include "sound_instance_id.h"
#include "string_hash.h"
#include "us_lighting.h"
#include "utility.h"
#include "variables.h"
#include "wds.h"

#include <cassert>

VALIDATE_SIZE(character_viewer, 0x300);
VALIDATE_OFFSET(character_viewer, field_8C, 0x8C);
VALIDATE_OFFSET(character_viewer, field_2A4, 0x2A4);

VALIDATE_SIZE(character_viewer::internal, 0xC);

Var<const char *[1]> character_files { 0x00937948 };

character_viewer::character_viewer(FEMenuSystem *a2, int a3, int a4) : FEMenu(a2, 0, a3, a4, 8, 0) {
    for (auto &v : this->field_B0) {
        v.field_8 = 0;
    }

    this->field_2C = a2;

    this->field_34 = new menu_nav_bar{};
    this->field_2E0[0] = 0;
    this->field_2FC = 0;
    this->field_2F0 = 0.30000001;
    this->field_2F8 = 1;
    this->field_38 = 0;
    this->field_3C = 0;
    this->field_40 = 0;
    this->field_44 = 0;
    this->field_48 = 0;
    this->field_4C = 0;
    this->field_50 = 0;
    this->field_54 = 0;
    this->field_58 = 0;
    this->field_5C = 0;
    this->field_60 = 0;
    this->field_64 = 0;
    this->field_68 = 0;
    this->field_6C = 0;
    this->field_70 = 0;
    this->field_74 = 0;
    this->field_78 = 0;
    this->field_7C = 0;
    this->field_80 = 0;
    this->field_84 = 0;
    this->field_88 = nullptr;
}

void character_viewer::Init() {
    CDECL_CALL(0x00646000, this);
}

void character_viewer::set_info() {
    CDECL_CALL(0x006142B0, this);
}

void character_viewer::update_selected(int a2) {
    if constexpr (1) {
        auto *v3 = this->field_B0;

        for (auto i = 40; i; --i) {
            auto v7 = (float) (a2 * this->field_290);

            auto *vtbl = bit_cast<std::intptr_t(*)[1]>(v3->field_0->m_vtbl);

            auto func = (*vtbl)[48];
            assert(func == 0x00616710);
            //printf("func = 0x%08X", func);

            v3->field_0->sub_616710(Float{0}, Float{v7});
            ++v3;
        }

        auto v5 = this->field_298;

        int idx;
        if (a2 >= 0) {
            idx = (v5 + 39) % 40;
        } else {
            idx = (v5 + 38) % 40;
        }

        auto v8 = (float) -(a2 * this->field_294);

        {
            auto &_this = this->field_B0[idx].field_0;

            auto *vtbl = bit_cast<std::intptr_t(*)[1]>(_this->m_vtbl);

            auto func = (*vtbl)[48];
            assert(func == 0x00616710);
            //printf("func = 0x%08X", func);

            _this->sub_616710(Float{0}, Float{v8});
        }

        this->sub_6143B0(a2);
        this->set_info();
        this->sub_63FEC0(this->field_29C);
        this->sub_63FC90(this->field_298);
        this->field_29C = this->field_298;
    } else {
        CDECL_CALL(0x00640070, this, a2);
    }
}

void character_viewer::sub_63FEC0(int idx)
{

        CDECL_CALL(0x0063FEC0, this, idx);
    
}

void character_viewer::sub_6143B0(int a2) {
    if constexpr (1) {
        auto *v3 = &this->field_2A4;

        for (auto i = 7; i; --i) {
            auto v5 = v3->field_0;
            if (v3->field_0) {
                if (v5 == 1) {
                    v3->field_0 = (a2 >= 0 ? 0 : 2);
                } else if (v5 == 2) {
                    v3->field_0 = (a2 >= 0);
                }

            } else {
                v3->field_0 = (a2 >= 0) + 1;
            }

            nglTexture *tex;
            if (v3->field_0) {
                if (v3->field_0 == 1) {
                    tex = this->field_8C[4];
                } else {
                    tex = this->field_A4;
                }

            } else {
                tex = this->field_8C[2];
            }

            {
                auto &self = v3[-1].field_4;
                auto *vtbl = bit_cast<std::intptr_t(*)[1]>(self->m_vtbl);
                auto func = (*vtbl)[29];
                assert(func = 0x00616290);

                self->SetTexture(tex);

                //printf("func = 0x%08X", func);

                //func(self, v6);
            }

            ++v3;
        };
    } else {
        CDECL_CALL(0x006143B0, this, a2);
    }
}

void character_viewer::OnDown(int a2) {
    printf("character_viewer::OnDown(): %d", a2);

    CDECL_CALL(0x00640230, this, a2);
}

void character_viewer::OnUp([[maybe_unused]] int a2) {
    printf("character_viewer::OnUp(): %d", a2);

  //  printf("index = %d", field_2C->m_index);

    if constexpr (0) {
        if (--this->field_298 < 0) {
            this->field_298 = 39;
        }

        this->update_selected(1);

        static string_hash sfx_id_hash{"FE_PS_UDScroll"};

        [[maybe_unused]] sound_instance_id v4 = sub_60B960(sfx_id_hash, {1.0}, {1.0});

    } else {
        CDECL_CALL(0x00640180, this, a2);
    }
}

void character_viewer::OnActivate()
{
    TRACE("character_viewer::OnActivate");

  
        CDECL_CALL(0x00641EE0, this);
    
}

void character_viewer::sub_63FC90(int a2) {
    CDECL_CALL(0x0063FC90, this, a2);
}

void character_viewer::OnDeactivate(FEMenu *a2) {
    printf("character_viewer::OnDeactivate():");

    CDECL_CALL(0x00614260, this, a2);
}

void character_viewer_patch() {
    {
        auto address = func_address(&character_viewer::OnActivate);
        //set_vfunc(0x008949F4, address);
    }

    {
        auto address = func_address(&character_viewer::OnUp);
        set_vfunc(0x00894A04, address);
    }

    {
        auto address = func_address(&character_viewer::OnDown);
        set_vfunc(0x00894A08, address);
    }
}
