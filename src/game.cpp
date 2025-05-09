#include "game.h"

#include "common.h"
#include "debug_menu.h"
#include "message_board.h"
#include "link_system.h"
#include "smoke_test.h"
#include "geometry_manager.h"
#include "debug_struct.h"
#include "keyboard.h"
#include "game_data_meat.h"
#include "game_button.h"
#include "entity.h"
#include "entity_base.h"
#include "po.h"
#include "femenusystem.h"
#include "resource_manager.h"
#include "resource_key.h"
#include "resource_partition.h"
#include "mission_stack_manager.h"
#include "us_lighting.h"
#include "filespec.h"



#include "spider_monkey.h"
#include "app.h"

#include "base_ai_core.h"

#include "color32.h"

#include "chuck_callbacks.h"


#include "physical_interface.h"
#include "region.h"

#include "resource_pack_streamer.h"
#include "script_manager.h"

#include "slc_manager.h"


#include "utility.h"
#include "variables.h"
#include "vector2di.h"
#include "vtbl.h"
#include "wds.h"
#include "levelmenu.h"

#include "FrontEndMenuSystem.h"
#include "femanager.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <numeric>

#include <algorithm>

#include "os_developer_options.h"

#include "fetext.h"


    // ----- constants that used to come from stack temps -----
    constexpr int  kFirstDistrictX = 270;
    constexpr int  kFirstDistrictY = 410;
    constexpr int  kDistrictColStep = 90;
    constexpr int  kDistrictRowStep = 18;
    constexpr int  kDistrictColsPerRow = 4;

    constexpr int  kFirstStripX = 240;
    constexpr int  kStripBaseY = 446;
    constexpr int  kStripColStep = 90;

    constexpr float kBaseScale = 0.80f;



static int main_flow[] = { 5, 6, 14 };

static game_process main_process { "main", main_flow, 3 };

static int start_flow[] = { 1, 2, 3, 4, 14 };
static game_process start_process { "start", start_flow, 5 };

static int pause_flow[] = { 7, 14 };
static game_process pause_process { "pause", pause_flow, 2 };

static int& g_debug_mem_dump_frame = var<int>(0x00921DCC);

static auto& off_921DAC = var<char* [1]>(0x00921DAC);


inline Var<game*> g_game_ptr { 0x009682E0 };

static Var<game_process> lores_game_process { 0x00922074 };

void game::push_process(game_process& process)
{
    void(__fastcall * sub_570FD0)(void*, void*, void*) = CAST(sub_570FD0, 0x00570FD0);

    sub_570FD0(&this->process_stack, nullptr, &process);

    auto& last_proc = this->process_stack.back();
    last_proc.index = 0;
    last_proc.field_10 = 0;
}




    void game::enable_marky_cam(bool a2, bool a3, Float a4, Float a5)
    {
        void(__fastcall * func)(void*, void*, bool, bool, Float, Float) = bit_cast<decltype(func)>(0x005241E0);

        func(this, nullptr, a2, a3, a4, a5);

    }

void game::push_lores()
    {
        this->push_process(lores_game_process());
    }



void game::unload_hero_packfile()
{

    void(__fastcall * func)(void*, void*) = bit_cast<decltype(func)>(0x00558320);

    func(this, nullptr);
}

game::game()
{
    TRACE("game::game");

    if constexpr (1) {
        static auto& setup_inputs_p = var<void (*)(game*)>(0x0095C8FC);
        setup_inputs_p = game__setup_inputs;
        ;
    }

    if constexpr (1) {
        this->field_2B4 = false;
        this->field_1 = false;
        this->field_BC = false;
        float fov = os_developer_options::instance()->get_int(mString { "CAMERA_FOV" }) * 0.017453292;

        geometry_manager::set_field_of_view(fov);

        g_debug().field_0 |= 0x80u;
        if (os_developer_options::instance()->get_flag(mString { "OUTPUT_WARNING_DISABLE" })) {
            g_debug().field_0 &= 0x7Fu;
        }

        g_debug().field_1 |= 4u;
        if (os_developer_options::instance()->get_flag(mString { "OUTPUT_ASSERT_DISABLE" })) {
            g_debug().field_1 &= 0xFBu;
        }

        if (os_developer_options::instance()->get_flag(mString { "SMOKE_TEST" })) {
            float a3 = os_developer_options::instance()->get_int(mString { "SOAK_SMOKE" });
        

            static Var<char* [2]> smoke_test_levels { 0x00921DB0 };

            if (os_developer_options::instance()->get_flag(mString { "SMOKE_TEST_LEVEL" })) {
                g_smoke_test() = new smoke_test(bit_cast<const char**>(&g_scene_name()), a3);
            } else {
                g_smoke_test() = new smoke_test(bit_cast<const char**>(&smoke_test_levels()[0]),
                    a3);

                strcpy(g_scene_name(), smoke_test_levels()[0]);
            }
        } else if (!os_developer_options::instance()->get_flag(mString { "SMOKE_TEST_LEVEL" })) {
            auto v23 = os_developer_options::instance()->get_string(os_developer_options::strings_t::SCENE_NAME);

            if (v23) {
                strcpy(g_scene_name(), v23->c_str());
            }
        }

        this->the_world = new world_dynamics_system();
        g_world_ptr() = this->the_world;

        this->mb = nullptr;

        this->gamefile = new game_settings();

        this->field_278 = 0.0;
        this->field_27C = 0;
        this->field_280 = 0;
        this->field_284 = 1.0e10;
        this->field_288 = 0.0;
        this->field_28C = 0.1;
        this->field_270 = 0;
        this->field_274 = 0;
        this->field_15E = 0;
        this->field_15F = 0;
        this->field_160 = 0;
        this->field_164 = false;
        bool bExit = false;
        this->field_165 = false;
        this->field_166 = false;
        this->field_163 = false;
        this->field_161 = false;
        this->field_162 = false;
        this->flag.level_is_loaded = 0;
        this->field_170 = 0;
        this->field_167 = 0;
        this->flag.single_step = false;
        this->flag.physics_enabled = false;
        this->flag.field_3 = false;
        this->flag.game_paused = false;
        this->field_16D = 0;
        this->field_16E = 0;
        this->m_hero_start_enabled = true;
        this->field_171 = 0;
        this->m_user_camera_enabled = false;
        this->field_2 = 0;
        this->field_148 = 0;
        this->field_149 = 0;
        this->field_13C = 9999.9502f;
        this->field_140 = 3.0;
        this->field_144 = -1;
        this->field_14A = 64;
        this->field_158 = 0;
        this->field_159 = 0;

        this->field_14C = 9999.9004f;
        this->field_150 = -3.0;
        this->field_154 = -1;
        this->field_15A = 64;
        this->field_58 = 0;
        this->current_game_camera = nullptr;
        this->field_5C = nullptr;
        this->field_64 = nullptr;
        this->field_7C = nullptr;

        this->setup_input_registrations();
        this->setup_inputs();

        static Var<bool> g_console_command { 0x0095C068 };
        g_console_command() = false;

        {
            this->push_process(main_process);
            this->push_process(start_process);
        }

        this->field_2B8 = 0;
        this->field_2BC = 0;

        this->field_15C = false;
        this->field_15D = false;

        this->field_2B5 = false;

        this->field_80 = game_button {
        };


        g_debug_mem_dump_frame = os_developer_options::instance()->get_int(mString { "MEM_DUMP_FRAME" });

    } else {
        CDECL_CALL(0x00557610, this);
    }
}

void game__setup_input_registrations(game* a1)
{
    if constexpr (0) {
    } else {
        CDECL_CALL(0x006063C0, a1);
    }
}

void game::setup_input_registrations()
{
    setup_input_registrations_p(this);
}

void game::setup_inputs()
{
    setup_inputs_p(this);
}


void game::render_interface()
{
    CDECL_CALL(0x0524290, this);
}

    mString game::get_hero_info() const
{
        CDECL_CALL(0x05242D0, this);

        return 0;
    }

        mString game::get_camera_info() const
    {
            CDECL_CALL(0x05242D0, this);

            return 0;
    }

        void game::load_hero_packfile(const char* str, bool a3)
    {
        CDECL_CALL(0x05242D0, str, a3);
    }
    void game::unload_current_level()
    {
        void(__fastcall * func)(void*, void*) = (decltype(func))0x00557E10;
        func(this, nullptr);
    }

        // 0x00559D10
    game::~game()
    {
        CDECL_CALL(0x0559D10, this);
    }

        // 0x0054E1B0
    void game::render_world()
    {
        CDECL_CALL(0x054E1B0, this);
    }

    // 0x00558100
    void game::advance_state_legal(Float a2)
    {
        CDECL_CALL(0x0558100, a2);
    }

    // 0x00545B00
    void game::pop_process()
    {
        CDECL_CALL(0x0545B00, this);
    }

    void game::advance_state_paused(Float a1)
    {
        void(__fastcall * func)(void*, void*, Float) = bit_cast<decltype(func)>(0x00558220);
        func(this, nullptr, a1);
    }

    void game::advance_state_running(Float a2)
    {
        CDECL_CALL(0x0559FA0, a2);
    }

        void game::message_board_clear()
    {

    }

            // 0x00514A50
    camera* game::get_current_view_camera(int a2)
    {
        CDECL_CALL(0x0514A50, a2);

        return 0;
    }

    void game::handle_cameras(input_mgr* a2, const Float& a3)
    {
        CDECL_CALL(0x0552F50, a2, a3);
    }

void map_spiderman_controls()
{
    auto* v1 = input_mgr::instance();
    auto id = v1->field_58;
    v1->map_control(96, id, 10);
    v1->map_control(98, id, 11);
    v1->map_control(97, id, 13);
    v1->map_control(99, id, 14);
    v1->map_control(100, id, 17);
    v1->map_control(101, id, 19);
    v1->map_control(102, id, 9);
    v1->map_control(103, id, 16);
    v1->map_control(104, id, 18);
    v1->map_control(105, id, 6);
    v1->map_control(106, id, 5);
    v1->map_control(107, id, 4);
    v1->map_control(108, id, 8);
    v1->map_control(109, id, 7);
    v1->map_control(110, id, 1);
    v1->map_control(111, id, 0);
    v1->map_control(112, id, 3);
    v1->map_control(113, id, 2);
    v1->map_control(112, id, 1);
    v1->map_control(113, id, 0);
    v1->map_control(114, id, 20);
    v1->map_control(115, id, 21);
}

void map_pc_2_playstation_inputs()
{
    auto* v0 = input_mgr::instance();
    auto id = v0->field_58;
    v0->map_control(80, id, 10);
    v0->map_control(81, id, 11);
    v0->map_control(82, id, 14);
    v0->map_control(83, id, 13);
    v0->map_control(84, id, 16);
    v0->map_control(85, id, 18);
    v0->map_control(86, id, 6);
    v0->map_control(87, id, 17);
    v0->map_control(88, id, 19);
    v0->map_control(89, id, 9);
    v0->map_control(90, id, 3);
    v0->map_control(91, id, 2);
    v0->map_control(90, id, 1);
    v0->map_control(91, id, 0);
    v0->map_control(94, id, 8);
    v0->map_control(95, id, 7);
    v0->map_control(92, id, 20);
    v0->map_control(93, id, 21);
}





void game::frame_advance(Float a2)
{
    TRACE("game::frame_advance");

    printf("%f", float(a2));

    if constexpr (0) {
    } else {
        CDECL_CALL(0x0055D780, this, a2);
    }
}



mString sub_55DD80(const vector3d &a2)
{
    mString a1 {0, "<%.3f,%.3f,%.3f>", a2.x, a2.y, a2.z};
    return a1;
}

mString game::get_camera_info()
{
    auto *v2 = this->field_5C;

    mString v22;
    if ( v2->get_primary_region() != nullptr )
    {
        auto *v4 = v2->get_primary_region();
        auto &v5 = v4->get_name();
        auto *v6 = v5.to_string();
        v22 = mString{v6};
    }
    else
    {
        v22 = mString{"none"};
    }

    mString v33 = v22;


    auto &v18 = *v2->get_abs_position();
    auto *v8 = g_world_ptr()->the_terrain;
    auto *v32 = v8->find_region(v18, nullptr);
    if ( v32 != nullptr )
    {
        auto &v9 = v32->get_name();
        auto *v10 = v9.to_string();
        v33 = {v10};
    }

    auto &v12 = *v2->get_abs_position();
    auto v31 = sub_55DD80(v12);

    auto &v14 = *v2->get_abs_po();
    auto &v15 = v14.get_z_facing();

    auto v30 = sub_55DD80(v15);
    auto *v20 = v30.c_str();
    auto *v19 = v31.c_str();
    auto *v16 = v33.c_str();

    mString v29 {0, "CAMERA @ %s %s, f = %s", v16, v19, v20};

    auto v24 = " " + v33;

    v29 += v24;
    
    return v29;
}

mString game::get_analyzer_info()
{
    auto v16 = string_hash("SCENE_ANALYZER_CAM");
    auto *v3 = entity_handle_manager::find_entity(v16, entity_flavor_t::CAMERA, false);

    auto &v14 = *v3->get_abs_position();
    auto *v4 = g_world_ptr()->the_terrain;
    auto *v26 = v4->find_region(v14, nullptr);

    mString v25 {""};
    if ( v26 != nullptr )
    {
        auto &v5 = v26->get_name();
        auto *v6 = v5.to_string();
        v25 = v6;
    }

    auto &v8 = *v3->get_abs_position();
    auto v24 = sub_55DD80(v8);

    auto &v10 = *v3->get_abs_po();
    auto &v11 = v10.get_z_facing();
    auto v23 = sub_55DD80(v11);

    auto *v15 = v23.c_str();
    auto *v12 = v24.c_str();

    mString a1 {0, "ANALYZER @ %s, f = %s", v12, v15};
    auto v17 = " " + v25;
    a1 += v17;
    return a1;
}


mString game::get_hero_info()
{
    auto *hero_ptr = g_world_ptr()->get_hero_ptr(0);
    if ( hero_ptr == nullptr )
    {
        mString result {"(hero does not exist!)"};
        return result;
    }

    region *v29 = nullptr;
    if ( hero_ptr != nullptr )
    {
        v29 = hero_ptr->get_primary_region();
    }

    mString v28 {"none"};
    if ( v29 != nullptr )
    {
        auto &v4 = v29->get_name();
        auto *v5 = v4.to_string();
        v28 = {v5};
    }

    auto v27 = [&]() -> mString
    {
        if ( hero_ptr != nullptr )
        {
            auto &v6 = *hero_ptr->get_abs_position();
            return sub_55DD80(v6);
        }

        return mString{"N/A"};
    }();

    vector3d v15;
    if ( hero_ptr != nullptr )
    {
        auto *v7 = bit_cast<actor *>(hero_ptr)->physical_ifc();
        v15 = v7->get_velocity();
    }
    else
    {
        v15 = ZEROVEC;
    }

    mString v25{0, "HERO @ %s ", v28.c_str()};

    auto *v9 = v27.c_str();
    v25.append(v9, -1);
    v25.append(", v = ", -1);

    auto v14 = sub_55DD80(v15);
    v25.append(v14.c_str(), -1);

    return v25;
}

void game::show_debug_info()
{
    auto DEBUG_INFO_FONT_PCT = os_developer_options::instance()->get_int(mString{"DEBUG_INFO_FONT_PCT"});
    auto v15 = (float)DEBUG_INFO_FONT_PCT / 100.0;
    auto a1 = this->get_hero_info();

    vector2di v13 {50, 40};
    auto *v4 = a1.c_str();
    nglListAddString(nglSysFont(), (float)v13.x, (float)v13.y, 1.0, v15, v15, v4);

    auto v12 = this->get_camera_info();
    v13.y += 20;
    auto *v5 = v12.c_str();
    nglListAddString(nglSysFont(), (float)v13.x, (float)v13.y, 1.0, v15, v15, v5);

    auto v11 = this->get_analyzer_info();
    v13.y += 20;
    auto *v6 = v11.c_str();
    nglListAddString(nglSysFont(), (float)v13.x, (float)v13.y, 1.0, v15, v15, v6);
}




void game::initStreamerInfo()
{
    clear();
    if (!os_developer_options::instance()->get_int(mString{ "SHOW_STREAMER_INFO" })) return;

    constexpr resource_partition_enum districtPartitionEnum = static_cast<resource_partition_enum>(6);
    constexpr resource_partition_enum stripPartitionEnum = static_cast<resource_partition_enum>(5);

    auto* districtPartition = resource_manager::get_partition_pointer(districtPartitionEnum);
    auto* stripPartition = resource_manager::get_partition_pointer(stripPartitionEnum);
    if (!districtPartition || !stripPartition) return;

    auto* districtStreamer = districtPartition->get_streamer();
    auto* stripStreamer = stripPartition->get_streamer();
    if (!districtStreamer || !stripStreamer) return;

    std::vector<resource_pack_slot*> districtSlots = *districtStreamer->get_pack_slots();
    std::vector<resource_pack_slot*> stripSlots = *stripStreamer->get_pack_slots();

    const float uiScale = static_cast<float>(dword_14F2C4C) / 100.0f * kBaseScale;

    int x = kFirstDistrictX;
    int y = kFirstDistrictY;
    for (size_t i = 0; i < districtSlots.size(); ++i)
    {
        auto text = std::to_string(i); // Replace with actual text generation logic
        vector2di pos{ (int)x, (int)y };
        nglListAddString(nglSysFont(), (float)pos.x, (float)pos.y, 1.0, uiScale, uiScale, text.c_str());

        if ((i + 1) % kDistrictColsPerRow == 0)
        {
            x = kFirstDistrictX;
            y += kDistrictRowStep;
        }
        else
        {
            x += kDistrictColStep;
        }
    }

    x = kFirstStripX;
    y = kStripBaseY;
    for (size_t i = 0; i < stripSlots.size(); ++i)
    {
        auto text = std::to_string(i); // Replace with actual text generation logic
        vector2di pos{ (int)x, (int)y };
        nglListAddString(nglSysFont(), (float)pos.x, (float)pos.y, 1.0, uiScale, uiScale, text.c_str());
        x += kStripColStep;
    }

    initialised_ = true;
}




void game::frame_advance_level(Float time_inc)
{
    printf("game::frame_advance_level\n");

    {
        static int dword_14EEAC4 {-1};
        mem_check_leaks_since_checkpoint(dword_14EEAC4, 1);
        dword_14EEAC4 = mem_set_checkpoint();
        mem_print_stats("\nMemory log\n");
    }

    void (__fastcall *func)(void *, void *edx, Float) = (decltype(func)) 0x0055D650;
    func(this, nullptr, time_inc);
}

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

    bool isRunning = false;
double speedMultiplier = 1.0;

    void gameLoop()
{
    int gameTime = 0;
    while (isRunning) {
        std::cout << "Game time: " << gameTime << " seconds" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000 / speedMultiplier)));
        gameTime++;
    }
}

    void startGame()
    {
        isRunning = true;
        gameLoop();
    }

    void stopGame()
    {
        isRunning = false;
    }

    void game::setSpeedMultiplier(double multiplier)
    {
        if (multiplier > 0) {
            speedMultiplier = multiplier;
            std::cout << "Game speed set to: " << speedMultiplier << "x" << std::endl;
        } else {
            std::cout << "Multiplier must be greater than 0." << std::endl;
        }
    }







void game::handle_frame_locking(float* a1)
{
    auto frame_lock = os_developer_options::instance()->get_int(mString { "FRAME_LOCK" });
    if (frame_lock > 0) {
        *a1 = 1.0 / frame_lock;
    }
}

void game::set_camera(int camera_state)
{
    printf("camera_state = %d\n", camera_state);

    void (__fastcall *func)(void *, void *edx, int) = CAST(func, 0x0054F8C0);
    func(this, nullptr, camera_state);

    {
        if ( 1 == camera_state )
        {
            if ( g_debug_camera_entry != nullptr ) {
                g_debug_camera_entry->set_ival(true, true);
            }
        }
        else if ( 2 != camera_state )
        { 
            if ( g_debug_camera_entry != nullptr ) {
                g_debug_camera_entry->set_ival(false, true);
            }
        }
    }
}

void game::clear_screen()
{

    void(__fastcall * func)(void*, void*) = (decltype(func))0x00515140;
    func(this, nullptr);
    }

static int& g_mem_checkpoint_level = var<int>(0x00921DC4);


void register_chuck_callbacks()
{

    CDECL_CALL(0x006607E0);
}

void glow_init()
{
    ;
}


void game__setup_inputs(game* a1)
{
    TRACE("game::setup_inputs");


        CDECL_CALL(0x00605950, a1);
    
}



void game::load_this_level()
{



        CDECL_CALL(0x0055C6E0, this);
}




void game::freeze_hero(bool a2)
{


                void(__fastcall * func)(void*, void*, bool) = bit_cast<decltype(func)>(0x00514F00);
        func(this, nullptr, a2);
    }


void game::pause()
{
    CDECL_CALL(0x054FBE0, this);
}

void game::unpause()
{
    CDECL_CALL(0x053A880, this);
}


bool game::is_physics_enabled() const
{
    return this->flag.physics_enabled;
}

bool game::is_paused() const
{
    return this->flag.game_paused;
}


bool game::level_is_loaded() const
{
    return this->flag.level_is_loaded;
}


inline double sub_A26B70()
{
    if (g_game_ptr()->field_290 == 0.0) {
        return 0.0;
    } else {
        return 1.0 / g_game_ptr()->field_290;
    }
}

inline double sub_690BF1()
{
    return sub_A26B70();
}

inline void nglSetQuadBlend(nglQuad* a1, nglBlendModeType a2, unsigned a3)
{
    a1->field_58 = a2;
    a1->field_5C = a3;
}

game_state game::get_cur_state() const
{
    assert(process_stack.size() != 0);

    auto& last = this->process_stack.back();

    return static_cast<game_state>(last.field_4[last.index]);
}

void game::render_bar_of_shame()
{
    if (os_developer_options::instance()->get_flag(mString { "SHOW_BAR_OF_SHAME" })) {
        auto v39 = g_game_ptr()->field_284 * 1000.0;

        int dword_1568744 = 0;
        if (v39 > dword_1568744) {
            dword_1568744 = v39;
        }

        color v38 { 0 };
        if (sub_690BF1() >= 25.0) {
            if (sub_690BF1() >= 30.0) {
                color v5 { 0.0, 1.0, 0.0, 1.0 };
                v38 = v5;
            } else {
                color v4 { 1.0, 1.0, 0.0, 1.0 };
                v38 = v4;
            }
        } else {
            color v3 { 0.0, 1.0, 0.0, 1.0 };
            v38 = v3;
        }

        nglQuad v1;
        nglInitQuad(&v1);
        nglSetQuadColor(&v1, (int)(v38.b * 255.0) | ((int)(v38.g * 255.0) << 8) | ((int)(v38.r * 255.0) << 16) | ((int)(v38.a * 255.0) << 24));
        nglSetQuadBlend(&v1, (nglBlendModeType)2, 0);
        nglSetQuadRect(&v1, 32.0, 16.0, (v39 * 3.0) + 32.0, 16.0 + 16.0);
        nglSetQuadZ(&v1, 1.0);
        nglListAddQuad(&v1);
    }
}
static Var<float> waiting_time { 0x00960B60 };

void game::advance_state_wait_link(Float a2)
{
    if constexpr (1) {
        if (!link_system::use_link_system() || !os_developer_options::instance()->get_flag(mString { "WAIT_FOR_LINK" })) {
            ++this->process_stack.back().index;
        }

        auto v2 = waiting_time() > 10.0f;

        if (v2 || this->field_171) {
            ++this->process_stack.back().index;
        }

        waiting_time() += a2;

    } else {
        CDECL_CALL(0x00536410, this, a2);
    }
}

void game::level_load_stuff::reset_level_load_data()
{
    this->descriptor = nullptr;
    this->field_30 = 0;
    this->load_completed = false;
    this->field_3A = false;
    this->load_widgets_created = false;
}


                                     
 void game::load_complete()
{
     g_game_ptr()->level.load_completed = true;

     void(__fastcall * func)(void*, void*) = (decltype(func))0x00514AA0;
     func(this, nullptr);
 }

void game::render_intros()
{

    void(__fastcall * func)(void*, void*) = (decltype(func))0x00557B80;
    func(this, nullptr);
}

void game::render_motion_blur()
{

    void(__fastcall * func)(void*, void*) = (decltype(func))0x00521610;
    func(this, nullptr);
}

bool game::is_marky_cam_enabled() const
{
    return this->the_world->field_28.is_marky_cam_enabled();
}

void game::one_time_init_stuff()
{
    void(__fastcall * func)(void*, void*) = (decltype(func))0x00552E50;
    func(this, nullptr);
}

    // 0x00514AB0
void game::init_motion_blur()
{

    void(__fastcall * func)(void*, void*) = (decltype(func))0x00514AB0;
    func(this, nullptr);
}

game_settings* game::get_game_settings()
{
    assert(gamefile != nullptr);

    return this->gamefile;
}


    // 0x00548C10
void game::begin_hires_screenshot(int a2, int a3)
{

        void(__fastcall * func)(void*, void*, int, int) = (decltype(func))0x00548C10;
    func(this, nullptr, a2, a3);
}

// 0x00515230
void game::enable_physics(bool a2)
{

        void(__fastcall * func)(void*, int, bool) = (decltype(func))0x00515230;
    func(this, 0, a2);
}


void game::one_time_deinit_stuff()
{

    CDECL_CALL(0x00524100, this);
}

void game::frame_advance_game_overlays(Float a1)
{

    CDECL_CALL(0x00515010, a1);
}

game::level_load_stuff::level_load_stuff()
{

    CDECL_CALL(0x00561EA0,this);
}

        // 0x0050B5F0
bool game::level_load_stuff::wait_for_mem_check()
{

    CDECL_CALL(0x0050B5F0, this);

    return true;
}

        // 0x0050B560
void game::level_load_stuff::destroy_loading_widgets()
{

    CDECL_CALL(0x0050B560, this);
}


float game::get_script_game_clock_timer() const
{

    CDECL_CALL(0x005244E0, this);

    return true;
}


void game::soft_reset_process()
{

    CDECL_CALL(0x00548C70, this);
}


#include <cstdint>



void game::level_load_stuff::look_up_level_descriptor()
{
    TRACE("game::level_load_stuff::look_up_level_descriptor");

        void(__fastcall * func)(void*, void*) = (decltype(func))0x055A1C0;
        func(this, nullptr);
    
}
void game::level_load_stuff::construct_loading_widgets()
{
    mString a1 { "spidermanlogo" };



    assert(!load_widgets_created);
    load_widgets_created = true;

    if (!os_developer_options::instance()->get_flag(mString { "NO_LOAD_SCREEN" })) {
        g_femanager().LoadFrontEnd();
    }
}

void game::go_next_state()
{
    assert(process_stack.size() != 0);
    this->process_stack.front().go_next_state();
}

void game::advance_state_load_level(Float a2)
{
    TRACE("game::advance_state_load_level");




            int TOD = os_developer_options::instance()->get_int(mString { "TIME_OF_DAY" });
            if (TOD == -1) {
                TOD = g_TOD();
            }





        void(__fastcall * func)(void*, void*, Float) = (decltype(func))0x0055D3A0;
        func(this, nullptr, a2);
}


 void game::show_max_velocity()
{
    if constexpr (1) {
    } else {
        void(__fastcall * func)(void*, void*) = (decltype(func))0x00514D00;
        func(this, nullptr);
    }
}
void game::message_board_init()
{
    this->mb = new message_board {};
}




void game::load_new_level(const mString& a2, const vector3d& a3)
{


        void(__fastcall * func)(void*, void*, const mString&, const vector3d&) = (decltype(func))0x0514C40;
        func(this, nullptr, a2, a3);
    
}

void game::load_new_level(const mString& a1, int a2)
{

        void(__fastcall * func)(void*, void*, const mString&, int) = (decltype(func))0x0514C70;
        func(this, nullptr, a1, a2);

}

void game::set_current_camera(camera* a2, bool a3)
{
    void(__fastcall * func)(void*, void*, camera*, bool) = (decltype(func))0x0514FD0;
    func(this, nullptr, a2, a3);
}

void game_patch()
{
    auto address = func_address(&game::frame_advance_level);
    REDIRECT(0x0055D8C2, address);

    {
        auto address = func_address(&game::set_camera);
        REDIRECT(0x00552FA3, address);
        REDIRECT(0x00552FC2, address);
    }


    {
        auto address = func_address(&game::message_board_init);
        REDIRECT(0x00552E7C, address);
    }

    {
        
    //     auto address = func_address(&game::load_this_level);
  //     REDIRECT(0x0055D409, address);
    }
}
