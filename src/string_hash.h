#pragma once

#include <cstdint>

#include "bit.h"

struct string_hash
{
    std::uint32_t source_hash_code;

    string_hash() = default;

    string_hash(const char* a1)
    {
        void(__fastcall * func)(void*, int idx, const char*) = bit_cast<decltype(func)>(0x00401960);
        func(this, 0, a1);
    }

    const char* to_string() const
    {
        const char* (__fastcall * func)(const void*) = (decltype(func))0x005374B0;
        return func(this);
    }

    void initialize(int a2, const char* a3, int a4)
    {
        typedef void(__fastcall* string_hash_initialize_ptr)(string_hash*, void* edx, int a2, const char* Str1, int a4);
        string_hash_initialize_ptr string_hash_initialize = (string_hash_initialize_ptr)0x00547A00;
        string_hash_initialize(this, nullptr, a2, a3, a4);
    }

    uint32_t get_hash() const {
        return source_hash_code;
    }






    string_hash(int a4) {
        this->initialize(0, nullptr, a4);
    }



};


inline constexpr bool is_alpha(unsigned char c) {
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
        return true;
    }

    return false;
}

inline constexpr unsigned char to_lower(unsigned char c) {
    constexpr auto delta = 'a' - 'A';

    if (c >= 'A' && c <= 'Z') {
        return (c + delta);
    }

    return c;
}

//0x00501BE0
extern constexpr inline std::uint32_t to_hash(const char* str) {
    std::uint32_t res = 0;

    for (int c = *str; c != '\0'; ++str, c = *str) {
        int ch_lower = [](auto c) -> int {
            if (is_alpha(c)) {
                return to_lower(c);
            }

            return c;
            }(c);

        res = ch_lower + 33 * res;
    }

    return res;
}

