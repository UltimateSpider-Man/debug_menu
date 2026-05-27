#include "panelanimobject.h"
#include "float.hpp"
#include "vtbl.h"
#include "utility.h"


void PanelAnimObject::SetShown(bool a2) {
    void (__fastcall *func)(void *, void *, bool) = CAST(func, get_vfunc(m_vtbl, 0x64));

    func(this, nullptr, a2);

}