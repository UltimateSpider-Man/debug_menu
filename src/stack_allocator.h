#pragma once

#include "log.h"
#include "memory.h"
#include "trace.h"

#include <cassert>

struct stack_allocator {
    int segment_size_bytes;
    int alignment;
    char* segment;
    char* current;

    // Local power-of-2 helper. The reference openusm9 code uses
    // bitmath::is_power_of_2 but the project doesn't have a bitmath
    // header, so the check is inlined here.
    static bool is_power_of_2_(int v) {
        return v > 0 && (v & (v - 1)) == 0;
    }

    inline bool allocate(int size, int alignment_arg, int external_alignment_arg)
    {
        TRACE("stack_allocator::allocate");

        assert(is_power_of_2_(alignment_arg));
        assert(is_power_of_2_(external_alignment_arg));
        assert(external_alignment_arg >= alignment_arg);

        this->alignment = alignment_arg;
        this->segment = static_cast<char *>(arch_memalign(external_alignment_arg, size));
        this->current = this->segment;
        this->segment_size_bytes = size;
        assert((long(current) & (alignment - 1)) == 0);

        return this->segment != nullptr;
    }

    inline void* push(int size_bytes)
    {
        TRACE("stack_allocator::push");

        assert((alignment >= 2) && is_power_of_2_(alignment));

        auto size = ~(this->alignment - 1) & (size_bytes + this->alignment - 1);
        assert((size >= size_bytes) && (size & (alignment - 1)) == 0);

        this->current += size;

        assert(current <= segment + segment_size_bytes);
        assert((long(current) & (alignment - 1)) == 0);

        return &this->current[-size];
    }

    inline int get_total_allocated_bytes() {
        assert(current >= segment);
        return this->current - this->segment;
    }

    inline void reset() {
        this->current = this->segment;
    }

    inline void pop(void* pointer, int size_bytes) {
        assert((alignment >= 2) && is_power_of_2_(alignment));
        assert(current <= segment + segment_size_bytes);
        assert(size_bytes >= 1);

        auto size = ~(this->alignment - 1) & (size_bytes + this->alignment - 1);
        assert((size >= size_bytes) && (size & (alignment - 1)) == 0);

        this->current -= size;

        assert("Stack allocator failed to pop a pointer -- mismatched alloc/pop pair" &&
               (current == (char *) pointer));
        assert((int) current >= 0 && "Stack allocator underflow.");
        assert((long(current) & (alignment - 1)) == 0);
    }

    inline void free() {
        mem_freealign(this->segment);
        this->segment = nullptr;
        this->current = nullptr;
    }

    inline void print() const {
        sp_log("segment_size_bytes = %d, alignment = %d, segment = %p, current = %p",
               this->segment_size_bytes,
               this->alignment,
               this->segment,
               this->current);
    }
};
