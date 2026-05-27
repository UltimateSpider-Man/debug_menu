#pragma once
#include "fetext.h"

#include "variable.h"

struct global_text_enum;

struct localized_string_table {
    struct internal {
        const char *field_0[479];
        const char *field_778[1];
    };

    internal *field_0;
    int field_4;
    int field_8;
    int scripttext_number;


    //0x0060BDC0
    const char *lookup_localized_string(global_text_enum num);


    static inline localized_string_table *&instance =
        var<localized_string_table *>(0x0096190C);

};

extern void localized_string_table_patch();


    
	

