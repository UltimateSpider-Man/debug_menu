#include "fe_mini_map_widget.h"


#include "func_wrapper.h"



#include "os_developer_options.h"
#include "panelfile.h"

#include "resource_manager.h"

#include "trace.h"
#include "utility.h"

#include "wds.h"



VALIDATE_SIZE(fe_mini_map_widget, 0x54);

fe_mini_map_widget::fe_mini_map_widget()
{
    if constexpr (0)
    {
    }
    else
    {
        CDECL_CALL(0x006343C0, this);
    }
}

fe_mini_map_widget::~fe_mini_map_widget()
{
    this->m_vtbl = 0x00895A00;

    operator delete(this->field_3A4);
    this->field_3A4 = nullptr;

    //for (auto& mat : this->field_4)
    {
     //   mat.m_texture = nullptr;
    }
}

void fe_mini_map_widget::Init()
{

        CDECL_CALL(0x006432F0, this);
    }


void fe_mini_map_widget::PrepareRegions()
{
    TRACE("fe_mini_map_widget::PrepareRegions");


        CDECL_CALL(0x00619690, this);
    
}

void fe_mini_map_widget::RenderMeshes(matrix4x4 *a2, float &a4)
{
    TRACE("fe_mini_map_widget::RenderMeshes");




        CDECL_CALL(0x00638C30, this, a2, &a4);
    
}

struct poi_sort_record_t {
    float field_0;
    fe_mini_map_dot *field_4;
};



void sort__poi_sort_record_t(poi_sort_record_t *a1, poi_sort_record_t *a2, int a3)
{


    CDECL_CALL(0x0064BCA0, a1, a2, a3);


}

void fe_mini_map_widget::UpdatePOIs(matrix4x4 *a2,
                                    Float a3,
                                    Float a4,
                                    Float a5,
                                    Float a6,
                                    Float a7)
{
    TRACE("fe_mini_map_widget::UpdatePOIs");

    if constexpr (0)
    {
    }
    else
    {
        CDECL_CALL(0x0063AEC0, this, a2, a3, a4, a5, a6, a7);
    }
}

void fe_mini_map_widget::Draw()
{

    CDECL_CALL(0x00641990);
    
}

void fe_mini_map_widget::Update(Float a2)
{
    CDECL_CALL(0x00641810, this, a2);
}

void fe_mini_map_widget_patch()
{
    {
       auto address = func_address(&fe_mini_map_widget::Init);
        set_vfunc(0x00895A04, address);
    }

    {
       auto address = func_address(&fe_mini_map_widget::RenderMeshes);
        REDIRECT(0x00641B06, address);
    }

    {
       auto address = func_address(&fe_mini_map_widget::UpdatePOIs);
        REDIRECT(0x00641B2E, address);
    }

    REDIRECT(0x0063B09C, sort__poi_sort_record_t);

    return;

    {
       auto address = func_address(&fe_mini_map_widget::Draw);
        set_vfunc(0x00895A08, address);
    }
}
