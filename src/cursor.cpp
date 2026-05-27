#include "cursor.h"

#include "common.h"
#include "func_wrapper.h"
#include "utility.h"
#include "variables.h"

#include "trace.h"

#include <cassert>


VALIDATE_SIZE(Cursor, 0x13C);

Var<Cursor *> g_cursor = {0x0096191C};

Cursor::Cursor(LPCWSTR lpWideCharStr, int a3, int a4) {
    if constexpr (1) {

        lpWideCharStr = L"data\\ump.dat";





        THISCALL(0x005A6670, this, lpWideCharStr, a3, a4);
    }
}

Cursor * __fastcall hookCtor(Cursor *self, void *, LPCWSTR lpWideCharStr, int a3, int a4) {
    new (self) Cursor{lpWideCharStr, a3, a4};
    return self;
}

Cursor::~Cursor() {
    THISCALL(0x005A6810, this);
}

void Cursor::sub_5A67D0(int a1, int a2, int a3, int a4)
{
    TRACE("Cursor::sub_5A67D0");

    if constexpr (0) {
        // ----------------------------------------------------------------
        // This is std::vector<hit_rect>::push_back where hit_rect is
        // 4 ints (x1, y1, x2, y2) — IDA's "sGuildOneResult" is a generic
        // 16-byte type the decompiler picked because it doesn't know what
        // the actual element type is.
        //
        // The MSVC 7.1 std::vector layout sits across:
        //   field_12C  : empty allocator (4-byte EBO placeholder)
        //   field_130  : begin pointer
        //   field_134  : end pointer (as int)
        //   field_138  : capacity-end pointer (as int)
        // sub_5A6790() already manages this trio manually with
        // operator delete (no destructor calls -- safe because hit_rect
        // is trivially destructible). We continue that hand-rolled
        // style here rather than declaring an std::vector field, which
        // would couple Cursor's layout to the host compiler's STL ABI.
        // ----------------------------------------------------------------
        struct hit_rect { int x1, y1, x2, y2; };
        const hit_rect rect{a1, a2, a3, a4};

        hit_rect *begin   = bit_cast<hit_rect *>(this->field_130);
        hit_rect *end     = bit_cast<hit_rect *>(this->field_134);
        hit_rect *cap_end = bit_cast<hit_rect *>(this->field_138);

        // Fast path: room in the existing allocation.
        if (end < cap_end) {
            *end = rect;
            this->field_134 = bit_cast<int>(end + 1);
            return;
        }

        // Grow path: double capacity (matches std::vector's growth
        // policy, and matches what sub_5A6790 expects to free).
        const std::ptrdiff_t old_size = end - begin;
        const std::ptrdiff_t new_cap  = (old_size == 0) ? 1 : old_size * 2;

        hit_rect *new_begin = static_cast<hit_rect *>(
            ::operator new(new_cap * sizeof(hit_rect)));

        // Trivial copy; hit_rect has no user-defined copy / move.
        for (std::ptrdiff_t i = 0; i < old_size; ++i) {
            new_begin[i] = begin[i];
        }
        new_begin[old_size] = rect;

        if (begin != nullptr) {
            ::operator delete(begin);
        }

        this->field_130 = new_begin;
        this->field_134 = bit_cast<int>(new_begin + old_size + 1);
        this->field_138 = bit_cast<int>(new_begin + new_cap);
    } else {
        THISCALL(0x005A67D0, this, a1, a2, a3, a4);
    }
}
void Cursor::sub_5A6790() {
    if (this->field_130 != nullptr) {
        operator delete(this->field_130);
    }

    this->field_130 = nullptr;
    this->field_134 = 0;
    this->field_138 = 0;
	THISCALL(0x005A6790, this);
}

void Cursor::sub_581C60() {
    if (!this->field_120) {
        this->field_114 = 0;
    }

    GetCursorPos(&this->m_cursorLoc);

    this->field_104.x = (uint64_t)(this->m_cursorLoc.x * 640.0 / this->m_screenWidth);
    this->field_104.y = (uint64_t)(this->m_cursorLoc.y * 480.0 / this->m_screenHeight);
    ScreenToClient(g_appHwnd(), &this->field_104);
}

void Cursor::sub_5B0D70() {
    if (!this->field_120) {
        this->field_114 = false;
    }
}

void cursor_patch() {
    REDIRECT(0x005AD24A, hookCtor);
	
}
