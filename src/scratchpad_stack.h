#pragma once

#include "variable.h"
#include "func_wrapper.h"
#include "memory.h"
#include "stack_allocator.h"
#include "trace.h"
#include "utility.h"

#include <cassert>

namespace scratchpad_stack {

    // `inline` on a namespace-scope variable (C++17) gives every TU
    // that includes this header the *same* object, not its own copy.
    // Without inline, every translation unit got its own definition
    // and the linker emitted LNK2005 across every .obj that
    // transitively included scratchpad_stack.h.
    inline Var<stack_allocator> stk{ 0x0095C724 };
    inline Var<bool> tlScratchpadLocked{ 0x00970D60 };

    inline int get_total_allocated_bytes()
    {
        return stk().get_total_allocated_bytes();
    }

    inline void lock() {
        assert(!tlScratchpadLocked() && "Scratchpad is already locked!");
        tlScratchpadLocked() = true;
    }

    inline void* alloc(int n_bytes)
    {
        TRACE("alloc");

        assert(n_bytes != 0 && "Invalid scratchpad stack allocation (size 0)");

        if (get_total_allocated_bytes() == 0) {
            lock();
        }

        return stk().push(n_bytes);
    }

    inline void unlock() {
        assert(tlScratchpadLocked() && "Scratchpad is already unlocked!");
        tlScratchpadLocked() = false;
    }

    inline void pop(void* a1, int n_bytes) {
        stk().pop(a1, n_bytes);
        if (get_total_allocated_bytes() == 0) {
            unlock();
        }
    }

} // namespace scratchpad_stack
