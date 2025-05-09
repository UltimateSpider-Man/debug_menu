#pragma once

#include "conglomerate_interface.h"

#include "mashable_vector.h"
#include "string_hash.h"
#include "variant_info.h"
#include "variable.h"

struct conglomerate;
struct variant_info;
struct nglMeshFile;
struct nglMesh;
struct nglMorphSet;
struct variant_speaker_id_set;

struct variant_interface : conglomerate_interface {
    mashable_vector<variant_info> variants;
    mashable_vector<variant_speaker_id_set> field_14;
    variant_info* current_variant;
    nglMesh* current_mesh;
    nglMorphSet* field_24;
    nglMeshFile* field_28;
    char* field_2C[1];
    int field_30;
    int field_34;
    int field_38;
    int field_3C;
    int field_40;
    int field_44;
    int field_48;
    int field_4C;
    int field_50;
    int field_54;



    static variant_info* get_random_variant();


    void apply_variant(string_hash a2);

    void apply_variant(variant_info* info);

    void destroy_ifl_frames();



    variant_interface * get_interface() {
        return 0;
    }

    variant_interface* sub_679A4B(void* this_ptr)
    {
        // Assuming this_ptr is a pointer to a pointer to variant_interface
        return *static_cast<variant_interface**>(this_ptr);
    }


   static mashable_vector<variant_info>* get_variants(variant_interface*);


};


struct param_data {


    string_hash m_name;




    string_hash get_name() const {
        return m_name;
    }


};
