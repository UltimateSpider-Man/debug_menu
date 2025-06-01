#pragma once

#include "fetext.h"
#include "variable.h"
#include "common.h"
#include "os_developer_options.h"
#include "entity_handle_manager.h"
#include "vector2di.h"
#include "memory.h"
#include "ngl.h"
#include "entity.h"
#include "physical_interface.h"

#include "po.h"
#include "region.h"
#include "terrain.h"
#include "utility.h"
#include "wds.h"
#include "limited_timer.h"

#include "game_button.h"

#include <vector>

struct game_settings;
struct message_board;
struct world_dynamics_system;
struct entity_base;
struct localized_string_table;
struct game_process;
struct camera;
struct input_mgr;
struct mic;
struct nglMesh;
struct vector2di;
struct resource_key;
struct level_descriptor_t;

struct game_process {
    const char* field_0;
    int* field_4;
    int index;
    int num_states;
    int field_10;
    bool field_14;

    int go_next_state() { return true; };
};




// app_rebooter.hpp
#pragma once
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#if defined(_WIN32)
#define NOMINMAX
#include <shellapi.h>
#include <windows.h>
#else // POSIX
#include <limits.h>
#include <unistd.h>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif
#endif

namespace AppRebooter {

// ──────────────────────────────────────────────────────────────────────────────
// Global constants
// ──────────────────────────────────────────────────────────────────────────────
constexpr const char* kIniPath = "data/game.ini"; // default INI location
constexpr const char* kLevelList[2] = { "city_arena", "characterb_arena" };
// ──────────────────────────────────────────────────────────────────────────────
// INI helpers – cross‑platform read / write of single values
// ──────────────────────────────────────────────────────────────────────────────
inline std::string ReadINI(const char* section,
    const char* key,
    const char* defaultValue = "")
{
#if defined(_WIN32)
    char buf[256] {}; // zero‑initialise
    GetPrivateProfileStringA(section,
        key,
        defaultValue,
        buf,
        static_cast<DWORD>(sizeof(buf)),
        kIniPath);
    return buf;
#else
    std::ifstream file(kIniPath);
    if (!file)
        return defaultValue;

    std::string line, currentSection;
    auto trim = [](std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isspace(c); }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), s.end());
    };
    std::string targetSec(section), targetKey(key);
    std::transform(targetSec.begin(), targetSec.end(), targetSec.begin(), ::toupper);
    std::transform(targetKey.begin(), targetKey.end(), targetKey.begin(), ::toupper);

    while (std::getline(file, line)) {
        trim(line);
        if (line.empty() || line[0] == ';')
            continue;
        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            std::transform(currentSection.begin(), currentSection.end(), currentSection.begin(), ::toupper);
            continue;
        }
        if (currentSection != targetSec)
            continue;
        auto pos = line.find('=');
        if (pos == std::string::npos)
            continue;
        std::string k = line.substr(0, pos);
        trim(k);
        std::transform(k.begin(), k.end(), k.begin(), ::toupper);
        if (k != targetKey)
            continue;
        std::string value = line.substr(pos + 1);
        trim(value);
        return value;
    }
    return defaultValue;
#endif
}

inline void WriteINI(const char* section,
    const char* key,
    const char* value)
{
#if defined(_WIN32)
    if (!WritePrivateProfileStringA(section, key, value, kIniPath))
        throw std::runtime_error("WritePrivateProfileString failed");
#else
    // Very small, naïve re‑write: load file into memory, modify or append key.
    std::ifstream in(kIniPath);
    std::vector<std::string> lines;
    std::string currentSection, targetSec(section);
    std::transform(targetSec.begin(), targetSec.end(), targetSec.begin(), ::toupper);

    bool keyWritten = false;
    if (in) {
        std::string l;
        while (std::getline(in, l))
            lines.push_back(l);
    }

    auto trim = [](std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isspace(c); }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), s.end());
    };

    for (auto& l : lines) {
        std::string line = l;
        trim(line);
        if (line.empty() || line[0] == ';')
            continue;
        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            std::transform(currentSection.begin(), currentSection.end(), currentSection.begin(), ::toupper);
            continue;
        }
        if (currentSection != targetSec)
            continue;
        auto pos = line.find('=');
        if (pos == std::string::npos)
            continue;
        std::string k = line.substr(0, pos);
        trim(k);
        std::transform(k.begin(), k.end(), k.begin(), ::toupper);
        if (k == key) {
            l = std::string(key) + "=" + value;
            keyWritten = true;
            break;
        }
    }

    if (!keyWritten) {
        // append [section] + key if missing
        bool sectionExists = false;
        for (auto& l : lines) {
            std::string chk = l;
            trim(chk);
            if (chk.front() == '[' && chk.back() == ']') {
                std::string s = chk.substr(1, chk.size() - 2);
                std::transform(s.begin(), s.end(), s.begin(), ::toupper);
                if (s == targetSec) {
                    sectionExists = true;
                    break;
                }
            }
        }
        if (!sectionExists) {
            lines.push_back("[" + std::string(section) + "]");
        }
        lines.push_back(std::string(key) + "=" + value);
    }

    std::ofstream out(kIniPath, std::ios::trunc);
    for (auto& l : lines)
        out << l << "\n";
#endif
}

// Convenience: switch active scene by overwriting the value in [Strings]
inline void SelectScene(const std::string& sceneName)
{
    WriteINI("Strings", "SCENE_NAME", sceneName.c_str());
}

// ──────────────────────────────────────────────────────────────────────────────
// Process‑restart helpers (unchanged logic)
// ──────────────────────────────────────────────────────────────────────────────
namespace detail {
    inline std::string currentExecutablePath()
    {
#if defined(_WIN32)
        char buf[MAX_PATH];
        DWORD len = GetModuleFileNameA(nullptr, buf, MAX_PATH);
        if (len == 0 || len == MAX_PATH)
            throw std::runtime_error("GetModuleFileName failed");
        return std::string(buf, len);
#elif defined(__APPLE__)
        uint32_t size = 0;
        _NSGetExecutablePath(nullptr, &size);
        std::string path(size, '\\0');
        if (_NSGetExecutablePath(path.data(), &size) != 0)
            throw std::runtime_error("_NSGetExecutablePath failed");
        return path;
#else
        char buf[PATH_MAX];
        ssize_t len = ::readlink("/proc/self/exe", buf, sizeof(buf) - 1);
        if (len == -1)
            throw std::runtime_error("readlink(/proc/self/exe) failed");
        buf[len] = '\\0';
        return std::string(buf, len);
#endif
    }

    inline void launch(const std::string& exePath,
        const std::string& args = {})
    {
#if defined(_WIN32)
        std::string cmd = "\"" + exePath + "\" " + args;
        STARTUPINFOA si { sizeof(si) };
        PROCESS_INFORMATION pi {};
        if (!CreateProcessA(nullptr, cmd.data(), nullptr, nullptr, FALSE,
                0, nullptr, nullptr, &si, &pi))
            throw std::runtime_error("CreateProcess failed");
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
#else
        if (args.empty())
            execl(exePath.c_str(), exePath.c_str(), static_cast<char*>(nullptr));
        else
            execl("/bin/sh", "sh", "-c", (exePath + " " + args).c_str(), static_cast<char*>(nullptr));
        throw std::runtime_error("execl failed");
#endif
    }
} // namespace detail

[[noreturn]] inline void restart(const std::string& extraArgs = {})
{
    detail::launch(detail::currentExecutablePath(), extraArgs);
    std::exit(EXIT_SUCCESS);
}

[[noreturn]] inline void restartWith(const std::string& exePath,
    const std::string& args = {})
{
    detail::launch(exePath, args);

    std::exit(EXIT_SUCCESS);
}

} // namespace AppRebooter// namespace AppRebooter



enum class game_state {
    LEGAL = 1,
    WAIT_LINK = 4,
    LOAD_LEVEL = 5,
    RUNNING = 6,
    PAUSED = 7,
};

struct game;

inline Var<game_process> lores_game_process { 0x00922074 };

struct game {
    struct level_load_stuff {
        level_descriptor_t* descriptor;
        mString name_mission_table;
        mString field_14;
        vector3d field_24;
        int field_30;
        limited_timer_base field_34;
        bool load_widgets_created;
        bool load_completed;
        bool field_3A;
        bool field_3B;

    };

    struct flag_t {
        bool level_is_loaded;
        bool single_step;
        bool physics_enabled;
        bool field_3;
        bool game_paused;
    };

    char field_0;
    char field_1;
    char field_2;
    mString field_4;
    level_load_stuff level;
    world_dynamics_system* the_world;
    int field_54;
    int field_58;
    entity* field_5C;
    camera* current_game_camera;
    mic* field_64;
    message_board* mb;
    std::vector<game_process> process_stack;
    localized_string_table* field_7C;
    game_button field_80;
    nglMesh* field_B4;
    nglMesh* field_B8;
    char field_BC;
    char empty3[6];
    game_settings* gamefile;
    int empty4[30];
    float field_13C;
    float field_140;
    int field_144;
    char field_148;
    char field_149;
    char field_14A;
    float field_14C;
    float field_150;
    int field_154;
    bool field_158;
    bool field_159;
    bool field_15A;
    bool field_15B;
    bool field_15C;
    bool field_15D;
    bool field_15E;
    bool field_15F;
    bool field_160;
    bool field_161;
    bool field_162;
    bool field_163;
    bool field_164;
    bool field_165;
    bool field_166;
    bool field_167;
    flag_t flag;
    bool field_16D;
    bool field_16E;
    bool m_hero_start_enabled;
    bool field_170;
    bool field_171;
    bool m_user_camera_enabled;
    bool field_173;
    vector3d field_174;
    vector3d field_180[10];
    vector3d field_1F8[10];
    int field_270;
    float field_274;
    float field_278;
    int field_27C;
    int field_280;
    int field_281;
    bool field_282;
    float field_284;
    int field_288;
    float field_28C;
    float field_290;
    bool field_2B4;
    bool field_2B5;
    int field_2B8;
    int field_2BC;
    limited_timer_base field_2C0;

    game_settings *get_game_settings() {
        assert(gamefile != nullptr);

        return this->gamefile;
    }

  void render_bar_of_shame();

bool is_physics_enabled() const;


    void enable_user_camera(bool a2) {
        this->m_user_camera_enabled = a2;
    }

    bool is_user_camera_enabled() const {
        return this->m_user_camera_enabled;
    }

    void set_camera(int camera_state);

    void enable_physics(bool a2)
    {
        void (__fastcall *func)(void*, int, bool) = (decltype(func)) 0x00515230;
        func(this, 0, a2);
    }

    void message_board_init();

    void enable_marky_cam(bool a2, bool a3, Float a4, Float a5)
    {
        void (__fastcall *func)(void *, void *, bool, bool, Float, Float) = bit_cast<decltype(func)>(0x005241E0);

        func(this, nullptr, a2, a3, a4, a5);
    }

    void push_process(game_process &process) {
        void (__fastcall *sub_570FD0)(void *, int, void *) = (decltype(sub_570FD0)) 0x00570FD0;

        sub_570FD0(&this->process_stack, 0, &process);

        auto &last_proc = this->process_stack.back();
        last_proc.index = 0;
        last_proc.field_10 = 0;
    }

    void push_lores()
    {
        this->push_process(lores_game_process());
    }
 
    void load_new_level(const mString &a1, int a2)
    {

        AppRebooter::SelectScene("characterB_arena"); // or "city_arena"
        AppRebooter::restart();
        void (__fastcall *func)(void *, void *, const mString *, int) = bit_cast<decltype(func)>(0x00514C70);

        func(this, nullptr, &a1, a2);
    }

        void load_level()
    {

        AppRebooter::SelectScene("city_arena");
        AppRebooter::restart();

    }

            void swap_level()
    {

        AppRebooter::SelectScene("characterb_arena"); // or "city_arena"
        AppRebooter::restart();
    }

    void begin_hires_screenshot(int a2, int a3)
    {
        void (__fastcall *func)(void *, void *, int, int) = (decltype(func)) 0x00548C10;
        func(this, nullptr, a2, a3);
    }

    mString get_hero_info();

    mString get_camera_info();

    mString get_analyzer_info();

    void show_debug_info();

    void frame_advance_level(Float time_inc);
};

inline Var<game *> g_game_ptr{0x009682E0};

extern void game_patch();

