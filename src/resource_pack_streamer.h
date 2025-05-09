#pragma once

#include "fixedstring.h"
#include "resource_key.h"
#include "resource_pack_location.h"
#include "resource_pack_slot.h"
#include "limited_timer.h"
#include "float.hpp"
#include "trace.h"
#include <vector>

#include <list>


#ifdef TEST_CASE
#include <list>
#endif

struct limited_timer;
struct resource_pack_token;

struct resource_pack_queue_entry {
    fixedstring<8> field_0;
    int field_20;

};

struct resource_pack_streamer {
    bool active;
    bool currently_streaming;

    std::vector<resource_pack_slot *> *pack_slots;
    resource_key field_8;
    resource_pack_slot *curr_slot;
    int m_slot_index;
    resource_pack_location curr_loc;
    int field_68;

    std::list<resource_pack_queue_entry> field_6C;

    uint8_t *field_78;
    float field_7C;
    int m_data_size;


   auto *get_pack_slots() {
        return this->pack_slots;
    }

   bool is_active() {
        return active;
    }

   bool is_idle() const
    {
    return 0;
}

   bool all_slots_idle() const
{
    if (this->pack_slots == nullptr) {
        return true;
    }

    auto &slots = (*this->pack_slots);
    for (auto &slot : slots)
    {

    }

    return true;
}
    void flush(void (*a2)(void), Float a3)
    {
        TRACE("resource_pack_streamer::flush");

        if constexpr (1) {
            assert(this->active);

            limited_timer timer { a3 };

            while (this->currently_streaming
                || !this->field_6C.empty()
                || !this->all_slots_idle()) {
                if (a2 != nullptr) {
                    a2();
                }

                

                timer.reset();
                this->frame_advance({ 0.0 }, &timer);

#ifdef TARGET_XBOX
                if (g_resource_directory != nullptr) {
                    auto* tlres = g_resource_directory->get_tlresource(to_hash("mini_map_frame"), TLRESOURCE_TYPE_MESH_FILE);
                    sp_log("0x%08X", tlres);
                    assert(tlres != nullptr);
                }
#endif
            }

        } else {
            CDECL_CALL(0x00551200, this, a2, a3);
        }
    }


   void flush(void (*a2)(void))
{
    this->flush(a2, Float { 0.02f });
}
    
void frame_advance(Float a2, limited_timer* a3)
{
    TRACE("resource_pack_streamer::frame_advance");

        CDECL_CALL(0x005510F0, this, a2, a3);
    
}





 


};

