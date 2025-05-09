#pragma once

#include "singleton.h"

#include "variable.h"

#include <cstdint>
#include <windows.h>

struct window_manager : singleton {
    HWND field_4 = nullptr;

    //0x005B24E0
    virtual ~window_manager();

    static Var<window_manager *> instance;
};

extern void create_window_handle();
