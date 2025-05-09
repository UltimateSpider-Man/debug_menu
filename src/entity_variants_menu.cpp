
// entity_variants_menu.cpp

#include "debug_menu.h"
#include "entity.h"
#include "conglom.h"


#include <string>
#include <memory>
#include <list>
#include "osassert.h"


#include "variant_interface.h"



/*
void applyVariantHandler(debug_menu_entry* entry)
{
    if (!entry) return;

    auto* conglomeratePtr = entry->get_data();
    if (!conglomeratePtr) return;

    const auto& entryName = entry->get_name();
    string_hash hash(entryName.c_str());


}

void populate_entity_variants_menu(debug_menu_entry* entry)
{
    if (!entry)
        return;

    auto submenu = create_menu("Entity Variants");
    entry->set_submenu(submenu);

    std::list<std::shared_ptr<entity>> entities;
    entity::find_entities(256);

    for (const auto& entity : entities)
    {
        debug_menu_entry menuEntry;

        menuEntry.set_game_flags_handler(applyVariantHandler);
        menuEntry.set_data(entity.get());
        menuEntry.set_submenu(nullptr);

        submenu->add_entry(&menuEntry);
    }

    // Ownership transferred to entry
    entry->set_submenu(submenu);
}


*/

