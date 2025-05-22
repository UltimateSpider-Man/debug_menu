#pragma once

#include "generic_interface.h"

struct conglomerate;
struct entity_base;
struct po;

#ifdef TARGET_XBOX
struct mash_info_struct;
#endif

struct skeleton_interface : generic_interface {
    conglomerate *field_4;
    bool field_8;
    po *abs_po;
    int po_count;

    skeleton_interface();

#ifdef TARGET_XBOX
    void unmash(mash_info_struct *a2, void *a3);
#endif

    void connect_bone_abs_po(int bone_idx, entity_base *new_bone);
};
