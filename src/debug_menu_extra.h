#pragma once

struct debug_menu;
struct debug_menu_entry;

extern void create_gamefile_menu(debug_menu *parent);

extern void create_warp_menu(debug_menu *parent);

extern void create_debug_render_menu(debug_menu *parent);

extern void create_district_variants_menu(debug_menu* parent);

extern void create_camera_menu_items(debug_menu *parent);

extern void game_flags_handler(debug_menu_entry *a1);

extern void create_dvars_menu(debug_menu* arg0);

extern void create_replay_menu(debug_menu* parent);

extern void create_entity_variants_menu(debug_menu* parent);
