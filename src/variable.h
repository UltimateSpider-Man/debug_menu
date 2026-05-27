#pragma once

#include "bit.h"

#include <cstddef>
#include <type_traits>

template<typename T>
struct Var {
    using value_type = T;

    Var() = delete;

    Var(const Var&) = delete;
    Var& operator=(const Var&) = delete;

    Var(Var&&) = delete;
    Var& operator=(Var&&) = delete;

    Var(ptrdiff_t&& address) : pointer(bit_cast<T*>(address)) {
    }

    T* pointer;

    inline T& operator()() {
        return (*pointer);
    }

    inline T& operator*() {
        return (*pointer);
    }

    inline const T& operator()() const {
        return (*pointer);
    }

    // ---- Pointer-like access when T itself is a pointer ----
    // Many globals are declared as Var<Foo*>, and the legacy code uses
    // them as if they were Foo* directly: g_world_ptr->method(),
    // g_world_ptr != nullptr, *g_world_ptr, static_cast<Foo*>(g_world_ptr).
    // Provide the conversions that make those work.

    // operator-> returns the underlying pointer when T is a pointer.
    template<typename U = T>
    std::enable_if_t<std::is_pointer_v<U>, U> operator->() const {
        return *pointer;
    }

    // Implicit conversion to T when T is a pointer (covers != nullptr,
    // assignment to T variables, static_cast<T>(var), etc.).
    template<typename U = T>
    operator std::enable_if_t<std::is_pointer_v<U>, U>() const {
        return *pointer;
    }

    // Equality / inequality vs nullptr when T is a pointer.
    template<typename U = T>
    std::enable_if_t<std::is_pointer_v<U>, bool> operator==(std::nullptr_t) const {
        return *pointer == nullptr;
    }
    template<typename U = T>
    std::enable_if_t<std::is_pointer_v<U>, bool> operator!=(std::nullptr_t) const {
        return *pointer != nullptr;
    }
};
template <typename T>
inline auto& var(ptrdiff_t&& address)
{
    return *bit_cast<T*>(address);
}

