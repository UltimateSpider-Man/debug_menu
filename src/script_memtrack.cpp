#include "script_memtrack.h"

#include "debugutil.h"
#include "debug_menu.h"
#include "entity_base.h"
#include "filespec.h"
#include "memory.h"
#include "trace.h"
#include "utility.h"

#include <cassert>
#include <map>
#include <string>

namespace script_memtrack {

static std::string current_class_name {""}; 

static int current_heap_usage {0};

static std::map<entity_base_vhandle, script_memtrack::entity_class_t *> handle_to_class_map {};

static std::map<mString, script_memtrack::entity_class_t *> name_to_class_map {};

static debug_menu *script_memtrack_debug_menu {nullptr};

void begin_entity_creation(const mString &a1)
{
    TRACE("script_memtrack::begin_entity_creation");

    assert(current_class_name == "");
    assert(current_heap_usage == 0);

    current_class_name = a1.c_str();
    current_heap_usage = 0;
}

entity_class_t *find_entity_class(entity_base_vhandle a1)
{
    auto it = handle_to_class_map.find(a1);
    auto end = handle_to_class_map.end();
    if ( it != end ) {
        return it->second;
    }

    return nullptr;
}



mString entity_class_render_callback(debug_menu_entry *entry)
{
    auto *entity_class = static_cast<entity_class_t *>(entry->get_data());
    auto v4 = entity_class->field_C;
    auto v2 = entity_class->field_10.size();
    mString a1 {0, "(%d, %dbytes) >", v2, v4};
    return a1;
}

entity_class_t *create_entity_class(const mString &a1)
{
    auto *mem = mem_alloc(sizeof(entity_class_t));
    auto *entity_class = new (mem) entity_class_t {};
    entity_class->field_0 = a1;
    entity_class->field_C = 0;
    script_memtrack::name_to_class_map.insert({entity_class->field_0, entity_class});
    
    if (script_memtrack_debug_menu != nullptr) {
        auto *v10 = create_menu_entry(entity_class->field_0);
        v10->set_submenu(nullptr);
    //    v10->set_game_flags_handler(populate_entity_class_menu);
        v10->set_render_cb(entity_class_render_callback);
        v10->set_data(entity_class);
        script_memtrack_debug_menu->add_entry(v10);
    }

    return entity_class;
}

entity_class_t *find_entity_class(const mString &a1)
{
    auto it = name_to_class_map.find(a1);
    auto end = name_to_class_map.end();
    if ( it != end ) {
        return it->second;
    }

    return nullptr;
}

void register_entity_instance(const mString &a1, entity_base_vhandle a2, int a3)
{
    filespec v10 {a1};
    auto v9 = v10.m_name;
    v9.to_upper();
    auto entity_class = find_entity_class(v9);
    if ( entity_class == nullptr ) {
        entity_class = create_entity_class(v9);
    }

    entity_class->field_10.emplace_back(a2, a3);
    entity_class->field_C += a3;
    handle_to_class_map.insert({a2, entity_class});
}

void end_entity_creation(entity_base_vhandle a1)
{
    TRACE("script_memtrack::end_entity_creation");

    assert(current_class_name != "\"");
    auto v1 = 0 - current_heap_usage;
    if ( a1 != entity_base_vhandle {0} ) {
        register_entity_instance(current_class_name.c_str(), a1, v1);
    }

    current_class_name = "";
    current_heap_usage = 0;
}

void frame_advance() {
    ;
}

void dump_info(debug_menu_entry *)
{
    TRACE("script_memtrack::dump_info");

    for ( auto it = name_to_class_map.begin(), end = name_to_class_map.end();
            it != end; ) {
        auto *cls = it->second;
        if ( !cls->field_10.empty() ) {
            auto v5 = cls->field_C;
            auto v4 = cls->field_10.size();
            auto *v1 = cls->field_0.c_str();
            debug_print_va("%s, %d instances, %d bytes", v1, v4, v5);

            for ( auto &p : cls->field_10 ) {
                auto v15 = p.first;
                auto *ent = v15.get_volatile_ptr();
                if ( ent != nullptr ) {
                    auto v6 = p.second;
                    auto id = ent->get_id();
                    auto *v3 = id.to_string();
                    debug_print_va("    %s, %d bytes", v3, v6);
                }
            }

            ++it;
        } else {
            ++it;
        }
    }
}

void create_debug_menu(debug_menu *arg0)
{
    script_memtrack_debug_menu = create_menu("Script Memtrack", debug_menu::sort_mode_t::undefined);
    arg0->add_entry(script_memtrack_debug_menu);
    
    auto *v7 = create_menu_entry(mString {"Dump Memtrack Info"});
    v7->set_game_flags_handler(dump_info);
    script_memtrack_debug_menu->add_entry(v7);
}

} // namespace script_memtrack

void script_memtrack_patch()
{
    SET_JUMP(0x0064EB20, script_memtrack::begin_entity_creation);

    SET_JUMP(0x0064EB30, script_memtrack::end_entity_creation);
}
