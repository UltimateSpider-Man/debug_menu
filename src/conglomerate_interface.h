#pragma once


struct conglomerate;

struct conglomerate_interface  {
    conglomerate *my_conglomerate;
    bool dynamic;

    conglomerate_interface(conglomerate *a2);

    //virtual
    const char * get_ifc_type_str() const;
};