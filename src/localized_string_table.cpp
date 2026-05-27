#include "localized_string_table.h"



#include <cassert>




const char *localized_string_table::lookup_localized_string(global_text_enum num)
{
 

       const char*(__fastcall * func)(void*, void*, global_text_enum) = bit_cast<decltype(func)>(0x0060BDC0);

    func(this, nullptr, num);
}

void localized_string_table_patch() {

}
