#include "panelanimfile.h"

#include "common.h"
#include "func_wrapper.h"
#include "utility.h"

VALIDATE_SIZE(PanelAnimFile, 0x30);

void PanelAnimFile::Update(Float a2) {
    THISCALL(0x00628A40, this, a2);
}

void PanelAnimFile::PostUnmashFixup(PanelFile *a1)
{
    THISCALL(0x00617170, this, a1);
}

void PanelAnimFile_patch()
{
    FUNC_ADDRESS(address, &PanelAnimFile::PostUnmashFixup);
    REDIRECT(0x006289DC, address);
}
