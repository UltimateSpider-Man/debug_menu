#include "panelfile.h"

#include "common.h"
#include "femanager.h"
#include "fixedstring.h"
#include "func_wrapper.h"
#include "ngl.h"

#include "fetext.h"

#include "pausemenusystem.h"
#include "resource_manager.h"
#include "resource_key.h"
#include "resource_directory.h"
#include "trace.h"
#include "utility.h"

VALIDATE_SIZE(PanelFile, 0x3c);
// VALIDATE_OFFSET(PanelFile, field_28, 0x20);

void PanelFile::Draw() {
    CDECL_CALL(0x00616A60, this);
}

PanelAnimFile* PanelFile::GetAnimationPointer(int a1) 
{
    return this->field_28.at(a1);
}


FEText *PanelFile::GetTextPointer(const char *a2) {
    TRACE("PanelFile::GetTextPointer", a2);


    CDECL_CALL(0x00616B40 ,a2);

    printf("Object %s not found in PANEL file\n", a2);
    assert(0);

    return nullptr;


}


