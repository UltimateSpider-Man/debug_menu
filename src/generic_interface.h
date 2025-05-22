#pragma once

#include <cstdint>

struct generic_mash_header;
struct generic_mash_data_ptrs;

struct generic_interface {
    std::intptr_t m_vtbl;

    generic_interface();

#ifndef TARGET_XBOX
    void un_mash(generic_mash_header *, void *, void *, generic_mash_data_ptrs *);
#endif
};
