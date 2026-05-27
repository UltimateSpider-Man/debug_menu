#pragma once

// custom_ints.h
//
// Storage and lookup for integer dev-options. Originally intended for
// *new* options not baked into the stock USM.exe int_names[76] table at
// 0x00936940, but now mirrors the full stock list as well.
//
// Pattern:
//
//     1. Declare the option here with a unique name and a default value.
//     2. Read it via `custom_ints::get("FOO")` (or directly through
//        `custom_ints::ptr("FOO")` if you need to mutate it).
//     3. `create_devopt_ints_menu` walks `entries()` and adds each one
//        as a normal int slider, so the in-game "Devopt Ints" menu
//        shows them alongside the stock options.

#include <cstddef>

namespace custom_ints {

    struct entry {
        const char* name;
        int* value;
    };

    // Storage. One static int per option, exposed by pointer so the
    // existing debug_menu_entry::set_p_ival path works unchanged.
    inline int difficulty = 0;
    inline int camera_style = 0;
    inline int camera_state = 0;
    inline int camera_fov = 0;
    inline int fog_red = 0;
    inline int fog_green = 0;
    inline int fog_blue = 0;
    inline int fog_distance = 0;
    inline int bit_depth = 0;
    inline int monkey_mode = 0;
    inline int random_seed = 0;
    inline int force_win = 0;
    inline int controller_type = 0;
    inline int frame_lock = 0;
    inline int frame_limit = 0;
    inline int swing_debug_trails = 0;
    inline int soak_smoke = 0;
    inline int far_clip_plane = 0;
    inline int poi_display_type = 0;
    inline int story_mission = 0;
    inline int exec_delay = 0;
    inline int run_length = 0;
    inline int pc_window_top = 0;
    inline int pc_window_left = 0;
    inline int pc_window_width = 0;
    inline int pc_window_height = 0;
    inline int allow_screenshot = 0;
    inline int amalga_refresh_interval = 0;
    inline int enable_long_malor_asserts = 0;
    inline int god_mode = 0;
    inline int pclistbuffer = 0;
    inline int pcscratchbuffer = 0;
    inline int pcscratchindexbuffer = 0;
    inline int pcscratchvertexbuffer = 0;
    inline int nal_heap_size = 0;
    inline int assert_box_margin = 0;
    inline int assert_text_margin = 0;
    inline int assert_font_pct_x = 0;
    inline int assert_font_pct_y = 0;
    inline int streamer_info_font_pct = 0;
    inline int debug_info_font_pct = 0;
    inline int pitch_factor = 0;
    inline int bank_factor = 0;
    inline int swing_interpolation_time = 0;
    inline int both_hands_interpolation_time = 0;
    inline int mem_dump_frame = 0;
    inline int hero_start_x = 0;
    inline int hero_start_y = 0;
    inline int hero_start_z = 0;
    inline int show_sound_info = 0;
    inline int show_voice_box_info = 0;
    inline int debug_camera_pitch_multiplier = 0;
    inline int debug_camera_yaw_multiplier = 0;
    inline int debug_camera_move_multiplier = 0;
    inline int debug_camera_strafe_multiplier = 0;
    inline int tam_scale_min_distance = 0;
    inline int tam_scale_max_distance = 0;
    inline int tam_scale_min_percent = 0;
    inline int thug_health_ui_scale_min_distance = 0;
    inline int thug_health_ui_scale_max_distance = 0;
    inline int thug_health_ui_scale_min_percent = 0;
    inline int targeting_reticle_scale_min_distance = 0;
    inline int targeting_reticle_scale_max_distance = 0;
    inline int targeting_reticle_scale_min_percent = 0;
    inline int hires_screenshot_x = 0;
    inline int hires_screenshot_y = 0;
    inline int time_of_day = 0;
    inline int mini_map_zoom = 0;
    inline int rtdt_replay_buffer_size = 0;
    inline int timer_widget_time_delta_percent = 0;
    inline int debug_particle_level = 0;
    inline int debug_particle_memory = 0;
    inline int max_aeps_entities = 0;
    inline int max_aeps_spawners = 0;
    inline int max_aeps_emitters = 0;
    inline int max_aeps_particles = 0;

    // Registry. Append new entries here; keep the sentinel at the end.
    inline entry* entries() {
        static entry table[] = {
            { "DIFFICULTY",                            &difficulty                            },
            { "CAMERA_STYLE",                          &camera_style                          },
            { "CAMERA_STATE",                          &camera_state                          },
            { "CAMERA_FOV",                            &camera_fov                            },
            { "FOG_RED",                               &fog_red                               },
            { "FOG_GREEN",                             &fog_green                             },
            { "FOG_BLUE",                              &fog_blue                              },
            { "FOG_DISTANCE",                          &fog_distance                          },
            { "BIT_DEPTH",                             &bit_depth                             },
            { "MONKEY_MODE",                           &monkey_mode                           },
            { "RANDOM_SEED",                           &random_seed                           },
            { "FORCE_WIN",                             &force_win                             },
            { "CONTROLLER_TYPE",                       &controller_type                       },
            { "FRAME_LOCK",                            &frame_lock                            },
            { "FRAME_LIMIT",                           &frame_limit                           },
            { "SWING_DEBUG_TRAILS",                    &swing_debug_trails                    },
            { "SOAK_SMOKE",                            &soak_smoke                            },
            { "FAR_CLIP_PLANE",                        &far_clip_plane                        },
            { "POI_DISPLAY_TYPE",                      &poi_display_type                      },
            { "STORY_MISSION",                         &story_mission                         },
            { "EXEC_DELAY",                            &exec_delay                            },
            { "RUN_LENGTH",                            &run_length                            },
            { "PC_WINDOW_TOP",                         &pc_window_top                         },
            { "PC_WINDOW_LEFT",                        &pc_window_left                        },
            { "PC_WINDOW_WIDTH",                       &pc_window_width                       },
            { "PC_WINDOW_HEIGHT",                      &pc_window_height                      },
            { "ALLOW_SCREENSHOT",                      &allow_screenshot                      },
            { "AMALGA_REFRESH_INTERVAL",               &amalga_refresh_interval               },
            { "ENABLE_LONG_MALOR_ASSERTS",             &enable_long_malor_asserts             },
            { "GOD_MODE",                              &god_mode                              },
            { "PCLISTBUFFER",                          &pclistbuffer                          },
            { "PCSCRATCHBUFFER",                       &pcscratchbuffer                       },
            { "PCSCRATCHINDEXBUFFER",                  &pcscratchindexbuffer                  },
            { "PCSCRATCHVERTEXBUFFER",                 &pcscratchvertexbuffer                 },
            { "NAL_HEAP_SIZE",                         &nal_heap_size                         },
            { "ASSERT_BOX_MARGIN",                     &assert_box_margin                     },
            { "ASSERT_TEXT_MARGIN",                    &assert_text_margin                    },
            { "ASSERT_FONT_PCT_X",                     &assert_font_pct_x                     },
            { "ASSERT_FONT_PCT_Y",                     &assert_font_pct_y                     },
            { "STREAMER_INFO_FONT_PCT",                &streamer_info_font_pct                },
            { "DEBUG_INFO_FONT_PCT",                   &debug_info_font_pct                   },
            { "PITCH_FACTOR",                          &pitch_factor                          },
            { "BANK_FACTOR",                           &bank_factor                           },
            { "SWING_INTERPOLATION_TIME",              &swing_interpolation_time              },
            { "BOTH_HANDS_INTERPOLATION_TIME",         &both_hands_interpolation_time         },
            { "MEM_DUMP_FRAME",                        &mem_dump_frame                        },
            { "HERO_START_X",                          &hero_start_x                          },
            { "HERO_START_Y",                          &hero_start_y                          },
            { "HERO_START_Z",                          &hero_start_z                          },
            { "SHOW_SOUND_INFO",                       &show_sound_info                       },
            { "SHOW_VOICE_BOX_INFO",                   &show_voice_box_info                   },
            { "DEBUG_CAMERA_PITCH_MULTIPLIER",         &debug_camera_pitch_multiplier         },
            { "DEBUG_CAMERA_YAW_MULTIPLIER",           &debug_camera_yaw_multiplier           },
            { "DEBUG_CAMERA_MOVE_MULTIPLIER",          &debug_camera_move_multiplier          },
            { "DEBUG_CAMERA_STRAFE_MULTIPLIER",        &debug_camera_strafe_multiplier        },
            { "TAM_SCALE_MIN_DISTANCE",                &tam_scale_min_distance                },
            { "TAM_SCALE_MAX_DISTANCE",                &tam_scale_max_distance                },
            { "TAM_SCALE_MIN_PERCENT",                 &tam_scale_min_percent                 },
            { "THUG_HEALTH_UI_SCALE_MIN_DISTANCE",     &thug_health_ui_scale_min_distance     },
            { "THUG_HEALTH_UI_SCALE_MAX_DISTANCE",     &thug_health_ui_scale_max_distance     },
            { "THUG_HEALTH_UI_SCALE_MIN_PERCENT",      &thug_health_ui_scale_min_percent      },
            { "TARGETING_RETICLE_SCALE_MIN_DISTANCE",  &targeting_reticle_scale_min_distance  },
            { "TARGETING_RETICLE_SCALE_MAX_DISTANCE",  &targeting_reticle_scale_max_distance  },
            { "TARGETING_RETICLE_SCALE_MIN_PERCENT",   &targeting_reticle_scale_min_percent   },
            { "HIRES_SCREENSHOT_X",                    &hires_screenshot_x                    },
            { "HIRES_SCREENSHOT_Y",                    &hires_screenshot_y                    },
            { "TIME_OF_DAY",                           &time_of_day                           },
            { "MINI_MAP_ZOOM",                         &mini_map_zoom                         },
            { "RTDT_REPLAY_BUFFER_SIZE",               &rtdt_replay_buffer_size               },
            { "TIMER_WIDGET_TIME_DELTA_PERCENT",       &timer_widget_time_delta_percent       },
            { "DEBUG_PARTICLE_LEVEL",                  &debug_particle_level                  },
            { "DEBUG_PARTICLE_MEMORY",                 &debug_particle_memory                 },
            { "MAX_AEPS_ENTITIES",                     &max_aeps_entities                     },
            { "MAX_AEPS_SPAWNERS",                     &max_aeps_spawners                     },
            { "MAX_AEPS_EMITTERS",                     &max_aeps_emitters                     },
            { "MAX_AEPS_PARTICLES",                    &max_aeps_particles                    },
            { nullptr, nullptr }, // sentinel
        };
        return table;
    }

    // Returns the int's current value, or `fallback` if not registered.
    inline int get(const char* name, int fallback = 0) {
        for (entry* e = entries(); e->name != nullptr; ++e) {
            if (std::char_traits<char>::compare(
                e->name, name,
                std::char_traits<char>::length(name) + 1) == 0)
            {
                return *e->value;
            }
        }
        return fallback;
    }

    // Returns the int's storage pointer (for direct read/write), or
    // nullptr if not registered.
    inline int* ptr(const char* name) {
        for (entry* e = entries(); e->name != nullptr; ++e) {
            if (std::char_traits<char>::compare(
                e->name, name,
                std::char_traits<char>::length(name) + 1) == 0)
            {
                return e->value;
            }
        }
        return nullptr;
    }

}
