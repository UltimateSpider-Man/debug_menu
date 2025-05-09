#include "memory.h"





inline void* mem_alloc(size_t Size)
{


       void(__fastcall * func)(void*, void*, size_t) = bit_cast<decltype(func)>(0x0043A100);

   func(nullptr, nullptr, Size);


   return 0;
   }



inline void mem_dealloc(void* a1, size_t Size)
{
    void(__fastcall * func)(void*, void*, void*, size_t) = bit_cast<decltype(func)>(0x00501E10);

    func(nullptr, nullptr, a1, Size);

}