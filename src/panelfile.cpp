#include "panelfile.h"

#include "common.h"
#include "femanager.h"
#include "fixedstring.h"
#include "func_wrapper.h"
#include "ngl.h"


#include "mvector.h"

#include "vtbl.h"

#include "pausemenusystem.h"
#include "resource_manager.h"
#include "resource_key.h"
#include "resource_directory.h"
#include "trace.h"
#include "utility.h"
#include "log.h"

VALIDATE_SIZE(PanelFile, 0x3c);
VALIDATE_OFFSET(PanelFile, field_28, 0x28);

void PanelFile::Draw() {
    THISCALL(0x00616A60, this);
}



constexpr auto UnmashPanelFile_hook = 1;
PanelFile *PanelFile::UnmashPanelFile(const char *a1, panel_layer a2)
{

        return (PanelFile *) CDECL_CALL(0x00643000, a1, a2);
    

}




FEText* PanelFile::GetTextPointer(const char* a2) {
    TRACE("PanelFile::GetTextPointer", a2);
    for (uint16_t i = 0; i < this->ptext.size(); ++i) {
        auto* v5 = this->ptext.m_data[i];

        struct Vtbl {
            int field_0[44];
            mString* (__fastcall* GetName)(void*, int edx, const mString*);
        };
        
        Vtbl* vtbl = CAST(vtbl, v5->m_vtbl);

        mString v8;
        auto v6 = (strcmp(vtbl->GetName(v5, 0, &v8)->c_str(), a2) == 0);
        if (v6)
        {
            return this->ptext.m_data[i];
        }
    }

    sp_log("Object %s not found in PANEL file\n", a2);
    assert(0);

    return nullptr;
}

PanelQuad* PanelFile::GetPQ(const char* a2) {
    TRACE("PanelFile::GetPQ", a2);

    // 1. Guard against a null name — strcmp would crash on it.
    if (a2 == nullptr) {
        sp_log("PanelFile::GetPQ called with null name\n");
        return g_femanager().GetDefaultPQ();
    }

    const uint16_t count = this->pquads.size();
    sp_log("size = %d", count);

    // 2. Guard against empty / unallocated backing storage.
    if (count == 0 || this->pquads.m_data == nullptr) {
        sp_log("Object %s not found in PANEL file (empty)\n", a2);
        return g_femanager().GetDefaultPQ();
    }

    auto* const data = this->pquads.m_data;

    for (uint16_t i = 0; i < count; ++i) {
        PanelQuad* pquad = data[i];

        // 3. Guard against null slots — the table can have gaps.
        if (pquad == nullptr) continue;

        const char* name = pquad->field_3C.c_str();
        if (name != nullptr && strcmp(name, a2) == 0) {
            return pquad;
        }
    }

    sp_log("Object %s not found in PANEL file\n", a2);
    return g_femanager().GetDefaultPQ();
}

void PanelFile::Update(Float a2)
{

        THISCALL(0x0062E4D0, this, a2);
    
}

void PanelFile::PostUnmashFixup(panel_layer a3)
{

        THISCALL(0x00628960, this, a3);
    
}

void PanelFile_patch()
{
    if constexpr (UnmashPanelFile_hook)
    {
        SET_JUMP(0x000643000, PanelFile::UnmashPanelFile);
    }

    {
        FUNC_ADDRESS(address, &PanelFile::GetPQ);
        SET_JUMP(0x00639E40, address);
    }

    {
        FUNC_ADDRESS(address, &PanelFile::GetTextPointer);
        SET_JUMP(0x00616B40, address);
    }

    if constexpr(1)
    {
        FUNC_ADDRESS(address, &PanelFile::PostUnmashFixup);
        REDIRECT(0x006430AE, address);
    }


    return;


}
