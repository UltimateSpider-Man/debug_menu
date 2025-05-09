#pragma once

#include "float.hpp"
#include "mstring.h"
#include "vector2di.h"
#include "keyboard.h"
#include "femanager.h"
#include "panelquad.h"
#include "tokenizer.h"
#include "ngl.h"

#include <list>
#include <stack>
#include <string>
#include <vector>

struct tokenizer;
struct color32;
struct mString;
struct PanelQuad;
struct ConsoleCommand;
struct ConsoleVariable;



static void (*kbevcb)(KeyEvent, Key_Axes, void*) = nullptr;

static void* kbevudata = nullptr;

static void (*kbchcb)(char, void*) = nullptr;

static void* kbchudata = nullptr;

extern std::stack<tokenizer*> s_exec_tok_stack;

extern float s_exec_tick;


 void console_char_callback(char a1, void* a2);

 void console_event_callback(KeyEvent a1, Key_Axes a2, void* a3);

     void _kbevcb(KeyEvent a1, Key_Axes a2)
 {
     if (kbevcb != nullptr) {
         kbevcb(a1, a2, kbevudata);
     }
 }

 void _kbchcb(char a1)
 {
     if (kbchcb != nullptr) {
         kbchcb(a1, kbchudata);
     }
 }

struct Console {
    bool m_visible;
    char oldCurrent[256];
    char current[256];

    std::list<mString> m_log;
    std::list<mString> m_command_log;
    bool field_220;
    float field_224;
    float m_height = 240.0f;
    uint32_t lineNumber;
    uint32_t field_230;
    int field_234;
    int cmdLogNumber;
    std::string field_23C;
    PanelQuad* field_248;
    bool field_24C;
    bool field_24D;
    bool field_24E;
    bool field_24F;



        char KB_register_event_callback(void (*a1)(KeyEvent, Key_Axes, void*), void* a2)
    {
        kbevcb = a1;
        kbevudata = a2;
        return 1;
    }

    char KB_register_char_callback(void (*a1)(char, void*), void* a2)
    {
        kbchcb = a1;
        kbchudata = a2;
        return 1;
    }


   

   Console()
    {
        TRACE("Console::Console");

        this->m_visible = false;
        this->oldCurrent[0] = '\0';
        this->current[0] = '\0';

        this->field_220 = false;
        this->m_height = 240.0;

        this->field_224 = 0.0;
        this->lineNumber = 0;
        this->field_230 = 16;
        this->field_234 = 16;
        this->cmdLogNumber = 0;
        this->field_24E = true;

        this->field_248 = new PanelQuad {};

        vector2d v8[4];
        v8[0] = { 0.0, 0.0 };
        v8[1] = { 640.0, 0.0 };
        v8[2] = { 0.0, this->m_height };
        v8[3] = { 640.0, this->m_height };

        color32 v4[4];
        v4[0] = { 0, 0, 0, 255 };
        v4[1] = { 0, 0, 0, 255 };
        v4[2] = { 0, 0, 0, 255 };
        v4[3] = { 0, 0, 0, 255 };

        Var<char[3]> Source { 0x00871BD1 };

        this->field_248->Init(v8, v4, panel_layer { 0 }, 10.0f, Source());

        this->field_24C = false;
        this->field_24D = false;

        this->field_23C = { "console_log.txt" };

        this->setHeight(200.0);
        this->hide();
        KB_register_event_callback(console_event_callback, nullptr);
        KB_register_char_callback(console_char_callback, nullptr);
    }

    void* operator new(size_t Size)
    {
        void* result; // eax

        while (1) {
            result = malloc(Size);
            if (result)
                break;
        }
        return result;
    }

    void operator delete(void* ptr, size_t size)
    {

        malloc(size);
    }
    bool StrnCopy(const char* src, char* dest, int* a3);

void exec_frame_advance(Float a2)
    {
        if (!s_exec_tok_stack.empty()) {
            s_exec_tick += a2;

            if (s_exec_tick >= os_developer_options::instance()->get_int(mString { "EXEC_DELAY" })) {
                s_exec_tick = 0.0;
                auto& v4 = s_exec_tok_stack.top();
                auto* v3 = v4->get_token();
                if (v3 != nullptr) {
                    this->processCommand(v3, false);
                } else {
                    auto& v2 = s_exec_tok_stack.top();
                    if (v2 != nullptr) {
                        delete v2;
                    }

                    s_exec_tok_stack.pop();
                }
            }
        }
    }

    virtual ~Console()
    {
    }

    virtual void getMatchingCmds(const char* a2, std::list<mString>& cmds)
    {
    }

    virtual void partialCompleteCmd(char* a1, std::list<mString>& list)
    {
    }

    virtual void hide()
    {
    }

    void render_console_text(const mString& a1, vector2di a2, const color32& a4)
    {
        FEText v7 { font_index { 0 },
            global_text_enum { 0 },
            (float)a2.x,
            (float)a2.y,
            1,
            panel_layer { 0 },
            1.0,
            16,
            0,
            a4 };

        auto* v3 = a1.c_str();

        mString v4 { v3 };

        v7.field_1C = v4;

        v7.Draw();
    }

    void nglGetStringDimensions3(
        nglFont* Font,
        uint32_t* Text,
        uint32_t* Width,
        const char* Height)
    {

        CDECL_CALL(0x007798E0, Font, Text, Width, Height);
    }

    void render()
    {
        TRACE("Console::render");

        const color32 font_color { 255, 255, 255, 255 };
        if (this->m_visible) {
            this->field_248->Draw();

            auto* font = g_femanager().GetFont(font_index { 0 });

            uint32_t v26, v25;
            nglGetStringDimensions3(font, &v26, &v25, "M");
            auto v24 = (float)v25;
            auto v23 = this->m_height - 20.0;

            const char* v11;
            if (this->field_220) {
                v11 = "_";
            } else {
                v11 = "";
            }

            auto v9 = mString { "> " } + this->current;

            auto v8 = v9 + v11;

            mString v22 = v8;

            vector2di v2 { 10, (int)v23 };
            render_console_text(v22, v2, font_color);
            v23 = v23 - v24;
            if (this->lineNumber > 0) {
                mString v17 { "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" };

                auto v3 = vector2di { 10, (int)v23 };
                render_console_text(v17, v3, font_color);

                v23 = v23 - v24;
            }

            auto it = this->m_log.begin();
            for (auto i = 0u;; ++i) {
                auto end = this->m_log.end();
                if (it == end || v23 <= 0.0) {
                    break;
                }

                if (i >= this->lineNumber) {
                    auto& v5 = *it;
                    if (v5.size() > 0) {
                        auto v7 = vector2di { 10, (int)v23 };

                        render_console_text(v5, v7, font_color);
                    }

                    v23 = v23 - v24;
                }

                ++it;
            }
        }
    }



    void exec(const mString& a2)
    {
        os_file v10 {};

        v10.open(a2, 1);
        if (v10.is_open()) {
            auto file_size = v10.get_size();
            auto* data = new char[file_size + 1];

            v10.read(data, file_size);
            data[file_size] = '\0';

            v10.close();

            auto* mem = mem_alloc(sizeof(tokenizer));
            auto* v6 = new (mem) tokenizer { true };

            v6->parse(data, "\n\r");
            v6->setup_current_iterator();
            s_exec_tok_stack.push(v6);

            auto* v4 = a2.c_str();
            this->addToLog("Executing '%s'", v4);
        } else {
            auto* v5 = a2.c_str();
            this->addToLog("File '%s' not found", v5);
        }
    }

    virtual void addToLog(const char* arg4, ...)
    {
    }

    virtual void processCommand(const char* a2, bool is_log)
    {
    }

    virtual void addToCommandLog(const char* a1)
    {
    }

ConsoleCommand* getCommand(const std::string& a1)
    {
        if (g_console_cmds == nullptr) {
            return nullptr;
        }

        auto& cmds = (*g_console_cmds);

        auto it = std::find_if(cmds.begin(), cmds.end(), [&a1](auto& cmd) -> bool {
            return cmd->match(a1);
        });

        return (it != cmds.end() ? (*it) : nullptr);
    }


ConsoleVariable* getVariable(const std::string& a1)
    {
        if (g_console_vars == nullptr) {
            return nullptr;
        }

        auto& vars = (*g_console_vars);
        auto it = std::find_if(vars.begin(), vars.end(), [&a1](auto& var) -> bool {
            return var->match(a1);
        });

        return (it != vars.end() ? (*it) : nullptr);
    }

bool isVisible() const
    {
        return this->m_visible;
    }

    void show()
    {
        TRACE("Console::show");
        this->field_248->TurnOn(true);

        this->m_visible = true;
        color32 v1 { 100, 100, 100, 100 };

        this->field_248->SetColor(v1);

#if 0
    v2 = sub_668F07();
    sub_69EDB4(v2);
#else
        input_mgr::instance()->field_20 |= 1u;
#endif

        g_game_ptr()->pause();
    }

    void setRenderCursor(bool a2)
    {
        this->field_24E = a2;
    }


void frame_advance(Float a2)
    {
        this->field_224 += a2;
        if (this->field_24E && this->field_224 >= 0.5) {
            this->field_220 = !this->field_220;
            this->field_224 = 0.0;
        }

        this->exec_frame_advance(a2);
    }

float getHeight()
    {
        return this->m_height;
    }


void setHeight(Float a2)
    {
        this->m_height = a2;

        this->field_248->SetPos(float { 0.0 }, float { 0.0 });
    }

void handle_event(KeyEvent a2, Key_Axes a3, [[maybe_unused]] void* a4)
    {
        if (a2 == 1) {
            switch (a3) {
            case KB_RETURN:
                if (this->m_visible) {
                    this->processCommand(this->current, false);
                    this->current[0] = '\0';
                    strcpy(this->oldCurrent, this->current);
                }

                break;
            case KB_BACKSPACE:
                if (this->m_visible) {
                    auto len = strlen(this->current);
                    if (len > 0) {
                        this->current[len - 1] = '\0';
                    }

                    strcpy(this->oldCurrent, this->current);
                } else if (debug_menu::active_menu != nullptr) {
                    debug_menu::hide();
                } else {
                    debug_menu::show();
                }
                break;
            case KB_TAB:
                if (this->m_visible && strlen(this->current)) {
                    std::list<mString> v28 {};

                    this->getMatchingCmds(this->current, v28);
                    if (v28.size()) {
                        if (v28.size() == 1) {
                            auto& v13 = v28.front();
                            auto* v14 = v13.c_str();
                            strcpy(this->current, v14);
                            strcat(this->current, " ");
                            strcpy(this->oldCurrent, this->current);
                        } else {
                            this->partialCompleteCmd(this->current, v28);
                            strcpy(this->oldCurrent, this->current);
                            this->addToLog((const char*)this);

                            this->addToLog((const char*)this);

                            for (auto& v16 : v28) {
                                auto* v17 = v16.c_str();
                                this->addToLog("%s", v17);
                            }
                        }

                        while (v28.size()) {
                            v28.pop_back();
                        }

                    } else {
                        this->addToLog((const char*)this);
                        this->addToLog((const char*)this);
                    }
                }
                break;
            case KB_TILDE:
                if (this->isVisible()) {
                    this->hide();
                } else {
                    this->show();
                }

                break;
            case KB_HOME:
                if (this->m_visible) {
                    this->lineNumber = this->m_log.size() - 1;
                }

                break;
            case KB_END:
                if (this->m_visible) {
                    this->lineNumber = 0;
                }

                break;
            case KB_PAGEUP:
                if (this->m_visible && this->lineNumber < this->m_log.size() - 1) {
                    ++this->lineNumber;
                }

                break;
            case KB_PAGEDOWN: {
                if (this->m_visible && this->lineNumber > 0) {
                    --this->lineNumber;
                }

                break;
            }
            case KB_DOWN:
                if (this->m_visible && this->m_command_log.size()) {
                    --this->cmdLogNumber;
                    if (this->cmdLogNumber > 0) {
                        auto it = this->m_command_log.begin();
                        auto end = this->m_command_log.end();

                        for (auto i = this->cmdLogNumber - 1; i > 0; --i) {
                            if (it == end) {
                                break;
                            }

                            ++it;
                        }

                        if (it != end) {
                            strcpy(this->current, it->c_str());
                        }

                    } else {
                        this->cmdLogNumber = 0;
                        strcpy(this->current, this->oldCurrent);
                    }
                }

                break;
            case KB_UP:
                if (this->m_visible && this->m_command_log.size()) {
                    ++this->cmdLogNumber;
                    while (this->cmdLogNumber > 0 && this->cmdLogNumber > this->m_command_log.size()) {
                        --this->cmdLogNumber;
                    }

                    if (this->cmdLogNumber > 0 && this->cmdLogNumber <= this->m_command_log.size()) {
                        auto it = this->m_command_log.begin();
                        auto end = this->m_command_log.end();
                        for (auto j = this->cmdLogNumber - 1; j > 0; --j) {
                            if (it == end) {
                                break;
                            }

                            ++it;
                        }

                        if (it != end) {
                            auto* v7 = it->c_str();
                            strcpy(this->current, v7);
                        }
                    }
                }

                break;
            default:
                return;
            }
        }
    }



void handle_char(char a2, void*)
    {
        if (this->m_visible && a2 >= ' ' && a2 != 127 && a2 != '`' && a2 != '~') {

            if (a2 == '[') {
                auto v3 = strlen(this->current);
                if (v3 > 0) {
                    this->current[v3 - 1] = '\0';
                }

                strcpy(this->oldCurrent, this->current);
                return;
            }

            auto v3 = strlen(this->current);
            this->current[v3] = a2;
            this->current[v3 + 1] = '\0';
            strcpy(this->oldCurrent, this->current);
        }
    }

};



extern char KB_register_event_callback(void (*a1)(KeyEvent, Key_Axes, void*), void* a2);

extern void render_console_text(const mString& a1, vector2di a2, const color32& a4);

Console* g_console;



    void console_event_callback(KeyEvent a1, Key_Axes a2, void* a3)
{
    if (g_console != nullptr) {
        g_console->handle_event(a1, a2, a3);
    }
}

    void console_char_callback(char a1, void *a2) {
    if (g_console != nullptr) {
        g_console->handle_char(a1, a2);
    }
}




void hook_nglListEndScene()
{
    g_console->render();

    CDECL_CALL(0x0076A030);
}

void terrain_types_manager_create_inst()
{
    CDECL_CALL(0x005C54B0);

    g_console = new Console {};
}

void terrain_types_manager_delete_inst()
{
    CDECL_CALL(0x005BA680);

    delete g_console;
}

void console_patch()
{
    REDIRECT(0x0052B5D7, hook_nglListEndScene);


    REDIRECT(0x00524155, terrain_types_manager_delete_inst);
}








#pragma once

#include "panelanimobject.h"

#include "float.hpp"
#include "mstring.h"
#include "mvector.h"
#include "vector2d.h"
#include "color32.h"

#include "trace.h"

#include <cstdint>

struct from_mash_in_place_constructor;
struct nglTexture;
struct PanelQuadSection;
struct PanelMeshSection;

struct PanelQuad : PanelAnimObject {
    vector2d field_14;
    mVector<PanelQuadSection> pqs;
    PanelMeshSection *pmesh;
    float field_34;
    float field_38;
    mString field_3C;
    int m_vtbl;

    void Init(vector2d* a2, color32* a3, panel_layer a4, Float a5, const char* a6)
    {
        void(__fastcall * func)(void*, void*, vector2d*, color32*, panel_layer, Float, const char*) = CAST(func, get_vfunc(m_vtbl, 0x54));

        func(this, nullptr, a2, a3, a4, a5, a6);
    }
    void SetPos(float* a2, float* a3)
    {
        void(__fastcall * func)(void*, void*, float*, float*) = CAST(func, get_vfunc(m_vtbl, 0x94));
        func(this, nullptr, a2, a3);
    }

    void Draw()
    {
        TRACE("Draw");

        CDECL_CALL(0x00616090, this);
    }

    void TurnOn(bool a2)
    {
        void(__fastcall * func)(void*, void*, bool) = CAST(func, get_vfunc(m_vtbl, 0x5C));

        func(this, nullptr, a2);
    }

    void SetColor(color32 a2)
    {
        void(__fastcall * func)(void*, void*, color32) = CAST(func, get_vfunc(m_vtbl, 0x7C));
        func(this, nullptr, a2);
    }
              
    };

extern void PanelQuad_patch();
#pragma once

#include "mstring.h"

#include <algorithm>
#include <cassert>
#include <string>

#include <cstdint>
#include <windows.h>

constexpr auto EXTEND_NEW_ENTRIES = 20;

constexpr auto MAX_CHARS_SAFE = 63;
constexpr auto MAX_CHARS = MAX_CHARS_SAFE + 1;

enum debug_menu_entry_type {
    UNDEFINED = 0,
    FLOAT_E,
    POINTER_FLOAT,
    INTEGER,
    POINTER_INT,
    BOOLEAN_E,
    POINTER_BOOL,
    POINTER_MENU,
    SWITCH_TIME,
    TIME,
    SCRIPTTEXT,
    NORMAL
};

extern const char *to_string(debug_menu_entry_type entry_type);

enum custom_key_type {
	LEFT,
	RIGHT,
	ENTER
};

struct debug_menu_entry;

extern void entry_frame_advance_callback_default(debug_menu_entry *a1);

struct debug_menu;

extern std::string entry_render_callback_default(debug_menu_entry* entry);

typedef void (*menu_handler_function)(debug_menu_entry*, custom_key_type key_type);

typedef void (*custom_entry_handler_ptr)(struct debug_menu_entry* entry, custom_key_type key_type, menu_handler_function menu_handler);

typedef void (*custom_entry_handler2_ptr)(struct debug_menu_entry* entry, custom_key_type key_type, menu_handler_function menu_handler);

typedef void (*custom_entry_handler3_ptr)(struct debug_menu_entry* entry, custom_key_type key_type ,menu_handler_function menu_handler);

typedef void (*custom_string_generator_ptr)(struct debug_menu_entry* entry);

struct script_instance;

struct debug_menu_entry {
	char text[MAX_CHARS];
	debug_menu_entry_type entry_type;
    union {
        float fval;
        float *p_fval;
        bool bval;
        bool *p_bval;
        int ival;
        int* p_ival;
        debug_menu *p_menu;
    } m_value;
	void* data;
    void* data1;
    uint16_t m_id {0};
    std::string (*render_callback)(debug_menu_entry *) = entry_render_callback_default;
    void (*m_game_flags_handler)(debug_menu_entry *) = nullptr;
    void (*frame_advance_callback)(debug_menu_entry *) = entry_frame_advance_callback_default;
    script_instance *field_14;
    int field_18;
    struct {
        float m_min_value;
        float m_max_value;
        float m_step_size;
        float m_step_scale;
    } field_20 {0.f, 1.f, 0.1f, 10.f};
    bool m_value_initialized {false};
    void *m_data = nullptr;
    custom_string_generator_ptr custom_string_generator;
    custom_entry_handler_ptr custom_handler;
    custom_entry_handler2_ptr custom_handler2;
    custom_entry_handler3_ptr custom_handler3;
    mString m_name;

    void set_step_size(float a2)
    {
        this->field_20.m_step_size = a2;
    }

    void set_step_scale(float a2)
    {
        this->field_20.m_step_scale = a2;
    }

    void set_data(void *a2)
    {
        this->m_data = a2;
    }

    void * get_data()
    {
        return this->m_data;
    }

    std::string get_script_handler()
    {
        return std::string {this->text};
    }

    void set_id(int id)
    {
        m_id = id;
    }

    auto get_id() const
    {
        return m_id;
    }

    void set_frame_advance_cb(void (*a2)(debug_menu_entry *))
    {
        this->frame_advance_callback = a2;
    }

    void set_submenu(debug_menu *submenu);

    void on_select(float a2);

    bool set_script_handler(script_instance *inst, const mString &a3);

    debug_menu *remove_menu();

    void on_change(float a3, bool a4);

    void set_fval(float a2, bool a3)
    {
        if ( !this->is_value_initialized() )
        {
            if ( a2 > this->field_20.m_max_value )
            {
                a2 = this->field_20.m_max_value;
            }

            if ( this->field_20.m_min_value > a2 )
            {
                a2 = this->field_20.m_min_value;
            }

            auto v3 = this->entry_type;
            if ( v3 == FLOAT_E )
            {
                this->m_value.fval = a2;
            }
            else if ( v3 == POINTER_FLOAT )
            {
                *this->m_value.p_fval = a2;
            }
            else
            {
                assert(0);
            }

            if ( this->m_game_flags_handler != nullptr && a3 )
            {
                this->m_game_flags_handler(this);
            }
        }

        this->get_fval();
    }

    float get_fval()
    {
        auto v2 = this->entry_type;
        if (v2 == FLOAT_E) {
            return this->m_value.fval;
        }

        if (v2 == POINTER_FLOAT) {
            return *this->m_value.p_fval;
        }

        assert(0);
        return 0.0;
    }

    const mString& get_name() const
    {
        return this->m_name;
    }

    bool get_bval() const
    {
        auto v2 = this->entry_type;
        if ( v2 == BOOLEAN_E )
        {
            return this->m_value.bval;
        }

        if ( v2 == POINTER_BOOL )
        {
            return *this->m_value.p_bval;
        }

        assert(0);
        return 0;
    }

    int get_ival()
    {
        auto v2 = this->entry_type;
        if ( v2 == INTEGER )
        {
            return this->m_value.ival;
        }

        if ( v2 == POINTER_INT )
        {
            return *this->m_value.p_ival;
        }

        assert(0);
        return 0;
    }

    int g_tod()
    {
        auto v2 = this->entry_type;
        if (v2 == SWITCH_TIME)
        {
            return this->m_value.ival;
        }
        if (v2 == TIME)
        {
            return *this->m_value.p_ival;
        }

        assert(0);
        return 0;
    }

    bool is_value_initialized() const
    {
        return this->m_value_initialized;
    }

    void set_value_initialized(bool a2)
    {
        m_value_initialized = a2;
    }

    int set_ival(int a2, bool a3)
    {
        printf("debug_menu_entry::set_ival: a2 = %d\n", a2);

        if ( !this->is_value_initialized() )
        {
            if ( a2 > this->field_20.m_max_value ) {
                a2 = this->field_20.m_max_value;
            }

            if ( this->field_20.m_min_value > a2 ) {
                a2 = this->field_20.m_min_value;
            }

            auto v4 = this->entry_type;
            if ( v4 == INTEGER )
            {
                this->m_value.ival = a2;
            }
            else if ( v4 == POINTER_INT )
            {
                *this->m_value.p_ival = a2;
            }
            else
            {
                assert(0);
            }

            if ( this->m_game_flags_handler != nullptr && a3 )
            {
                this->m_game_flags_handler(this);
            }
        }

        return this->get_ival();
    }

    int set_g_tod(int a2, bool a3)
    {
        printf("debug_menu_entry::set_ival: a2 = %d\n", a2);

        if (!this->is_value_initialized())
        {
            if (a2 > this->field_20.m_max_value) {
                a2 = this->field_20.m_max_value;
            }

            if (this->field_20.m_min_value > a2) {
                a2 = this->field_20.m_min_value;
            }

            auto v4 = this->entry_type;
            if (v4 == SWITCH_TIME)
            {
                this->m_value.ival = a2;
            }
            else if (v4 == TIME)
            {
                *this->m_value.p_ival = a2;
            }
            else
            {
                assert(0);
            }

            if (this->m_game_flags_handler != nullptr && a3)
            {
                this->m_game_flags_handler(this);
            }
        

        }

        return this->g_tod();
    }

    void set_p_ival(int *a2)
    {
        this->entry_type = POINTER_INT;
        this->m_value.p_ival= a2;
    }

    void set_pt_fval(float *a2)
    {
        this->entry_type = POINTER_FLOAT;
        this->m_value.p_fval = a2;
    }

    void set_min_value(float a2)
    {
        this->field_20.m_min_value = a2;
    }

    void set_max_value(float a2)
    {
        this->field_20.m_max_value = a2;
    }

    void set_bval(bool a2)
    {
        this->entry_type = BOOLEAN_E;
        this->m_value.bval = a2;
    }

    bool set_bval(bool a2, bool a3)
    {
        if ( !this->is_value_initialized() )
        {
            auto v4 = this->entry_type;
            if ( v4 == BOOLEAN_E )
            {
                this->m_value.bval = a2;
            }
            else if ( v4 == POINTER_BOOL )
            {
                *this->m_value.p_bval = a2;
            }
            else
            {
                assert(0);
            }

            if ( this->m_game_flags_handler != nullptr && a3 )
            {
                this->m_game_flags_handler(this);
            }
        }

        return this->get_bval();
    }

    void set_pt_bval(bool *a2)
    {
        this->entry_type = POINTER_BOOL;
        this->m_value.p_bval = a2;
    }

    void set_ival(int a2)
    {
        this->entry_type = INTEGER;
        this->m_value.ival= a2;
    }

    void set_g_tod(int a2)
    {
        this->entry_type = SWITCH_TIME;
        this->m_value.ival = a2;
    }

    void set_fl_values(const float *a2)
    {
        auto &v2 = this->field_20;
        v2.m_min_value = a2[0];
        v2.m_max_value = a2[1];
        v2.m_step_size = a2[2];
        v2.m_step_scale = a2[3];
    }

    void set_game_flags_handler(void (*a2)(debug_menu_entry *))
    {
        this->m_game_flags_handler = a2;
    }

    void set_render_cb(std::string (*a2)(debug_menu_entry *))
    {
        this->render_callback = a2;
    }

    debug_menu_entry() = default;

    debug_menu_entry(const char *p_text) : entry_type(UNDEFINED)
    {
        m_value.p_ival = nullptr;
        strncpy(this->text, p_text, MAX_CHARS_SAFE);
    }

    debug_menu_entry(const mString &p_text) : debug_menu_entry(p_text.c_str())
    {
        strncpy(this->text, p_text.c_str(), MAX_CHARS_SAFE);
    }

    debug_menu_entry(debug_menu *submenu);
};

extern debug_menu_entry *g_debug_camera_entry;


extern void close_debug();

extern debug_menu* current_menu;

struct debug_menu {
    enum class sort_mode_t {
        undefined = 0,
        ascending = 1,
        descending = 2,
    };

	char title[MAX_CHARS];
	DWORD capacity;
	DWORD used_slots;
	DWORD window_start;
	DWORD cur_index;
	menu_handler_function handler;
	debug_menu_entry* entries;
    debug_menu *m_parent {nullptr};
    sort_mode_t m_sort_mode;

    void add_entry(debug_menu_entry *entry);


    void add_entry(debug_menu *a1)
    {
        debug_menu_entry entry {a1};
        this->add_entry(&entry);
    }

    static void hide()
    {
        close_debug();
    }

    void go_back()
    {
        if (this->m_parent != nullptr)
        { 
            current_menu = this->m_parent;
            return;
        }
        
        close_debug();
    }

    static void init();

   static void show();


  static  void grab_focus();

        static inline bool has_focus = false;

    static inline debug_menu *root_menu = nullptr;

 static inline  debug_menu* active_menu = nullptr;

    static inline bool physics_state_on_exit = true;
};

extern void* add_debug_menu_entry(debug_menu* menu, debug_menu_entry* entry);

extern debug_menu * create_menu(const char* title, menu_handler_function function, DWORD capacity);

extern debug_menu * create_menu(const char* title, debug_menu::sort_mode_t mode = debug_menu::sort_mode_t::undefined);

extern debug_menu_entry *create_menu_entry(const mString &str);

extern debug_menu_entry *create_menu_entry(debug_menu *menu);

extern const char *to_string(custom_key_type key_type);

extern void handle_game_entry(debug_menu_entry *entry, custom_key_type key_type);

static inline bool debug_disabled = false;
static inline bool debug_enabled = false;
#include "debug_menu.h"

#include "mstring.h"
#include "script_instance.h"
#include "script_object.h"
#include "string_hash.h"
#include "vm_executable.h"
#include "game.h"

#include "ngl.h"

#include <algorithm>
#include <cassert>
#include <string>

#include <windows.h>

const char *to_string(debug_menu_entry_type entry_type)
{
    const char *strings[] = {
        "UNDEFINED",
        "FLOAT_E",
        "POINTER_FLOAT",
        "INTEGER",
        "POINTER_INT",
        "BOOLEAN_E",
        "POINTER_BOOL",
        "POINTER_MENU",
        "SWITCH_TIME",
        "NORMAL"
    };

    return strings[entry_type];
}

debug_menu_entry *g_debug_camera_entry {nullptr};

void entry_frame_advance_callback_default(debug_menu_entry *a1) {}

struct debug_menu;

std::string entry_render_callback_default(debug_menu_entry* entry);

std::string entry_render_callback_default(debug_menu_entry* entry)
{
    switch(entry->entry_type)
    {
    case FLOAT_E:
    case POINTER_FLOAT:
    {
		auto val = entry->get_fval();

        char str[64]{}; 
		snprintf(str, 64, "%.2f", val);
		return {str};
	}
    case BOOLEAN_E:
    case POINTER_BOOL:
    {
		bool val = entry->get_bval();

		auto *str = (val ? "True" : "False");
		return {str};
	}
    case POINTER_MENU: {

     char str[100] {};
     sprintf(str, ">");
     return { str };
    }
    case INTEGER:
    case POINTER_INT:
    {
		auto val = entry->get_ival();

        char str[100]{}; 
		sprintf(str, "%d", val);
        return {str};
    }
    case SWITCH_TIME:
    {
        auto val = entry->g_tod();

        char str[100]{};
        sprintf(str, "%d", val);
        return { str };
    }
    default:
        break;
    }

    return std::string{""};
}

typedef void (*menu_handler_function)(debug_menu_entry*, custom_key_type key_type);

void close_debug();

debug_menu* current_menu = nullptr;

void script_handler_helper(debug_menu_entry *a2)
{
    if ( a2->field_18 >= 0 && a2->field_14 != nullptr )
    {
        auto *parent = a2->field_14->get_parent();
        auto *exe = parent->get_func(a2->field_18);
        assert(exe != nullptr);

        if ( exe->get_parms_stacksize() == 4 ) {
            a2->field_14->add_thread(exe, (const char *)&a2);
        } else {
            a2->field_14->add_thread(exe);
        }

        debug_menu::hide();
    }
}



bool debug_menu_entry::set_script_handler(script_instance *inst, const mString &a3)
{
    auto *v14 = this;

    assert(inst != nullptr);

    auto *v3 = a3.c_str();
    string_hash v8 {v3};

    auto *v5 = inst->get_parent();
    auto v9 = v5->find_func(v8);
    auto v13 = v9;

    bool result;
    if ( v9 >= 0 )
    {
        v14->field_14 = inst;
        v14->field_18 = v13;
        v14->m_game_flags_handler = script_handler_helper;
        result = true;
    }
    else
    {
        auto *v6 = a3.c_str();
        printf("Could not find handler: %s\n", v6);
        result = false;
    }

    return result;
}



debug_menu *debug_menu_entry::remove_menu()
{
    if ( this->m_game_flags_handler != nullptr )
    {
        if ( this->m_value.p_menu != nullptr )
        {
            this->m_value.p_menu->~debug_menu();
        }

        this->m_value.p_menu = nullptr;
        this->m_game_flags_handler(this);
    }

    return this->m_value.p_menu;
}

void debug_menu_entry::on_change(float a3, bool a4)
{
    printf("debug_menu_entry::on_change: text = %s, entry_type = %s, a5 = %d\n", this->text, to_string(this->entry_type), a4);

    switch ( this->entry_type )
    {
    case FLOAT_E:
    case POINTER_FLOAT:
    {
        float v6;
        if ( a4 )
        {
            v6 = this->field_20.m_step_size * this->field_20.m_step_scale;
        }
        else
        {
            v6 = this->field_20.m_step_size;
        }

        auto v5 = this->get_fval() + a3 * v6;
        this->set_fval(v5, true);
        break;
    }
    case BOOLEAN_E:
    case POINTER_BOOL:
    {
        auto v3 = this->get_bval();
        this->set_bval(!v3, true);
        break;
    }
    case INTEGER:
    case POINTER_INT:
    {
        float v7 = (a4
            ? this->field_20.m_step_size * this->field_20.m_step_scale
            : this->field_20.m_step_size
            );

        printf("%f\n", v7);
        auto v8 = std::abs(v7);
        if (v8 < 1.0)
        {
            v8 = 1.0;
        }

        auto v4 = this->get_ival();
        if (a3 >= 0.0)
        {
            this->set_ival((int)(v4 + v8), true);
        }
        else
        {
            this->set_ival((int)(v4 - v8), true);
        }

        break;
    }
    case SWITCH_TIME:
    {
        float v7 = (a4
            ? this->field_20.m_step_size * this->field_20.m_step_scale
            : this->field_20.m_step_size
            );

        printf("%f\n", v7);
        auto v8 = std::abs(v7);
        if (v8 < 1.0)
        {
            v8 = 1.0;
        }

        auto v4 = this->g_tod();
        if (a3 >= 0.0)
        {
            this->set_g_tod((int)(v4 + v8), true);
        }
        else
        {
            this->set_g_tod((int)(v4 - v8), true);
        }

        break;
    }
    default:
    return;
    }
}

void debug_menu_entry::on_select(float a2)
{
    printf("debug_menu_entry::on_select: text = %s, entry_type = %s\n", this->text, to_string(this->entry_type));

    switch ( this->entry_type )
    {
    case UNDEFINED:
        if ( this->m_game_flags_handler != nullptr )
        {
            this->m_game_flags_handler(this);
        }

        break;
    case BOOLEAN_E:
    case POINTER_BOOL:
        this->on_change(a2, false);
        break;
    case POINTER_MENU:
        this->remove_menu();
        if ( this->m_value.p_menu != nullptr )
        {
            current_menu = this->m_value.p_menu;
        }

        break;
    default:
        return;
    }
}

void debug_menu_entry::set_submenu(debug_menu *submenu)
{
    this->entry_type = POINTER_MENU;
    this->m_value.p_menu = submenu;

    if (submenu != nullptr) {
        submenu->m_parent = current_menu;
    }
}

debug_menu_entry::debug_menu_entry(debug_menu *submenu) : entry_type(POINTER_MENU)
{
    m_value.p_menu = submenu;
    strncpy(this->text, submenu->title, MAX_CHARS_SAFE);
}

void* add_debug_menu_entry(debug_menu* menu, debug_menu_entry* entry)
{
    if (entry->entry_type == POINTER_MENU)
    {
        auto *submenu = entry->m_value.p_menu;
        if (submenu != nullptr) {
            submenu->m_parent = menu;
        }
    }

	if (menu->used_slots < menu->capacity) {
		void* ret = &menu->entries[menu->used_slots];
		memcpy(ret, entry, sizeof(debug_menu_entry));
		++menu->used_slots;

        if (entry->entry_type == NORMAL && menu->used_slots > 1) {
            std::swap(menu->entries[0], menu->entries[menu->used_slots - 1]);
        }

        if (menu->m_sort_mode != debug_menu::sort_mode_t::undefined ) {

            auto begin = menu->entries;
            auto end = begin + menu->used_slots;
            auto find_it = std::find_if(begin, end, [](debug_menu_entry &entry) {
                return entry.entry_type != POINTER_MENU;
            });

            if (find_it != end) {
                auto sort = [mode = menu->m_sort_mode](debug_menu_entry &e0, debug_menu_entry &e1) {
                    auto v7 = e0.get_script_handler();
                    auto v2 = e1.get_script_handler();
                    if (mode == debug_menu::sort_mode_t::ascending) {
                        return v7 < v2;
                    } else { //descending
                        return v7 > v2;
                    }
                };

                std::sort(begin, find_it, sort);

                std::sort(find_it, end, sort);
            }
        }

		return ret;
	}
	else {
		DWORD current_entries_size = sizeof(debug_menu_entry) * menu->capacity;
		DWORD new_entries_size = sizeof(debug_menu_entry) * EXTEND_NEW_ENTRIES;

		void* new_ptr = realloc(menu->entries, current_entries_size + new_entries_size);

		if (new_ptr == nullptr) {
			printf("RIP\n");
			__debugbreak();
		} else {
			menu->capacity += EXTEND_NEW_ENTRIES;
			menu->entries = static_cast<decltype(menu->entries)>(new_ptr);
			memset(&menu->entries[menu->used_slots], 0, new_entries_size);

			return add_debug_menu_entry(menu, entry);
		}
	}
	
	return nullptr;
}

void debug_menu::add_entry(debug_menu_entry *entry)
{
    add_debug_menu_entry(this, entry);
}



debug_menu * create_menu(const char* title, menu_handler_function function, DWORD capacity)
{
    auto *mem = malloc(sizeof(debug_menu));
    debug_menu *menu = new (mem) debug_menu {};

	strncpy(menu->title, title, MAX_CHARS_SAFE);

	menu->capacity = capacity;
	menu->handler = function;
	DWORD total_entries_size = sizeof(debug_menu_entry) * capacity;
	menu->entries = static_cast<decltype(menu->entries)>(malloc(total_entries_size));
	memset(menu->entries, 0, total_entries_size);

	return menu;
}

debug_menu * create_menu(const char* title, debug_menu::sort_mode_t mode)
{
    const auto capacity = 100u;
    auto *mem = malloc(sizeof(debug_menu));
    debug_menu* menu = new (mem) debug_menu {};

	strncpy(menu->title, title, MAX_CHARS_SAFE);

	menu->capacity = capacity;
	DWORD total_entries_size = sizeof(debug_menu_entry) * capacity;
	menu->entries = static_cast<decltype(menu->entries)>(malloc(total_entries_size));
	memset(menu->entries, 0, total_entries_size);

    menu->m_sort_mode = mode;

	return menu;
}

debug_menu_entry *create_menu_entry(const mString &str)
{
    auto *entry = new debug_menu_entry {str};
    return entry;
}

debug_menu_entry *create_menu_entry(debug_menu *menu)
{
    auto *entry = new debug_menu_entry{menu};
    return entry;
}

const char *to_string(custom_key_type key_type)
{
    if (key_type == ENTER)
    {
        return "ENTER";
    }
    else if (key_type == LEFT)
    {
        return "LEFT";
    }
    else if (key_type == RIGHT)
    {
        return "RIGHT";
    }

    return "";
}



void debug_menu::show()
{
    active_menu = root_menu;
    grab_focus();
}

void debug_menu::grab_focus()
{
    physics_state_on_exit = !g_game_ptr()->is_physics_enabled();
    g_game_ptr()->enable_physics(false);
    has_focus = true;
}




void handle_game_entry(debug_menu_entry *entry, custom_key_type key_type)
{
    printf("handle_game_entry = %s, %s, entry_type = %s\n", entry->text, to_string(key_type), to_string(entry->entry_type));

    if (key_type == ENTER)
    {
        switch(entry->entry_type)
        {
        case UNDEFINED:
        {    
            if ( entry->m_game_flags_handler != nullptr )
            {
                entry->m_game_flags_handler(entry);
            }
            break;
        }
        case BOOLEAN_E: 
        case POINTER_BOOL:
        {
            auto v3 = entry->get_bval();
            entry->set_bval(!v3, true);
            break;
        } 
        case POINTER_MENU:
        {
            if (entry->m_value.p_menu != nullptr)
            {
                current_menu = entry->m_value.p_menu;
            }
            return;
        }
        default:
            break;
        }
    }
    else if (key_type == LEFT)
    {
        entry->on_change(-1.0, false);
    }
    else if (key_type == RIGHT)
    {
        entry->on_change(1.0, true);
    }
}
#pragma once

#include <cstdint>

#include "float.hpp"

struct PanelAnimObject  {
    float field_4;
    float field_8;
    int field_C;
    char field_10;

 
};
#pragma once

struct Float {
    float value{0.0};

    constexpr Float(float p) : value(p) {}

    constexpr operator float() const {
        return value;
    }
};
#pragma once

#include "float.hpp"
#include "variable.h"

struct nglFont;
struct IGOFrontEnd;
struct PauseMenuSystem;
struct FrontEndMenuSystem;
struct PanelQuad;

struct FEManager {
    FEManager() = default;

    int empty;
    nglFont *field_4[5];
    IGOFrontEnd *IGO;
    PauseMenuSystem *m_pause_menu_system;
    FrontEndMenuSystem *m_fe_menu_system;
    int field_24;
    char field_28;
    char field_29;
    bool field_2A;


    nglFont* GetFont(font_index idx)
    {

        return nullptr;
    }

    void RenderLoadMeter(bool a2);

    //0x00648FF0
    void InitIGO();

    //0x00618CA0
    void LoadFonts();

    //0x00640E30
    void Draw();

    //0x00638180
    PanelQuad *GetDefaultPQ();

    //0x00642B30
    void Update(Float a2);

    //0x0060B920
    void ReleaseFrontEnd();

    //0x00648AB0
    void LoadFrontEnd();

    //0x00618D10
    void ReleaseFonts();




    static const char *font_name_array[5];
};

extern bool sub_581C30();

//0x006299B0
extern void RenderLoadMeter();



inline Var<FEManager> g_femanager{0x00937B00};

extern void FEManager_patch();
#pragma once

#include "femanager.h"
#include "fixedstring.h"
#include "float.hpp"
#include "func_wrapper.h"
#include "hashstring.h"
#include "igofrontend.h"
#include "matrix4x4.h"
#include "ngl_math.h"
#include "po.h"
#include "utility.h"
#include "tl_system.h"
#include "trace.h"
#include "variables.h"
#include "vector3d.h"
#include "vector4d.h"
#include "vtbl.h"

#include <psapi.h>
#include <windows.h>

#include <d3d9.h>

inline Var<HANDLE> h_sceneDump {0x00976E20};

struct nglGlyphInfo
{
    char field_0[0x18];
    char field_18[14];
};

struct nglFontHeader
{
    int Version;
    int CellHeight;
    int Ascent;
    int FirstGlyph;
    int NumGlyphs;
};

VALIDATE_SIZE(nglFontHeader, 0x14);

enum nglTextureFileFormat {};

struct nglPalette {
    uint32_t m_palette_idx;
    PALETTEENTRY *m_palette_entries;
    int m_size;

    int sub_782A70(int a2, int a3);

    void sub_782A40();
};

struct nglTexture
{
    nglTexture *field_0;
    nglTexture *field_4;
    int field_8;
    uint32_t m_format;
    D3DFORMAT m_d3d_format;
    nglTextureFileFormat field_14;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_numLevel;
    D3DLOCKED_RECT field_24;
    int field_2C;
    uint8_t *field_30;
    uint32_t field_34;
    int field_38;

    IDirect3DTexture9 *DXTexture;

    IDirect3DSurface9 **DXSurfaces;
    nglTexture *field_44;
    nglPalette *field_48;
    int field_4C;
    int field_50;
    int field_54;
    uint32_t m_num_palettes;
    nglTexture **Frames;
    tlFixedString field_60;
};

VALIDATE_SIZE(nglTexture, 0x80);

struct nglFont
{
    char field_0[0x24];
    nglTexture *field_24;
    nglGlyphInfo *GlyphInfo;
    nglFontHeader Header;
    int field_40;
    int field_44;
    int field_48;

    void sub_77E2F0(
        uint8_t a2,
        float *a3,
        float *a4,
        float *a5,
        float *a6,
        Float a7,
        Float a8)
    {
        void (__fastcall *func)(void *, void *,  
            uint8_t,
            float *,
            float *,
            float *,
            float *,
            Float ,
            Float ) = bit_cast<decltype(func)>(0x0077E2F0);
        func(this, nullptr, a2, a3, a4, a5, a6, a7, a8);
    }

};

VALIDATE_OFFSET(nglFont, field_40, 0x40);

struct nglDebug_t
{
    int field_0;
    int field_4;
    int field_8;
    int field_C;
    uint8_t RenderSingleNode;
    int field_14;
    uint8_t ShowPerfInfo;
    uint8_t ShowPerfBar;
    uint8_t ScreenShot;
    uint8_t DisableQuads;
    uint8_t DisableFonts;
    uint8_t field_1D;
    uint8_t DisableVSync;
    uint8_t DisableScratch;
    uint8_t DebugPrints;
    uint8_t DumpFrameLog;
    uint8_t DumpSceneFile;
    uint8_t DumpTextures;
    uint8_t DrawLightSpheres;
    uint8_t DrawMeshSpheres;
    uint8_t DisableDuplicateMaterialWarning;
    uint8_t DisableMissingTextureWarning;
};

VALIDATE_SIZE(nglDebug_t, 0x28);

inline Var<nglDebug_t> nglDebug {0x00975830};
inline Var<nglDebug_t> nglSyncDebug {0x009758E0};

LARGE_INTEGER query_perf_counter();

void nglQueueFlip();

struct nglPerfomanceInfo
{
    int field_0;
    int field_4;
    int field_8;
    int field_C;
    int field_10;
    int field_14;
    float field_18;
    float field_1C;
    LARGE_INTEGER field_20;
    LARGE_INTEGER field_28;
    LARGE_INTEGER field_30;
    LARGE_INTEGER field_38;
    LARGE_INTEGER field_40;
    LARGE_INTEGER m_counterQuads;
    LARGE_INTEGER field_50;
    float m_fps;
    float field_5C;
    float field_60;
    float m_render_time;
    float m_cpu_time;
    float field_6C;
    float field_70;
    float field_74;
    float m_num_polys;
    float m_num_verts;
    float m_num_nodes;
    float field_84;
};

VALIDATE_SIZE(nglPerfomanceInfo, 0x88);
VALIDATE_OFFSET(nglPerfomanceInfo, m_fps, 0x58);

inline Var<nglPerfomanceInfo> nglPerfInfo {0x00975858};
inline Var<nglPerfomanceInfo> nglSyncPerfInfo {0x00975908};

enum nglBlendModeType {};

struct nglQuad {
    struct Quad {
        struct {
            float field_0;
            float field_4;
        } pos;
        struct {
            float field_0;
            float field_4;
        } uv;
        uint32_t m_color;

    };
    Quad field_0[4];
    nglTexture *field_50;
    int field_54;
    nglBlendModeType field_58;
    uint32_t field_5C;
    nglTexture *m_tex;
};

VALIDATE_SIZE(nglQuad, 0x64);

inline constexpr auto NGLP_SCALE = 0x2;

inline constexpr auto NGLP_NO_CULLING = 0x40;
inline constexpr auto NGLP_FORCE_LOD = 0x80;

struct nglVertexBuffer
{
    union {
        IDirect3DIndexBuffer9 *m_indexBuffer;
        struct
        {
            int pad;
            uint32_t field_4;
            IDirect3DVertexBuffer9 *m_vertexBuffer;
        };
    };
};

VALIDATE_SIZE(nglVertexBuffer, 0xC);

struct nglScratchBuffer_t
{
    nglVertexBuffer field_0[2];
    IDirect3DIndexBuffer9 *field_18[2];
    int16_t *field_20;
    int field_24;
    int field_28;
    int field_2C;
    int field_30;
    int field_34;
    int field_38;
    int field_3C;
    int field_40;
    int field_44;
    IDirect3DIndexBuffer9 *field_48;
    nglVertexBuffer field_4C;
};

VALIDATE_SIZE(nglScratchBuffer_t, 0x58);

inline Var<nglScratchBuffer_t> nglScratchBuffer {0x00972A18};

struct nglLightContext {};

inline Var<nglLightContext *> nglDefaultLightContext {0x00973B70};

struct nglRenderNode;

struct nglScene
{
    int field_0;
    int field_4;
    int field_8;
    matrix4x4 field_C;
    float field_4C[16];
    float field_8C[16];
    float field_CC[16];
    float field_10C[16];
    float field_14C[16];
    float field_18C[16];
    float field_1CC[16];
    float field_20C[16];
    float field_24C[16];
    vector3d field_28C;
    vector3d field_298;
    int field_2A4;
    int field_2A8;
    vector4d field_2AC[1];
    int field_2BC[20];
    nglScene *field_30C;
    nglScene *field_310;
    nglScene *field_314;
    nglScene *field_318;
    void (*field_31C)(unsigned int *&, void *);
    void *field_320;
    void (*field_324)(unsigned int *&, void *);
    void *field_328;
    void (*field_32C)(unsigned int *&, void *);
    void *field_330;
    nglTexture *field_334;
    nglTexture *field_338;
    int field_33C;
    nglRenderNode *field_340;
    nglRenderNode *field_344;
    int OpaqueListCount;
    int TransListCount;
    nglLightContext *field_350;
    int empty0[18];
    int field_39C;
    int field_3A0;
    float field_3A4[4];
    int field_3B4;
    bool field_3B8;
    bool field_3B9;
    bool field_3BA;

    int empty5[8];
    bool field_3DC;
    int field_3E0;

    bool field_3E4;
    float field_3E8;
    float field_3EC;
    float m_nearz;
    float m_farz;
    float field_3F8;
    float field_3FC;
    int field_400;
    char field_404[0x3C];
};

VALIDATE_SIZE(nglScene, 0x440);

inline Var<nglScene *> nglCurScene {0x00971F00};

inline Var<nglScene *> nglRootScene {0x00971F04};

enum nglSceneParamType {};

struct nglRenderNode {
    std::intptr_t m_vtbl;
    nglRenderNode *field_4;
    nglTexture *field_8;

    void Render() {
        void (__fastcall *func)(void *) = CAST(func, get_vfunc(m_vtbl, 0x0));

        return func(this);
    }

};

inline Var<const float> PCFreq {0x0093A294};

enum nglFrameLockType {};

struct nglShaderNode : nglRenderNode {};

inline constexpr auto NGLMESH_PROCESSED = 0x10000;
inline constexpr auto NGLMESH_SCRATCH_MESH = 0x20000;

struct nglMesh;
struct nglMorphSet;
struct nglMaterialBase;

struct nglShaderParamSet_Pool {
    static inline Var<int> NextID {0x00971E8C};
};

struct nglParam {
    uint32_t field_0;
    uint32_t field_4;
};

inline Var<nglParam> EmptyParam {0x00971E80};

struct nglSceneParamSet_Pool {
    static inline Var<int> NextID = (0x00971E88);
};

struct USMMaterialListParam {
    nglMaterialBase **field_0;

    static inline Var<int> ID{0x009566D8};
};

struct USMMaterialIndicesParam {
    uint8_t *field_0;

    static inline Var<int> ID{0x00956730};
};

inline void *nglListAlloc(int size, int align) {
    TRACE("nglListAlloc");

    return (void *) CDECL_CALL(0x00401A20, size, align);
}


inline int nglHostPrintf(HANDLE hObject, const char *a2, ...)
{
    va_list va;
    va_start(va, a2);

    char a1[4096];
    auto result = vsprintf(a1, a2, va);

    if (hObject) {
        auto v3 = strlen(a1);
        DWORD numOfBytesWritten;

        result = WriteFile(hObject, a1, v3, &numOfBytesWritten, nullptr);
        if (!result) {
            CloseHandle(hObject);
            printf("nglHostPrintf: write error !\n");
            assert(0);
        }
    }

    va_end(va);

    return result;
}


template<typename T>
struct nglParamSet {

    nglParam *field_0;

    nglParamSet() = default;

    nglParamSet(int a2) {
        if (a2 != 0) {
            if (a2 == 1) {
                auto v2 = 4 * T::NextID() + 8;
                this->field_0 = static_cast<decltype(field_0)>(nglListAlloc(v2, 8));

                field_0->field_0 = 0;
                field_0->field_4 = 0;
            }
        } else {
            this->field_0 = &EmptyParam();
        }
    }

    void operator=(const nglParamSet<T> &param)
    {
        this->field_0 = param.field_0;
    }

    template<typename Param>
    Param *Get() {
        assert(IsSet(Param::ID()) && "Parameter not set.");

        return static_cast<Param *>(this->Get(Param::ID()));
    }

    void *Get(int id) {
        struct {
            int field_0;
            int field_4;
            struct {
                int field_0;
            } field_8[1];
        } *temp = CAST(temp, this->field_0);

        return &temp->field_8[id];
    }

    bool IsSet(int id) {
        return ((1ull << id) & *bit_cast<uint64_t *>(&this->field_0->field_0)) == 1ull << id;
    }

    template<typename Param>
    bool IsSetParam() {
        return this->IsSet(Param::ID());
    }
};

struct nglMeshParams {
    uint32_t Flags;
    int NBones;
    math::MatClass<4, 3> *field_8;
    int field_C;

    math::VecClass<3, 1> Scale;
};

struct nglMeshFile {
    tlFixedString field_0;
    char field_20[256];
    int field_120;
    tlFileBuf field_124;
    bool field_130;
    int field_134;
    nglMesh *FirstMesh;
    nglMaterialBase *field_13C;
    nglMorphSet *field_140;
    int field_144;
};

struct nglMeshNode {
    matrix4x4 field_0;
    matrix4x4 field_40;

    int field_80;
    int field_84;
    nglMesh *field_88;
    nglParamSet<nglShaderParamSet_Pool> field_8C;
    nglMeshParams *field_90;
    float field_94;

    matrix4x4 sub_41D840();

    matrix4x4 sub_4199D0();
};

inline void nglRenderQuad(const nglQuad &a1)
{
    TRACE("nglRenderQuad");

    if ( nglSyncDebug().DisableQuads ) {
        return;
    }

    auto v10 = query_perf_counter();

    CDECL_CALL(0x00783300, &a1);

    nglPerfInfo().m_counterQuads.QuadPart += query_perf_counter().QuadPart - v10.QuadPart;
    printf("counterQuads = %lld\n", nglPerfInfo().m_counterQuads.QuadPart);
}


struct nglQuadNode : nglRenderNode {
    nglQuad field_C;

    nglQuadNode() {
        m_vtbl = 0x008B9FB4;
    }

    //virtual
    //0x00783670
    void Render()
    {
        TRACE("nglQuadNode::Render");
        if ( !nglSyncDebug().DisableQuads) {
            nglRenderQuad(this->field_C);
        }
    }

};

VALIDATE_SIZE(nglQuadNode, 0x70);


struct nglMeshSection;

struct nglShader {
    std::intptr_t m_vtbl;
    nglShader *field_4;
    int field_8;

    void AddNode(
            nglMeshNode *a1,
            nglMeshSection *a2,
            nglMaterialBase *a3
            )
    {
        printf("m_vtbl = 0x%08X\n", m_vtbl);
        void (__fastcall *func)(void *,
                void *,
                nglMeshNode *,
                nglMeshSection *,
                nglMaterialBase *
                ) = CAST(func, get_vfunc(m_vtbl, 0x8));

        func(this, nullptr, a1, a2, a3);
    }

    bool IsSwitchable() const {
        bool (__fastcall *func)(const void *) = bit_cast<decltype(func)>(get_vfunc(m_vtbl, 0x24));

        return func(this);
    }
};

struct nglVertexDef {};

struct nglMaterialBase
{
    tlFixedString *Name;
    nglShader *field_4;
    nglMeshFile *File;
    nglMaterialBase *field_C;
    int field_10;
    int field_14;
    tlFixedString *field_18;
    nglTexture *field_1C;
    nglTexture *field_20;
    nglTexture *field_24;
    vector4d field_28;
    float field_38;
    int field_3C;
    int field_40;
    int field_44;
    int m_outlineFeature;
    int m_blend_mode;
    int field_50;
    int field_54;
    int field_58;
    int field_5C;
    int field_60;
    nglTexture *field_64;
    int field_68;
    float field_6C[3];
    int field_78;
    float field_7C;
    int field_80;
    float field_84;

    bool IsSwitchable() const {
        return this->field_4->IsSwitchable();
    }
};

struct nglMeshSection {
    tlFixedString *Name;
    nglMaterialBase *Material;
    int NBones;
    uint16_t *BonesIdx;

    math::VecClass<3, 1> SphereCenter;
    float SphereRadius;
    uint32_t Flags;
    D3DPRIMITIVETYPE m_primitiveType;
    int NIndices;
    uint16_t *m_indices;
    IDirect3DIndexBuffer9 *m_indexBuffer;
    int NVertices;
    void *m_vertices;
    uint32_t field_40;
    IDirect3DVertexBuffer9 *m_vertexBuffer;
    int m_stride;
    int field_4C;
    int field_50;
    nglVertexDef *VertexDef;
    int field_58;
    uint32_t field_5C;
};

struct nglMesh {
    tlHashString Name;
    uint32_t Flags;
    uint32_t NSections;

    struct {
        int field_0;
        nglMeshSection *Section;
    } * Sections;
    int NBones;
    math::MatClass<4, 3> *Bones;
    int NLODs;
    struct Lod {
        nglMesh *field_0;
        int field_4;
    };

    Lod *LODs;
    math::VecClass<3, 1> field_20;
    float SphereRadius;
    nglMeshFile *File;
    nglMesh *NextMesh;
    int field_3C;
};

struct ptr_to_po {
    const po *m_rel_po;
    const po *m_abs_po;
};

inline Var<int> nglFrame {0x00972904};

inline Var<nglFrameLockType> nglFrameLock {0x0093AED0};

inline Var<int> nglFrameLockImmediate {0x00972668};

inline Var<int> nglVBlankCount{0x00972908};

inline Var<int> nglLastFlipVBlank{0x00972670};

inline Var<int> nglFrameVBlankCount = (0x0097290C);

inline Var<DWORD> nglFlipCycle{0x00972674};

inline Var<DWORD> nglLastFlipCycle{0x00972678};

inline Var<BOOL> nglFlipQueued = {0x00972668};

inline Var<char *> nglListWork {0x00971F08};

inline Var<int> nglScratchMeshPos {0x00975310};

void nglCalculateMatrices(bool a1);

inline void nglScreenShot(const char *a1)
{
    CDECL_CALL(0x007731D0, a1);
}

inline void nglVif1RenderScene()
{
    TRACE("nglVif1RenderScene");
    
    CDECL_CALL(0x0077D060);
}

inline void sub_781A30()
{
    ;
}

inline void sub_76DE60()
{
    nglPerfInfo().field_28 = query_perf_counter();
}

inline Var<float> g_renderTime {0x00972664};

inline void sub_76DE80()
{
    nglPerfInfo().field_30 = query_perf_counter();
    g_renderTime() = (nglPerfInfo().field_30.QuadPart - nglPerfInfo().field_28.QuadPart) / PCFreq();
    nglQueueFlip();
}

inline void nglInitQuad(nglQuad *Quad)
{
    CDECL_CALL(0x0077AC40, Quad);
}

inline void nglSetQuadRect(nglQuad *Quad, Float a2, Float a3, Float a4, Float a5)
{
    CDECL_CALL(0x0077AD30, Quad, a2, a3, a4, a5);
}

inline void nglSetQuadColor(nglQuad *Quad, uint32_t Color)
{
    CDECL_CALL(0x0077AD10, Quad, Color);
}

inline void nglDumpQuad(nglQuad *Quad) {
    nglHostPrintf(h_sceneDump(), "\n");
    nglHostPrintf(h_sceneDump(), "QUAD\n");
    auto *v1 = Quad->m_tex;

    const char *v2 = "[none]";
    if (v1 != nullptr) {
        v2 = v1->field_60.field_4;
    }

    nglHostPrintf(h_sceneDump(), "  TEXTURE %s\n", v2);
    nglHostPrintf(h_sceneDump(), "  BLEND %d %d\n", Quad->field_58, Quad->field_5C);
    nglHostPrintf(h_sceneDump(), "  MAPFLAGS 0x%x\n", Quad->field_54);
    nglHostPrintf(h_sceneDump(), "  Z %f\n", Quad->field_50);

    for (auto i = 0u; i < 4u; ++i) {
        nglHostPrintf(h_sceneDump(),
                      "  VERT %f %f 0x%08X %f %f\n",
                      Quad->field_0[i].pos.field_0,
                      Quad->field_0[i].pos.field_4,
                      Quad->field_0[i].m_color,
                      Quad->field_0[i].uv.field_0,
                      Quad->field_0[i].uv.field_4);
    }

    nglHostPrintf(h_sceneDump(), "ENDQUAD\n");
}

inline void nglListAddQuad(nglQuad *Quad)
{
    TRACE("nglListAddQuad");

    if constexpr (1) {
        if (Quad != nullptr) {
            auto *mem = nglListAlloc(sizeof(nglQuadNode), 16);
            auto *new_node = new (mem) nglQuadNode{};

            if (nglCurScene()->field_3E4) {
                nglCalculateMatrices(false);
            }

            new_node->field_C = *Quad;
            if (((1 << Quad->field_58) & 3) != 0) {
                new_node->field_8 = Quad->m_tex;
                new_node->field_4 = nglCurScene()->field_340;
                nglCurScene()->field_340 = new_node;
                ++nglCurScene()->OpaqueListCount;
            } else {
                new_node->field_8 = Quad->field_50;
                new_node->field_4 = nglCurScene()->field_344;
                nglCurScene()->field_344 = new_node;
                ++nglCurScene()->TransListCount;
            }

            if ( nglSyncDebug().DumpSceneFile )
            {
                nglDumpQuad(Quad);
            }

        } else {
            printf("NULL mesh passed to nglListAddMesh !\n");
            assert(0);
        }

    } else {
        CDECL_CALL(0x0077AFE0, Quad);
    }
}

inline void nglSetQuadZ(nglQuad *Quad, Float Z)
{
    CDECL_CALL(0x0077AD70, Quad, Z);
}

inline void nglGetStringDimensions(nglFont *Font, const char *a2, int *a3, int *a4, Float a5, Float a6) {
    CDECL_CALL(0x007798E0, Font, a2, a3, a4, a5, a6);
}

inline uint8_t *nglGetDebugFlagPtr(const char *Flag)
{
    if ( strcmp(Flag, "ShowPerfInfo") == 0 ) {
        return &nglDebug().ShowPerfInfo;
    }

    if ( strcmp(Flag, "ShowPerfBar") == 0 ) {
        return &nglDebug().ShowPerfBar;
    }

    if ( strcmp(Flag, "ScreenShot") == 0 ) {
        return &nglDebug().ScreenShot;
    }

    if ( strcmp(Flag, "DisableQuads") == 0 ) {
        return &nglDebug().DisableQuads;
    }

    if ( strcmp(Flag, "DisableVSync") == 0 ) {
        return &nglDebug().DisableVSync;
    }

    if ( strcmp(Flag, "DisableScratch") == 0 ) {
        return &nglDebug().DisableScratch;
    }

    if ( strcmp(Flag, "DebugPrints") == 0 ) {
        return &nglDebug().DebugPrints;
    }

    if ( strcmp(Flag, "DumpFrameLog") == 0 ) {
        return &nglDebug().DumpFrameLog;
    }

    if ( strcmp(Flag, "DumpSceneFile") == 0 ) {
        return &nglDebug().DumpSceneFile;
    }

    if ( strcmp(Flag, "DumpTextures") == 0 ) {
        return &nglDebug().DumpTextures;
    }

    if ( strcmp(Flag, "DrawLightSpheres") == 0 ) {
        return &nglDebug().DrawLightSpheres;
    }

    if ( strcmp(Flag, "DrawMeshSpheres") == 0 ) {
        return &nglDebug().DrawMeshSpheres;
    }

    if ( strcmp(Flag, "DisableDuplicateMaterialWarning") == 0 ) {
        return &nglDebug().DisableDuplicateMaterialWarning;
    }

    if ( strcmp(Flag, "DisableMissingTextureWarning") == 0 ) {
        return &nglDebug().DisableMissingTextureWarning;
    }

    if ( strcmp(Flag, "RenderSingleNode") == 0 ) {
        return &nglDebug().RenderSingleNode;
    }

    return nullptr;
}

inline uint8_t nglGetDebugFlag(const char *Flag)
{
    auto *Ptr = nglGetDebugFlagPtr(Flag);

    uint8_t result = 0;
    if ( Ptr != nullptr ) {
        result = *Ptr;
    }

    return result;
}

inline void nglSetDebugFlag(const char *Flag, uint8_t Set)
{
    printf("nglSetDebugFlag: Flag = %s, Set = %d\n", Flag, Set);
    auto *Ptr = nglGetDebugFlagPtr(Flag);
    if ( Ptr != nullptr ) {
        printf("value is initialized!\n");
        *Ptr = Set;
    }

    nglSyncDebug() = nglDebug();
}

inline Var<nglFont *> nglSysFont{0x00975208};

inline void nglListAddString(nglFont *font, const char *a2, Float a3, Float a4, Float z_value, int a6, Float a7, Float a8)
{
    CDECL_CALL(0x00779C40, font, a2, a3, a4, z_value, a6, a7, a8);
}

inline void nglListAddString(nglFont *a1, Float a2, Float a3, Float a4, unsigned int a5, Float a6, Float a8, const char *Format, ...)
{
    char buffer[1024];
    va_list Args;

    va_start(Args, Format);
    vsprintf(buffer, Format, Args);
    nglListAddString(a1, buffer, a2, a3, a4, a5, a6, a8);
}

inline void nglListAddString_3(nglFont *a1, Float a2, Float a3, Float a4, unsigned int a5, Float a6, Float a8, const char *Format, ...)
{
    char buffer[1024];
    va_list Args;

    va_start(Args, Format);
    vsprintf(buffer, Format, Args);
    nglListAddString(a1, buffer, a2, a3, a4, a5, a6, a8);
}

inline void nglListAddString(nglFont *font, Float arg4, Float a3, Float a4, Float a5, Float a6, const char *a2, ...)
{
    char a1[1024];
    va_list va;

    va_start(va, a2);
    vsprintf(a1, a2, va);

    nglListAddString(font, a1, arg4, a3, a4, -1, a5, a6);
}

inline size_t sub_74A650()
{
    size_t v0 = 0;

    MEMORYSTATUS Buffer;
    GlobalMemoryStatus(&Buffer);

    auto dwTotalPhys = Buffer.dwTotalPhys;
    GetCurrentProcess();

    auto CurrentProcessId = GetCurrentProcessId();
    auto v3 = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, CurrentProcessId);

    PROCESS_MEMORY_COUNTERS ppsmemCounters;
    if ( GetProcessMemoryInfo(v3, &ppsmemCounters, sizeof(ppsmemCounters)) ) {
        v0 = dwTotalPhys - ppsmemCounters.PeakWorkingSetSize;
    }

    CloseHandle(v3);
    return v0;
}

inline LARGE_INTEGER query_perf_counter()
{
    LARGE_INTEGER PerformanceCount;

    QueryPerformanceCounter(&PerformanceCount);
    return PerformanceCount;
}

inline void sub_76DD70()
{
    nglLastFlipCycle() = nglFlipCycle();
    nglFlipCycle() = query_perf_counter().LowPart;
    nglLastFlipVBlank() = nglVBlankCount();
    nglFlipQueued() = false;

    printf("nglFlipCycle = %d, nglLastFlipCycle = %d\n", nglFlipCycle(), nglLastFlipCycle());
}

inline void sub_76DE00()
{
    ++nglVBlankCount();
    if ( nglFlipQueued() ) {
        if (nglVBlankCount() - nglLastFlipVBlank() >= nglFrameLock() ) {
            sub_76DD70();
        }
    }
}

inline void nglInit(HWND hWnd)
{
    printf("nglInit\n");

    CDECL_CALL(0x0076E3E0, hWnd);
}

inline void nglSetFrameLock(nglFrameLockType a2)
{
    printf("nglSetFrameLock\n");

    CDECL_CALL(0x0076E750, a2);
}

inline void nglQueueFlip()
{
    printf("nglQueueFlip: nglFrameLock = %d, nglVBlankCount = %d, nglLastFlipVBlank = %d, nglFrameLockImmediate = %d\n",
            nglFrameLock(), nglVBlankCount(), nglLastFlipVBlank(), nglFrameLockImmediate() );

    //TODO
    if (0
        //nglFrameLock() )
        )
    {
        if (nglFrameLockImmediate() == 0
                || (nglVBlankCount() - nglLastFlipVBlank()) < (uint32_t) nglFrameLock() )
        {
            nglFlipQueued() = true;
            return;
        }
    }

    sub_76DD70();
}

inline void nglRenderPerfInfo()
{
    printf("nglRenderPerfInfo\n");

    if constexpr (0) {
        char Dest[2048];

        static Var<char[1]> byte_8B7DF8 = {0x008B7DF8};

        static Var<int> nglVif1WorkSize = {0x00971F10};

        if (nglSyncDebug().ShowPerfInfo == 1) {
            printf("%f\n", nglSyncPerfInfo().m_fps);
            size_t v0 = sub_74A650();
            sprintf(Dest,
                    byte_8B7DF8(),
                    "FINAL",
                    nglSyncPerfInfo().m_fps, 
                    nglSyncPerfInfo().m_render_time,
                    nglSyncPerfInfo().m_cpu_time,
                    nglSyncPerfInfo().field_70,
                    nglSyncPerfInfo().field_74,
                    nglSyncPerfInfo().field_18,
                    nglSyncPerfInfo().field_1C,
                    nglSyncPerfInfo().m_num_verts,
                    nglSyncPerfInfo().m_num_polys,
                    nglSyncPerfInfo().m_num_nodes,
                    nglSyncPerfInfo().field_0,
                    nglVif1WorkSize(),
                    nglSyncPerfInfo().field_8,
                    nglSyncPerfInfo().field_C,
                    nglSyncPerfInfo().field_10,
                    nglSyncPerfInfo().field_14,
                    nglDebug().field_8,
                    nglDebug().field_C,
                    *(int *) &nglDebug().RenderSingleNode,
                    v0);
        } else {
            sprintf(Dest,
                    "%.2f FPS\n%.2fms\n",
                    nglSyncPerfInfo().m_fps,
                    nglSyncPerfInfo().m_render_time);
        }

        uint32_t a3;
        uint32_t a4;

        nglGetStringDimensions(nglSysFont(), Dest, (int *)&a3, (int *) &a4, 1.0, 1.0);

        nglQuad a1;
        nglInitQuad(&a1);
        float v1 = a4 + 10;
        float v2 = 620 - a3;
        nglSetQuadRect(&a1, v2, 10.0, 640.0, v1);

        nglSetQuadColor(&a1, 0xC0000000);
        nglSetQuadZ(&a1, -9999.0);
        nglListAddQuad(&a1);
        float v3 = (630 - a3);
        nglListAddString(nglSysFont(), Dest, v3, 20.0, -9999.0, -1, 1.0, 1.0);
    } else {
        CDECL_CALL(0x0076E0E0);
    }
}

inline void PumpMessages()
{
    struct tagMSG Msg;

    while ( PeekMessageA(&Msg, 0, 0, 0, PM_REMOVE) ) {
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }
}

inline int __fastcall sub_781EA0(void *a1)
{
    int (__fastcall *func)(void *) = bit_cast<decltype(func)>(0x00781EA0);
    return func(a1);
}

inline void Reset3DDevice()
{
    CDECL_CALL(0x0076E800);
}

inline nglScene *nglListBeginScene(nglSceneParamType a1)
{
    return (nglScene *) CDECL_CALL(0x0076C970, a1);
}

inline void nglSceneDumpStart()
{
    printf("nglSceneDumpStart\n");

    printf("nglSyncDebug.DumpSceneFile = %d\n", nglSyncDebug().DumpSceneFile);
    if ( nglSyncDebug().DumpSceneFile == 0 ) {
        return;
    }

    auto nglHostOpen = [](const char *a1) -> HANDLE {
        CHAR FileName[512];

        strcpy(FileName, a1);
        auto v2 = CreateFileA(FileName, GENERIC_WRITE, 0, nullptr, 2u, 0x8000080u, nullptr);
        if (v2 != (HANDLE) -1) {
            return v2;
        }

        printf("cannot open \"%s\" for writing !\n", FileName);
        assert(0);

        return nullptr;
    };

    h_sceneDump() = nglHostOpen("scenedump.scene");
    nglHostPrintf(h_sceneDump(), "//\n");
    nglHostPrintf(h_sceneDump(), "// Midnight scene file dump.\n");
    nglHostPrintf(h_sceneDump(), "//\n");
    nglHostPrintf(h_sceneDump(), "\n");
    nglHostPrintf(h_sceneDump(), "\n");
}

inline nglLightContext *nglCreateLightContext()
{
    return (nglLightContext *) CDECL_CALL(0x00775EC0);
}

inline void nglDumpMesh(nglMesh *Mesh, const math::MatClass<4, 3> &a2, nglMeshParams *MeshParams)
{
    CDECL_CALL(0x007825A0, Mesh, &a2, MeshParams);
}

inline math::VecClass<3, 1> sub_413E90(
        const math::VecClass<3, 0> &arg4,
        const float &arg8,
        const math::VecClass<3, 0> &a2,
        const math::VecClass<3, 0> &a3,
        const math::VecClass<3, 0> &a6,
        const float *a7,
        const math::VecClass<3, 0> &a8)
{
    math::VecClass<3, 0> v14;
    v14[0] = a8[0];
    v14[1] = a8[1];
    v14[2] = a8[2];
    v14[3] = a8[3];
    v14.sub_413530(arg4, arg8);
    v14.sub_411A50(a2, a3);
    auto v11 = a6[0] * a7[2];
    auto v12 = a6[1] * a7[2];
    auto v15 = a6[2] * a7[2];
    auto v16 = a6[3] * a7[2];

    math::VecClass<3, 1> result;
    result[0] = v14[0] + v11;
    result[1] = v14[1] + v12;
    result[2] = v14[2] + v15;
    result[3] = v14[3] + v16;
    return result;
}

inline math::VecClass<3, 1> sub_414360(const math::VecClass<3, 1> &a2, const math::MatClass<4, 3> &a3)
{
    math::VecClass<3, 1> result;
    if constexpr (1) {
        math::VecClass<3, 0> a2a, a3a, a4, a5;
        a3.sub_4134B0(a2a, a3a, a4, a5);
        result = sub_413E90(
                a2a,
                a2.field_0[0],
                a3a,
                *(const math::VecClass<3, 0> *)&a2,
                a4,
                a2.field_0,
                a5);
    } else {
        CDECL_CALL(0x00414360, &result, &a2, a3);
    }

    return result;
}

inline matrix4x4 sub_507130(void *arg4) {
    matrix4x4 result;
    CDECL_CALL(0x00507130, &result, arg4);
    return result;
}

inline bool sub_755520(Float a1, Float a2, Float a3, Float, Float a5) {
    for (auto i = 0u; i < 6u; ++i) {
        auto &v = nglCurScene()->field_2AC[i];

        if (a3 * v[2] + a1 * v[0] + a2 * v[1] - v[3] + a5 < 0.0f) {
            return false;
        }
    }

    return true;
}

inline nglMaterialBase *sub_8EA2E0(nglParamSet<nglShaderParamSet_Pool> &a1,
                            nglMaterialBase *DefaultMaterial) {
    if (!a1.IsSetParam<USMMaterialListParam>()) {
        return DefaultMaterial;
    }

    assert(DefaultMaterial->IsSwitchable());

    auto v5 = *bit_cast<uint32_t*>(&DefaultMaterial->field_18);

    nglMaterialBase **v4 = nullptr;
    nglMaterialBase *v3 = nullptr;
    if (v5 < 4u &&
        (v4 = a1.Get<USMMaterialListParam>()->field_0,
         (v3 = v4[a1.Get<USMMaterialIndicesParam>()->field_0[v5]]) != nullptr) &&
        v3->field_4 == DefaultMaterial->field_4) {
        return v3;
    }

    return DefaultMaterial;
}

inline int sub_76F3E0(Float a1, Float a2, Float a3, Float a4, Float a5, char a6) {
    if constexpr (1) {
        if ((a6 & 0x40) != 0 || sub_755520(a1, a2, a3, a4, a5)) {
            return 0;
        }

        return -1;

    } else {
        return CDECL_CALL(0x0076F3E0, a1, a2, a3, a4, a5, a6);
    }
}

inline math::MatClass<4, 3> *nglListAddMesh_GetScaledMatrix(const math::MatClass<4, 3> &a1,
                                                     nglMeshParams *a2,
                                                     float *a3)
{
    return (math::MatClass<4, 3> *) CDECL_CALL(0x00770230, &a1, a2, a3);
}

inline nglMesh *nglListAddMesh_GetLOD(nglMesh *Mesh,
                               unsigned int a2,
                               nglMeshParams *a3,
                               math::VecClass<3, 1> a4)
{
    nglMesh *result;
    if ((a2 & 0x80u) == 0) {
        math::VecClass<3, 1> v10 = sub_414360(a4, bit_cast<math::MatClass<4, 3>>(nglCurScene()->field_14C));
        auto v7 = Mesh->NLODs - 1;
        if (v7 < 0) {
        LABEL_11:
            result = Mesh;
        } else {
            auto *v8 = Mesh->LODs;
            auto *v9 = (float *) &v8[8 * v7 + 4];
            while (v10.field_0[2] <= (double) *v9) {
                --v7;
                v9 -= 2;
                if (v7 < 0) {
                    goto LABEL_11;
                }
            }

            result = *(nglMesh **) (v8 + 8 * v7);
        }
    } else {
        auto v4 = a3->field_C;
        if (v4) {
            auto v6 = Mesh->NLODs;
            if (v4 - 1 <= v6)
                result = *(nglMesh **) &Mesh->LODs[8 * v4 - 8];
            else
                result = *(nglMesh **) &Mesh->LODs[8 * v6 - 8];
        } else {
            result = Mesh;
        }
    }
    return result;
}

inline void nglCalculateMatrices(bool a1)
{
    CDECL_CALL(0x0076ABA0, a1);
}

inline void nglDumpCamera(const math::MatClass<4, 3> &a1)
{
    TRACE("nglDumpCamera");

    CDECL_CALL(0x00782210, &a1);
}

inline void nglSetWorldToViewMatrix(const math::MatClass<4, 3> &a1) {
    memcpy(nglCurScene()->field_14C, &a1, sizeof(nglCurScene()->field_14C));
    nglCurScene()->field_3E4 = true;

    if ( nglSyncDebug().DumpSceneFile ) {
        nglDumpCamera(a1);
    }
}

inline void FastListAddMesh(nglMesh *Mesh,
                     const math::MatClass<4, 3> &LocalToWorld,
                     nglMeshParams *MeshParams,
                     nglParamSet<nglShaderParamSet_Pool> *ShaderParams)
{
    TRACE("FastListAddMesh");

    if constexpr (1) {
        assert(Mesh != nullptr && "NULL mesh passed to FastListAddMesh.\n");

        assert((Mesh->Flags & NGLMESH_PROCESSED) ||
               (Mesh->Flags & NGLMESH_SCRATCH_MESH) && "Mesh missing NGLMESH_PROCESSED flag.");

        assert(std::abs(AbsSquared(LocalToWorld.GetX()) - 1.0f) +
                       std::abs(AbsSquared(LocalToWorld.GetY()) - 1.0f) +
                       std::abs(AbsSquared(LocalToWorld.GetZ()) - 1.0f) <
                   0.01f &&
               "Invalid scale detected in local to world transform.  If scaling is desired, use "
               "MeshParams.\n");

        if ( nglSyncDebug().DumpSceneFile )
        {
            nglDumpMesh(Mesh, LocalToWorld, MeshParams);
        }

        assert(MeshParams != nullptr && "NULL MeshParams in FastListAddMesh.\n");

        math::VecClass<3, 1> v18;

        if (Mesh->NLODs != 0) {
            float *v5;
            if ((MeshParams->Flags & 1) != 0) {
                v5 = Mesh->field_20.field_0;
            } else {
                v18 = sub_414360(Mesh->field_20, LocalToWorld);
                v5 = v18.field_0;
            }

            math::VecClass<3, 1> a2a;
            a2a[0] = v5[0];
            a2a[1] = v5[1];
            a2a[2] = v5[2];
            a2a[3] = v5[3];

            v18 = sub_414360(a2a, *(const math::MatClass<4, 3> *) &nglCurScene()->field_14C);
            auto v9 = Mesh->NLODs - 1;
            if (v9 >= 0) {
                auto *v10 = Mesh->LODs;
                auto *v11 = (float *) &v10[v9].field_4;
                while (v18.field_0[2] <= (double) *v11) {
                    --v9;
                    v11 -= 2;
                    if (v9 < 0) {
                        goto LABEL_11;
                    }
                }

                Mesh = v10[v9].field_0;
            }
        }

    LABEL_11:

        auto *v12 = static_cast<nglMeshNode *>(nglListAlloc(sizeof(nglMeshNode), 64));

        v12->field_88 = Mesh;
        std::memcpy(&v12->field_0, &LocalToWorld, sizeof(LocalToWorld));

        ptr_to_po a2a;
        a2a.m_rel_po = CAST(a2a.m_rel_po, &LocalToWorld);
        a2a.m_abs_po = CAST(a2a.m_abs_po, nglCurScene()->field_18C);

        v12->field_40 = sub_507130(&a2a);

        v12->field_84 = 0;
        v12->field_80 = 0;
        v12->field_94 = 1.0;

        assert(!(MeshParams->Flags & NGLP_SCALE) && "No scale allowed in FastListAddMesh.\n");

        //assert((MeshParams->Flags & NGLP_REFERENCED) && "MeshParams must be referenced in FastListAddMesh.\n");

        assert((MeshParams->Flags & NGLP_NO_CULLING) &&
               "Mesh must be pre-culled for FastListAddMesh.\n");

        assert(!(MeshParams->Flags & NGLP_FORCE_LOD) &&
               "Force LOD not supported by FastListAddMesh.\n");

        v12->field_90 = MeshParams;

        assert(ShaderParams != nullptr && "NULL ShaderParams in FastListAddMesh.\n");

        v12->field_8C = *ShaderParams;

        for (auto i = 0u; i < Mesh->NSections; ++i) {
            auto *MeshSection = Mesh->Sections[i].Section;

            nglPerfInfo().m_num_verts += MeshSection->NVertices;

            nglMaterialBase *v15 = sub_8EA2E0(v12->field_8C, MeshSection->Material);

            MeshSection->Material->field_4->AddNode(v12, MeshSection, v15);
        }

        nglPerfInfo().m_num_polys += Mesh->field_3C;

        if (Mesh->File != nullptr) {
            Mesh->File->field_144 = nglFrame();
        }
    } else {
        CDECL_CALL(0x00507690, Mesh, LocalToWorld, MeshParams, ShaderParams);
    }
}

inline void nglListAddMesh(nglMesh *Mesh,
                    const math::MatClass<4, 3> &a2,
                    nglMeshParams *a3,
                    nglParamSet<nglShaderParamSet_Pool> *a4)
{
    printf("nglListAddMesh\n");

    if constexpr (1) {
        if (Mesh != nullptr) {

            assert(((Mesh->Flags & NGLMESH_PROCESSED) || (Mesh->Flags & NGLMESH_SCRATCH_MESH)) &&
               "Mesh missing NGLMESH_PROCESSED flag.");

            if ( nglSyncDebug().DisableScratch
                    || (Mesh->Flags & NGLMESH_SCRATCH_MESH) == 0 )
            {
                return;
            }

            if ( nglSyncDebug().DumpSceneFile ) {
                nglDumpMesh(Mesh, a2, a3);
            }

            int v20 = 0;
            if (a3 != nullptr) {
                v20 = a3->Flags;
            }

            auto *v5 = &a2;
            auto a5 = Mesh->SphereRadius;
            float v15 = 1.0;
            if ((v20 & 2) != 0) {
                v5 = nglListAddMesh_GetScaledMatrix(a2, a3, &v15);
                a5 *= v15;
            }

            math::VecClass<3, 1> v18 = sub_414360(Mesh->field_20, *v5);
            if (nglCurScene()->field_3E4) {
                nglCalculateMatrices(false);
            }

            if (Mesh->NLODs) {
                Mesh = nglListAddMesh_GetLOD(Mesh, v20, a3, v18);
            }

            auto *v6 = v5;
            auto v17 = nglListWorkPos();

            nglMeshNode *meshNode = static_cast<nglMeshNode *>(
                nglListAlloc(sizeof(nglMeshNode), 64));

            meshNode->field_88 = Mesh;
            meshNode->field_0 = {};

            ptr_to_po v14;
            v14.m_rel_po = CAST(v14.m_rel_po, v6);
            v14.m_abs_po = CAST(v14.m_abs_po, &nglCurScene()->field_18C);

            meshNode->field_40 = sub_507130(&v14);
            meshNode->field_84 = 0;
            meshNode->field_80 = 0;
            meshNode->field_94 = v15;
            if (a3 != nullptr) {
                if (v20 >= 0) {
                    meshNode->field_90 = static_cast<nglMeshParams *>(
                        nglListAlloc(sizeof(nglMeshParams), 16));

                    std::memcpy(meshNode->field_90, a3, sizeof(nglMeshParams));
                } else {
                    meshNode->field_90 = a3;
                }

            } else {
                static Var<nglMeshParams> nglEmptyMeshParams{0x00972820};
                meshNode->field_90 = &nglEmptyMeshParams();
            }

            if (a4 != nullptr) {
                meshNode->field_8C = *a4;
            } else {
                meshNode->field_8C = {0};
            }

            if (sub_76F3E0(v18.field_0[0], v18.field_0[1], v18.field_0[2], v18.field_0[3], a5, v20) ==
                -1) {
                nglListWorkPos() = v17;
            } else {
                for (auto i = 0u; i < Mesh->NSections; ++i)
                {
                    auto *MeshSection = Mesh->Sections[i].Section;
                    nglPerfInfo().m_num_verts += MeshSection->NVertices;

                    MeshSection->Material->field_4->AddNode(
                            meshNode,
                            MeshSection,
                            MeshSection->Material);
                }

                nglPerfInfo().m_num_polys += Mesh->field_3C;
                auto *v13 = Mesh->File;
                if (v13 != nullptr) {
                    v13->field_144 = nglFrame();
                }
            }
        }

    } else {
        CDECL_CALL(0x00770360, Mesh, &a2, a3, a4);
    }
}



inline void nglListInit()
{
    TRACE("nglListInit");

    if constexpr (1) {
        nglFrameVBlankCount() = nglVBlankCount();
        nglPerfInfo().field_38 = query_perf_counter();
        nglListWorkPos() = bit_cast<int>(nglListWork());
        nglDefaultLightContext() = nglCreateLightContext();
        if ( nglSyncDebug().DumpFrameLog ) {
            nglDebug().DumpFrameLog = 1;
        }

        if ( nglSyncDebug().DumpSceneFile ) {
            nglDebug().DumpSceneFile = 1;
        }

        if ( nglSyncDebug().DumpTextures ) {
            nglDebug().DumpTextures = 1;
        }

        nglSyncDebug() = nglDebug();
        nglCurScene() = nullptr;
        nglListBeginScene(static_cast<nglSceneParamType>(0));
        if ( nglSyncDebug().DumpFrameLog ) {
            printf("LOG: ============================= Frame log start ===========================\n");
        }

        nglSceneDumpStart();

        auto *v3 = (int16_t *)&nglScratchBuffer().field_0[0].pad;
        auto v0 = nglScratchBuffer().field_44;
        nglScratchBuffer().field_4C = nglScratchBuffer().field_0[v0];

        nglScratchBuffer().field_48 = (IDirect3DIndexBuffer9 *)nglScratchBuffer().field_18[v0];
        if ( nglScratchBuffer().field_4C.m_vertexBuffer != nullptr ) {
            nglScratchBuffer().field_4C.m_vertexBuffer->Lock(0, 0, (void **)&v3, D3DLOCK_DISCARD);
            nglScratchBuffer().field_4C.pad = (int)v3;
        }

        auto *v2 = nglScratchBuffer().field_48;
        if ( v2 != nullptr ) {
            v2->Lock(0, 0, (void **)&v3, 0);
            nglScratchBuffer().field_20 = v3;
        }
    } else {
        CDECL_CALL(0x0076E050);
    }
}






inline void nglFlip(bool a1)
{
    printf("nglFlip\n");

    ++nglVBlankCount();

    g_Direct3DDevice()->BeginScene();
    sub_781EA0(nullptr);
    g_Direct3DDevice()->EndScene();

    if ( !byte_971F9C()
            && g_Direct3DDevice()->Present(0, 0, 0, 0) == D3DERR_DEVICELOST )
    {
        Sleep(100u);
        if ( g_Direct3DDevice()->TestCooperativeLevel() == D3DERR_DEVICENOTRESET ) {
            Reset3DDevice();
        }
    }

    PumpMessages();
    ++nglFrame();
    if ( a1 ) {
        nglQueueFlip();
    }
}

inline void nglRenderPerfBar()
{
    CDECL_CALL(0x0077ECF0);
}




inline void nglRenderDebug()
{
    if ( nglSyncDebug().ShowPerfInfo ) {
        nglRenderPerfInfo();
    }

    if ( nglSyncDebug().ShowPerfBar ) {
        nglRenderPerfBar();
    }
}

inline void nglSceneDumpEnd()
{
    if ( nglSyncDebug().DumpSceneFile )
    {
        nglHostPrintf(h_sceneDump(), "\n");
        nglHostPrintf(h_sceneDump(), "ENDSCENE\n");
        CloseHandle(h_sceneDump());
    }
}


inline void sub_77E0A0(float *a1, float *a2, int a3, int a4)
{
    CDECL_CALL(0x0077E0A0, a1, a2, a3, a4);
}

inline Var<nglRenderNode *> nglPrevNode {0x00971F18};

namespace nglRenderList {

template<typename T>
void nglTransCompare(T *a1, int count, int a3);

template<>
inline void nglTransCompare<nglRenderNode>(nglRenderNode *node, int count, int a3)
{
    TRACE("nglTransCompare<nglRenderNode>");

    if constexpr (1) {
        auto v3 = node;
        auto v4 = (float *)((unsigned int)(nglListWorkPos() + 15) & 0xFFFFFFF0);
        auto v5 = count;
        auto v6 = (uint8_t *)&v4[2 * count];
        nglListWorkPos() = int(v6);
        struct {
            nglRenderNode *field_0;
            nglTexture *field_4;
        } *i = CAST(i, v4);
        for ( ; v3 != nullptr; ++i )
        {
            i->field_0 = v3;
            i->field_4 = v3->field_8;
            v3 = v3->field_4;
        }

        sub_77E0A0(v4, &v4[2 * count], (8 * count) >> 3, a3);
        auto **v8 = (nglRenderNode **)(v6 - 8);
        nglRenderNode *v9 = nullptr;
        if ( count != 0 ) {
            do
            {
                (*v8)->field_4 = v9;
                v9 = *v8;
                v8 -= 2;
                --v5;
            }
            while ( v5 );
        }

        for ( ; v9 != nullptr; v9 = v9->field_4 )
        {
            v9->Render();
            nglPrevNode() = v9;
        }
    } else {
        CDECL_CALL(0x0077E220, node, count, a3);
    }
}

}

inline void nglListSend(bool Flip)
{
    TRACE("nglListSend");

    if constexpr (1) {
        if ( EnableShader() ) {
            float v10[4] {0, 0, 1, 1};
            g_Direct3DDevice()->SetVertexShaderConstantF(90, v10, 1);
        }

        if ( nglSyncDebug().DumpSceneFile ) {
            nglSceneDumpEnd();
        }

        if ( nglSyncDebug().DumpFrameLog ) {
            nglDebug().DumpFrameLog = 0;
        }

        if ( nglSyncDebug().DumpSceneFile ) {
            nglDebug().DumpSceneFile = 0;
        }

        if ( nglSyncDebug().DumpTextures ) {
            nglDebug().DumpTextures = 0;
        }

        nglRenderDebug();

        sub_76DE60();
#if 0
        if (nglCurScene() != nglRootScene()) {
            error("nglListSend called while one or more scenes were still active (need to call nglListEndScene).\n");
        }
#endif

        nglPerfInfo().field_28 = query_perf_counter();

        auto v3 = []() {
            auto perf_counter = query_perf_counter();
            LARGE_INTEGER v3 = bit_cast<LARGE_INTEGER>(*(uint64_t *)&perf_counter - nglPerfInfo().field_38.QuadPart);
            return v3;
        }();

        nglPerfInfo().field_38 = v3;
        nglPerfInfo().field_74 = v3.QuadPart / PCFreq();
        nglPerfInfo().field_40 = query_perf_counter();
        nglScratchBuffer().field_44 ^= 1u;
        nglScratchBuffer().field_28 = 0;
        nglScratchBuffer().field_2C = 0;
        nglScratchBuffer().field_24 = 0;
        nglScratchBuffer().field_30 = 0;

        nglScratchBuffer().field_4C.m_vertexBuffer->Unlock();
        nglScratchBuffer().field_48->Unlock();

        nglCurScene() = nglRootScene();

        g_Direct3DDevice()->BeginScene();
        nglVif1RenderScene();
        g_Direct3DDevice()->EndScene();

        sub_781A30();

        sub_76DE80();

        auto v5 = 1.0 / PCFreq();
        printf("PCFreq = %f\n", PCFreq());
        nglPerfInfo().field_40.QuadPart = query_perf_counter().QuadPart - nglPerfInfo().field_40.QuadPart;
        nglPerfInfo().field_70 = nglPerfInfo().field_40.QuadPart * v5;
        auto v6 = dword_975308();

        nglPerfInfo().field_18 = nglPerfInfo().m_counterQuads.QuadPart * v5;

        nglPerfInfo().field_1C = nglPerfInfo().field_50.QuadPart * v5;

        if ( dword_975314() == dword_975308() ) {
            v6 = dword_97530C();
        }

        dword_975314() = v6;
        nglScratchMeshPos() = v6;

        //dword_972AB4 = 0;
        //dword_972ABC = 0;

        g_Direct3DDevice()->SetStreamSource(0, nullptr, 0, 0);
        g_Direct3DDevice()->SetVertexShader(nullptr);
        g_Direct3DDevice()->SetPixelShader(nullptr);

#if 0
        if ( dword_971F24() != nullptr ) {
            dword_971F24()(dword_971F28());
        }
#endif

        float v8 = []() -> double {
            return query_perf_counter().QuadPart - nglPerfInfo().field_20.QuadPart;
        }();

        nglPerfInfo().m_cpu_time = v8 / PCFreq();

#if 0
        if ( dword_971F1C() != nullptr )
            dword_971F1C()(dword_971F20());
#endif

        if ( Flip ) {
            nglFlip(0);
        }

        nglPerfInfo().field_20 = query_perf_counter();
        nglPerfInfo().m_render_time = g_renderTime();
        //sp_log("m_render_time = %f", nglPerfInfo().m_render_time);

        double v9 = nglFlipCycle() - nglLastFlipCycle();
        if ( (nglFlipCycle() - nglLastFlipCycle()) < 0 ) {
            v9 += flt_86F860();
        }

        printf("nglFlipCycle = %u, nglLastFlipCycle = %u\n", nglFlipCycle(), nglLastFlipCycle());

        nglPerfInfo().field_6C = v9 / PCFreq();
        nglPerfInfo().field_5C += nglPerfInfo().field_6C;
        nglPerfInfo().m_fps = 1000.f / nglPerfInfo().field_6C;

        printf("v9 = %f, PCFreq = %f\n", v9, PCFreq());
        printf("nglPerfInfo.m_fps == %f\n", nglPerfInfo().m_fps);

        nglPerfInfo().field_60 = nglPerfInfo().field_5C * 0.001f;
        if ( nglDebug().ScreenShot ) {
            nglScreenShot(nullptr);
            nglDebug().ScreenShot = 0;
        }

        nglSyncPerfInfo() = nglPerfInfo();

        nglPerfInfo().m_num_nodes = 0;
        nglPerfInfo().field_18 = 0.0;
        nglPerfInfo().field_1C = 0.0;
        nglPerfInfo().m_num_verts = 0;
        nglPerfInfo().m_num_polys = 0;
        nglPerfInfo().m_counterQuads.QuadPart = 0;
        nglPerfInfo().field_50.QuadPart = 0;

#if 0
        if ( dword_971F2C() ) {
            dword_971F2C()(dword_971F30());
        }
#endif

        nglCurScene() = nullptr;
    } else {
        CDECL_CALL(0x0076EA10, Flip);
    }
}

inline double sub_77E820(Float a1)
{
    double (__cdecl *func)(Float) = bit_cast<decltype(func)>(0x0077E820);
    return func(a1);
}

inline double sub_77E940(Float a1)
{
    double (__cdecl *func)(Float) = bit_cast<decltype(func)>(0x0077E940);
    return func(a1);
}

inline double sub_77EA00(Float a1)
{
    double (__cdecl *func)(Float) = bit_cast<decltype(func)>(0x0077EA00);
    return func(a1);
}

inline void sub_779570(nglFont *a1, void *a2, Float a3, Float a4, Float a5, Float a6, int a7, char *a8, DWORD *a9)
{
    CDECL_CALL(0x00779570, a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

struct RenderStateCacheStruct
{
    void setCullingMode(D3DCULL a2)
    {
        void (__fastcall *func)(void *, void *, D3DCULL) = bit_cast<decltype(func)>(0x00401DD0);
        func(this, nullptr, a2);
    }

    void setBlending(int blend_mode, int a3, int ref_value)
    {
        void (__fastcall *func)(void *, void *, int, int, int) = bit_cast<decltype(func)>(0x00774A90);
        func(this, nullptr, blend_mode, a3, ref_value);
    }

    void SetLighting(uint8_t a2)
    {
        void (__fastcall *func)(void *, void *, uint8_t) = bit_cast<decltype(func)>(0x0077E2B0);
        func(this, nullptr, a2);
    }

    void setFogEnable(bool a2)
    {
        void (__fastcall *func)(void *, void *, bool) = bit_cast<decltype(func)>(0x00401D40);
        func(this, nullptr, a2);
    }
};

inline Var<RenderStateCacheStruct> g_renderStateCache {0x009739A0};

struct VShader
{};

inline Var<VShader> stru_975780 {0x00975780};

inline Var<IDirect3DPixelShader9 *> dword_9757A0 {0x009757A0};

inline Var<IDirect3DVertexDeclaration9 *[1]> dword_9738E0 {0x009738E0};

inline void SetPixelShader(IDirect3DPixelShader9 **a1)
{
    CDECL_CALL(0x00772250, a1);
}


inline void SetVertexDeclarationAndShader(VShader *a1)
{
    CDECL_CALL(0x00772270, a1);
}

inline void SetSamplerState(DWORD stage, D3DSAMPLERSTATETYPE type, DWORD value)
{
    CDECL_CALL(0x0076DC30, stage, type, value);
}

inline void SetTextureStageState(DWORD a1, D3DTEXTURESTAGESTATETYPE a2, DWORD a3)
{
    CDECL_CALL(0x0076DC70, a1, a2, a3);
}

inline void nglDxSetTexture(unsigned int a1, nglTexture *Tex, uint8_t a3, int a4)
{
    CDECL_CALL(0x007754B0, a1, Tex, a3, a4);
}


struct nglStringNode
{
    int m_vtbl;
    nglRenderNode *field_4;
    float field_8;
    char *field_C;
    nglFont *field_10;
    float field_14;
    float field_18;
    float field_1C;
    float field_20;
    float field_24;
    int field_28;

    void Render()
    {
        TRACE("nglStringNode::Render");

        if (nglSyncDebug().DisableFonts) {
            return;
        }

        auto perf_counter = query_perf_counter();

        if constexpr (0)
        {
            if ( this->field_C != nullptr ) {
                nglFont *v2 = this->field_10;
                auto v3 = v2->field_40;
                if ( v2->field_24 != nullptr ) {
                    g_renderStateCache().setCullingMode(D3DCULL_NONE);
                    g_renderStateCache().setBlending(this->field_10->field_44, this->field_10->field_48, 128);

                    if ( (v3 & 0x40) != 0 )
                        SetSamplerState(0, D3DSAMP_ADDRESSU, 3u);
                    else
                        SetSamplerState(0, D3DSAMP_ADDRESSU, 1u);

                    if ( (v3 & 0x80u) == 0 )
                        SetSamplerState(0, D3DSAMP_ADDRESSV, 1u);
                    else
                        SetSamplerState(0, D3DSAMP_ADDRESSV, 3u);

                    nglDxSetTexture(0, this->field_10->field_24, v3, 3);

                    if ( EnableShader() ) {
                        SetVertexDeclarationAndShader(&stru_975780());
                    } else {
                        g_Direct3DDevice()->SetVertexDeclaration(dword_9738E0()[28]);
                        g_Direct3DDevice()->SetTransform(
                            (D3DTRANSFORMSTATETYPE)256,
                            (const D3DMATRIX *)nglCurScene()->field_24C);
                    }

                    if ( EnableShader() ) {
                        SetPixelShader(&dword_9757A0());
                    } else {
                        SetTextureStageState(0, D3DTSS_COLOROP, 4u);
                        SetTextureStageState(0, D3DTSS_COLORARG1, 2u);
                        SetTextureStageState(0, D3DTSS_COLORARG2, 0);
                        SetTextureStageState(0, D3DTSS_ALPHAOP, 4u);
                        SetTextureStageState(0, D3DTSS_ALPHAARG1, 2u);
                        SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
                        SetTextureStageState(1u, D3DTSS_COLOROP, 1u);
                        SetTextureStageState(1u, D3DTSS_ALPHAOP, 1u);
                        g_renderStateCache().SetLighting(0);
                    }

                    g_renderStateCache().setFogEnable(false);
                    auto v4 = this->field_18;
                    auto v20 = this->field_1C;
                    auto v25 = this->field_14;
                    auto a4 = v4;
                    [[maybe_unused]] auto v27 = sub_77E820(v20);

                    struct temp_struct
                    {
                        temp_struct *field_0;
                        int field_4;
                        int field_8;
                        int field_C;
                        float field_10[4];
                        float field_20;
                    };

                    VALIDATE_SIZE(temp_struct, 0x24);

                    static Var<temp_struct> dword_975690 {0x00975690};
                    DWORD a9;
                    sub_779570(
                        this->field_10,
                        &dword_975690(),
                        v25,
                        a4,
                        this->field_20,
                        this->field_24,
                        this->field_28,
                        this->field_C,
                        &a9);

                    for ( auto *i = dword_975690().field_0;
                          i != nullptr;
                          i = i->field_0
                          )
                    {
                        auto v6 = i->field_10[2];
                        auto v7 = i->field_10[3];
                        auto v8 = i->field_20;
                        auto v25 = i->field_10[0];
                        auto v9 = i->field_8;
                        auto a7 = v6;
                        auto v10 = i->field_10[1];
                        auto a8 = v7;
                        auto a4 = v10;
                        if ( v9 != 0 ) {
                            do {
                                a9 = v9 - 1;
                                auto v12 = this->field_10;

                                auto v11 = *(BYTE *) i->field_4;

                                float v21[2];
                                float v23[2];
                                float a5[2];
                                float v31[2];
                                v12->sub_77E2F0(v11, v21, v23, a5, v31, a7, a8);
                                v21[0] = v21[0] + v25;
                                v23[0] = v23[0] + v21[0];
                                v21[1] = v21[1] + a4;
                                v23[1] = v23[1] + v21[1];
                                v31[0] = v31[0] + a5[0];
                                v31[1] = v31[1] + a5[1];
                                v21[0] = sub_77E940(v21[0]);
                                v21[1] = sub_77EA00(v21[1]);
                                v23[0] = sub_77E940(v23[0]);
                                v23[1] = sub_77EA00(v23[1]);

                                float v35[24];
                                v35[13] = v23[1];
                                v35[0] = v21[0];
                                v35[1] = v21[1];
                                v35[2] = v25;
                                v35[4] = a5[0];
                                v35[5] = a5[1];
                                v35[6] = v23[0];
                                v35[7] = v21[1];
                                v35[8] = v25;
                                v35[3] = v8;
                                v35[9] = v8;
                                v35[10] = v31[0];
                                v35[11] = a5[1];
                                v35[12] = v21[0];
                                v35[19] = v23[1];
                                v35[14] = v25;
                                v35[18] = v23[0];
                                v35[23] = v31[1];
                                v35[17] = v31[1];
                                v35[16] = a5[0];
                                v35[22] = v31[0];
                                v35[15] = v8;
                                v35[20] = v25;
                                v35[21] = v8;

                                g_Direct3DDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v35, 24);
                                auto v13 = this->field_10;
                                auto FirstGlyph = v13->Header.FirstGlyph;
                                auto v15 = v11;
                                if ( v11 < FirstGlyph || v11 >= FirstGlyph + v13->Header.NumGlyphs ) {
                                    v15 = 32;
                                }

                                auto v16 = v15 - FirstGlyph;
                                auto &GlyphInfo = v13->GlyphInfo;
                                v16 *= 28;
                                auto v18 = (double)*(int *)&GlyphInfo->field_18[v16];
                                if ( *(int *)(&GlyphInfo->field_18[v16]) < 0 ) {
                                    v18 += flt_86F860();
                                }

                                auto v19 = v18 * i->field_10[2];
                                ++i->field_4;
                                v9 = a9;
                                v25 = v19 + v25;
                            }
                            while ( a9 );
                        }
                        a9 = v9 - 1;
                    }

                    dword_975690().field_0 = nullptr;
                    if ( g_distance_clipping_enabled() && !sub_581C30() ) {
                        g_renderStateCache().setFogEnable(true);
                    }
                }
            }
        } else {
            void (__fastcall *func)(void *) = bit_cast<decltype(func)>(0x0077E3E0);
            func(this);
        }

        nglPerfInfo().field_50.QuadPart += query_perf_counter().QuadPart - perf_counter.QuadPart;
    }

};

VALIDATE_SIZE(nglStringNode, 0x2C);

inline bool sub_581C30()
{
    return CDECL_CALL(0x00581C30);
}

inline void ngl_patch()
{
    SET_JUMP(0x0076B8C0, nglSetWorldToViewMatrix);

    //SET_JUMP(0x00770360, nglListAddMesh);

    {
        auto func = &nglRenderList::nglTransCompare<nglRenderNode>;
        //REDIRECT(0x0077D1D4, func);
    }
    
    if constexpr (1)
    {
        REDIRECT(0x004D72B3, FastListAddMesh);
        REDIRECT(0x004E35A6, FastListAddMesh);
        REDIRECT(0x00537465, FastListAddMesh);
    }

    {
        auto address = func_address(&nglStringNode::Render);
        set_vfunc(0x0088EBB4, address);
    }

    {
        auto address = func_address(&nglQuadNode::Render);
        SET_JUMP(0x00783670, address);
    }

    {
        REDIRECT(0x005AD264, nglSetFrameLock);
        REDIRECT(0x0076E966, nglSetFrameLock);
    }

    REDIRECT(0x005AD218, nglInit);

    SET_JUMP(0x0076E050, nglListInit);

    SET_JUMP(0x0076EA10, nglListSend);

    REDIRECT(0x0076EA59, nglRenderPerfInfo);
}
#pragma once

#include "func_wrapper.h"


#include "utility.h"
#include "wds.h"

#include <cstdarg>
#include <cstdio>



struct mString
{
    struct fmtd {
        int value;
    };

    int field_0;
    int m_size;
    const char *guts;
    void* field_C;

    mString()
    {
        void (__fastcall *func)(void *) = bit_cast<decltype(func)>(0x00420F00);
        func(this);
    }

    mString(const mString &a2)
    {
        void (__fastcall *func)(void *, int, const mString *) = bit_cast<decltype(func)>(0x00421220);
        func(this, 0, &a2);
    }

    mString(float a2)
    {
        void(__fastcall * func)(void*, int, float) = bit_cast<decltype(func)>(0x00421220);
        func(this, 0, a2);
    }

    mString(int , const char *Format, ...)
    {
        char Dest[1024];
        va_list Args;

        va_start(Args, Format);
        vsprintf(Dest, Format, Args);
        CDECL_CALL(0x00420F60, this, 0, Dest);
    }

    mString(const char *str)
    {
        typedef int (__fastcall* mString_constructor_ptr)(mString* , void* edx, const char* str);
        mString_constructor_ptr mString_constructor = (mString_constructor_ptr) 0x00421100;
        mString_constructor(this, 0, str);
    }

    ~mString()
    {
        typedef int (__fastcall* mString_finalize_ptr)(mString* , void* edx, int zero);
        mString_finalize_ptr mString_finalize = (mString_finalize_ptr) 0x004209C0;

        mString_finalize(this, 0, 0);
    }

    mString &operator=(const mString &a2)
    {
        if (this != &a2)
        {
            this->update_guts(a2.c_str(), a2.m_size);
        }

        return (*this);
    }

    mString &operator=(const char *a2)
    {
        this->update_guts(a2, -1);
        return (*this);
    }

    int length() const {
        return this->m_size;
    }

    void update_guts(const char* a2, signed int a3) {

        typedef void(__fastcall* mString_update_guts_ptr)(void *, void* edx, const char* a2, signed int a3);
        mString_update_guts_ptr mString_update_guts = (mString_update_guts_ptr) 0x41F9D0;

        //printf("mString:%s\n", a2);


        return mString_update_guts(this, 0, a2, a3);
    }

    const char *c_str() const
    {
        return guts;
    }

    void append(const char *from_string, size_t Count)
    {
        void (__fastcall *func)(void *, int, const char *, unsigned int) = bit_cast<decltype(func)>(0x0041FAB0);
        func(this, 0, from_string, Count);
    }

    mString &operator+=(const mString &a1)
    {
        this->append(a1.c_str(), -1);
        return (*this); 
    }

    friend mString operator+(const char *a2, const mString &a3)
    {
        mString result;
        CDECL_CALL(0x00421410, &result, a2, &a3);
        return result;
    }

    friend mString operator+(const mString &a2, const mString &a3)
    {
        mString result;
        CDECL_CALL(0x00421330, &result, &a2, &a3);
        return result;
    }

        double atof(const char* i) const;

    double to_float() const
    {
        return atof(this->guts);
    }

    mString*  sub_67086F(void*, const char* a2, char* a1)
    {

    }
    inline int size() const
    {
        return m_size;
    }

    inline void set_size(size_t size)
    {
        this->m_size = size;
    }
    static inline Var<char*> null { 0x0091E7C0 };


    static inline Var<int> mString_count { 0x00957CEC };


    mString([[maybe_unused]] mString::fmtd fmt, const char* Format, ...)
    {
        char Dest[1024];
        va_list Args;

        va_start(Args, Format);
        this->set_size(0);
        this->field_0 = 0;
        vsprintf(Dest, Format, Args);

        this->guts = (char*)mString::null();
        ++mString_count();

        this->field_C = nullptr;
        this->update_guts(Dest, -1);
    }

};


#pragma once

#include "func_wrapper.h"


#include <string>

inline void *arch_memalign(size_t Alignment, size_t Size) {
#if 0
    if constexpr (0) {
        if (mem_first_memalign()) {
            mem_on_first_allocation();

            mem_first_memalign() = false;
        }

        auto *mem = arch_memalign_internal(Alignment, Size);
        if (mem == nullptr) {
            debug_print_va("tried to allocate %d bytes", Size);
            mem_print_stats("mem_memalign failed");
        }

        return mem;

    } else
#endif
    {
        return (void *) CDECL_CALL(0x005357B0, Alignment, Size);
    }
}
inline void* mem_alloc(size_t Size)
{
    return (void*)CDECL_CALL(0x0043A100, Size);
}


inline void *arch_malloc(size_t Size) {
    return (void *) CDECL_CALL(0x00535780, Size);
}

inline void *mem_freealign(void *Memory) {
    return (void *) CDECL_CALL(0x0058EC80, Memory);
}

inline void mem_print_stats(const char *a1) {
    printf("mem_print_stats: %s\n", a1);
    printf("peak: %10lu   curr: %10lu   free: %10lu\n", 0ul, 0ul, 0ul);
}

inline int mem_set_checkpoint()
{
    return 0;
}



inline void mem_check_leaks_since_checkpoint(int , unsigned int )
{
}



#include "input_mgr.h"

#include "common.h"
#include "input.h"
#include "keyboard_device.h"

#include "trace.h"
#include "utility.h"

#include "pc_joypad_device.h"
#include <cassert>
#include <xinput.h>

VALIDATE_SIZE(input_mgr, 0x60);

bool IS_JOYSTICK_DEVICE(int id) {
    return id >= 0xF4240 && id <= 0xF4247;
}

bool IS_KEYBOARD_DEVICE(int id) {
    return id == 0x1E8480;
}

bool IS_MOUSE_DEVICE(device_id_t id) {
    return id == 0x2DC6C0;
}

void input_mgr::map_control(int a2, device_id_t a3, int axis)
{
    void (__fastcall *func)(void *, void *, int, device_id_t, int) = bit_cast<decltype(func)>(0x005D8660);
    func(this, nullptr, a2, a3, axis);
}

void input_mgr::scan_devices()
{
    TRACE("input_mgr::scan_devices");

    if constexpr (1)
    {
        if (Input::instance() == nullptr || Input::instance()->m_din == nullptr) {
            pc_joypad_device::instance->clear();
        } else {
            pc_joypad_device::instance->initialize(1);
            this->insert_device(pc_joypad_device::instance);
        }
        if (Input::instance() == nullptr || Input::instance()->m_din == nullptr) {
            keyboard_device::instance->clear();
        } else {
            keyboard_device::instance->initialize(0);
            this->insert_device(keyboard_device::instance);
        }

    }

    if constexpr (0)
    {}
    else
    {
        void (__fastcall *func)(void *) = CAST(func, 0x00599090);
        func(this);
    }

}

void input_mgr::insert_device(input_device *a2)
{
    TRACE("input_mgr::insert_device");

    printf("0x%X\n", a2->get_id());

    if constexpr(0)
    {
        auto *v2 = a2;
        auto id = a2->get_id();

        input_device ** (__fastcall *find)(void *, void *edx, const device_id_t *) = CAST(find, 0x005E8400);
        auto found_device = find(&this->field_8, nullptr, &id);
        *found_device = v2;

        if ( !IS_JOYSTICK_DEVICE(v2->get_id()) )
        {
            if ( !IS_KEYBOARD_DEVICE(v2->get_id()) )
            {
                if ( IS_MOUSE_DEVICE(v2->get_id()) ) {
                    *((DWORD *)&this[0xFFFE027F] + v2->get_id() - 0x14) = (DWORD)v2;
                }
            }
            else
            {
                *((DWORD *)&this[0xFFFEAC55] + v2->get_id() - 0xF) = (DWORD)v2;
            }
        }
        else
        {
            *((DWORD *)&this[0xFFFF562B] + v2->get_id() - 0x11) = (DWORD)v2;
        }
    }
    else
    {
        void (__fastcall *func)(void *, void *edx, input_device *) = CAST(func, 0x005DC140);
        func(this, nullptr, a2);
    }
}

void input_mgr_patch()
{
    {
        auto address = func_address(&input_mgr::scan_devices);
        REDIRECT(0x0055D696, address);
    }

    if constexpr (0)
    {
        auto address = func_address(&input_mgr::map_control);
        REDIRECT(0x00605C41, address);
    }
}
#pragma once

#include <vector>

struct tokenizer {
    char *field_0;
    int field_4;
    bool field_8;
    char field_9;
    char field_A;
    char field_B;

    std::vector<char *> field_C;

    decltype(field_C)::iterator field_1C;

    tokenizer(bool a2);

    ~tokenizer();

    char *get_token();

    void setup_current_iterator();

    int parse(char *a2, const char *a3);

    void clear();
};

extern void tokenizer_unit_test();
#pragma once

#include "device_id.h"
#include "variable.h"

struct input_device;

struct input_mgr
{
    int field_0;
    struct {

        void disable_vibration()
        {
            void (__fastcall *func)(void *) = (decltype(func)) 0x005C5440;
            func(this);
        }

        void enable_vibration()
        {
            void (__fastcall *func)(void *) = (decltype(func)) 0x005C5430;
            func(this);
        }

    } *rumble_ptr;
    int field_8[10];
    int field_20;
    void *field_30[1];
    int field_34;
    int field_38;
    int field_3C;
    int field_40;
    int field_44;
    int field_48;
    int field_4C;
    void *keyboard_devices[1];
    void *mouse_devices[1];
    device_id_t field_58;

    void map_control(int a2, device_id_t a3, int axis);

    void scan_devices();

    void insert_device(input_device *a2);

    static inline Var<input_mgr *> instance {0x009685DC};
};

extern void input_mgr_patch();
#include "game.h"

#include "common.h"
#include "debug_menu.h"
#include "message_board.h"

VALIDATE_OFFSET(game, gamefile, 0xC0);
VALIDATE_OFFSET(game, field_172, 0x172);
VALIDATE_OFFSET(game, field_280, 0x280);

VALIDATE_SIZE(game::level_load_stuff, 0x38);

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

void game::message_board_init()
{
    this->mb = new message_board {};
}

void game::handle_frame_locking(float* a1)
{
    auto frame_lock = os_developer_options::instance()->get_int(mString { "FRAME_LOCK" });
    if (frame_lock > 0) {
        *a1 = 1.0 / frame_lock;
    }
}

game_state game::get_cur_state() const
{
    assert(process_stack.size() != 0);

    auto& last = g_game_ptr()->process_stack.back();

    return static_cast<game_state>(last.field_4[last.index]);
}

bool game::is_physics_enabled() const
{
    return this->flag.physics_enabled;
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
}
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

struct game_process
{
    const char *field_0;
    int *field_4;
    int index;
    int num_states;
    int field_10;
    bool field_14;
};

inline Var<game_process> lores_game_process{0x00922074};

struct game_settings;
struct message_board;
struct level_descriptor_t;

enum class game_state {
    LEGAL = 1,
    WAIT_LINK = 4,
    LOAD_LEVEL = 5,
    RUNNING = 6,
    PAUSED = 7,
};

    struct game {
    char field_0[0x5C];
    entity* field_5C;
    entity* current_game_camera;
    void* field_64;
    message_board* mb;
    struct {
        int field_0;
        game_process* m_first;
        game_process* m_last;
        game_process* m_end;

        auto& back()
        {
            return *(m_last - 1);
        }
    } process_stack;

    int field_7C[17];
    game_settings* gamefile;
    int field_C4[41];

    struct flag_t {
        bool level_is_loaded;
        bool single_step;
        bool physics_enabled;
        bool field_3;
        bool game_paused;
    } flag;
    bool field_16D;
    bool field_16E;
    bool m_hero_start_enabled;
    bool field_170;
    bool field_171;
    bool field_172;
    char empty5[253];
    int field_270;
    int field_274;
    float field_278;
    int field_27C;
    int field_280;
    float field_284;
    int field_288;
    float field_28C;
    bool field_2B4;
    bool field_2B5;
    int field_2B8;
    int field_2BC;
    bool field_15C;
    bool field_15D;
    bool field_15E;
    bool field_15F;
    bool field_160;
    bool field_167;
    float field_290;
    world_dynamics_system* the_world;


        struct level_load_stuff {
        level_descriptor_t* descriptor;
        mString name_mission_table;
        mString field_14;
        vector3d field_24;
        int field_30;
        bool load_widgets_created;
        bool load_completed;
        bool field_3A;
        bool field_3B;

        void reset_level_load_data()
        {
            this->descriptor = nullptr;
            this->field_30 = 0;
            this->load_completed = false;
            this->field_3A = false;
            this->load_widgets_created = false;
        }
    };

    game_settings *get_game_settings() {
        assert(gamefile != nullptr);

        return this->gamefile;
    }

    void enable_user_camera(bool a2) {
        this->field_172 = a2;
    }

    bool is_user_camera_enabled() const {
        return this->field_172;
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

    void load_new_level(const mString& a1, int a2)
    {


        
    void(__fastcall * func)(void*, void*, const mString*,int) = CAST(func, 0x00514C70);
    return func(this, nullptr,&a1, a2);

    this->load_this_level();
    level_load_stuff*  reset_load_level_data();
    }

        void load_this_level()
    {

        void(__fastcall * func)(void*, void*) = CAST(func, 0x0055C6E0);
        return func(this, nullptr);
    }

    void begin_hires_screenshot(int a2, int a3)
    {
        void (__fastcall *func)(void *, void *, int, int) = (decltype(func)) 0x00548C10;
        func(this, nullptr, a2, a3);
    }


    void pause()
    {
        {
            void(__fastcall * func)(void*, void*) = (decltype(func))0x0054FBE0;
            func(this, nullptr);
        }
    }

    void unpause()
    {
        {
            void(__fastcall * func)(void*, void*) = (decltype(func))0x0053A880;
            func(this, nullptr);
        }
        }



    mString get_hero_info();

    mString get_camera_info();

    mString get_analyzer_info();

    void show_debug_info();

    void frame_advance_level(Float time_inc);

    void handle_frame_locking(float* a1);

        bool is_physics_enabled() const;

    game_state get_cur_state() const;
    };

inline Var<game *> g_game_ptr{0x009682E0};

extern void game_patch();

#pragma once

#include "debug_menu.h"
#include "memory.h"
#include "utility.h"
#include "variable.h"

#include <cassert>

static constexpr auto SLAB_HEADER_BEGIN_SENTRY = 0xE1E1E1E1;
static constexpr auto SLAB_HEADER_END_SENTRY = 0x1E1E1E1E;

static constexpr auto SLAB_FROM_STATIC = 0x10;

static constexpr auto SLAB_FROM_HEAP = 0x8;

static constexpr auto SLAB_FROM_AUX = 0x20;

static constexpr auto SLAB_ON_PARTIAL_LIST = 0x1;

static constexpr auto SLAB_ON_FULL_LIST = 0x2;

static constexpr auto SLAB_ON_FREE_LIST = 0x4;

static constexpr auto MAX_OBJECT_SIZE = 176;

namespace slab_allocator {
struct slab_list_t;

struct slab_t {
    struct iterator {
        slab_t *_ptr;


    };

    uint32_t begin_sentry;
    char *arena;
    uint16_t field_8;
    uint16_t m_size;
    int16_t total_object_count;
    int16_t alloc_object_count;
    void *field_10;

    struct {
            slab_t *_sl_next_element;
            slab_t *_sl_prev_element;
            slab_list_t *_sl_list_owner;
        } simple_list_vars;
    uint32_t end_sentry;

    slab_t(char *a2) {
        this->simple_list_vars = {};

        this->begin_sentry = SLAB_HEADER_BEGIN_SENTRY;
        this->arena = a2;
        this->field_8 = 0;
        this->m_size = 0;
        this->total_object_count = 0;
        this->alloc_object_count = 0;
        this->field_10 = nullptr;
        this->end_sentry = SLAB_HEADER_END_SENTRY;
    }

    bool contains(uint32_t a2) {
        uint32_t v2 = (uint32_t) this->arena;
        return (a2 >= v2 && a2 < v2 + 4096);
    }

    bool is_set(int a2) {
        assert(this->begin_sentry == SLAB_HEADER_BEGIN_SENTRY);
        assert(this->end_sentry == SLAB_HEADER_END_SENTRY);
        return (a2 & this->field_8) != 0;
    }



    void set(int16_t a2) {
        assert(this->begin_sentry == SLAB_HEADER_BEGIN_SENTRY);
        assert(this->end_sentry == SLAB_HEADER_END_SENTRY);

        this->field_8 |= a2;
    }

    void unset(uint16_t a2) {
        assert(this->begin_sentry == SLAB_HEADER_BEGIN_SENTRY);
        assert(this->end_sentry == SLAB_HEADER_END_SENTRY);

        this->field_8 &= ~a2;
    }



    void free_object(void *object) {
        assert(this->begin_sentry == SLAB_HEADER_BEGIN_SENTRY);
        assert(this->end_sentry == SLAB_HEADER_END_SENTRY);
        assert(this->contains((uint32_t) object));
        *(uint32_t *) object = bit_cast<uint32_t>(this->field_10);
        this->field_10 = object;

        --this->alloc_object_count;
        assert(alloc_object_count >= 0);
    }

    int get_total_object_count() const {
        return this->total_object_count;
    }

    int get_free_object_count() {
        return this->total_object_count - this->alloc_object_count;
    }

    int get_alloc_object_count() {
        return this->alloc_object_count;
    }



    void *get_obj() {
        assert(this->begin_sentry == SLAB_HEADER_BEGIN_SENTRY);
        assert(this->end_sentry == SLAB_HEADER_END_SENTRY);
        if (this->field_10 == nullptr) {
            return nullptr;
        }

        auto v2 = this->field_10;
        this->field_10 = *bit_cast<void **>(v2);
        ++this->alloc_object_count;

        assert(alloc_object_count <= total_object_count);

        return v2;
    }

    void sub_592D00(int16_t size) {
        assert(this->begin_sentry == SLAB_HEADER_BEGIN_SENTRY);
        assert(this->end_sentry == SLAB_HEADER_END_SENTRY);
        assert(alloc_object_count == 0);
        assert(size > 0);

        this->m_size = size;
        this->total_object_count = 4096 / this->m_size;
        auto *v2 = this->arena;

        this->field_10 = nullptr;
        for (int i = 0; i < this->total_object_count; ++i) {
            *(uint32_t *) v2 = bit_cast<uint32_t>(this->field_10);
            this->field_10 = v2;
            v2 += this->m_size;
        }
    }
};


    
struct slab_list_t {
    slab_t *_first_element;
    slab_t *_last_element;
    int _size;

    slab_list_t() {
        this->_first_element = nullptr;
        this->_last_element = nullptr;
        this->_size = 0;
    }

    ~slab_list_t() {
        this->_first_element = nullptr;
        this->_last_element = nullptr;
        this->_size = 0;
    }

    slab_t *front() {
        return this->_first_element;
    }

    auto begin() {
        slab_t::iterator iter{this->_first_element};

        return iter;
    }

    slab_t::iterator end() {
        slab_t::iterator iter{};

        return iter;
    }

    bool contains(slab_t *iter) {
        return iter && iter->simple_list_vars._sl_list_owner == this;
    }

    slab_t::iterator add_slab(slab_allocator::slab_t *tmp) {
        assert(tmp != nullptr);
        assert(tmp->simple_list_vars._sl_next_element == nullptr);
        assert(tmp->simple_list_vars._sl_prev_element == nullptr);
        assert(tmp->simple_list_vars._sl_list_owner == nullptr);

        tmp->simple_list_vars._sl_next_element = this->_first_element;
        tmp->simple_list_vars._sl_prev_element = nullptr;
        if (this->_first_element != nullptr) {
            this->_first_element->simple_list_vars._sl_prev_element = tmp;
        }

        this->_first_element = tmp;
        if (tmp->simple_list_vars._sl_next_element == nullptr) {
            this->_last_element = tmp;
        }

        tmp->simple_list_vars._sl_list_owner = this;
        ++this->_size;

        slab_t::iterator a2{tmp};
        return a2;
    }

    bool remove_slab(slab_t *slab) {
        if (slab == nullptr) {
            return false;
        }

        auto v2 = slab->simple_list_vars._sl_list_owner;
        if (v2 != this) {
            return false;
        }

        auto *v3 = slab->simple_list_vars._sl_prev_element;
        if (v3 != nullptr) {
            v3->simple_list_vars._sl_next_element = slab->simple_list_vars._sl_next_element;
        } else {
            v2->_first_element = slab->simple_list_vars._sl_next_element;
        }

        auto *v4 = slab->simple_list_vars._sl_next_element;
        auto *v5 = slab->simple_list_vars._sl_prev_element;
        if (v4 != nullptr) {
            v4->simple_list_vars._sl_prev_element = v5;
        } else {
            slab->simple_list_vars._sl_list_owner->_last_element = v5;
        }

        --slab->simple_list_vars._sl_list_owner->_size;

        slab->simple_list_vars = {};
        return true;
    }

    slab_t::iterator push_back(slab_t *tmp) {
        assert(tmp != nullptr);
        assert(tmp->simple_list_vars._sl_next_element == nullptr);
        assert(tmp->simple_list_vars._sl_prev_element == nullptr);
        assert(tmp->simple_list_vars._sl_list_owner == nullptr);

        if (this->_last_element != nullptr) {
            assert(_last_element->simple_list_vars._sl_next_element == nullptr);

            this->_last_element->simple_list_vars._sl_next_element = tmp;
            tmp->simple_list_vars._sl_prev_element = this->_last_element;
            tmp->simple_list_vars._sl_next_element = nullptr;
            this->_last_element = tmp;
            tmp->simple_list_vars._sl_list_owner = this;

            slab_t::iterator a2{tmp};
            return a2;
        }

        auto it = this->add_slab(tmp);

        return it;
    }
};

int allocated_object_count[44]{};
int free_object_count[44]{};

int partial_slab_count[44]{};
int full_slab_count[44]{};

int free_slab_count{};
int total_slab_count{};

Var<bool> initialized{0x00965F33};

Var<bool> g_dump_slab_info{0x00965F40};

Var<char *> static_slab_arena = {0x00965F34};
Var<slab_t *> static_slab_headers = {0x00965F38};

Var<slab_list_t *> slab_partial_list{0x00965984};

Var<slab_list_t *> slab_full_list{0x00965980};

Var<slab_list_t *> slab_free_list{0x0096597C};

struct slab_partial_list_t {
    int field_0;
    slab_list_t field_4[44];
};

void initialize() {
    assert(!initialized());

    if constexpr (1) {
        slab_list_t *v1 = nullptr;
        auto *v0 = (slab_partial_list_t *) operator new(0x214u);
        if (v0 != nullptr) {
            v1 = v0->field_4;
            v0->field_0 = 44;
            new (v0->field_4) slab_list_t[44];
        }

        slab_partial_list() = v1;

        slab_list_t *v3 = nullptr;
        auto *v2 = (slab_partial_list_t *) operator new(0x214u);
        if (v2) {
            v3 = v2->field_4;
            v2->field_0 = 44;
            new (v3) slab_list_t[44];
        }

        slab_full_list() = v3;

        auto *v4 = (slab_list_t *) operator new(12u);
        if (v4 != nullptr) {
            v4->_first_element = nullptr;
            v4->_last_element = nullptr;
            v4->_size = 0;
        } else {
            v4 = nullptr;
        }

        slab_free_list() = v4;
        static_slab_arena() = static_cast<char *>(arch_memalign(4096u, 4096 * 1024u));
        static_slab_headers() = static_cast<slab_t *>(arch_malloc(sizeof(slab_t) * 1024u));
        auto *arena = static_slab_arena();
        auto *headers = static_slab_headers();
        for (auto i = 0u; i < 1024u; ++i) {
            new (headers) slab_t {arena};
            headers->set(20);

            slab_free_list()->push_back(headers++);

            arena += 4096u;
        }

        initialized() = true;

    } else {
        CDECL_CALL(0x0059F5A0);
    }
}

slab_t *find_slab_for_object(void *obj) {
    return (slab_t *) CDECL_CALL(0x00592D50, obj);
}

slab_t *create_slab(int size)
{
    if constexpr (1) {
        assert(initialized());

        auto *slab = slab_free_list()->front();
        if (slab == nullptr) {
            goto LABEL_9;
        } else {
            auto *v2 = slab->simple_list_vars._sl_prev_element;
            auto v3 = slab->simple_list_vars._sl_next_element;
            if (v2 != nullptr) {
                v2->simple_list_vars._sl_next_element = v3;
            } else {
                slab->simple_list_vars._sl_list_owner->_first_element = v3;
            }

            auto *v4 = slab->simple_list_vars._sl_next_element;
            auto v5 = slab->simple_list_vars._sl_prev_element;
            if (v4 != nullptr) {
                v4->simple_list_vars._sl_prev_element = v5;
            } else {
                slab->simple_list_vars._sl_list_owner->_last_element = v5;
            }

            --slab->simple_list_vars._sl_list_owner->_size;

            slab->simple_list_vars = {};
        }

        if (slab != nullptr) {
            assert(slab->is_set(SLAB_ON_FREE_LIST));

            slab->unset(SLAB_ON_FREE_LIST);

        } else {
        LABEL_9:
            auto *v6 = static_cast<char *>(arch_memalign(4096u, 4132u));
            auto *mem = (v6 + 4096);

            if (mem != nullptr) {
                slab = new (mem) slab_t{v6};
            }

            slab->set(SLAB_FROM_HEAP);

            ++free_slab_count;
            ++total_slab_count;
        }

        int index = 0;
        if (size >= 4) {
            index = (size + 3) / 4 - 1;
        }

        slab->sub_592D00(4 * index + 4);

        slab->set(SLAB_ON_PARTIAL_LIST);

        slab_partial_list()[index].add_slab(slab);

        --free_slab_count;
        free_object_count[index] += slab->get_total_object_count();
        ++partial_slab_count[index];

        return slab;

    } else {
        return (slab_t *) CDECL_CALL(0x0059DE20, size);
    }
}

void *allocate(int size, slab_t **a2) {
    printf("slab_allocator::allocate\n");

    assert(size >= 0);
    assert(size <= MAX_OBJECT_SIZE);

    if (!initialized()) {
        initialize();
    }

    int index = 0;
    if (size >= 4) {
        index = (size + 3) / 4 - 1;
    }

    auto *slab = slab_partial_list()[index].front();
    if (slab == nullptr) {
        slab = create_slab(size);
    }

    assert(slab != nullptr);
    assert(slab->is_set(SLAB_ON_PARTIAL_LIST));

    auto *obj = slab->get_obj();
    assert(obj != nullptr);

    if (a2 != nullptr) {
        *a2 = slab;
    }

    ++allocated_object_count[index];
    --free_object_count[index];

    if (!slab->get_free_object_count()) {
        slab->unset(SLAB_ON_PARTIAL_LIST);

        assert(slab_partial_list()[index].front() == slab);

        auto *v8 = slab_partial_list()[index]._first_element;
        if (v8 != nullptr) {
            auto *v9 = v8->simple_list_vars._sl_prev_element;
            auto *v10 = v8->simple_list_vars._sl_next_element;
            if (v9 != nullptr) {
                v9->simple_list_vars._sl_next_element = v10;
            } else {
                v8->simple_list_vars._sl_list_owner->_first_element = v10;
            }

            auto *v11 = v8->simple_list_vars._sl_next_element;
            auto *v12 = v8->simple_list_vars._sl_prev_element;
            if (v11 != nullptr) {
                v11->simple_list_vars._sl_prev_element = v12;
            } else {
                v8->simple_list_vars._sl_list_owner->_last_element = v12;
            }

            --v8->simple_list_vars._sl_list_owner->_size;

            v8->simple_list_vars = {};
        }

        slab_full_list()[index].push_back(slab);

        slab->set(SLAB_ON_FULL_LIST);
        --partial_slab_count[index];
        ++full_slab_count[index];
    }

    return obj;
}

void swap(slab_allocator::slab_t::iterator &a, slab_allocator::slab_t::iterator &b) {
    assert(a._ptr->simple_list_vars._sl_list_owner == b._ptr->simple_list_vars._sl_list_owner);

    auto v2 = a._ptr->simple_list_vars._sl_list_owner;
    if (v2->_first_element == a._ptr) {
        v2->_first_element = b._ptr;
    } else {
        auto *v3 = b._ptr->simple_list_vars._sl_list_owner;
        if (v3->_first_element == b._ptr) {
            v3->_first_element = a._ptr;
        }
    }

    auto *v4 = a._ptr->simple_list_vars._sl_list_owner;
    if (v4->_last_element == a._ptr) {
        v4->_last_element = b._ptr;
    } else {
        auto *v5 = b._ptr->simple_list_vars._sl_list_owner;
        if (v5->_last_element == b._ptr) {
            v5->_last_element = a._ptr;
        }
    }

    auto *v6 = a._ptr->simple_list_vars._sl_prev_element;
    a._ptr->simple_list_vars._sl_prev_element = b._ptr->simple_list_vars._sl_prev_element;
    b._ptr->simple_list_vars._sl_prev_element = v6;
    auto *v7 = a._ptr->simple_list_vars._sl_prev_element;
    if (v7 != nullptr) {
        v7->simple_list_vars._sl_next_element = a._ptr;
    }

    auto *v8 = b._ptr->simple_list_vars._sl_prev_element;
    if (v8 != nullptr) {
        v8->simple_list_vars._sl_next_element = b._ptr;
    }

    auto *v9 = a._ptr->simple_list_vars._sl_next_element;
    a._ptr->simple_list_vars._sl_next_element = b._ptr->simple_list_vars._sl_next_element;
    b._ptr->simple_list_vars._sl_next_element = v9;
    auto *v10 = a._ptr->simple_list_vars._sl_next_element;
    if (v10 != nullptr) {
        v10->simple_list_vars._sl_prev_element = a._ptr;
    }

    auto *v11 = b._ptr->simple_list_vars._sl_next_element;
    if (v11 != nullptr) {
        v11->simple_list_vars._sl_prev_element = b._ptr;
    }

    std::swap(a._ptr, b._ptr);
}

void deallocate(void *a1, slab_t *slab) {
    printf("slab_allocator::deallocate\n");

    if constexpr (1) {
        assert(initialized());

        if (a1 != nullptr) {
            if (slab == nullptr) {
                slab = slab_allocator::find_slab_for_object(a1);
            }

            assert(slab != nullptr);

            int index = 0;
            auto size = slab->m_size;
            if (size >= 4) {
                index = (size + 3) / 4 - 1;
            }

            slab->free_object(a1);
            --allocated_object_count[index];
            ++free_object_count[index];

            if (slab->is_set(SLAB_ON_FULL_LIST)) {
                assert(slab->get_free_object_count() == 1);

                slab->unset(SLAB_ON_FULL_LIST);

                slab_full_list()[index].remove_slab(slab);

                slab_partial_list()[index].add_slab(slab);

                slab->set(SLAB_ON_PARTIAL_LIST);
                --full_slab_count[index];
                ++partial_slab_count[index];
            } else {
                assert(slab->is_set(SLAB_ON_PARTIAL_LIST));

                if (slab->get_alloc_object_count()) {
                    auto *v11 = slab;
                    auto *v12 = slab->simple_list_vars._sl_next_element;

                    slab_t::iterator iter{v11};

                    slab_t::iterator v13;
                    for (v13._ptr = v12; v13._ptr; v12 = v13._ptr) {
                        if (v12->total_object_count - v12->alloc_object_count >=
                            v11->total_object_count - v11->alloc_object_count) {
                            break;
                        }

                        swap(iter, v13);
                        v11 = iter._ptr;
                        if (iter._ptr == nullptr) {
                            break;
                        }

                        v13._ptr = iter._ptr->simple_list_vars._sl_next_element;
                    }

                } else {
                    slab->unset(SLAB_ON_PARTIAL_LIST);
                    slab_partial_list()[index].remove_slab(slab);

                    free_object_count[index] -= slab->get_total_object_count();
                    --partial_slab_count[index];

                    if (slab->is_set(SLAB_FROM_HEAP)) {
                        --total_slab_count;

                        auto *v10 = slab->arena;
                        slab = {};
                        mem_freealign(v10);
                    } else {
                        ++free_slab_count;

                        assert(slab->is_set(SLAB_FROM_STATIC | SLAB_FROM_AUX));

                        slab_free_list()->add_slab(slab);
                        slab->set(SLAB_ON_FREE_LIST);
                    }
                }
            }
        }
    } else {
        CDECL_CALL(0x0059DCA0, a1, slab);
    }
}



void create_slab_debug_menu(debug_menu *parent)
{
    auto *slabs_menu = create_menu("Slabs", debug_menu::sort_mode_t::undefined);
    parent->add_entry(slabs_menu);

    auto *entry = create_menu_entry(mString {"Total Slabs"});
    entry->set_p_ival(&total_slab_count);
    entry->set_value_initialized(true);
    slabs_menu->add_entry(entry);

    entry = create_menu_entry(mString {"Free Slabs"});
    entry->set_p_ival(&free_slab_count);
    entry->set_value_initialized(true);
    slabs_menu->add_entry(entry);

    auto *menu = create_menu("Full Slabs", debug_menu::sort_mode_t::undefined);
    slabs_menu->add_entry(menu);
    for ( auto i = 0; i < 44; ++i )
    {
        auto *entry = create_menu_entry(mString {0, "%3d byte", 4 * i + 4});
        entry->set_value_initialized(true);
        entry->set_p_ival(&full_slab_count[i]);
        menu->add_entry(entry);
    }

    menu = create_menu("Partial Slabs", debug_menu::sort_mode_t::undefined);
    slabs_menu->add_entry(menu);
    for ( auto j = 0; j < 44; ++j )
    {
        auto *entry = create_menu_entry(mString {0, "%3d byte", 4 * j + 4});
        entry->set_value_initialized(true);
        entry->set_p_ival(&partial_slab_count[j]);
        menu->add_entry(entry);
    }

    menu = create_menu("Allocated Objects", debug_menu::sort_mode_t::undefined);
    slabs_menu->add_entry(menu);
    for ( auto k = 0; k < 44; ++k )
    {
        auto *entry = create_menu_entry(mString {0, "%3d byte", 4 * k + 4});
        entry->set_value_initialized(true);
        entry->set_p_ival(&allocated_object_count[k]);
        menu->add_entry(entry);
    }

    menu = create_menu("Free Objects", debug_menu::sort_mode_t::undefined);
    slabs_menu->add_entry(menu);

    for ( auto m = 0; m < 44; ++m )
    {
        auto *entry = create_menu_entry(mString {0, "%3d byte", 4 * m + 4});
        entry->set_value_initialized(true);
        entry->set_p_ival(&free_object_count[m]);
        menu->add_entry(entry);
    }
}




} // namespace slab_allocator

void slab_allocator_patch() {
    printf("Patching slab_allocator\n");

    SET_JUMP(0x0059F750, slab_allocator::allocate);

    SET_JUMP(0x0059DCA0, slab_allocator::deallocate);

    SET_JUMP(0x0059F5A0, slab_allocator::initialize);

    SET_JUMP(0x0059DE20, slab_allocator::create_slab);
}

#pragma once

#ifdef _MSC_VER
#define FORWARDED_EXPORT_WITH_ORDINAL(exp_name, ordinal, target_name) __pragma (comment (linker, "/export:" #exp_name "=" #target_name ",@" #ordinal))
#endif
#ifdef __GNUC__
#define FORWARDED_EXPORT_WITH_ORDINAL(exp_name, ordinal, target_name) __asm__(".section .drectve\n\t.ascii \" -export:" #exp_name "= " #target_name " @" #ordinal "\"");
#endif




FORWARDED_EXPORT_WITH_ORDINAL(_BinkBufferBlit@12, 1, binkw32_._BinkBufferBlit@12)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkBufferCheckWinPos@12, 2, binkw32_._BinkBufferCheckWinPos@12)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkBufferClear@8, 3, binkw32_._BinkBufferClear@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkBufferClose@4, 4, binkw32_._BinkBufferClose@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkBufferGetDescription@4, 5, binkw32_._BinkBufferGetDescription@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkBufferGetError@0, 6, binkw32_._BinkBufferGetError@0)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkBufferLock@4, 7, binkw32_._BinkBufferLock@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkBufferOpen@16, 8, binkw32_._BinkBufferOpen@16)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkBufferSetDirectDraw@8, 9, binkw32_._BinkBufferSetDirectDraw@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkBufferSetHWND@8, 10, binkw32_._BinkBufferSetHWND@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkBufferSetOffset@12, 11, binkw32_._BinkBufferSetOffset@12)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkBufferSetResolution@12, 12, binkw32_._BinkBufferSetResolution@12)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkBufferSetScale@12, 13, binkw32_._BinkBufferSetScale@12)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkBufferUnlock@4, 14, binkw32_._BinkBufferUnlock@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkCheckCursor@20, 15, binkw32_._BinkCheckCursor@20)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkClose@4, 16, binkw32_._BinkClose@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkCloseTrack@4, 17, binkw32_._BinkCloseTrack@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkCopyToBuffer@28, 18, binkw32_._BinkCopyToBuffer@28)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkCopyToBufferRect@44, 19, binkw32_._BinkCopyToBufferRect@44)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkDDSurfaceType@4, 20, binkw32_._BinkDDSurfaceType@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkDX8SurfaceType@4, 21, binkw32_._BinkDX8SurfaceType@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkDX9SurfaceType@4, 22, binkw32_._BinkDX9SurfaceType@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkDoFrame@4, 23, binkw32_._BinkDoFrame@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkGetError@0, 24, binkw32_._BinkGetError@0)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkGetFrameBuffersInfo@8, 25, binkw32_._BinkGetFrameBuffersInfo@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkGetKeyFrame@12, 26, binkw32_._BinkGetKeyFrame@12)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkGetRealtime@12, 27, binkw32_._BinkGetRealtime@12)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkGetRects@8, 28, binkw32_._BinkGetRects@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkGetSummary@8, 29, binkw32_._BinkGetSummary@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkGetTrackData@8, 30, binkw32_._BinkGetTrackData@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkGetTrackID@8, 31, binkw32_._BinkGetTrackID@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkGetTrackMaxSize@8, 32, binkw32_._BinkGetTrackMaxSize@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkGetTrackType@8, 33, binkw32_._BinkGetTrackType@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkGoto@12, 34, binkw32_._BinkGoto@12)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkIsSoftwareCursor@8, 35, binkw32_._BinkIsSoftwareCursor@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkLogoAddress@0, 36, binkw32_._BinkLogoAddress@0)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkNextFrame@4, 37, binkw32_._BinkNextFrame@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkOpen@8, 38, binkw32_._BinkOpen@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkOpenDirectSound@4, 39, binkw32_._BinkOpenDirectSound@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkOpenMiles@4, 40, binkw32_._BinkOpenMiles@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkOpenTrack@8, 41, binkw32_._BinkOpenTrack@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkOpenWaveOut@4, 42, binkw32_._BinkOpenWaveOut@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkPause@8, 43, binkw32_._BinkPause@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkRegisterFrameBuffers@8, 44, binkw32_._BinkRegisterFrameBuffers@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkRestoreCursor@4, 45, binkw32_._BinkRestoreCursor@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkService@4, 46, binkw32_._BinkService@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkSetError@4, 47, binkw32_._BinkSetError@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkSetFrameRate@8, 48, binkw32_._BinkSetFrameRate@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkSetIO@4, 49, binkw32_._BinkSetIO@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkSetIOSize@4, 50, binkw32_._BinkSetIOSize@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkSetMemory@8, 51, binkw32_._BinkSetMemory@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkSetMixBinVolumes@20, 52, binkw32_._BinkSetMixBinVolumes@20)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkSetMixBins@16, 53, binkw32_._BinkSetMixBins@16)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkSetPan@12, 54, binkw32_._BinkSetPan@12)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkSetSimulate@4, 55, binkw32_._BinkSetSimulate@4)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkSetSoundOnOff@8, 56, binkw32_._BinkSetSoundOnOff@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkSetSoundSystem@8, 57, binkw32_._BinkSetSoundSystem@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkSetSoundTrack@8, 58, binkw32_._BinkSetSoundTrack@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkSetVideoOnOff@8, 59, binkw32_._BinkSetVideoOnOff@8)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkSetVolume@12, 60, binkw32_._BinkSetVolume@12)
FORWARDED_EXPORT_WITH_ORDINAL(_BinkWait@4, 61, binkw32_._BinkWait@4)#ifndef __DINPUT_INCLUDED__
#define __DINPUT_INCLUDED__

#ifndef DIJ_RINGZERO

#ifdef _WIN32
#define COM_NO_WINDOWS_H
#include <objbase.h>
#endif

#endif /* DIJ_RINGZERO */

#ifdef __cplusplus
extern "C" {
#endif

#define DIRECTINPUT_HEADER_VERSION  0x0800
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION         DIRECTINPUT_HEADER_VERSION
#pragma message(__FILE__ ": DIRECTINPUT_VERSION undefined. Defaulting to version 0x0800")
#endif

#ifndef DIJ_RINGZERO

/****************************************************************************
 *
 *      Class IDs
 *
 ****************************************************************************/

DEFINE_GUID(CLSID_DirectInput,       0x25E609E0,0xB259,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(CLSID_DirectInputDevice, 0x25E609E1,0xB259,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

DEFINE_GUID(CLSID_DirectInput8,      0x25E609E4,0xB259,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(CLSID_DirectInputDevice8,0x25E609E5,0xB259,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

/****************************************************************************
 *
 *      Interfaces
 *
 ****************************************************************************/

DEFINE_GUID(IID_IDirectInputA,     0x89521360,0xAA8A,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(IID_IDirectInputW,     0x89521361,0xAA8A,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(IID_IDirectInput2A,    0x5944E662,0xAA8A,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(IID_IDirectInput2W,    0x5944E663,0xAA8A,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(IID_IDirectInput7A,    0x9A4CB684,0x236D,0x11D3,0x8E,0x9D,0x00,0xC0,0x4F,0x68,0x44,0xAE);
DEFINE_GUID(IID_IDirectInput7W,    0x9A4CB685,0x236D,0x11D3,0x8E,0x9D,0x00,0xC0,0x4F,0x68,0x44,0xAE);
DEFINE_GUID(IID_IDirectInput8A,    0xBF798030,0x483A,0x4DA2,0xAA,0x99,0x5D,0x64,0xED,0x36,0x97,0x00);
DEFINE_GUID(IID_IDirectInput8W,    0xBF798031,0x483A,0x4DA2,0xAA,0x99,0x5D,0x64,0xED,0x36,0x97,0x00);
DEFINE_GUID(IID_IDirectInputDeviceA, 0x5944E680,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(IID_IDirectInputDeviceW, 0x5944E681,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(IID_IDirectInputDevice2A,0x5944E682,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(IID_IDirectInputDevice2W,0x5944E683,0xC92E,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(IID_IDirectInputDevice7A,0x57D7C6BC,0x2356,0x11D3,0x8E,0x9D,0x00,0xC0,0x4F,0x68,0x44,0xAE);
DEFINE_GUID(IID_IDirectInputDevice7W,0x57D7C6BD,0x2356,0x11D3,0x8E,0x9D,0x00,0xC0,0x4F,0x68,0x44,0xAE);
DEFINE_GUID(IID_IDirectInputDevice8A,0x54D41080,0xDC15,0x4833,0xA4,0x1B,0x74,0x8F,0x73,0xA3,0x81,0x79);
DEFINE_GUID(IID_IDirectInputDevice8W,0x54D41081,0xDC15,0x4833,0xA4,0x1B,0x74,0x8F,0x73,0xA3,0x81,0x79);
DEFINE_GUID(IID_IDirectInputEffect,  0xE7E1F7C0,0x88D2,0x11D0,0x9A,0xD0,0x00,0xA0,0xC9,0xA0,0x6E,0x35);

/****************************************************************************
 *
 *      Predefined object types
 *
 ****************************************************************************/

DEFINE_GUID(GUID_XAxis,   0xA36D02E0,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_YAxis,   0xA36D02E1,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_ZAxis,   0xA36D02E2,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_RxAxis,  0xA36D02F4,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_RyAxis,  0xA36D02F5,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_RzAxis,  0xA36D02E3,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_Slider,  0xA36D02E4,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

DEFINE_GUID(GUID_Button,  0xA36D02F0,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_Key,     0x55728220,0xD33C,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

DEFINE_GUID(GUID_POV,     0xA36D02F2,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

DEFINE_GUID(GUID_Unknown, 0xA36D02F3,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

/****************************************************************************
 *
 *      Predefined product GUIDs
 *
 ****************************************************************************/

DEFINE_GUID(GUID_SysMouse,   0x6F1D2B60,0xD5A0,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_SysKeyboard,0x6F1D2B61,0xD5A0,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_Joystick   ,0x6F1D2B70,0xD5A0,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_SysMouseEm, 0x6F1D2B80,0xD5A0,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_SysMouseEm2,0x6F1D2B81,0xD5A0,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_SysKeyboardEm, 0x6F1D2B82,0xD5A0,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_SysKeyboardEm2,0x6F1D2B83,0xD5A0,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);

/****************************************************************************
 *
 *      Predefined force feedback effects
 *
 ****************************************************************************/

DEFINE_GUID(GUID_ConstantForce, 0x13541C20,0x8E33,0x11D0,0x9A,0xD0,0x00,0xA0,0xC9,0xA0,0x6E,0x35);
DEFINE_GUID(GUID_RampForce,     0x13541C21,0x8E33,0x11D0,0x9A,0xD0,0x00,0xA0,0xC9,0xA0,0x6E,0x35);
DEFINE_GUID(GUID_Square,        0x13541C22,0x8E33,0x11D0,0x9A,0xD0,0x00,0xA0,0xC9,0xA0,0x6E,0x35);
DEFINE_GUID(GUID_Sine,          0x13541C23,0x8E33,0x11D0,0x9A,0xD0,0x00,0xA0,0xC9,0xA0,0x6E,0x35);
DEFINE_GUID(GUID_Triangle,      0x13541C24,0x8E33,0x11D0,0x9A,0xD0,0x00,0xA0,0xC9,0xA0,0x6E,0x35);
DEFINE_GUID(GUID_SawtoothUp,    0x13541C25,0x8E33,0x11D0,0x9A,0xD0,0x00,0xA0,0xC9,0xA0,0x6E,0x35);
DEFINE_GUID(GUID_SawtoothDown,  0x13541C26,0x8E33,0x11D0,0x9A,0xD0,0x00,0xA0,0xC9,0xA0,0x6E,0x35);
DEFINE_GUID(GUID_Spring,        0x13541C27,0x8E33,0x11D0,0x9A,0xD0,0x00,0xA0,0xC9,0xA0,0x6E,0x35);
DEFINE_GUID(GUID_Damper,        0x13541C28,0x8E33,0x11D0,0x9A,0xD0,0x00,0xA0,0xC9,0xA0,0x6E,0x35);
DEFINE_GUID(GUID_Inertia,       0x13541C29,0x8E33,0x11D0,0x9A,0xD0,0x00,0xA0,0xC9,0xA0,0x6E,0x35);
DEFINE_GUID(GUID_Friction,      0x13541C2A,0x8E33,0x11D0,0x9A,0xD0,0x00,0xA0,0xC9,0xA0,0x6E,0x35);
DEFINE_GUID(GUID_CustomForce,   0x13541C2B,0x8E33,0x11D0,0x9A,0xD0,0x00,0xA0,0xC9,0xA0,0x6E,0x35);

#endif /* DIJ_RINGZERO */

/****************************************************************************
 *
 *      Interfaces and Structures...
 *
 ****************************************************************************/

#if(DIRECTINPUT_VERSION >= 0x0500)

/****************************************************************************
 *
 *      IDirectInputEffect
 *
 ****************************************************************************/

#define DIEFT_ALL                   0x00000000

#define DIEFT_CONSTANTFORCE         0x00000001
#define DIEFT_RAMPFORCE             0x00000002
#define DIEFT_PERIODIC              0x00000003
#define DIEFT_CONDITION             0x00000004
#define DIEFT_CUSTOMFORCE           0x00000005
#define DIEFT_HARDWARE              0x000000FF
#define DIEFT_FFATTACK              0x00000200
#define DIEFT_FFFADE                0x00000400
#define DIEFT_SATURATION            0x00000800
#define DIEFT_POSNEGCOEFFICIENTS    0x00001000
#define DIEFT_POSNEGSATURATION      0x00002000
#define DIEFT_DEADBAND              0x00004000
#define DIEFT_STARTDELAY            0x00008000
#define DIEFT_GETTYPE(n)            LOBYTE(n)

#define DI_DEGREES                  100
#define DI_FFNOMINALMAX             10000
#define DI_SECONDS                  1000000

typedef struct DICONSTANTFORCE {
    LONG  lMagnitude;
} DICONSTANTFORCE, *LPDICONSTANTFORCE;
typedef const DICONSTANTFORCE *LPCDICONSTANTFORCE;

typedef struct DIRAMPFORCE {
    LONG  lStart;
    LONG  lEnd;
} DIRAMPFORCE, *LPDIRAMPFORCE;
typedef const DIRAMPFORCE *LPCDIRAMPFORCE;

typedef struct DIPERIODIC {
    DWORD dwMagnitude;
    LONG  lOffset;
    DWORD dwPhase;
    DWORD dwPeriod;
} DIPERIODIC, *LPDIPERIODIC;
typedef const DIPERIODIC *LPCDIPERIODIC;

typedef struct DICONDITION {
    LONG  lOffset;
    LONG  lPositiveCoefficient;
    LONG  lNegativeCoefficient;
    DWORD dwPositiveSaturation;
    DWORD dwNegativeSaturation;
    LONG  lDeadBand;
} DICONDITION, *LPDICONDITION;
typedef const DICONDITION *LPCDICONDITION;

typedef struct DICUSTOMFORCE {
    DWORD cChannels;
    DWORD dwSamplePeriod;
    DWORD cSamples;
    LPLONG rglForceData;
} DICUSTOMFORCE, *LPDICUSTOMFORCE;
typedef const DICUSTOMFORCE *LPCDICUSTOMFORCE;


typedef struct DIENVELOPE {
    DWORD dwSize;                   /* sizeof(DIENVELOPE)   */
    DWORD dwAttackLevel;
    DWORD dwAttackTime;             /* Microseconds         */
    DWORD dwFadeLevel;
    DWORD dwFadeTime;               /* Microseconds         */
} DIENVELOPE, *LPDIENVELOPE;
typedef const DIENVELOPE *LPCDIENVELOPE;


/* This structure is defined for DirectX 5.0 compatibility */
typedef struct DIEFFECT_DX5 {
    DWORD dwSize;                   /* sizeof(DIEFFECT_DX5) */
    DWORD dwFlags;                  /* DIEFF_*              */
    DWORD dwDuration;               /* Microseconds         */
    DWORD dwSamplePeriod;           /* Microseconds         */
    DWORD dwGain;
    DWORD dwTriggerButton;          /* or DIEB_NOTRIGGER    */
    DWORD dwTriggerRepeatInterval;  /* Microseconds         */
    DWORD cAxes;                    /* Number of axes       */
    LPDWORD rgdwAxes;               /* Array of axes        */
    LPLONG rglDirection;            /* Array of directions  */
    LPDIENVELOPE lpEnvelope;        /* Optional             */
    DWORD cbTypeSpecificParams;     /* Size of params       */
    LPVOID lpvTypeSpecificParams;   /* Pointer to params    */
} DIEFFECT_DX5, *LPDIEFFECT_DX5;
typedef const DIEFFECT_DX5 *LPCDIEFFECT_DX5;

typedef struct DIEFFECT {
    DWORD dwSize;                   /* sizeof(DIEFFECT)     */
    DWORD dwFlags;                  /* DIEFF_*              */
    DWORD dwDuration;               /* Microseconds         */
    DWORD dwSamplePeriod;           /* Microseconds         */
    DWORD dwGain;
    DWORD dwTriggerButton;          /* or DIEB_NOTRIGGER    */
    DWORD dwTriggerRepeatInterval;  /* Microseconds         */
    DWORD cAxes;                    /* Number of axes       */
    LPDWORD rgdwAxes;               /* Array of axes        */
    LPLONG rglDirection;            /* Array of directions  */
    LPDIENVELOPE lpEnvelope;        /* Optional             */
    DWORD cbTypeSpecificParams;     /* Size of params       */
    LPVOID lpvTypeSpecificParams;   /* Pointer to params    */
#if(DIRECTINPUT_VERSION >= 0x0600)
    DWORD  dwStartDelay;            /* Microseconds         */
#endif /* DIRECTINPUT_VERSION >= 0x0600 */
} DIEFFECT, *LPDIEFFECT;
typedef DIEFFECT DIEFFECT_DX6;
typedef LPDIEFFECT LPDIEFFECT_DX6;
typedef const DIEFFECT *LPCDIEFFECT;


#if(DIRECTINPUT_VERSION >= 0x0700)
#ifndef DIJ_RINGZERO
typedef struct DIFILEEFFECT{
    DWORD       dwSize;
    GUID        GuidEffect;
    LPCDIEFFECT lpDiEffect;
    CHAR        szFriendlyName[MAX_PATH];
}DIFILEEFFECT, *LPDIFILEEFFECT;
typedef const DIFILEEFFECT *LPCDIFILEEFFECT;
typedef BOOL (FAR PASCAL * LPDIENUMEFFECTSINFILECALLBACK)(LPCDIFILEEFFECT , LPVOID);
#endif /* DIJ_RINGZERO */
#endif /* DIRECTINPUT_VERSION >= 0x0700 */

#define DIEFF_OBJECTIDS             0x00000001
#define DIEFF_OBJECTOFFSETS         0x00000002
#define DIEFF_CARTESIAN             0x00000010
#define DIEFF_POLAR                 0x00000020
#define DIEFF_SPHERICAL             0x00000040

#define DIEP_DURATION               0x00000001
#define DIEP_SAMPLEPERIOD           0x00000002
#define DIEP_GAIN                   0x00000004
#define DIEP_TRIGGERBUTTON          0x00000008
#define DIEP_TRIGGERREPEATINTERVAL  0x00000010
#define DIEP_AXES                   0x00000020
#define DIEP_DIRECTION              0x00000040
#define DIEP_ENVELOPE               0x00000080
#define DIEP_TYPESPECIFICPARAMS     0x00000100
#if(DIRECTINPUT_VERSION >= 0x0600)
#define DIEP_STARTDELAY             0x00000200
#define DIEP_ALLPARAMS_DX5          0x000001FF
#define DIEP_ALLPARAMS              0x000003FF
#else /* DIRECTINPUT_VERSION < 0x0600 */
#define DIEP_ALLPARAMS              0x000001FF
#endif /* DIRECTINPUT_VERSION < 0x0600 */
#define DIEP_START                  0x20000000
#define DIEP_NORESTART              0x40000000
#define DIEP_NODOWNLOAD             0x80000000
#define DIEB_NOTRIGGER              0xFFFFFFFF

#define DIES_SOLO                   0x00000001
#define DIES_NODOWNLOAD             0x80000000

#define DIEGES_PLAYING              0x00000001
#define DIEGES_EMULATED             0x00000002

typedef struct DIEFFESCAPE {
    DWORD   dwSize;
    DWORD   dwCommand;
    LPVOID  lpvInBuffer;
    DWORD   cbInBuffer;
    LPVOID  lpvOutBuffer;
    DWORD   cbOutBuffer;
} DIEFFESCAPE, *LPDIEFFESCAPE;

#ifndef DIJ_RINGZERO

#undef INTERFACE
#define INTERFACE IDirectInputEffect

DECLARE_INTERFACE_(IDirectInputEffect, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputEffect methods ***/
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
    STDMETHOD(GetEffectGuid)(THIS_ LPGUID) PURE;
    STDMETHOD(GetParameters)(THIS_ LPDIEFFECT,DWORD) PURE;
    STDMETHOD(SetParameters)(THIS_ LPCDIEFFECT,DWORD) PURE;
    STDMETHOD(Start)(THIS_ DWORD,DWORD) PURE;
    STDMETHOD(Stop)(THIS) PURE;
    STDMETHOD(GetEffectStatus)(THIS_ LPDWORD) PURE;
    STDMETHOD(Download)(THIS) PURE;
    STDMETHOD(Unload)(THIS) PURE;
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
};

typedef struct IDirectInputEffect *LPDIRECTINPUTEFFECT;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInputEffect_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInputEffect_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInputEffect_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInputEffect_Initialize(p,a,b,c) (p)->lpVtbl->Initialize(p,a,b,c)
#define IDirectInputEffect_GetEffectGuid(p,a) (p)->lpVtbl->GetEffectGuid(p,a)
#define IDirectInputEffect_GetParameters(p,a,b) (p)->lpVtbl->GetParameters(p,a,b)
#define IDirectInputEffect_SetParameters(p,a,b) (p)->lpVtbl->SetParameters(p,a,b)
#define IDirectInputEffect_Start(p,a,b) (p)->lpVtbl->Start(p,a,b)
#define IDirectInputEffect_Stop(p) (p)->lpVtbl->Stop(p)
#define IDirectInputEffect_GetEffectStatus(p,a) (p)->lpVtbl->GetEffectStatus(p,a)
#define IDirectInputEffect_Download(p) (p)->lpVtbl->Download(p)
#define IDirectInputEffect_Unload(p) (p)->lpVtbl->Unload(p)
#define IDirectInputEffect_Escape(p,a) (p)->lpVtbl->Escape(p,a)
#else
#define IDirectInputEffect_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInputEffect_AddRef(p) (p)->AddRef()
#define IDirectInputEffect_Release(p) (p)->Release()
#define IDirectInputEffect_Initialize(p,a,b,c) (p)->Initialize(a,b,c)
#define IDirectInputEffect_GetEffectGuid(p,a) (p)->GetEffectGuid(a)
#define IDirectInputEffect_GetParameters(p,a,b) (p)->GetParameters(a,b)
#define IDirectInputEffect_SetParameters(p,a,b) (p)->SetParameters(a,b)
#define IDirectInputEffect_Start(p,a,b) (p)->Start(a,b)
#define IDirectInputEffect_Stop(p) (p)->Stop()
#define IDirectInputEffect_GetEffectStatus(p,a) (p)->GetEffectStatus(a)
#define IDirectInputEffect_Download(p) (p)->Download()
#define IDirectInputEffect_Unload(p) (p)->Unload()
#define IDirectInputEffect_Escape(p,a) (p)->Escape(a)
#endif

#endif /* DIJ_RINGZERO */

#endif /* DIRECTINPUT_VERSION >= 0x0500 */

/****************************************************************************
 *
 *      IDirectInputDevice
 *
 ****************************************************************************/

#if DIRECTINPUT_VERSION <= 0x700
#define DIDEVTYPE_DEVICE        1
#define DIDEVTYPE_MOUSE         2
#define DIDEVTYPE_KEYBOARD      3
#define DIDEVTYPE_JOYSTICK      4

#else
#define DI8DEVCLASS_ALL             0
#define DI8DEVCLASS_DEVICE          1
#define DI8DEVCLASS_POINTER         2
#define DI8DEVCLASS_KEYBOARD        3
#define DI8DEVCLASS_GAMECTRL        4

#define DI8DEVTYPE_DEVICE           0x11
#define DI8DEVTYPE_MOUSE            0x12
#define DI8DEVTYPE_KEYBOARD         0x13
#define DI8DEVTYPE_JOYSTICK         0x14
#define DI8DEVTYPE_GAMEPAD          0x15
#define DI8DEVTYPE_DRIVING          0x16
#define DI8DEVTYPE_FLIGHT           0x17
#define DI8DEVTYPE_1STPERSON        0x18
#define DI8DEVTYPE_DEVICECTRL       0x19
#define DI8DEVTYPE_SCREENPOINTER    0x1A
#define DI8DEVTYPE_REMOTE           0x1B
#define DI8DEVTYPE_SUPPLEMENTAL     0x1C
#endif /* DIRECTINPUT_VERSION <= 0x700 */

#define DIDEVTYPE_HID           0x00010000

#if DIRECTINPUT_VERSION <= 0x700
#define DIDEVTYPEMOUSE_UNKNOWN          1
#define DIDEVTYPEMOUSE_TRADITIONAL      2
#define DIDEVTYPEMOUSE_FINGERSTICK      3
#define DIDEVTYPEMOUSE_TOUCHPAD         4
#define DIDEVTYPEMOUSE_TRACKBALL        5

#define DIDEVTYPEKEYBOARD_UNKNOWN       0
#define DIDEVTYPEKEYBOARD_PCXT          1
#define DIDEVTYPEKEYBOARD_OLIVETTI      2
#define DIDEVTYPEKEYBOARD_PCAT          3
#define DIDEVTYPEKEYBOARD_PCENH         4
#define DIDEVTYPEKEYBOARD_NOKIA1050     5
#define DIDEVTYPEKEYBOARD_NOKIA9140     6
#define DIDEVTYPEKEYBOARD_NEC98         7
#define DIDEVTYPEKEYBOARD_NEC98LAPTOP   8
#define DIDEVTYPEKEYBOARD_NEC98106      9
#define DIDEVTYPEKEYBOARD_JAPAN106     10
#define DIDEVTYPEKEYBOARD_JAPANAX      11
#define DIDEVTYPEKEYBOARD_J3100        12

#define DIDEVTYPEJOYSTICK_UNKNOWN       1
#define DIDEVTYPEJOYSTICK_TRADITIONAL   2
#define DIDEVTYPEJOYSTICK_FLIGHTSTICK   3
#define DIDEVTYPEJOYSTICK_GAMEPAD       4
#define DIDEVTYPEJOYSTICK_RUDDER        5
#define DIDEVTYPEJOYSTICK_WHEEL         6
#define DIDEVTYPEJOYSTICK_HEADTRACKER   7

#else
#define DI8DEVTYPEMOUSE_UNKNOWN                     1
#define DI8DEVTYPEMOUSE_TRADITIONAL                 2
#define DI8DEVTYPEMOUSE_FINGERSTICK                 3
#define DI8DEVTYPEMOUSE_TOUCHPAD                    4
#define DI8DEVTYPEMOUSE_TRACKBALL                   5
#define DI8DEVTYPEMOUSE_ABSOLUTE                    6

#define DI8DEVTYPEKEYBOARD_UNKNOWN                  0
#define DI8DEVTYPEKEYBOARD_PCXT                     1
#define DI8DEVTYPEKEYBOARD_OLIVETTI                 2
#define DI8DEVTYPEKEYBOARD_PCAT                     3
#define DI8DEVTYPEKEYBOARD_PCENH                    4
#define DI8DEVTYPEKEYBOARD_NOKIA1050                5
#define DI8DEVTYPEKEYBOARD_NOKIA9140                6
#define DI8DEVTYPEKEYBOARD_NEC98                    7
#define DI8DEVTYPEKEYBOARD_NEC98LAPTOP              8
#define DI8DEVTYPEKEYBOARD_NEC98106                 9
#define DI8DEVTYPEKEYBOARD_JAPAN106                10
#define DI8DEVTYPEKEYBOARD_JAPANAX                 11
#define DI8DEVTYPEKEYBOARD_J3100                   12

#define DI8DEVTYPE_LIMITEDGAMESUBTYPE               1

#define DI8DEVTYPEJOYSTICK_LIMITED                  DI8DEVTYPE_LIMITEDGAMESUBTYPE
#define DI8DEVTYPEJOYSTICK_STANDARD                 2

#define DI8DEVTYPEGAMEPAD_LIMITED                   DI8DEVTYPE_LIMITEDGAMESUBTYPE
#define DI8DEVTYPEGAMEPAD_STANDARD                  2
#define DI8DEVTYPEGAMEPAD_TILT                      3

#define DI8DEVTYPEDRIVING_LIMITED                   DI8DEVTYPE_LIMITEDGAMESUBTYPE
#define DI8DEVTYPEDRIVING_COMBINEDPEDALS            2
#define DI8DEVTYPEDRIVING_DUALPEDALS                3
#define DI8DEVTYPEDRIVING_THREEPEDALS               4
#define DI8DEVTYPEDRIVING_HANDHELD                  5

#define DI8DEVTYPEFLIGHT_LIMITED                    DI8DEVTYPE_LIMITEDGAMESUBTYPE
#define DI8DEVTYPEFLIGHT_STICK                      2
#define DI8DEVTYPEFLIGHT_YOKE                       3
#define DI8DEVTYPEFLIGHT_RC                         4

#define DI8DEVTYPE1STPERSON_LIMITED                 DI8DEVTYPE_LIMITEDGAMESUBTYPE
#define DI8DEVTYPE1STPERSON_UNKNOWN                 2
#define DI8DEVTYPE1STPERSON_SIXDOF                  3
#define DI8DEVTYPE1STPERSON_SHOOTER                 4

#define DI8DEVTYPESCREENPTR_UNKNOWN                 2
#define DI8DEVTYPESCREENPTR_LIGHTGUN                3
#define DI8DEVTYPESCREENPTR_LIGHTPEN                4
#define DI8DEVTYPESCREENPTR_TOUCH                   5

#define DI8DEVTYPEREMOTE_UNKNOWN                    2

#define DI8DEVTYPEDEVICECTRL_UNKNOWN                2
#define DI8DEVTYPEDEVICECTRL_COMMSSELECTION         3
#define DI8DEVTYPEDEVICECTRL_COMMSSELECTION_HARDWIRED 4

#define DI8DEVTYPESUPPLEMENTAL_UNKNOWN              2
#define DI8DEVTYPESUPPLEMENTAL_2NDHANDCONTROLLER    3
#define DI8DEVTYPESUPPLEMENTAL_HEADTRACKER          4
#define DI8DEVTYPESUPPLEMENTAL_HANDTRACKER          5
#define DI8DEVTYPESUPPLEMENTAL_SHIFTSTICKGATE       6
#define DI8DEVTYPESUPPLEMENTAL_SHIFTER              7
#define DI8DEVTYPESUPPLEMENTAL_THROTTLE             8
#define DI8DEVTYPESUPPLEMENTAL_SPLITTHROTTLE        9
#define DI8DEVTYPESUPPLEMENTAL_COMBINEDPEDALS      10
#define DI8DEVTYPESUPPLEMENTAL_DUALPEDALS          11
#define DI8DEVTYPESUPPLEMENTAL_THREEPEDALS         12
#define DI8DEVTYPESUPPLEMENTAL_RUDDERPEDALS        13
#endif /* DIRECTINPUT_VERSION <= 0x700 */

#define GET_DIDEVICE_TYPE(dwDevType)    LOBYTE(dwDevType)
#define GET_DIDEVICE_SUBTYPE(dwDevType) HIBYTE(dwDevType)

#if(DIRECTINPUT_VERSION >= 0x0500)
/* This structure is defined for DirectX 3.0 compatibility */
typedef struct DIDEVCAPS_DX3 {
    DWORD   dwSize;
    DWORD   dwFlags;
    DWORD   dwDevType;
    DWORD   dwAxes;
    DWORD   dwButtons;
    DWORD   dwPOVs;
} DIDEVCAPS_DX3, *LPDIDEVCAPS_DX3;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */

typedef struct DIDEVCAPS {
    DWORD   dwSize;
    DWORD   dwFlags;
    DWORD   dwDevType;
    DWORD   dwAxes;
    DWORD   dwButtons;
    DWORD   dwPOVs;
#if(DIRECTINPUT_VERSION >= 0x0500)
    DWORD   dwFFSamplePeriod;
    DWORD   dwFFMinTimeResolution;
    DWORD   dwFirmwareRevision;
    DWORD   dwHardwareRevision;
    DWORD   dwFFDriverVersion;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */
} DIDEVCAPS, *LPDIDEVCAPS;

#define DIDC_ATTACHED           0x00000001
#define DIDC_POLLEDDEVICE       0x00000002
#define DIDC_EMULATED           0x00000004
#define DIDC_POLLEDDATAFORMAT   0x00000008
#if(DIRECTINPUT_VERSION >= 0x0500)
#define DIDC_FORCEFEEDBACK      0x00000100
#define DIDC_FFATTACK           0x00000200
#define DIDC_FFFADE             0x00000400
#define DIDC_SATURATION         0x00000800
#define DIDC_POSNEGCOEFFICIENTS 0x00001000
#define DIDC_POSNEGSATURATION   0x00002000
#define DIDC_DEADBAND           0x00004000
#endif /* DIRECTINPUT_VERSION >= 0x0500 */
#define DIDC_STARTDELAY         0x00008000
#if(DIRECTINPUT_VERSION >= 0x050a)
#define DIDC_ALIAS              0x00010000
#define DIDC_PHANTOM            0x00020000
#endif /* DIRECTINPUT_VERSION >= 0x050a */
#if(DIRECTINPUT_VERSION >= 0x0800)
#define DIDC_HIDDEN             0x00040000
#endif /* DIRECTINPUT_VERSION >= 0x0800 */

#define DIDFT_ALL           0x00000000

#define DIDFT_RELAXIS       0x00000001
#define DIDFT_ABSAXIS       0x00000002
#define DIDFT_AXIS          0x00000003

#define DIDFT_PSHBUTTON     0x00000004
#define DIDFT_TGLBUTTON     0x00000008
#define DIDFT_BUTTON        0x0000000C

#define DIDFT_POV           0x00000010
#define DIDFT_COLLECTION    0x00000040
#define DIDFT_NODATA        0x00000080

#define DIDFT_ANYINSTANCE   0x00FFFF00
#define DIDFT_INSTANCEMASK  DIDFT_ANYINSTANCE
#define DIDFT_MAKEINSTANCE(n) ((WORD)(n) << 8)
#define DIDFT_GETTYPE(n)     LOBYTE(n)
#define DIDFT_GETINSTANCE(n) LOWORD((n) >> 8)
#define DIDFT_FFACTUATOR        0x01000000
#define DIDFT_FFEFFECTTRIGGER   0x02000000
#if(DIRECTINPUT_VERSION >= 0x050a)
#define DIDFT_OUTPUT            0x10000000
#define DIDFT_VENDORDEFINED     0x04000000
#define DIDFT_ALIAS             0x08000000
#endif /* DIRECTINPUT_VERSION >= 0x050a */
#ifndef DIDFT_OPTIONAL
#define DIDFT_OPTIONAL          0x80000000
#endif

#define DIDFT_ENUMCOLLECTION(n) ((WORD)(n) << 8)
#define DIDFT_NOCOLLECTION      0x00FFFF00

#ifndef DIJ_RINGZERO

typedef struct _DIOBJECTDATAFORMAT {
    const GUID *pguid;
    DWORD   dwOfs;
    DWORD   dwType;
    DWORD   dwFlags;
} DIOBJECTDATAFORMAT, *LPDIOBJECTDATAFORMAT;
typedef const DIOBJECTDATAFORMAT *LPCDIOBJECTDATAFORMAT;

typedef struct _DIDATAFORMAT {
    DWORD   dwSize;
    DWORD   dwObjSize;
    DWORD   dwFlags;
    DWORD   dwDataSize;
    DWORD   dwNumObjs;
    LPDIOBJECTDATAFORMAT rgodf;
} DIDATAFORMAT, *LPDIDATAFORMAT;
typedef const DIDATAFORMAT *LPCDIDATAFORMAT;

#define DIDF_ABSAXIS            0x00000001
#define DIDF_RELAXIS            0x00000002

#ifdef __cplusplus
extern "C" {
#endif
extern const DIDATAFORMAT c_dfDIMouse;

#if(DIRECTINPUT_VERSION >= 0x0700)
extern const DIDATAFORMAT c_dfDIMouse2;
#endif /* DIRECTINPUT_VERSION >= 0x0700 */

extern const DIDATAFORMAT c_dfDIKeyboard;

#if(DIRECTINPUT_VERSION >= 0x0500)
extern const DIDATAFORMAT c_dfDIJoystick;
extern const DIDATAFORMAT c_dfDIJoystick2;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */

#ifdef __cplusplus
};
#endif


#if DIRECTINPUT_VERSION > 0x0700

typedef struct _DIACTIONA {
                UINT_PTR    uAppData;
                DWORD       dwSemantic;
    OPTIONAL    DWORD       dwFlags;
    OPTIONAL    union {
                    LPCSTR      lptszActionName;
                    UINT        uResIdString;
                };
    OPTIONAL    GUID        guidInstance;
    OPTIONAL    DWORD       dwObjID;
    OPTIONAL    DWORD       dwHow;
} DIACTIONA, *LPDIACTIONA ;
typedef struct _DIACTIONW {
                UINT_PTR    uAppData;
                DWORD       dwSemantic;
    OPTIONAL    DWORD       dwFlags;
    OPTIONAL    union {
                    LPCWSTR     lptszActionName;
                    UINT        uResIdString;
                };
    OPTIONAL    GUID        guidInstance;
    OPTIONAL    DWORD       dwObjID;
    OPTIONAL    DWORD       dwHow;
} DIACTIONW, *LPDIACTIONW ;
#ifdef UNICODE
typedef DIACTIONW DIACTION;
typedef LPDIACTIONW LPDIACTION;
#else
typedef DIACTIONA DIACTION;
typedef LPDIACTIONA LPDIACTION;
#endif // UNICODE

typedef const DIACTIONA *LPCDIACTIONA;
typedef const DIACTIONW *LPCDIACTIONW;
#ifdef UNICODE
typedef DIACTIONW DIACTION;
typedef LPCDIACTIONW LPCDIACTION;
#else
typedef DIACTIONA DIACTION;
typedef LPCDIACTIONA LPCDIACTION;
#endif // UNICODE
typedef const DIACTION *LPCDIACTION;


#define DIA_FORCEFEEDBACK       0x00000001
#define DIA_APPMAPPED           0x00000002
#define DIA_APPNOMAP            0x00000004
#define DIA_NORANGE             0x00000008
#define DIA_APPFIXED            0x00000010

#define DIAH_UNMAPPED           0x00000000
#define DIAH_USERCONFIG         0x00000001
#define DIAH_APPREQUESTED       0x00000002
#define DIAH_HWAPP              0x00000004
#define DIAH_HWDEFAULT          0x00000008
#define DIAH_DEFAULT            0x00000020
#define DIAH_ERROR              0x80000000

typedef struct _DIACTIONFORMATA {
                DWORD       dwSize;
                DWORD       dwActionSize;
                DWORD       dwDataSize;
                DWORD       dwNumActions;
                LPDIACTIONA rgoAction;
                GUID        guidActionMap;
                DWORD       dwGenre;
                DWORD       dwBufferSize;
    OPTIONAL    LONG        lAxisMin;
    OPTIONAL    LONG        lAxisMax;
    OPTIONAL    HINSTANCE   hInstString;
                FILETIME    ftTimeStamp;
                DWORD       dwCRC;
                CHAR        tszActionMap[MAX_PATH];
} DIACTIONFORMATA, *LPDIACTIONFORMATA;
typedef struct _DIACTIONFORMATW {
                DWORD       dwSize;
                DWORD       dwActionSize;
                DWORD       dwDataSize;
                DWORD       dwNumActions;
                LPDIACTIONW rgoAction;
                GUID        guidActionMap;
                DWORD       dwGenre;
                DWORD       dwBufferSize;
    OPTIONAL    LONG        lAxisMin;
    OPTIONAL    LONG        lAxisMax;
    OPTIONAL    HINSTANCE   hInstString;
                FILETIME    ftTimeStamp;
                DWORD       dwCRC;
                WCHAR       tszActionMap[MAX_PATH];
} DIACTIONFORMATW, *LPDIACTIONFORMATW;
#ifdef UNICODE
typedef DIACTIONFORMATW DIACTIONFORMAT;
typedef LPDIACTIONFORMATW LPDIACTIONFORMAT;
#else
typedef DIACTIONFORMATA DIACTIONFORMAT;
typedef LPDIACTIONFORMATA LPDIACTIONFORMAT;
#endif // UNICODE
typedef const DIACTIONFORMATA *LPCDIACTIONFORMATA;
typedef const DIACTIONFORMATW *LPCDIACTIONFORMATW;
#ifdef UNICODE
typedef DIACTIONFORMATW DIACTIONFORMAT;
typedef LPCDIACTIONFORMATW LPCDIACTIONFORMAT;
#else
typedef DIACTIONFORMATA DIACTIONFORMAT;
typedef LPCDIACTIONFORMATA LPCDIACTIONFORMAT;
#endif // UNICODE
typedef const DIACTIONFORMAT *LPCDIACTIONFORMAT;

#define DIAFTS_NEWDEVICELOW     0xFFFFFFFF
#define DIAFTS_NEWDEVICEHIGH    0xFFFFFFFF
#define DIAFTS_UNUSEDDEVICELOW  0x00000000
#define DIAFTS_UNUSEDDEVICEHIGH 0x00000000

#define DIDBAM_DEFAULT          0x00000000
#define DIDBAM_PRESERVE         0x00000001
#define DIDBAM_INITIALIZE       0x00000002
#define DIDBAM_HWDEFAULTS       0x00000004

#define DIDSAM_DEFAULT          0x00000000
#define DIDSAM_NOUSER           0x00000001
#define DIDSAM_FORCESAVE        0x00000002

#define DICD_DEFAULT            0x00000000
#define DICD_EDIT               0x00000001

/*
 * The following definition is normally defined in d3dtypes.h
 */
#ifndef D3DCOLOR_DEFINED
typedef DWORD D3DCOLOR;
#define D3DCOLOR_DEFINED
#endif

typedef struct _DICOLORSET{
    DWORD dwSize;
    D3DCOLOR cTextFore;
    D3DCOLOR cTextHighlight;
    D3DCOLOR cCalloutLine;
    D3DCOLOR cCalloutHighlight;
    D3DCOLOR cBorder;
    D3DCOLOR cControlFill;
    D3DCOLOR cHighlightFill;
    D3DCOLOR cAreaFill;
} DICOLORSET, *LPDICOLORSET;
typedef const DICOLORSET *LPCDICOLORSET;


typedef struct _DICONFIGUREDEVICESPARAMSA{
     DWORD             dwSize;
     DWORD             dwcUsers;
     LPSTR             lptszUserNames;
     DWORD             dwcFormats;
     LPDIACTIONFORMATA lprgFormats;
     HWND              hwnd;
     DICOLORSET        dics;
     IUnknown FAR *    lpUnkDDSTarget;
} DICONFIGUREDEVICESPARAMSA, *LPDICONFIGUREDEVICESPARAMSA;
typedef struct _DICONFIGUREDEVICESPARAMSW{
     DWORD             dwSize;
     DWORD             dwcUsers;
     LPWSTR            lptszUserNames;
     DWORD             dwcFormats;
     LPDIACTIONFORMATW lprgFormats;
     HWND              hwnd;
     DICOLORSET        dics;
     IUnknown FAR *    lpUnkDDSTarget;
} DICONFIGUREDEVICESPARAMSW, *LPDICONFIGUREDEVICESPARAMSW;
#ifdef UNICODE
typedef DICONFIGUREDEVICESPARAMSW DICONFIGUREDEVICESPARAMS;
typedef LPDICONFIGUREDEVICESPARAMSW LPDICONFIGUREDEVICESPARAMS;
#else
typedef DICONFIGUREDEVICESPARAMSA DICONFIGUREDEVICESPARAMS;
typedef LPDICONFIGUREDEVICESPARAMSA LPDICONFIGUREDEVICESPARAMS;
#endif // UNICODE
typedef const DICONFIGUREDEVICESPARAMSA *LPCDICONFIGUREDEVICESPARAMSA;
typedef const DICONFIGUREDEVICESPARAMSW *LPCDICONFIGUREDEVICESPARAMSW;
#ifdef UNICODE
typedef DICONFIGUREDEVICESPARAMSW DICONFIGUREDEVICESPARAMS;
typedef LPCDICONFIGUREDEVICESPARAMSW LPCDICONFIGUREDEVICESPARAMS;
#else
typedef DICONFIGUREDEVICESPARAMSA DICONFIGUREDEVICESPARAMS;
typedef LPCDICONFIGUREDEVICESPARAMSA LPCDICONFIGUREDEVICESPARAMS;
#endif // UNICODE
typedef const DICONFIGUREDEVICESPARAMS *LPCDICONFIGUREDEVICESPARAMS;


#define DIDIFT_CONFIGURATION    0x00000001
#define DIDIFT_OVERLAY          0x00000002

#define DIDAL_CENTERED      0x00000000
#define DIDAL_LEFTALIGNED   0x00000001
#define DIDAL_RIGHTALIGNED  0x00000002
#define DIDAL_MIDDLE        0x00000000
#define DIDAL_TOPALIGNED    0x00000004
#define DIDAL_BOTTOMALIGNED 0x00000008

typedef struct _DIDEVICEIMAGEINFOA {
    CHAR        tszImagePath[MAX_PATH];
    DWORD       dwFlags; 
    // These are valid if DIDIFT_OVERLAY is present in dwFlags.
    DWORD       dwViewID;      
    RECT        rcOverlay;             
    DWORD       dwObjID;
    DWORD       dwcValidPts;
    POINT       rgptCalloutLine[5];  
    RECT        rcCalloutRect;  
    DWORD       dwTextAlign;     
} DIDEVICEIMAGEINFOA, *LPDIDEVICEIMAGEINFOA;
typedef struct _DIDEVICEIMAGEINFOW {
    WCHAR       tszImagePath[MAX_PATH];
    DWORD       dwFlags; 
    // These are valid if DIDIFT_OVERLAY is present in dwFlags.
    DWORD       dwViewID;      
    RECT        rcOverlay;             
    DWORD       dwObjID;
    DWORD       dwcValidPts;
    POINT       rgptCalloutLine[5];  
    RECT        rcCalloutRect;  
    DWORD       dwTextAlign;     
} DIDEVICEIMAGEINFOW, *LPDIDEVICEIMAGEINFOW;
#ifdef UNICODE
typedef DIDEVICEIMAGEINFOW DIDEVICEIMAGEINFO;
typedef LPDIDEVICEIMAGEINFOW LPDIDEVICEIMAGEINFO;
#else
typedef DIDEVICEIMAGEINFOA DIDEVICEIMAGEINFO;
typedef LPDIDEVICEIMAGEINFOA LPDIDEVICEIMAGEINFO;
#endif // UNICODE
typedef const DIDEVICEIMAGEINFOA *LPCDIDEVICEIMAGEINFOA;
typedef const DIDEVICEIMAGEINFOW *LPCDIDEVICEIMAGEINFOW;
#ifdef UNICODE
typedef DIDEVICEIMAGEINFOW DIDEVICEIMAGEINFO;
typedef LPCDIDEVICEIMAGEINFOW LPCDIDEVICEIMAGEINFO;
#else
typedef DIDEVICEIMAGEINFOA DIDEVICEIMAGEINFO;
typedef LPCDIDEVICEIMAGEINFOA LPCDIDEVICEIMAGEINFO;
#endif // UNICODE
typedef const DIDEVICEIMAGEINFO *LPCDIDEVICEIMAGEINFO;

typedef struct _DIDEVICEIMAGEINFOHEADERA {
    DWORD       dwSize;
    DWORD       dwSizeImageInfo;
    DWORD       dwcViews;
    DWORD       dwcButtons;
    DWORD       dwcAxes;
    DWORD       dwcPOVs;
    DWORD       dwBufferSize;
    DWORD       dwBufferUsed;
    LPDIDEVICEIMAGEINFOA lprgImageInfoArray;
} DIDEVICEIMAGEINFOHEADERA, *LPDIDEVICEIMAGEINFOHEADERA;
typedef struct _DIDEVICEIMAGEINFOHEADERW {
    DWORD       dwSize;
    DWORD       dwSizeImageInfo;
    DWORD       dwcViews;
    DWORD       dwcButtons;
    DWORD       dwcAxes;
    DWORD       dwcPOVs;
    DWORD       dwBufferSize;
    DWORD       dwBufferUsed;
    LPDIDEVICEIMAGEINFOW lprgImageInfoArray;
} DIDEVICEIMAGEINFOHEADERW, *LPDIDEVICEIMAGEINFOHEADERW;
#ifdef UNICODE
typedef DIDEVICEIMAGEINFOHEADERW DIDEVICEIMAGEINFOHEADER;
typedef LPDIDEVICEIMAGEINFOHEADERW LPDIDEVICEIMAGEINFOHEADER;
#else
typedef DIDEVICEIMAGEINFOHEADERA DIDEVICEIMAGEINFOHEADER;
typedef LPDIDEVICEIMAGEINFOHEADERA LPDIDEVICEIMAGEINFOHEADER;
#endif // UNICODE
typedef const DIDEVICEIMAGEINFOHEADERA *LPCDIDEVICEIMAGEINFOHEADERA;
typedef const DIDEVICEIMAGEINFOHEADERW *LPCDIDEVICEIMAGEINFOHEADERW;
#ifdef UNICODE
typedef DIDEVICEIMAGEINFOHEADERW DIDEVICEIMAGEINFOHEADER;
typedef LPCDIDEVICEIMAGEINFOHEADERW LPCDIDEVICEIMAGEINFOHEADER;
#else
typedef DIDEVICEIMAGEINFOHEADERA DIDEVICEIMAGEINFOHEADER;
typedef LPCDIDEVICEIMAGEINFOHEADERA LPCDIDEVICEIMAGEINFOHEADER;
#endif // UNICODE
typedef const DIDEVICEIMAGEINFOHEADER *LPCDIDEVICEIMAGEINFOHEADER;

#endif /* DIRECTINPUT_VERSION > 0x0700 */

#if(DIRECTINPUT_VERSION >= 0x0500)
/* These structures are defined for DirectX 3.0 compatibility */

typedef struct DIDEVICEOBJECTINSTANCE_DX3A {
    DWORD   dwSize;
    GUID    guidType;
    DWORD   dwOfs;
    DWORD   dwType;
    DWORD   dwFlags;
    CHAR    tszName[MAX_PATH];
} DIDEVICEOBJECTINSTANCE_DX3A, *LPDIDEVICEOBJECTINSTANCE_DX3A;
typedef struct DIDEVICEOBJECTINSTANCE_DX3W {
    DWORD   dwSize;
    GUID    guidType;
    DWORD   dwOfs;
    DWORD   dwType;
    DWORD   dwFlags;
    WCHAR   tszName[MAX_PATH];
} DIDEVICEOBJECTINSTANCE_DX3W, *LPDIDEVICEOBJECTINSTANCE_DX3W;
#ifdef UNICODE
typedef DIDEVICEOBJECTINSTANCE_DX3W DIDEVICEOBJECTINSTANCE_DX3;
typedef LPDIDEVICEOBJECTINSTANCE_DX3W LPDIDEVICEOBJECTINSTANCE_DX3;
#else
typedef DIDEVICEOBJECTINSTANCE_DX3A DIDEVICEOBJECTINSTANCE_DX3;
typedef LPDIDEVICEOBJECTINSTANCE_DX3A LPDIDEVICEOBJECTINSTANCE_DX3;
#endif // UNICODE
typedef const DIDEVICEOBJECTINSTANCE_DX3A *LPCDIDEVICEOBJECTINSTANCE_DX3A;
typedef const DIDEVICEOBJECTINSTANCE_DX3W *LPCDIDEVICEOBJECTINSTANCE_DX3W;
typedef const DIDEVICEOBJECTINSTANCE_DX3  *LPCDIDEVICEOBJECTINSTANCE_DX3;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */

typedef struct DIDEVICEOBJECTINSTANCEA {
    DWORD   dwSize;
    GUID    guidType;
    DWORD   dwOfs;
    DWORD   dwType;
    DWORD   dwFlags;
    CHAR    tszName[MAX_PATH];
#if(DIRECTINPUT_VERSION >= 0x0500)
    DWORD   dwFFMaxForce;
    DWORD   dwFFForceResolution;
    WORD    wCollectionNumber;
    WORD    wDesignatorIndex;
    WORD    wUsagePage;
    WORD    wUsage;
    DWORD   dwDimension;
    WORD    wExponent;
    WORD    wReportId;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */
} DIDEVICEOBJECTINSTANCEA, *LPDIDEVICEOBJECTINSTANCEA;
typedef struct DIDEVICEOBJECTINSTANCEW {
    DWORD   dwSize;
    GUID    guidType;
    DWORD   dwOfs;
    DWORD   dwType;
    DWORD   dwFlags;
    WCHAR   tszName[MAX_PATH];
#if(DIRECTINPUT_VERSION >= 0x0500)
    DWORD   dwFFMaxForce;
    DWORD   dwFFForceResolution;
    WORD    wCollectionNumber;
    WORD    wDesignatorIndex;
    WORD    wUsagePage;
    WORD    wUsage;
    DWORD   dwDimension;
    WORD    wExponent;
    WORD    wReportId;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */
} DIDEVICEOBJECTINSTANCEW, *LPDIDEVICEOBJECTINSTANCEW;
#ifdef UNICODE
typedef DIDEVICEOBJECTINSTANCEW DIDEVICEOBJECTINSTANCE;
typedef LPDIDEVICEOBJECTINSTANCEW LPDIDEVICEOBJECTINSTANCE;
#else
typedef DIDEVICEOBJECTINSTANCEA DIDEVICEOBJECTINSTANCE;
typedef LPDIDEVICEOBJECTINSTANCEA LPDIDEVICEOBJECTINSTANCE;
#endif // UNICODE
typedef const DIDEVICEOBJECTINSTANCEA *LPCDIDEVICEOBJECTINSTANCEA;
typedef const DIDEVICEOBJECTINSTANCEW *LPCDIDEVICEOBJECTINSTANCEW;
typedef const DIDEVICEOBJECTINSTANCE  *LPCDIDEVICEOBJECTINSTANCE;

typedef BOOL (FAR PASCAL * LPDIENUMDEVICEOBJECTSCALLBACKA)(LPCDIDEVICEOBJECTINSTANCEA, LPVOID);
typedef BOOL (FAR PASCAL * LPDIENUMDEVICEOBJECTSCALLBACKW)(LPCDIDEVICEOBJECTINSTANCEW, LPVOID);
#ifdef UNICODE
#define LPDIENUMDEVICEOBJECTSCALLBACK  LPDIENUMDEVICEOBJECTSCALLBACKW
#else
#define LPDIENUMDEVICEOBJECTSCALLBACK  LPDIENUMDEVICEOBJECTSCALLBACKA
#endif // !UNICODE

#if(DIRECTINPUT_VERSION >= 0x0500)
#define DIDOI_FFACTUATOR        0x00000001
#define DIDOI_FFEFFECTTRIGGER   0x00000002
#define DIDOI_POLLED            0x00008000
#define DIDOI_ASPECTPOSITION    0x00000100
#define DIDOI_ASPECTVELOCITY    0x00000200
#define DIDOI_ASPECTACCEL       0x00000300
#define DIDOI_ASPECTFORCE       0x00000400
#define DIDOI_ASPECTMASK        0x00000F00
#endif /* DIRECTINPUT_VERSION >= 0x0500 */
#if(DIRECTINPUT_VERSION >= 0x050a)
#define DIDOI_GUIDISUSAGE       0x00010000
#endif /* DIRECTINPUT_VERSION >= 0x050a */

typedef struct DIPROPHEADER {
    DWORD   dwSize;
    DWORD   dwHeaderSize;
    DWORD   dwObj;
    DWORD   dwHow;
} DIPROPHEADER, *LPDIPROPHEADER;
typedef const DIPROPHEADER *LPCDIPROPHEADER;

#define DIPH_DEVICE             0
#define DIPH_BYOFFSET           1
#define DIPH_BYID               2
#if(DIRECTINPUT_VERSION >= 0x050a)
#define DIPH_BYUSAGE            3
#endif /* DIRECTINPUT_VERSION >= 0x050a */

#if(DIRECTINPUT_VERSION >= 0x050a)
#define DIMAKEUSAGEDWORD(UsagePage, Usage) \
                                (DWORD)MAKELONG(Usage, UsagePage)
#endif /* DIRECTINPUT_VERSION >= 0x050a */

typedef struct DIPROPDWORD {
    DIPROPHEADER diph;
    DWORD   dwData;
} DIPROPDWORD, *LPDIPROPDWORD;
typedef const DIPROPDWORD *LPCDIPROPDWORD;

#if(DIRECTINPUT_VERSION >= 0x0800)
typedef struct DIPROPPOINTER {
    DIPROPHEADER diph;
    UINT_PTR uData;
} DIPROPPOINTER, *LPDIPROPPOINTER;
typedef const DIPROPPOINTER *LPCDIPROPPOINTER;
#endif /* DIRECTINPUT_VERSION >= 0x0800 */

typedef struct DIPROPRANGE {
    DIPROPHEADER diph;
    LONG    lMin;
    LONG    lMax;
} DIPROPRANGE, *LPDIPROPRANGE;
typedef const DIPROPRANGE *LPCDIPROPRANGE;

#define DIPROPRANGE_NOMIN       ((LONG)0x80000000)
#define DIPROPRANGE_NOMAX       ((LONG)0x7FFFFFFF)

#if(DIRECTINPUT_VERSION >= 0x050a)
typedef struct DIPROPCAL {
    DIPROPHEADER diph;
    LONG    lMin;
    LONG    lCenter;
    LONG    lMax;
} DIPROPCAL, *LPDIPROPCAL;
typedef const DIPROPCAL *LPCDIPROPCAL;

typedef struct DIPROPCALPOV {
    DIPROPHEADER diph;
    LONG   lMin[5];
    LONG   lMax[5];
} DIPROPCALPOV, *LPDIPROPCALPOV;
typedef const DIPROPCALPOV *LPCDIPROPCALPOV;

typedef struct DIPROPGUIDANDPATH {
    DIPROPHEADER diph;
    GUID    guidClass;
    WCHAR   wszPath[MAX_PATH];
} DIPROPGUIDANDPATH, *LPDIPROPGUIDANDPATH;
typedef const DIPROPGUIDANDPATH *LPCDIPROPGUIDANDPATH;

typedef struct DIPROPSTRING {
    DIPROPHEADER diph;
    WCHAR   wsz[MAX_PATH];
} DIPROPSTRING, *LPDIPROPSTRING;
typedef const DIPROPSTRING *LPCDIPROPSTRING;

#endif /* DIRECTINPUT_VERSION >= 0x050a */

#if(DIRECTINPUT_VERSION >= 0x0800)
#define MAXCPOINTSNUM          8

typedef struct _CPOINT
{
    LONG  lP;     // raw value
    DWORD dwLog;  // logical_value / max_logical_value * 10000
} CPOINT, *PCPOINT;

typedef struct DIPROPCPOINTS {
    DIPROPHEADER diph;
    DWORD  dwCPointsNum;
    CPOINT cp[MAXCPOINTSNUM];
} DIPROPCPOINTS, *LPDIPROPCPOINTS;
typedef const DIPROPCPOINTS *LPCDIPROPCPOINTS;
#endif /* DIRECTINPUT_VERSION >= 0x0800 */


#ifdef __cplusplus
#define MAKEDIPROP(prop)    (*(const GUID *)(prop))
#else
#define MAKEDIPROP(prop)    ((REFGUID)(prop))
#endif

#define DIPROP_BUFFERSIZE       MAKEDIPROP(1)

#define DIPROP_AXISMODE         MAKEDIPROP(2)

#define DIPROPAXISMODE_ABS      0
#define DIPROPAXISMODE_REL      1

#define DIPROP_GRANULARITY      MAKEDIPROP(3)

#define DIPROP_RANGE            MAKEDIPROP(4)

#define DIPROP_DEADZONE         MAKEDIPROP(5)

#define DIPROP_SATURATION       MAKEDIPROP(6)

#define DIPROP_FFGAIN           MAKEDIPROP(7)

#define DIPROP_FFLOAD           MAKEDIPROP(8)

#define DIPROP_AUTOCENTER       MAKEDIPROP(9)

#define DIPROPAUTOCENTER_OFF    0
#define DIPROPAUTOCENTER_ON     1

#define DIPROP_CALIBRATIONMODE  MAKEDIPROP(10)

#define DIPROPCALIBRATIONMODE_COOKED    0
#define DIPROPCALIBRATIONMODE_RAW       1

#if(DIRECTINPUT_VERSION >= 0x050a)
#define DIPROP_CALIBRATION      MAKEDIPROP(11)

#define DIPROP_GUIDANDPATH      MAKEDIPROP(12)

#define DIPROP_INSTANCENAME     MAKEDIPROP(13)

#define DIPROP_PRODUCTNAME      MAKEDIPROP(14)
#endif /* DIRECTINPUT_VERSION >= 0x050a */

#if(DIRECTINPUT_VERSION >= 0x05b2)
#define DIPROP_JOYSTICKID       MAKEDIPROP(15)

#define DIPROP_GETPORTDISPLAYNAME       MAKEDIPROP(16)

#endif /* DIRECTINPUT_VERSION >= 0x05b2 */

#if(DIRECTINPUT_VERSION >= 0x0700)
#define DIPROP_PHYSICALRANGE            MAKEDIPROP(18)

#define DIPROP_LOGICALRANGE             MAKEDIPROP(19)
#endif /* DIRECTINPUT_VERSION >= 0x0700 */

#if(DIRECTINPUT_VERSION >= 0x0800)
#define DIPROP_KEYNAME                     MAKEDIPROP(20)

#define DIPROP_CPOINTS                 MAKEDIPROP(21)

#define DIPROP_APPDATA       MAKEDIPROP(22)

#define DIPROP_SCANCODE      MAKEDIPROP(23)

#define DIPROP_VIDPID           MAKEDIPROP(24)

#define DIPROP_USERNAME         MAKEDIPROP(25)

#define DIPROP_TYPENAME         MAKEDIPROP(26)
#endif /* DIRECTINPUT_VERSION >= 0x0800 */


typedef struct DIDEVICEOBJECTDATA_DX3 {
    DWORD       dwOfs;
    DWORD       dwData;
    DWORD       dwTimeStamp;
    DWORD       dwSequence;
} DIDEVICEOBJECTDATA_DX3, *LPDIDEVICEOBJECTDATA_DX3;
typedef const DIDEVICEOBJECTDATA_DX3 *LPCDIDEVICEOBJECTDATA_DX;

typedef struct DIDEVICEOBJECTDATA {
    DWORD       dwOfs;
    DWORD       dwData;
    DWORD       dwTimeStamp;
    DWORD       dwSequence;
#if(DIRECTINPUT_VERSION >= 0x0800)
    UINT_PTR    uAppData;
#endif /* DIRECTINPUT_VERSION >= 0x0800 */
} DIDEVICEOBJECTDATA, *LPDIDEVICEOBJECTDATA;
typedef const DIDEVICEOBJECTDATA *LPCDIDEVICEOBJECTDATA;

#define DIGDD_PEEK          0x00000001

#define DISEQUENCE_COMPARE(dwSequence1, cmp, dwSequence2) \
                        ((int)((dwSequence1) - (dwSequence2)) cmp 0)
#define DISCL_EXCLUSIVE     0x00000001
#define DISCL_NONEXCLUSIVE  0x00000002
#define DISCL_FOREGROUND    0x00000004
#define DISCL_BACKGROUND    0x00000008
#define DISCL_NOWINKEY      0x00000010

#if(DIRECTINPUT_VERSION >= 0x0500)
/* These structures are defined for DirectX 3.0 compatibility */

typedef struct DIDEVICEINSTANCE_DX3A {
    DWORD   dwSize;
    GUID    guidInstance;
    GUID    guidProduct;
    DWORD   dwDevType;
    CHAR    tszInstanceName[MAX_PATH];
    CHAR    tszProductName[MAX_PATH];
} DIDEVICEINSTANCE_DX3A, *LPDIDEVICEINSTANCE_DX3A;
typedef struct DIDEVICEINSTANCE_DX3W {
    DWORD   dwSize;
    GUID    guidInstance;
    GUID    guidProduct;
    DWORD   dwDevType;
    WCHAR   tszInstanceName[MAX_PATH];
    WCHAR   tszProductName[MAX_PATH];
} DIDEVICEINSTANCE_DX3W, *LPDIDEVICEINSTANCE_DX3W;
#ifdef UNICODE
typedef DIDEVICEINSTANCE_DX3W DIDEVICEINSTANCE_DX3;
typedef LPDIDEVICEINSTANCE_DX3W LPDIDEVICEINSTANCE_DX3;
#else
typedef DIDEVICEINSTANCE_DX3A DIDEVICEINSTANCE_DX3;
typedef LPDIDEVICEINSTANCE_DX3A LPDIDEVICEINSTANCE_DX3;
#endif // UNICODE
typedef const DIDEVICEINSTANCE_DX3A *LPCDIDEVICEINSTANCE_DX3A;
typedef const DIDEVICEINSTANCE_DX3W *LPCDIDEVICEINSTANCE_DX3W;
typedef const DIDEVICEINSTANCE_DX3  *LPCDIDEVICEINSTANCE_DX3;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */

typedef struct DIDEVICEINSTANCEA {
    DWORD   dwSize;
    GUID    guidInstance;
    GUID    guidProduct;
    DWORD   dwDevType;
    CHAR    tszInstanceName[MAX_PATH];
    CHAR    tszProductName[MAX_PATH];
#if(DIRECTINPUT_VERSION >= 0x0500)
    GUID    guidFFDriver;
    WORD    wUsagePage;
    WORD    wUsage;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */
} DIDEVICEINSTANCEA, *LPDIDEVICEINSTANCEA;
typedef struct DIDEVICEINSTANCEW {
    DWORD   dwSize;
    GUID    guidInstance;
    GUID    guidProduct;
    DWORD   dwDevType;
    WCHAR   tszInstanceName[MAX_PATH];
    WCHAR   tszProductName[MAX_PATH];
#if(DIRECTINPUT_VERSION >= 0x0500)
    GUID    guidFFDriver;
    WORD    wUsagePage;
    WORD    wUsage;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */
} DIDEVICEINSTANCEW, *LPDIDEVICEINSTANCEW;
#ifdef UNICODE
typedef DIDEVICEINSTANCEW DIDEVICEINSTANCE;
typedef LPDIDEVICEINSTANCEW LPDIDEVICEINSTANCE;
#else
typedef DIDEVICEINSTANCEA DIDEVICEINSTANCE;
typedef LPDIDEVICEINSTANCEA LPDIDEVICEINSTANCE;
#endif // UNICODE

typedef const DIDEVICEINSTANCEA *LPCDIDEVICEINSTANCEA;
typedef const DIDEVICEINSTANCEW *LPCDIDEVICEINSTANCEW;
#ifdef UNICODE
typedef DIDEVICEINSTANCEW DIDEVICEINSTANCE;
typedef LPCDIDEVICEINSTANCEW LPCDIDEVICEINSTANCE;
#else
typedef DIDEVICEINSTANCEA DIDEVICEINSTANCE;
typedef LPCDIDEVICEINSTANCEA LPCDIDEVICEINSTANCE;
#endif // UNICODE
typedef const DIDEVICEINSTANCE  *LPCDIDEVICEINSTANCE;

#undef INTERFACE
#define INTERFACE IDirectInputDeviceW

DECLARE_INTERFACE_(IDirectInputDeviceW, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDeviceW methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEW,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEW) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
};

typedef struct IDirectInputDeviceW *LPDIRECTINPUTDEVICEW;

#undef INTERFACE
#define INTERFACE IDirectInputDeviceA

DECLARE_INTERFACE_(IDirectInputDeviceA, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDeviceA methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
};

typedef struct IDirectInputDeviceA *LPDIRECTINPUTDEVICEA;

#ifdef UNICODE
#define IID_IDirectInputDevice IID_IDirectInputDeviceW
#define IDirectInputDevice IDirectInputDeviceW
#define IDirectInputDeviceVtbl IDirectInputDeviceWVtbl
#else
#define IID_IDirectInputDevice IID_IDirectInputDeviceA
#define IDirectInputDevice IDirectInputDeviceA
#define IDirectInputDeviceVtbl IDirectInputDeviceAVtbl
#endif
typedef struct IDirectInputDevice *LPDIRECTINPUTDEVICE;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInputDevice_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInputDevice_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInputDevice_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInputDevice_GetCapabilities(p,a) (p)->lpVtbl->GetCapabilities(p,a)
#define IDirectInputDevice_EnumObjects(p,a,b,c) (p)->lpVtbl->EnumObjects(p,a,b,c)
#define IDirectInputDevice_GetProperty(p,a,b) (p)->lpVtbl->GetProperty(p,a,b)
#define IDirectInputDevice_SetProperty(p,a,b) (p)->lpVtbl->SetProperty(p,a,b)
#define IDirectInputDevice_Acquire(p) (p)->lpVtbl->Acquire(p)
#define IDirectInputDevice_Unacquire(p) (p)->lpVtbl->Unacquire(p)
#define IDirectInputDevice_GetDeviceState(p,a,b) (p)->lpVtbl->GetDeviceState(p,a,b)
#define IDirectInputDevice_GetDeviceData(p,a,b,c,d) (p)->lpVtbl->GetDeviceData(p,a,b,c,d)
#define IDirectInputDevice_SetDataFormat(p,a) (p)->lpVtbl->SetDataFormat(p,a)
#define IDirectInputDevice_SetEventNotification(p,a) (p)->lpVtbl->SetEventNotification(p,a)
#define IDirectInputDevice_SetCooperativeLevel(p,a,b) (p)->lpVtbl->SetCooperativeLevel(p,a,b)
#define IDirectInputDevice_GetObjectInfo(p,a,b,c) (p)->lpVtbl->GetObjectInfo(p,a,b,c)
#define IDirectInputDevice_GetDeviceInfo(p,a) (p)->lpVtbl->GetDeviceInfo(p,a)
#define IDirectInputDevice_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInputDevice_Initialize(p,a,b,c) (p)->lpVtbl->Initialize(p,a,b,c)
#else
#define IDirectInputDevice_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInputDevice_AddRef(p) (p)->AddRef()
#define IDirectInputDevice_Release(p) (p)->Release()
#define IDirectInputDevice_GetCapabilities(p,a) (p)->GetCapabilities(a)
#define IDirectInputDevice_EnumObjects(p,a,b,c) (p)->EnumObjects(a,b,c)
#define IDirectInputDevice_GetProperty(p,a,b) (p)->GetProperty(a,b)
#define IDirectInputDevice_SetProperty(p,a,b) (p)->SetProperty(a,b)
#define IDirectInputDevice_Acquire(p) (p)->Acquire()
#define IDirectInputDevice_Unacquire(p) (p)->Unacquire()
#define IDirectInputDevice_GetDeviceState(p,a,b) (p)->GetDeviceState(a,b)
#define IDirectInputDevice_GetDeviceData(p,a,b,c,d) (p)->GetDeviceData(a,b,c,d)
#define IDirectInputDevice_SetDataFormat(p,a) (p)->SetDataFormat(a)
#define IDirectInputDevice_SetEventNotification(p,a) (p)->SetEventNotification(a)
#define IDirectInputDevice_SetCooperativeLevel(p,a,b) (p)->SetCooperativeLevel(a,b)
#define IDirectInputDevice_GetObjectInfo(p,a,b,c) (p)->GetObjectInfo(a,b,c)
#define IDirectInputDevice_GetDeviceInfo(p,a) (p)->GetDeviceInfo(a)
#define IDirectInputDevice_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInputDevice_Initialize(p,a,b,c) (p)->Initialize(a,b,c)
#endif

#endif /* DIJ_RINGZERO */


#if(DIRECTINPUT_VERSION >= 0x0500)

#define DISFFC_RESET            0x00000001
#define DISFFC_STOPALL          0x00000002
#define DISFFC_PAUSE            0x00000004
#define DISFFC_CONTINUE         0x00000008
#define DISFFC_SETACTUATORSON   0x00000010
#define DISFFC_SETACTUATORSOFF  0x00000020

#define DIGFFS_EMPTY            0x00000001
#define DIGFFS_STOPPED          0x00000002
#define DIGFFS_PAUSED           0x00000004
#define DIGFFS_ACTUATORSON      0x00000010
#define DIGFFS_ACTUATORSOFF     0x00000020
#define DIGFFS_POWERON          0x00000040
#define DIGFFS_POWEROFF         0x00000080
#define DIGFFS_SAFETYSWITCHON   0x00000100
#define DIGFFS_SAFETYSWITCHOFF  0x00000200
#define DIGFFS_USERFFSWITCHON   0x00000400
#define DIGFFS_USERFFSWITCHOFF  0x00000800
#define DIGFFS_DEVICELOST       0x80000000

#ifndef DIJ_RINGZERO

typedef struct DIEFFECTINFOA {
    DWORD   dwSize;
    GUID    guid;
    DWORD   dwEffType;
    DWORD   dwStaticParams;
    DWORD   dwDynamicParams;
    CHAR    tszName[MAX_PATH];
} DIEFFECTINFOA, *LPDIEFFECTINFOA;
typedef struct DIEFFECTINFOW {
    DWORD   dwSize;
    GUID    guid;
    DWORD   dwEffType;
    DWORD   dwStaticParams;
    DWORD   dwDynamicParams;
    WCHAR   tszName[MAX_PATH];
} DIEFFECTINFOW, *LPDIEFFECTINFOW;
#ifdef UNICODE
typedef DIEFFECTINFOW DIEFFECTINFO;
typedef LPDIEFFECTINFOW LPDIEFFECTINFO;
#else
typedef DIEFFECTINFOA DIEFFECTINFO;
typedef LPDIEFFECTINFOA LPDIEFFECTINFO;
#endif // UNICODE
typedef const DIEFFECTINFOA *LPCDIEFFECTINFOA;
typedef const DIEFFECTINFOW *LPCDIEFFECTINFOW;
typedef const DIEFFECTINFO  *LPCDIEFFECTINFO;

#define DISDD_CONTINUE          0x00000001

typedef BOOL (FAR PASCAL * LPDIENUMEFFECTSCALLBACKA)(LPCDIEFFECTINFOA, LPVOID);
typedef BOOL (FAR PASCAL * LPDIENUMEFFECTSCALLBACKW)(LPCDIEFFECTINFOW, LPVOID);
#ifdef UNICODE
#define LPDIENUMEFFECTSCALLBACK  LPDIENUMEFFECTSCALLBACKW
#else
#define LPDIENUMEFFECTSCALLBACK  LPDIENUMEFFECTSCALLBACKA
#endif // !UNICODE
typedef BOOL (FAR PASCAL * LPDIENUMCREATEDEFFECTOBJECTSCALLBACK)(LPDIRECTINPUTEFFECT, LPVOID);

#undef INTERFACE
#define INTERFACE IDirectInputDevice2W

DECLARE_INTERFACE_(IDirectInputDevice2W, IDirectInputDeviceW)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDeviceW methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEW,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEW) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;

    /*** IDirectInputDevice2W methods ***/
    STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN) PURE;
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOW,REFGUID) PURE;
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) PURE;
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) PURE;
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
    STDMETHOD(Poll)(THIS) PURE;
    STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
};

typedef struct IDirectInputDevice2W *LPDIRECTINPUTDEVICE2W;

#undef INTERFACE
#define INTERFACE IDirectInputDevice2A

DECLARE_INTERFACE_(IDirectInputDevice2A, IDirectInputDeviceA)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDeviceA methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;

    /*** IDirectInputDevice2A methods ***/
    STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN) PURE;
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOA,REFGUID) PURE;
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) PURE;
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) PURE;
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
    STDMETHOD(Poll)(THIS) PURE;
    STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
};

typedef struct IDirectInputDevice2A *LPDIRECTINPUTDEVICE2A;

#ifdef UNICODE
#define IID_IDirectInputDevice2 IID_IDirectInputDevice2W
#define IDirectInputDevice2 IDirectInputDevice2W
#define IDirectInputDevice2Vtbl IDirectInputDevice2WVtbl
#else
#define IID_IDirectInputDevice2 IID_IDirectInputDevice2A
#define IDirectInputDevice2 IDirectInputDevice2A
#define IDirectInputDevice2Vtbl IDirectInputDevice2AVtbl
#endif
typedef struct IDirectInputDevice2 *LPDIRECTINPUTDEVICE2;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInputDevice2_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInputDevice2_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInputDevice2_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInputDevice2_GetCapabilities(p,a) (p)->lpVtbl->GetCapabilities(p,a)
#define IDirectInputDevice2_EnumObjects(p,a,b,c) (p)->lpVtbl->EnumObjects(p,a,b,c)
#define IDirectInputDevice2_GetProperty(p,a,b) (p)->lpVtbl->GetProperty(p,a,b)
#define IDirectInputDevice2_SetProperty(p,a,b) (p)->lpVtbl->SetProperty(p,a,b)
#define IDirectInputDevice2_Acquire(p) (p)->lpVtbl->Acquire(p)
#define IDirectInputDevice2_Unacquire(p) (p)->lpVtbl->Unacquire(p)
#define IDirectInputDevice2_GetDeviceState(p,a,b) (p)->lpVtbl->GetDeviceState(p,a,b)
#define IDirectInputDevice2_GetDeviceData(p,a,b,c,d) (p)->lpVtbl->GetDeviceData(p,a,b,c,d)
#define IDirectInputDevice2_SetDataFormat(p,a) (p)->lpVtbl->SetDataFormat(p,a)
#define IDirectInputDevice2_SetEventNotification(p,a) (p)->lpVtbl->SetEventNotification(p,a)
#define IDirectInputDevice2_SetCooperativeLevel(p,a,b) (p)->lpVtbl->SetCooperativeLevel(p,a,b)
#define IDirectInputDevice2_GetObjectInfo(p,a,b,c) (p)->lpVtbl->GetObjectInfo(p,a,b,c)
#define IDirectInputDevice2_GetDeviceInfo(p,a) (p)->lpVtbl->GetDeviceInfo(p,a)
#define IDirectInputDevice2_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInputDevice2_Initialize(p,a,b,c) (p)->lpVtbl->Initialize(p,a,b,c)
#define IDirectInputDevice2_CreateEffect(p,a,b,c,d) (p)->lpVtbl->CreateEffect(p,a,b,c,d)
#define IDirectInputDevice2_EnumEffects(p,a,b,c) (p)->lpVtbl->EnumEffects(p,a,b,c)
#define IDirectInputDevice2_GetEffectInfo(p,a,b) (p)->lpVtbl->GetEffectInfo(p,a,b)
#define IDirectInputDevice2_GetForceFeedbackState(p,a) (p)->lpVtbl->GetForceFeedbackState(p,a)
#define IDirectInputDevice2_SendForceFeedbackCommand(p,a) (p)->lpVtbl->SendForceFeedbackCommand(p,a)
#define IDirectInputDevice2_EnumCreatedEffectObjects(p,a,b,c) (p)->lpVtbl->EnumCreatedEffectObjects(p,a,b,c)
#define IDirectInputDevice2_Escape(p,a) (p)->lpVtbl->Escape(p,a)
#define IDirectInputDevice2_Poll(p) (p)->lpVtbl->Poll(p)
#define IDirectInputDevice2_SendDeviceData(p,a,b,c,d) (p)->lpVtbl->SendDeviceData(p,a,b,c,d)
#else
#define IDirectInputDevice2_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInputDevice2_AddRef(p) (p)->AddRef()
#define IDirectInputDevice2_Release(p) (p)->Release()
#define IDirectInputDevice2_GetCapabilities(p,a) (p)->GetCapabilities(a)
#define IDirectInputDevice2_EnumObjects(p,a,b,c) (p)->EnumObjects(a,b,c)
#define IDirectInputDevice2_GetProperty(p,a,b) (p)->GetProperty(a,b)
#define IDirectInputDevice2_SetProperty(p,a,b) (p)->SetProperty(a,b)
#define IDirectInputDevice2_Acquire(p) (p)->Acquire()
#define IDirectInputDevice2_Unacquire(p) (p)->Unacquire()
#define IDirectInputDevice2_GetDeviceState(p,a,b) (p)->GetDeviceState(a,b)
#define IDirectInputDevice2_GetDeviceData(p,a,b,c,d) (p)->GetDeviceData(a,b,c,d)
#define IDirectInputDevice2_SetDataFormat(p,a) (p)->SetDataFormat(a)
#define IDirectInputDevice2_SetEventNotification(p,a) (p)->SetEventNotification(a)
#define IDirectInputDevice2_SetCooperativeLevel(p,a,b) (p)->SetCooperativeLevel(a,b)
#define IDirectInputDevice2_GetObjectInfo(p,a,b,c) (p)->GetObjectInfo(a,b,c)
#define IDirectInputDevice2_GetDeviceInfo(p,a) (p)->GetDeviceInfo(a)
#define IDirectInputDevice2_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInputDevice2_Initialize(p,a,b,c) (p)->Initialize(a,b,c)
#define IDirectInputDevice2_CreateEffect(p,a,b,c,d) (p)->CreateEffect(a,b,c,d)
#define IDirectInputDevice2_EnumEffects(p,a,b,c) (p)->EnumEffects(a,b,c)
#define IDirectInputDevice2_GetEffectInfo(p,a,b) (p)->GetEffectInfo(a,b)
#define IDirectInputDevice2_GetForceFeedbackState(p,a) (p)->GetForceFeedbackState(a)
#define IDirectInputDevice2_SendForceFeedbackCommand(p,a) (p)->SendForceFeedbackCommand(a)
#define IDirectInputDevice2_EnumCreatedEffectObjects(p,a,b,c) (p)->EnumCreatedEffectObjects(a,b,c)
#define IDirectInputDevice2_Escape(p,a) (p)->Escape(a)
#define IDirectInputDevice2_Poll(p) (p)->Poll()
#define IDirectInputDevice2_SendDeviceData(p,a,b,c,d) (p)->SendDeviceData(a,b,c,d)
#endif

#endif /* DIJ_RINGZERO */

#endif /* DIRECTINPUT_VERSION >= 0x0500 */

#if(DIRECTINPUT_VERSION >= 0x0700)
#define DIFEF_DEFAULT               0x00000000
#define DIFEF_INCLUDENONSTANDARD    0x00000001
#define DIFEF_MODIFYIFNEEDED            0x00000010

#ifndef DIJ_RINGZERO

#undef INTERFACE
#define INTERFACE IDirectInputDevice7W

DECLARE_INTERFACE_(IDirectInputDevice7W, IDirectInputDevice2W)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDevice2W methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEW,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEW) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
    STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN) PURE;
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOW,REFGUID) PURE;
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) PURE;
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) PURE;
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
    STDMETHOD(Poll)(THIS) PURE;
    STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;

    /*** IDirectInputDevice7W methods ***/
    STDMETHOD(EnumEffectsInFile)(THIS_ LPCWSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(WriteEffectToFile)(THIS_ LPCWSTR,DWORD,LPDIFILEEFFECT,DWORD) PURE;
};

typedef struct IDirectInputDevice7W *LPDIRECTINPUTDEVICE7W;

#undef INTERFACE
#define INTERFACE IDirectInputDevice7A

DECLARE_INTERFACE_(IDirectInputDevice7A, IDirectInputDevice2A)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDevice2A methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
    STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN) PURE;
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOA,REFGUID) PURE;
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) PURE;
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) PURE;
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
    STDMETHOD(Poll)(THIS) PURE;
    STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;

    /*** IDirectInputDevice7A methods ***/
    STDMETHOD(EnumEffectsInFile)(THIS_ LPCSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(WriteEffectToFile)(THIS_ LPCSTR,DWORD,LPDIFILEEFFECT,DWORD) PURE;
};

typedef struct IDirectInputDevice7A *LPDIRECTINPUTDEVICE7A;

#ifdef UNICODE
#define IID_IDirectInputDevice7 IID_IDirectInputDevice7W
#define IDirectInputDevice7 IDirectInputDevice7W
#define IDirectInputDevice7Vtbl IDirectInputDevice7WVtbl
#else
#define IID_IDirectInputDevice7 IID_IDirectInputDevice7A
#define IDirectInputDevice7 IDirectInputDevice7A
#define IDirectInputDevice7Vtbl IDirectInputDevice7AVtbl
#endif
typedef struct IDirectInputDevice7 *LPDIRECTINPUTDEVICE7;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInputDevice7_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInputDevice7_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInputDevice7_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInputDevice7_GetCapabilities(p,a) (p)->lpVtbl->GetCapabilities(p,a)
#define IDirectInputDevice7_EnumObjects(p,a,b,c) (p)->lpVtbl->EnumObjects(p,a,b,c)
#define IDirectInputDevice7_GetProperty(p,a,b) (p)->lpVtbl->GetProperty(p,a,b)
#define IDirectInputDevice7_SetProperty(p,a,b) (p)->lpVtbl->SetProperty(p,a,b)
#define IDirectInputDevice7_Acquire(p) (p)->lpVtbl->Acquire(p)
#define IDirectInputDevice7_Unacquire(p) (p)->lpVtbl->Unacquire(p)
#define IDirectInputDevice7_GetDeviceState(p,a,b) (p)->lpVtbl->GetDeviceState(p,a,b)
#define IDirectInputDevice7_GetDeviceData(p,a,b,c,d) (p)->lpVtbl->GetDeviceData(p,a,b,c,d)
#define IDirectInputDevice7_SetDataFormat(p,a) (p)->lpVtbl->SetDataFormat(p,a)
#define IDirectInputDevice7_SetEventNotification(p,a) (p)->lpVtbl->SetEventNotification(p,a)
#define IDirectInputDevice7_SetCooperativeLevel(p,a,b) (p)->lpVtbl->SetCooperativeLevel(p,a,b)
#define IDirectInputDevice7_GetObjectInfo(p,a,b,c) (p)->lpVtbl->GetObjectInfo(p,a,b,c)
#define IDirectInputDevice7_GetDeviceInfo(p,a) (p)->lpVtbl->GetDeviceInfo(p,a)
#define IDirectInputDevice7_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInputDevice7_Initialize(p,a,b,c) (p)->lpVtbl->Initialize(p,a,b,c)
#define IDirectInputDevice7_CreateEffect(p,a,b,c,d) (p)->lpVtbl->CreateEffect(p,a,b,c,d)
#define IDirectInputDevice7_EnumEffects(p,a,b,c) (p)->lpVtbl->EnumEffects(p,a,b,c)
#define IDirectInputDevice7_GetEffectInfo(p,a,b) (p)->lpVtbl->GetEffectInfo(p,a,b)
#define IDirectInputDevice7_GetForceFeedbackState(p,a) (p)->lpVtbl->GetForceFeedbackState(p,a)
#define IDirectInputDevice7_SendForceFeedbackCommand(p,a) (p)->lpVtbl->SendForceFeedbackCommand(p,a)
#define IDirectInputDevice7_EnumCreatedEffectObjects(p,a,b,c) (p)->lpVtbl->EnumCreatedEffectObjects(p,a,b,c)
#define IDirectInputDevice7_Escape(p,a) (p)->lpVtbl->Escape(p,a)
#define IDirectInputDevice7_Poll(p) (p)->lpVtbl->Poll(p)
#define IDirectInputDevice7_SendDeviceData(p,a,b,c,d) (p)->lpVtbl->SendDeviceData(p,a,b,c,d)
#define IDirectInputDevice7_EnumEffectsInFile(p,a,b,c,d) (p)->lpVtbl->EnumEffectsInFile(p,a,b,c,d)
#define IDirectInputDevice7_WriteEffectToFile(p,a,b,c,d) (p)->lpVtbl->WriteEffectToFile(p,a,b,c,d)
#else
#define IDirectInputDevice7_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInputDevice7_AddRef(p) (p)->AddRef()
#define IDirectInputDevice7_Release(p) (p)->Release()
#define IDirectInputDevice7_GetCapabilities(p,a) (p)->GetCapabilities(a)
#define IDirectInputDevice7_EnumObjects(p,a,b,c) (p)->EnumObjects(a,b,c)
#define IDirectInputDevice7_GetProperty(p,a,b) (p)->GetProperty(a,b)
#define IDirectInputDevice7_SetProperty(p,a,b) (p)->SetProperty(a,b)
#define IDirectInputDevice7_Acquire(p) (p)->Acquire()
#define IDirectInputDevice7_Unacquire(p) (p)->Unacquire()
#define IDirectInputDevice7_GetDeviceState(p,a,b) (p)->GetDeviceState(a,b)
#define IDirectInputDevice7_GetDeviceData(p,a,b,c,d) (p)->GetDeviceData(a,b,c,d)
#define IDirectInputDevice7_SetDataFormat(p,a) (p)->SetDataFormat(a)
#define IDirectInputDevice7_SetEventNotification(p,a) (p)->SetEventNotification(a)
#define IDirectInputDevice7_SetCooperativeLevel(p,a,b) (p)->SetCooperativeLevel(a,b)
#define IDirectInputDevice7_GetObjectInfo(p,a,b,c) (p)->GetObjectInfo(a,b,c)
#define IDirectInputDevice7_GetDeviceInfo(p,a) (p)->GetDeviceInfo(a)
#define IDirectInputDevice7_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInputDevice7_Initialize(p,a,b,c) (p)->Initialize(a,b,c)
#define IDirectInputDevice7_CreateEffect(p,a,b,c,d) (p)->CreateEffect(a,b,c,d)
#define IDirectInputDevice7_EnumEffects(p,a,b,c) (p)->EnumEffects(a,b,c)
#define IDirectInputDevice7_GetEffectInfo(p,a,b) (p)->GetEffectInfo(a,b)
#define IDirectInputDevice7_GetForceFeedbackState(p,a) (p)->GetForceFeedbackState(a)
#define IDirectInputDevice7_SendForceFeedbackCommand(p,a) (p)->SendForceFeedbackCommand(a)
#define IDirectInputDevice7_EnumCreatedEffectObjects(p,a,b,c) (p)->EnumCreatedEffectObjects(a,b,c)
#define IDirectInputDevice7_Escape(p,a) (p)->Escape(a)
#define IDirectInputDevice7_Poll(p) (p)->Poll()
#define IDirectInputDevice7_SendDeviceData(p,a,b,c,d) (p)->SendDeviceData(a,b,c,d)
#define IDirectInputDevice7_EnumEffectsInFile(p,a,b,c,d) (p)->EnumEffectsInFile(a,b,c,d)
#define IDirectInputDevice7_WriteEffectToFile(p,a,b,c,d) (p)->WriteEffectToFile(a,b,c,d)
#endif

#endif /* DIJ_RINGZERO */

#endif /* DIRECTINPUT_VERSION >= 0x0700 */

#if(DIRECTINPUT_VERSION >= 0x0800)

#ifndef DIJ_RINGZERO

#undef INTERFACE
#define INTERFACE IDirectInputDevice8W

DECLARE_INTERFACE_(IDirectInputDevice8W, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDevice8W methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEW,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEW) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
    STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN) PURE;
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOW,REFGUID) PURE;
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) PURE;
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) PURE;
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
    STDMETHOD(Poll)(THIS) PURE;
    STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(EnumEffectsInFile)(THIS_ LPCWSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(WriteEffectToFile)(THIS_ LPCWSTR,DWORD,LPDIFILEEFFECT,DWORD) PURE;
    STDMETHOD(BuildActionMap)(THIS_ LPDIACTIONFORMATW,LPCWSTR,DWORD) PURE;
    STDMETHOD(SetActionMap)(THIS_ LPDIACTIONFORMATW,LPCWSTR,DWORD) PURE;
    STDMETHOD(GetImageInfo)(THIS_ LPDIDEVICEIMAGEINFOHEADERW) PURE;
};

typedef struct IDirectInputDevice8W *LPDIRECTINPUTDEVICE8W;

#undef INTERFACE
#define INTERFACE IDirectInputDevice8A

DECLARE_INTERFACE_(IDirectInputDevice8A, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputDevice8A methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) PURE;
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) PURE;
    STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) PURE;
    STDMETHOD(Acquire)(THIS) PURE;
    STDMETHOD(Unacquire)(THIS) PURE;
    STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
    STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) PURE;
    STDMETHOD(SetEventNotification)(THIS_ HANDLE) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA,DWORD,DWORD) PURE;
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) PURE;
    STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN) PURE;
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOA,REFGUID) PURE;
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) PURE;
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) PURE;
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) PURE;
    STDMETHOD(Poll)(THIS) PURE;
    STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;
    STDMETHOD(EnumEffectsInFile)(THIS_ LPCSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(WriteEffectToFile)(THIS_ LPCSTR,DWORD,LPDIFILEEFFECT,DWORD) PURE;
    STDMETHOD(BuildActionMap)(THIS_ LPDIACTIONFORMATA,LPCSTR,DWORD) PURE;
    STDMETHOD(SetActionMap)(THIS_ LPDIACTIONFORMATA,LPCSTR,DWORD) PURE;
    STDMETHOD(GetImageInfo)(THIS_ LPDIDEVICEIMAGEINFOHEADERA) PURE;
};

typedef struct IDirectInputDevice8A *LPDIRECTINPUTDEVICE8A;

#ifdef UNICODE
#define IID_IDirectInputDevice8 IID_IDirectInputDevice8W
#define IDirectInputDevice8 IDirectInputDevice8W
#define IDirectInputDevice8Vtbl IDirectInputDevice8WVtbl
#else
#define IID_IDirectInputDevice8 IID_IDirectInputDevice8A
#define IDirectInputDevice8 IDirectInputDevice8A
#define IDirectInputDevice8Vtbl IDirectInputDevice8AVtbl
#endif
typedef struct IDirectInputDevice8 *LPDIRECTINPUTDEVICE8;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInputDevice8_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInputDevice8_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInputDevice8_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInputDevice8_GetCapabilities(p,a) (p)->lpVtbl->GetCapabilities(p,a)
#define IDirectInputDevice8_EnumObjects(p,a,b,c) (p)->lpVtbl->EnumObjects(p,a,b,c)
#define IDirectInputDevice8_GetProperty(p,a,b) (p)->lpVtbl->GetProperty(p,a,b)
#define IDirectInputDevice8_SetProperty(p,a,b) (p)->lpVtbl->SetProperty(p,a,b)
#define IDirectInputDevice8_Acquire(p) (p)->lpVtbl->Acquire(p)
#define IDirectInputDevice8_Unacquire(p) (p)->lpVtbl->Unacquire(p)
#define IDirectInputDevice8_GetDeviceState(p,a,b) (p)->lpVtbl->GetDeviceState(p,a,b)
#define IDirectInputDevice8_GetDeviceData(p,a,b,c,d) (p)->lpVtbl->GetDeviceData(p,a,b,c,d)
#define IDirectInputDevice8_SetDataFormat(p,a) (p)->lpVtbl->SetDataFormat(p,a)
#define IDirectInputDevice8_SetEventNotification(p,a) (p)->lpVtbl->SetEventNotification(p,a)
#define IDirectInputDevice8_SetCooperativeLevel(p,a,b) (p)->lpVtbl->SetCooperativeLevel(p,a,b)
#define IDirectInputDevice8_GetObjectInfo(p,a,b,c) (p)->lpVtbl->GetObjectInfo(p,a,b,c)
#define IDirectInputDevice8_GetDeviceInfo(p,a) (p)->lpVtbl->GetDeviceInfo(p,a)
#define IDirectInputDevice8_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInputDevice8_Initialize(p,a,b,c) (p)->lpVtbl->Initialize(p,a,b,c)
#define IDirectInputDevice8_CreateEffect(p,a,b,c,d) (p)->lpVtbl->CreateEffect(p,a,b,c,d)
#define IDirectInputDevice8_EnumEffects(p,a,b,c) (p)->lpVtbl->EnumEffects(p,a,b,c)
#define IDirectInputDevice8_GetEffectInfo(p,a,b) (p)->lpVtbl->GetEffectInfo(p,a,b)
#define IDirectInputDevice8_GetForceFeedbackState(p,a) (p)->lpVtbl->GetForceFeedbackState(p,a)
#define IDirectInputDevice8_SendForceFeedbackCommand(p,a) (p)->lpVtbl->SendForceFeedbackCommand(p,a)
#define IDirectInputDevice8_EnumCreatedEffectObjects(p,a,b,c) (p)->lpVtbl->EnumCreatedEffectObjects(p,a,b,c)
#define IDirectInputDevice8_Escape(p,a) (p)->lpVtbl->Escape(p,a)
#define IDirectInputDevice8_Poll(p) (p)->lpVtbl->Poll(p)
#define IDirectInputDevice8_SendDeviceData(p,a,b,c,d) (p)->lpVtbl->SendDeviceData(p,a,b,c,d)
#define IDirectInputDevice8_EnumEffectsInFile(p,a,b,c,d) (p)->lpVtbl->EnumEffectsInFile(p,a,b,c,d)
#define IDirectInputDevice8_WriteEffectToFile(p,a,b,c,d) (p)->lpVtbl->WriteEffectToFile(p,a,b,c,d)
#define IDirectInputDevice8_BuildActionMap(p,a,b,c) (p)->lpVtbl->BuildActionMap(p,a,b,c)
#define IDirectInputDevice8_SetActionMap(p,a,b,c) (p)->lpVtbl->SetActionMap(p,a,b,c)
#define IDirectInputDevice8_GetImageInfo(p,a) (p)->lpVtbl->GetImageInfo(p,a)
#else
#define IDirectInputDevice8_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInputDevice8_AddRef(p) (p)->AddRef()
#define IDirectInputDevice8_Release(p) (p)->Release()
#define IDirectInputDevice8_GetCapabilities(p,a) (p)->GetCapabilities(a)
#define IDirectInputDevice8_EnumObjects(p,a,b,c) (p)->EnumObjects(a,b,c)
#define IDirectInputDevice8_GetProperty(p,a,b) (p)->GetProperty(a,b)
#define IDirectInputDevice8_SetProperty(p,a,b) (p)->SetProperty(a,b)
#define IDirectInputDevice8_Acquire(p) (p)->Acquire()
#define IDirectInputDevice8_Unacquire(p) (p)->Unacquire()
#define IDirectInputDevice8_GetDeviceState(p,a,b) (p)->GetDeviceState(a,b)
#define IDirectInputDevice8_GetDeviceData(p,a,b,c,d) (p)->GetDeviceData(a,b,c,d)
#define IDirectInputDevice8_SetDataFormat(p,a) (p)->SetDataFormat(a)
#define IDirectInputDevice8_SetEventNotification(p,a) (p)->SetEventNotification(a)
#define IDirectInputDevice8_SetCooperativeLevel(p,a,b) (p)->SetCooperativeLevel(a,b)
#define IDirectInputDevice8_GetObjectInfo(p,a,b,c) (p)->GetObjectInfo(a,b,c)
#define IDirectInputDevice8_GetDeviceInfo(p,a) (p)->GetDeviceInfo(a)
#define IDirectInputDevice8_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInputDevice8_Initialize(p,a,b,c) (p)->Initialize(a,b,c)
#define IDirectInputDevice8_CreateEffect(p,a,b,c,d) (p)->CreateEffect(a,b,c,d)
#define IDirectInputDevice8_EnumEffects(p,a,b,c) (p)->EnumEffects(a,b,c)
#define IDirectInputDevice8_GetEffectInfo(p,a,b) (p)->GetEffectInfo(a,b)
#define IDirectInputDevice8_GetForceFeedbackState(p,a) (p)->GetForceFeedbackState(a)
#define IDirectInputDevice8_SendForceFeedbackCommand(p,a) (p)->SendForceFeedbackCommand(a)
#define IDirectInputDevice8_EnumCreatedEffectObjects(p,a,b,c) (p)->EnumCreatedEffectObjects(a,b,c)
#define IDirectInputDevice8_Escape(p,a) (p)->Escape(a)
#define IDirectInputDevice8_Poll(p) (p)->Poll()
#define IDirectInputDevice8_SendDeviceData(p,a,b,c,d) (p)->SendDeviceData(a,b,c,d)
#define IDirectInputDevice8_EnumEffectsInFile(p,a,b,c,d) (p)->EnumEffectsInFile(a,b,c,d)
#define IDirectInputDevice8_WriteEffectToFile(p,a,b,c,d) (p)->WriteEffectToFile(a,b,c,d)
#define IDirectInputDevice8_BuildActionMap(p,a,b,c) (p)->BuildActionMap(a,b,c)
#define IDirectInputDevice8_SetActionMap(p,a,b,c) (p)->SetActionMap(a,b,c)
#define IDirectInputDevice8_GetImageInfo(p,a) (p)->GetImageInfo(a)
#endif

#endif /* DIJ_RINGZERO */

#endif /* DIRECTINPUT_VERSION >= 0x0800 */

/****************************************************************************
 *
 *      Mouse
 *
 ****************************************************************************/

#ifndef DIJ_RINGZERO

typedef struct _DIMOUSESTATE {
    LONG    lX;
    LONG    lY;
    LONG    lZ;
    BYTE    rgbButtons[4];
} DIMOUSESTATE, *LPDIMOUSESTATE;

#if DIRECTINPUT_VERSION >= 0x0700
typedef struct _DIMOUSESTATE2 {
    LONG    lX;
    LONG    lY;
    LONG    lZ;
    BYTE    rgbButtons[8];
} DIMOUSESTATE2, *LPDIMOUSESTATE2;
#endif


#define DIMOFS_X        FIELD_OFFSET(DIMOUSESTATE, lX)
#define DIMOFS_Y        FIELD_OFFSET(DIMOUSESTATE, lY)
#define DIMOFS_Z        FIELD_OFFSET(DIMOUSESTATE, lZ)
#define DIMOFS_BUTTON0 (FIELD_OFFSET(DIMOUSESTATE, rgbButtons) + 0)
#define DIMOFS_BUTTON1 (FIELD_OFFSET(DIMOUSESTATE, rgbButtons) + 1)
#define DIMOFS_BUTTON2 (FIELD_OFFSET(DIMOUSESTATE, rgbButtons) + 2)
#define DIMOFS_BUTTON3 (FIELD_OFFSET(DIMOUSESTATE, rgbButtons) + 3)
#if (DIRECTINPUT_VERSION >= 0x0700)
#define DIMOFS_BUTTON4 (FIELD_OFFSET(DIMOUSESTATE2, rgbButtons) + 4)
#define DIMOFS_BUTTON5 (FIELD_OFFSET(DIMOUSESTATE2, rgbButtons) + 5)
#define DIMOFS_BUTTON6 (FIELD_OFFSET(DIMOUSESTATE2, rgbButtons) + 6)
#define DIMOFS_BUTTON7 (FIELD_OFFSET(DIMOUSESTATE2, rgbButtons) + 7)
#endif
#endif /* DIJ_RINGZERO */

/****************************************************************************
 *
 *      Keyboard
 *
 ****************************************************************************/

#ifndef DIJ_RINGZERO

/****************************************************************************
 *
 *      DirectInput keyboard scan codes
 *
 ****************************************************************************/
#define DIK_ESCAPE          0x01
#define DIK_1               0x02
#define DIK_2               0x03
#define DIK_3               0x04
#define DIK_4               0x05
#define DIK_5               0x06
#define DIK_6               0x07
#define DIK_7               0x08
#define DIK_8               0x09
#define DIK_9               0x0A
#define DIK_0               0x0B
#define DIK_MINUS           0x0C    /* - on main keyboard */
#define DIK_EQUALS          0x0D
#define DIK_BACK            0x0E    /* backspace */
#define DIK_TAB             0x0F
#define DIK_Q               0x10
#define DIK_W               0x11
#define DIK_E               0x12
#define DIK_R               0x13
#define DIK_T               0x14
#define DIK_Y               0x15
#define DIK_U               0x16
#define DIK_I               0x17
#define DIK_O               0x18
#define DIK_P               0x19
#define DIK_LBRACKET        0x1A
#define DIK_RBRACKET        0x1B
#define DIK_RETURN          0x1C    /* Enter on main keyboard */
#define DIK_LCONTROL        0x1D
#define DIK_A               0x1E
#define DIK_S               0x1F
#define DIK_D               0x20
#define DIK_F               0x21
#define DIK_G               0x22
#define DIK_H               0x23
#define DIK_J               0x24
#define DIK_K               0x25
#define DIK_L               0x26
#define DIK_SEMICOLON       0x27
#define DIK_APOSTROPHE      0x28
#define DIK_GRAVE           0x29    /* accent grave */
#define DIK_LSHIFT          0x2A
#define DIK_BACKSLASH       0x2B
#define DIK_Z               0x2C
#define DIK_X               0x2D
#define DIK_C               0x2E
#define DIK_V               0x2F
#define DIK_B               0x30
#define DIK_N               0x31
#define DIK_M               0x32
#define DIK_COMMA           0x33
#define DIK_PERIOD          0x34    /* . on main keyboard */
#define DIK_SLASH           0x35    /* / on main keyboard */
#define DIK_RSHIFT          0x36
#define DIK_MULTIPLY        0x37    /* * on numeric keypad */
#define DIK_LMENU           0x38    /* left Alt */
#define DIK_SPACE           0x39
#define DIK_CAPITAL         0x3A
#define DIK_F1              0x3B
#define DIK_F2              0x3C
#define DIK_F3              0x3D
#define DIK_F4              0x3E
#define DIK_F5              0x3F
#define DIK_F6              0x40
#define DIK_F7              0x41
#define DIK_F8              0x42
#define DIK_F9              0x43
#define DIK_F10             0x44
#define DIK_NUMLOCK         0x45
#define DIK_SCROLL          0x46    /* Scroll Lock */
#define DIK_NUMPAD7         0x47
#define DIK_NUMPAD8         0x48
#define DIK_NUMPAD9         0x49
#define DIK_SUBTRACT        0x4A    /* - on numeric keypad */
#define DIK_NUMPAD4         0x4B
#define DIK_NUMPAD5         0x4C
#define DIK_NUMPAD6         0x4D
#define DIK_ADD             0x4E    /* + on numeric keypad */
#define DIK_NUMPAD1         0x4F
#define DIK_NUMPAD2         0x50
#define DIK_NUMPAD3         0x51
#define DIK_NUMPAD0         0x52
#define DIK_DECIMAL         0x53    /* . on numeric keypad */
#define DIK_OEM_102         0x56    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define DIK_F11             0x57
#define DIK_F12             0x58
#define DIK_F13             0x64    /*                     (NEC PC98) */
#define DIK_F14             0x65    /*                     (NEC PC98) */
#define DIK_F15             0x66    /*                     (NEC PC98) */
#define DIK_KANA            0x70    /* (Japanese keyboard)            */
#define DIK_ABNT_C1         0x73    /* /? on Brazilian keyboard */
#define DIK_CONVERT         0x79    /* (Japanese keyboard)            */
#define DIK_NOCONVERT       0x7B    /* (Japanese keyboard)            */
#define DIK_YEN             0x7D    /* (Japanese keyboard)            */
#define DIK_ABNT_C2         0x7E    /* Numpad . on Brazilian keyboard */
#define DIK_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define DIK_PREVTRACK       0x90    /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
#define DIK_AT              0x91    /*                     (NEC PC98) */
#define DIK_COLON           0x92    /*                     (NEC PC98) */
#define DIK_UNDERLINE       0x93    /*                     (NEC PC98) */
#define DIK_KANJI           0x94    /* (Japanese keyboard)            */
#define DIK_STOP            0x95    /*                     (NEC PC98) */
#define DIK_AX              0x96    /*                     (Japan AX) */
#define DIK_UNLABELED       0x97    /*                        (J3100) */
#define DIK_NEXTTRACK       0x99    /* Next Track */
#define DIK_NUMPADENTER     0x9C    /* Enter on numeric keypad */
#define DIK_RCONTROL        0x9D
#define DIK_MUTE            0xA0    /* Mute */
#define DIK_CALCULATOR      0xA1    /* Calculator */
#define DIK_PLAYPAUSE       0xA2    /* Play / Pause */
#define DIK_MEDIASTOP       0xA4    /* Media Stop */
#define DIK_VOLUMEDOWN      0xAE    /* Volume - */
#define DIK_VOLUMEUP        0xB0    /* Volume + */
#define DIK_WEBHOME         0xB2    /* Web home */
#define DIK_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define DIK_DIVIDE          0xB5    /* / on numeric keypad */
#define DIK_SYSRQ           0xB7
#define DIK_RMENU           0xB8    /* right Alt */
#define DIK_PAUSE           0xC5    /* Pause */
#define DIK_HOME            0xC7    /* Home on arrow keypad */
#define DIK_UP              0xC8    /* UpArrow on arrow keypad */
#define DIK_PRIOR           0xC9    /* PgUp on arrow keypad */
#define DIK_LEFT            0xCB    /* LeftArrow on arrow keypad */
#define DIK_RIGHT           0xCD    /* RightArrow on arrow keypad */
#define DIK_END             0xCF    /* End on arrow keypad */
#define DIK_DOWN            0xD0    /* DownArrow on arrow keypad */
#define DIK_NEXT            0xD1    /* PgDn on arrow keypad */
#define DIK_INSERT          0xD2    /* Insert on arrow keypad */
#define DIK_DELETE          0xD3    /* Delete on arrow keypad */
#define DIK_LWIN            0xDB    /* Left Windows key */
#define DIK_RWIN            0xDC    /* Right Windows key */
#define DIK_APPS            0xDD    /* AppMenu key */
#define DIK_POWER           0xDE    /* System Power */
#define DIK_SLEEP           0xDF    /* System Sleep */
#define DIK_WAKE            0xE3    /* System Wake */
#define DIK_WEBSEARCH       0xE5    /* Web Search */
#define DIK_WEBFAVORITES    0xE6    /* Web Favorites */
#define DIK_WEBREFRESH      0xE7    /* Web Refresh */
#define DIK_WEBSTOP         0xE8    /* Web Stop */
#define DIK_WEBFORWARD      0xE9    /* Web Forward */
#define DIK_WEBBACK         0xEA    /* Web Back */
#define DIK_MYCOMPUTER      0xEB    /* My Computer */
#define DIK_MAIL            0xEC    /* Mail */
#define DIK_MEDIASELECT     0xED    /* Media Select */

/*
 *  Alternate names for keys, to facilitate transition from DOS.
 */
#define DIK_BACKSPACE       DIK_BACK            /* backspace */
#define DIK_NUMPADSTAR      DIK_MULTIPLY        /* * on numeric keypad */
#define DIK_LALT            DIK_LMENU           /* left Alt */
#define DIK_CAPSLOCK        DIK_CAPITAL         /* CapsLock */
#define DIK_NUMPADMINUS     DIK_SUBTRACT        /* - on numeric keypad */
#define DIK_NUMPADPLUS      DIK_ADD             /* + on numeric keypad */
#define DIK_NUMPADPERIOD    DIK_DECIMAL         /* . on numeric keypad */
#define DIK_NUMPADSLASH     DIK_DIVIDE          /* / on numeric keypad */
#define DIK_RALT            DIK_RMENU           /* right Alt */
#define DIK_UPARROW         DIK_UP              /* UpArrow on arrow keypad */
#define DIK_PGUP            DIK_PRIOR           /* PgUp on arrow keypad */
#define DIK_LEFTARROW       DIK_LEFT            /* LeftArrow on arrow keypad */
#define DIK_RIGHTARROW      DIK_RIGHT           /* RightArrow on arrow keypad */
#define DIK_DOWNARROW       DIK_DOWN            /* DownArrow on arrow keypad */
#define DIK_PGDN            DIK_NEXT            /* PgDn on arrow keypad */

/*
 *  Alternate names for keys originally not used on US keyboards.
 */
#define DIK_CIRCUMFLEX      DIK_PREVTRACK       /* Japanese keyboard */

#endif /* DIJ_RINGZERO */

/****************************************************************************
 *
 *      Joystick
 *
 ****************************************************************************/

#ifndef DIJ_RINGZERO

typedef struct DIJOYSTATE {
    LONG    lX;                     /* x-axis position              */
    LONG    lY;                     /* y-axis position              */
    LONG    lZ;                     /* z-axis position              */
    LONG    lRx;                    /* x-axis rotation              */
    LONG    lRy;                    /* y-axis rotation              */
    LONG    lRz;                    /* z-axis rotation              */
    LONG    rglSlider[2];           /* extra axes positions         */
    DWORD   rgdwPOV[4];             /* POV directions               */
    BYTE    rgbButtons[32];         /* 32 buttons                   */
} DIJOYSTATE, *LPDIJOYSTATE;

typedef struct DIJOYSTATE2 {
    LONG    lX;                     /* x-axis position              */
    LONG    lY;                     /* y-axis position              */
    LONG    lZ;                     /* z-axis position              */
    LONG    lRx;                    /* x-axis rotation              */
    LONG    lRy;                    /* y-axis rotation              */
    LONG    lRz;                    /* z-axis rotation              */
    LONG    rglSlider[2];           /* extra axes positions         */
    DWORD   rgdwPOV[4];             /* POV directions               */
    BYTE    rgbButtons[128];        /* 128 buttons                  */
    LONG    lVX;                    /* x-axis velocity              */
    LONG    lVY;                    /* y-axis velocity              */
    LONG    lVZ;                    /* z-axis velocity              */
    LONG    lVRx;                   /* x-axis angular velocity      */
    LONG    lVRy;                   /* y-axis angular velocity      */
    LONG    lVRz;                   /* z-axis angular velocity      */
    LONG    rglVSlider[2];          /* extra axes velocities        */
    LONG    lAX;                    /* x-axis acceleration          */
    LONG    lAY;                    /* y-axis acceleration          */
    LONG    lAZ;                    /* z-axis acceleration          */
    LONG    lARx;                   /* x-axis angular acceleration  */
    LONG    lARy;                   /* y-axis angular acceleration  */
    LONG    lARz;                   /* z-axis angular acceleration  */
    LONG    rglASlider[2];          /* extra axes accelerations     */
    LONG    lFX;                    /* x-axis force                 */
    LONG    lFY;                    /* y-axis force                 */
    LONG    lFZ;                    /* z-axis force                 */
    LONG    lFRx;                   /* x-axis torque                */
    LONG    lFRy;                   /* y-axis torque                */
    LONG    lFRz;                   /* z-axis torque                */
    LONG    rglFSlider[2];          /* extra axes forces            */
} DIJOYSTATE2, *LPDIJOYSTATE2;

#define DIJOFS_X            FIELD_OFFSET(DIJOYSTATE, lX)
#define DIJOFS_Y            FIELD_OFFSET(DIJOYSTATE, lY)
#define DIJOFS_Z            FIELD_OFFSET(DIJOYSTATE, lZ)
#define DIJOFS_RX           FIELD_OFFSET(DIJOYSTATE, lRx)
#define DIJOFS_RY           FIELD_OFFSET(DIJOYSTATE, lRy)
#define DIJOFS_RZ           FIELD_OFFSET(DIJOYSTATE, lRz)
#define DIJOFS_SLIDER(n)   (FIELD_OFFSET(DIJOYSTATE, rglSlider) + \
                                                        (n) * sizeof(LONG))
#define DIJOFS_POV(n)      (FIELD_OFFSET(DIJOYSTATE, rgdwPOV) + \
                                                        (n) * sizeof(DWORD))
#define DIJOFS_BUTTON(n)   (FIELD_OFFSET(DIJOYSTATE, rgbButtons) + (n))
#define DIJOFS_BUTTON0      DIJOFS_BUTTON(0)
#define DIJOFS_BUTTON1      DIJOFS_BUTTON(1)
#define DIJOFS_BUTTON2      DIJOFS_BUTTON(2)
#define DIJOFS_BUTTON3      DIJOFS_BUTTON(3)
#define DIJOFS_BUTTON4      DIJOFS_BUTTON(4)
#define DIJOFS_BUTTON5      DIJOFS_BUTTON(5)
#define DIJOFS_BUTTON6      DIJOFS_BUTTON(6)
#define DIJOFS_BUTTON7      DIJOFS_BUTTON(7)
#define DIJOFS_BUTTON8      DIJOFS_BUTTON(8)
#define DIJOFS_BUTTON9      DIJOFS_BUTTON(9)
#define DIJOFS_BUTTON10     DIJOFS_BUTTON(10)
#define DIJOFS_BUTTON11     DIJOFS_BUTTON(11)
#define DIJOFS_BUTTON12     DIJOFS_BUTTON(12)
#define DIJOFS_BUTTON13     DIJOFS_BUTTON(13)
#define DIJOFS_BUTTON14     DIJOFS_BUTTON(14)
#define DIJOFS_BUTTON15     DIJOFS_BUTTON(15)
#define DIJOFS_BUTTON16     DIJOFS_BUTTON(16)
#define DIJOFS_BUTTON17     DIJOFS_BUTTON(17)
#define DIJOFS_BUTTON18     DIJOFS_BUTTON(18)
#define DIJOFS_BUTTON19     DIJOFS_BUTTON(19)
#define DIJOFS_BUTTON20     DIJOFS_BUTTON(20)
#define DIJOFS_BUTTON21     DIJOFS_BUTTON(21)
#define DIJOFS_BUTTON22     DIJOFS_BUTTON(22)
#define DIJOFS_BUTTON23     DIJOFS_BUTTON(23)
#define DIJOFS_BUTTON24     DIJOFS_BUTTON(24)
#define DIJOFS_BUTTON25     DIJOFS_BUTTON(25)
#define DIJOFS_BUTTON26     DIJOFS_BUTTON(26)
#define DIJOFS_BUTTON27     DIJOFS_BUTTON(27)
#define DIJOFS_BUTTON28     DIJOFS_BUTTON(28)
#define DIJOFS_BUTTON29     DIJOFS_BUTTON(29)
#define DIJOFS_BUTTON30     DIJOFS_BUTTON(30)
#define DIJOFS_BUTTON31     DIJOFS_BUTTON(31)


#endif /* DIJ_RINGZERO */

/****************************************************************************
 *
 *  IDirectInput
 *
 ****************************************************************************/

#ifndef DIJ_RINGZERO

#define DIENUM_STOP             0
#define DIENUM_CONTINUE         1

typedef BOOL (FAR PASCAL * LPDIENUMDEVICESCALLBACKA)(LPCDIDEVICEINSTANCEA, LPVOID);
typedef BOOL (FAR PASCAL * LPDIENUMDEVICESCALLBACKW)(LPCDIDEVICEINSTANCEW, LPVOID);
#ifdef UNICODE
#define LPDIENUMDEVICESCALLBACK  LPDIENUMDEVICESCALLBACKW
#else
#define LPDIENUMDEVICESCALLBACK  LPDIENUMDEVICESCALLBACKA
#endif // !UNICODE
typedef BOOL (FAR PASCAL * LPDICONFIGUREDEVICESCALLBACK)(IUnknown FAR *, LPVOID);

#define DIEDFL_ALLDEVICES       0x00000000
#define DIEDFL_ATTACHEDONLY     0x00000001
#if(DIRECTINPUT_VERSION >= 0x0500)
#define DIEDFL_FORCEFEEDBACK    0x00000100
#endif /* DIRECTINPUT_VERSION >= 0x0500 */
#if(DIRECTINPUT_VERSION >= 0x050a)
#define DIEDFL_INCLUDEALIASES   0x00010000
#define DIEDFL_INCLUDEPHANTOMS  0x00020000
#endif /* DIRECTINPUT_VERSION >= 0x050a */
#if(DIRECTINPUT_VERSION >= 0x0800)
#define DIEDFL_INCLUDEHIDDEN    0x00040000
#endif /* DIRECTINPUT_VERSION >= 0x0800 */


#if(DIRECTINPUT_VERSION >= 0x0800)
typedef BOOL (FAR PASCAL * LPDIENUMDEVICESBYSEMANTICSCBA)(LPCDIDEVICEINSTANCEA, LPDIRECTINPUTDEVICE8A, DWORD, DWORD, LPVOID);
typedef BOOL (FAR PASCAL * LPDIENUMDEVICESBYSEMANTICSCBW)(LPCDIDEVICEINSTANCEW, LPDIRECTINPUTDEVICE8W, DWORD, DWORD, LPVOID);
#ifdef UNICODE
#define LPDIENUMDEVICESBYSEMANTICSCB  LPDIENUMDEVICESBYSEMANTICSCBW
#else
#define LPDIENUMDEVICESBYSEMANTICSCB  LPDIENUMDEVICESBYSEMANTICSCBA
#endif // !UNICODE
#endif /* DIRECTINPUT_VERSION >= 0x0800 */

#if(DIRECTINPUT_VERSION >= 0x0800)
#define DIEDBS_MAPPEDPRI1         0x00000001
#define DIEDBS_MAPPEDPRI2         0x00000002
#define DIEDBS_RECENTDEVICE       0x00000010
#define DIEDBS_NEWDEVICE          0x00000020
#endif /* DIRECTINPUT_VERSION >= 0x0800 */

#if(DIRECTINPUT_VERSION >= 0x0800)
#define DIEDBSFL_ATTACHEDONLY       0x00000000
#define DIEDBSFL_THISUSER           0x00000010
#define DIEDBSFL_FORCEFEEDBACK      DIEDFL_FORCEFEEDBACK
#define DIEDBSFL_AVAILABLEDEVICES   0x00001000
#define DIEDBSFL_MULTIMICEKEYBOARDS 0x00002000
#define DIEDBSFL_NONGAMINGDEVICES   0x00004000
#define DIEDBSFL_VALID              0x00007110
#endif /* DIRECTINPUT_VERSION >= 0x0800 */

#undef INTERFACE
#define INTERFACE IDirectInputW

DECLARE_INTERFACE_(IDirectInputW, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputW methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEW *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;
};

typedef struct IDirectInputW *LPDIRECTINPUTW;

#undef INTERFACE
#define INTERFACE IDirectInputA

DECLARE_INTERFACE_(IDirectInputA, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputA methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEA *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;
};

typedef struct IDirectInputA *LPDIRECTINPUTA;

#ifdef UNICODE
#define IID_IDirectInput IID_IDirectInputW
#define IDirectInput IDirectInputW
#define IDirectInputVtbl IDirectInputWVtbl
#else
#define IID_IDirectInput IID_IDirectInputA
#define IDirectInput IDirectInputA
#define IDirectInputVtbl IDirectInputAVtbl
#endif
typedef struct IDirectInput *LPDIRECTINPUT;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInput_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInput_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInput_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInput_CreateDevice(p,a,b,c) (p)->lpVtbl->CreateDevice(p,a,b,c)
#define IDirectInput_EnumDevices(p,a,b,c,d) (p)->lpVtbl->EnumDevices(p,a,b,c,d)
#define IDirectInput_GetDeviceStatus(p,a) (p)->lpVtbl->GetDeviceStatus(p,a)
#define IDirectInput_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInput_Initialize(p,a,b) (p)->lpVtbl->Initialize(p,a,b)
#else
#define IDirectInput_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInput_AddRef(p) (p)->AddRef()
#define IDirectInput_Release(p) (p)->Release()
#define IDirectInput_CreateDevice(p,a,b,c) (p)->CreateDevice(a,b,c)
#define IDirectInput_EnumDevices(p,a,b,c,d) (p)->EnumDevices(a,b,c,d)
#define IDirectInput_GetDeviceStatus(p,a) (p)->GetDeviceStatus(a)
#define IDirectInput_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInput_Initialize(p,a,b) (p)->Initialize(a,b)
#endif

#undef INTERFACE
#define INTERFACE IDirectInput2W

DECLARE_INTERFACE_(IDirectInput2W, IDirectInputW)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputW methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEW *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;

    /*** IDirectInput2W methods ***/
    STDMETHOD(FindDevice)(THIS_ REFGUID,LPCWSTR,LPGUID) PURE;
};

typedef struct IDirectInput2W *LPDIRECTINPUT2W;

#undef INTERFACE
#define INTERFACE IDirectInput2A

DECLARE_INTERFACE_(IDirectInput2A, IDirectInputA)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInputA methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEA *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;

    /*** IDirectInput2A methods ***/
    STDMETHOD(FindDevice)(THIS_ REFGUID,LPCSTR,LPGUID) PURE;
};

typedef struct IDirectInput2A *LPDIRECTINPUT2A;

#ifdef UNICODE
#define IID_IDirectInput2 IID_IDirectInput2W
#define IDirectInput2 IDirectInput2W
#define IDirectInput2Vtbl IDirectInput2WVtbl
#else
#define IID_IDirectInput2 IID_IDirectInput2A
#define IDirectInput2 IDirectInput2A
#define IDirectInput2Vtbl IDirectInput2AVtbl
#endif
typedef struct IDirectInput2 *LPDIRECTINPUT2;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInput2_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInput2_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInput2_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInput2_CreateDevice(p,a,b,c) (p)->lpVtbl->CreateDevice(p,a,b,c)
#define IDirectInput2_EnumDevices(p,a,b,c,d) (p)->lpVtbl->EnumDevices(p,a,b,c,d)
#define IDirectInput2_GetDeviceStatus(p,a) (p)->lpVtbl->GetDeviceStatus(p,a)
#define IDirectInput2_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInput2_Initialize(p,a,b) (p)->lpVtbl->Initialize(p,a,b)
#define IDirectInput2_FindDevice(p,a,b,c) (p)->lpVtbl->FindDevice(p,a,b,c)
#else
#define IDirectInput2_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInput2_AddRef(p) (p)->AddRef()
#define IDirectInput2_Release(p) (p)->Release()
#define IDirectInput2_CreateDevice(p,a,b,c) (p)->CreateDevice(a,b,c)
#define IDirectInput2_EnumDevices(p,a,b,c,d) (p)->EnumDevices(a,b,c,d)
#define IDirectInput2_GetDeviceStatus(p,a) (p)->GetDeviceStatus(a)
#define IDirectInput2_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInput2_Initialize(p,a,b) (p)->Initialize(a,b)
#define IDirectInput2_FindDevice(p,a,b,c) (p)->FindDevice(a,b,c)
#endif


#undef INTERFACE
#define INTERFACE IDirectInput7W

DECLARE_INTERFACE_(IDirectInput7W, IDirectInput2W)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInput2W methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEW *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;
    STDMETHOD(FindDevice)(THIS_ REFGUID,LPCWSTR,LPGUID) PURE;

    /*** IDirectInput7W methods ***/
    STDMETHOD(CreateDeviceEx)(THIS_ REFGUID,REFIID,LPVOID *,LPUNKNOWN) PURE;
};

typedef struct IDirectInput7W *LPDIRECTINPUT7W;

#undef INTERFACE
#define INTERFACE IDirectInput7A

DECLARE_INTERFACE_(IDirectInput7A, IDirectInput2A)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInput2A methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEA *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;
    STDMETHOD(FindDevice)(THIS_ REFGUID,LPCSTR,LPGUID) PURE;

    /*** IDirectInput7A methods ***/
    STDMETHOD(CreateDeviceEx)(THIS_ REFGUID,REFIID,LPVOID *,LPUNKNOWN) PURE;
};

typedef struct IDirectInput7A *LPDIRECTINPUT7A;

#ifdef UNICODE
#define IID_IDirectInput7 IID_IDirectInput7W
#define IDirectInput7 IDirectInput7W
#define IDirectInput7Vtbl IDirectInput7WVtbl
#else
#define IID_IDirectInput7 IID_IDirectInput7A
#define IDirectInput7 IDirectInput7A
#define IDirectInput7Vtbl IDirectInput7AVtbl
#endif
typedef struct IDirectInput7 *LPDIRECTINPUT7;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInput7_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInput7_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInput7_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInput7_CreateDevice(p,a,b,c) (p)->lpVtbl->CreateDevice(p,a,b,c)
#define IDirectInput7_EnumDevices(p,a,b,c,d) (p)->lpVtbl->EnumDevices(p,a,b,c,d)
#define IDirectInput7_GetDeviceStatus(p,a) (p)->lpVtbl->GetDeviceStatus(p,a)
#define IDirectInput7_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInput7_Initialize(p,a,b) (p)->lpVtbl->Initialize(p,a,b)
#define IDirectInput7_FindDevice(p,a,b,c) (p)->lpVtbl->FindDevice(p,a,b,c)
#define IDirectInput7_CreateDeviceEx(p,a,b,c,d) (p)->lpVtbl->CreateDeviceEx(p,a,b,c,d)
#else
#define IDirectInput7_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInput7_AddRef(p) (p)->AddRef()
#define IDirectInput7_Release(p) (p)->Release()
#define IDirectInput7_CreateDevice(p,a,b,c) (p)->CreateDevice(a,b,c)
#define IDirectInput7_EnumDevices(p,a,b,c,d) (p)->EnumDevices(a,b,c,d)
#define IDirectInput7_GetDeviceStatus(p,a) (p)->GetDeviceStatus(a)
#define IDirectInput7_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInput7_Initialize(p,a,b) (p)->Initialize(a,b)
#define IDirectInput7_FindDevice(p,a,b,c) (p)->FindDevice(a,b,c)
#define IDirectInput7_CreateDeviceEx(p,a,b,c,d) (p)->CreateDeviceEx(a,b,c,d)
#endif

#if(DIRECTINPUT_VERSION >= 0x0800)
#undef INTERFACE
#define INTERFACE IDirectInput8W

DECLARE_INTERFACE_(IDirectInput8W, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInput8W methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICE8W *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKW,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;
    STDMETHOD(FindDevice)(THIS_ REFGUID,LPCWSTR,LPGUID) PURE;
    STDMETHOD(EnumDevicesBySemantics)(THIS_ LPCWSTR,LPDIACTIONFORMATW,LPDIENUMDEVICESBYSEMANTICSCBW,LPVOID,DWORD) PURE;
    STDMETHOD(ConfigureDevices)(THIS_ LPDICONFIGUREDEVICESCALLBACK,LPDICONFIGUREDEVICESPARAMSW,DWORD,LPVOID) PURE;
};

typedef struct IDirectInput8W *LPDIRECTINPUT8W;

#undef INTERFACE
#define INTERFACE IDirectInput8A

DECLARE_INTERFACE_(IDirectInput8A, IUnknown)
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirectInput8A methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICE8A *,LPUNKNOWN) PURE;
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKA,LPVOID,DWORD) PURE;
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) PURE;
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) PURE;
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) PURE;
    STDMETHOD(FindDevice)(THIS_ REFGUID,LPCSTR,LPGUID) PURE;
    STDMETHOD(EnumDevicesBySemantics)(THIS_ LPCSTR,LPDIACTIONFORMATA,LPDIENUMDEVICESBYSEMANTICSCBA,LPVOID,DWORD) PURE;
    STDMETHOD(ConfigureDevices)(THIS_ LPDICONFIGUREDEVICESCALLBACK,LPDICONFIGUREDEVICESPARAMSA,DWORD,LPVOID) PURE;
};

typedef struct IDirectInput8A *LPDIRECTINPUT8A;

#ifdef UNICODE
#define IID_IDirectInput8 IID_IDirectInput8W
#define IDirectInput8 IDirectInput8W
#define IDirectInput8Vtbl IDirectInput8WVtbl
#else
#define IID_IDirectInput8 IID_IDirectInput8A
#define IDirectInput8 IDirectInput8A
#define IDirectInput8Vtbl IDirectInput8AVtbl
#endif
typedef struct IDirectInput8 *LPDIRECTINPUT8;

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectInput8_QueryInterface(p,a,b) (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectInput8_AddRef(p) (p)->lpVtbl->AddRef(p)
#define IDirectInput8_Release(p) (p)->lpVtbl->Release(p)
#define IDirectInput8_CreateDevice(p,a,b,c) (p)->lpVtbl->CreateDevice(p,a,b,c)
#define IDirectInput8_EnumDevices(p,a,b,c,d) (p)->lpVtbl->EnumDevices(p,a,b,c,d)
#define IDirectInput8_GetDeviceStatus(p,a) (p)->lpVtbl->GetDeviceStatus(p,a)
#define IDirectInput8_RunControlPanel(p,a,b) (p)->lpVtbl->RunControlPanel(p,a,b)
#define IDirectInput8_Initialize(p,a,b) (p)->lpVtbl->Initialize(p,a,b)
#define IDirectInput8_FindDevice(p,a,b,c) (p)->lpVtbl->FindDevice(p,a,b,c)
#define IDirectInput8_EnumDevicesBySemantics(p,a,b,c,d,e) (p)->lpVtbl->EnumDevicesBySemantics(p,a,b,c,d,e)
#define IDirectInput8_ConfigureDevices(p,a,b,c,d) (p)->lpVtbl->ConfigureDevices(p,a,b,c,d)
#else
#define IDirectInput8_QueryInterface(p,a,b) (p)->QueryInterface(a,b)
#define IDirectInput8_AddRef(p) (p)->AddRef()
#define IDirectInput8_Release(p) (p)->Release()
#define IDirectInput8_CreateDevice(p,a,b,c) (p)->CreateDevice(a,b,c)
#define IDirectInput8_EnumDevices(p,a,b,c,d) (p)->EnumDevices(a,b,c,d)
#define IDirectInput8_GetDeviceStatus(p,a) (p)->GetDeviceStatus(a)
#define IDirectInput8_RunControlPanel(p,a,b) (p)->RunControlPanel(a,b)
#define IDirectInput8_Initialize(p,a,b) (p)->Initialize(a,b)
#define IDirectInput8_FindDevice(p,a,b,c) (p)->FindDevice(a,b,c)
#define IDirectInput8_EnumDevicesBySemantics(p,a,b,c,d,e) (p)->EnumDevicesBySemantics(a,b,c,d,e)
#define IDirectInput8_ConfigureDevices(p,a,b,c,d) (p)->ConfigureDevices(a,b,c,d)
#endif
#endif /* DIRECTINPUT_VERSION >= 0x0800 */

// mrNuku - commented this out for resolving conflicting linkages
/*#if DIRECTINPUT_VERSION > 0x0700

extern HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);

#else
extern HRESULT WINAPI DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA *ppDI, LPUNKNOWN punkOuter);
extern HRESULT WINAPI DirectInputCreateW(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW *ppDI, LPUNKNOWN punkOuter);
#ifdef UNICODE
#define DirectInputCreate  DirectInputCreateW
#else
#define DirectInputCreate  DirectInputCreateA
#endif // !UNICODE

extern HRESULT WINAPI DirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);

#endif*/ /* DIRECTINPUT_VERSION > 0x700 */

#endif /* DIJ_RINGZERO */


/****************************************************************************
 *
 *  Return Codes
 *
 ****************************************************************************/

/*
 *  The operation completed successfully.
 */
#define DI_OK                           S_OK

/*
 *  The device exists but is not currently attached.
 */
#define DI_NOTATTACHED                  S_FALSE

/*
 *  The device buffer overflowed.  Some input was lost.
 */
#define DI_BUFFEROVERFLOW               S_FALSE

/*
 *  The change in device properties had no effect.
 */
#define DI_PROPNOEFFECT                 S_FALSE

/*
 *  The operation had no effect.
 */
#define DI_NOEFFECT                     S_FALSE

/*
 *  The device is a polled device.  As a result, device buffering
 *  will not collect any data and event notifications will not be
 *  signalled until GetDeviceState is called.
 */
#define DI_POLLEDDEVICE                 ((HRESULT)0x00000002L)

/*
 *  The parameters of the effect were successfully updated by
 *  IDirectInputEffect::SetParameters, but the effect was not
 *  downloaded because the device is not exclusively acquired
 *  or because the DIEP_NODOWNLOAD flag was passed.
 */
#define DI_DOWNLOADSKIPPED              ((HRESULT)0x00000003L)

/*
 *  The parameters of the effect were successfully updated by
 *  IDirectInputEffect::SetParameters, but in order to change
 *  the parameters, the effect needed to be restarted.
 */
#define DI_EFFECTRESTARTED              ((HRESULT)0x00000004L)

/*
 *  The parameters of the effect were successfully updated by
 *  IDirectInputEffect::SetParameters, but some of them were
 *  beyond the capabilities of the device and were truncated.
 */
#define DI_TRUNCATED                    ((HRESULT)0x00000008L)

/*
 *  The settings have been successfully applied but could not be 
 *  persisted. 
 */
#define DI_SETTINGSNOTSAVED				((HRESULT)0x0000000BL)

/*
 *  Equal to DI_EFFECTRESTARTED | DI_TRUNCATED.
 */
#define DI_TRUNCATEDANDRESTARTED        ((HRESULT)0x0000000CL)

/*
 *  A SUCCESS code indicating that settings cannot be modified.
 */
#define DI_WRITEPROTECT                 ((HRESULT)0x00000013L)

/*
 *  The application requires a newer version of DirectInput.
 */
#define DIERR_OLDDIRECTINPUTVERSION     \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_OLD_WIN_VERSION)

/*
 *  The application was written for an unsupported prerelease version
 *  of DirectInput.
 */
#define DIERR_BETADIRECTINPUTVERSION    \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_RMODE_APP)

/*
 *  The object could not be created due to an incompatible driver version
 *  or mismatched or incomplete driver components.
 */
#define DIERR_BADDRIVERVER              \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_BAD_DRIVER_LEVEL)

/*
 * The device or device instance or effect is not registered with DirectInput.
 */
#define DIERR_DEVICENOTREG              REGDB_E_CLASSNOTREG

/*
 * The requested object does not exist.
 */
#define DIERR_NOTFOUND                  \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_FILE_NOT_FOUND)

/*
 * The requested object does not exist.
 */
#define DIERR_OBJECTNOTFOUND            \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_FILE_NOT_FOUND)

/*
 * An invalid parameter was passed to the returning function,
 * or the object was not in a state that admitted the function
 * to be called.
 */
#define DIERR_INVALIDPARAM              E_INVALIDARG

/*
 * The specified interface is not supported by the object
 */
#define DIERR_NOINTERFACE               E_NOINTERFACE

/*
 * An undetermined error occured inside the DInput subsystem
 */
#define DIERR_GENERIC                   E_FAIL

/*
 * The DInput subsystem couldn't allocate sufficient memory to complete the
 * caller's request.
 */
#define DIERR_OUTOFMEMORY               E_OUTOFMEMORY

/*
 * The function called is not supported at this time
 */
#define DIERR_UNSUPPORTED               E_NOTIMPL

/*
 * This object has not been initialized
 */
#define DIERR_NOTINITIALIZED            \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_NOT_READY)

/*
 * This object is already initialized
 */
#define DIERR_ALREADYINITIALIZED        \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_ALREADY_INITIALIZED)

/*
 * This object does not support aggregation
 */
#define DIERR_NOAGGREGATION             CLASS_E_NOAGGREGATION

/*
 * Another app has a higher priority level, preventing this call from
 * succeeding.
 */
#define DIERR_OTHERAPPHASPRIO           E_ACCESSDENIED

/*
 * Access to the device has been lost.  It must be re-acquired.
 */
#define DIERR_INPUTLOST                 \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_READ_FAULT)

/*
 * The operation cannot be performed while the device is acquired.
 */
#define DIERR_ACQUIRED                  \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_BUSY)

/*
 * The operation cannot be performed unless the device is acquired.
 */
#define DIERR_NOTACQUIRED               \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_INVALID_ACCESS)

/*
 * The specified property cannot be changed.
 */
#define DIERR_READONLY                  E_ACCESSDENIED

/*
 * The device already has an event notification associated with it.
 */
#define DIERR_HANDLEEXISTS              E_ACCESSDENIED

/*
 * Data is not yet available.
 */
#ifndef E_PENDING
#define E_PENDING                       0x8000000AL
#endif

/*
 * Unable to IDirectInputJoyConfig_Acquire because the user
 * does not have sufficient privileges to change the joystick
 * configuration.
 */
#define DIERR_INSUFFICIENTPRIVS         0x80040200L

/*
 * The device is full.
 */
#define DIERR_DEVICEFULL                0x80040201L

/*
 * Not all the requested information fit into the buffer.
 */
#define DIERR_MOREDATA                  0x80040202L

/*
 * The effect is not downloaded.
 */
#define DIERR_NOTDOWNLOADED             0x80040203L

/*
 *  The device cannot be reinitialized because there are still effects
 *  attached to it.
 */
#define DIERR_HASEFFECTS                0x80040204L

/*
 *  The operation cannot be performed unless the device is acquired
 *  in DISCL_EXCLUSIVE mode.
 */
#define DIERR_NOTEXCLUSIVEACQUIRED      0x80040205L

/*
 *  The effect could not be downloaded because essential information
 *  is missing.  For example, no axes have been associated with the
 *  effect, or no type-specific information has been created.
 */
#define DIERR_INCOMPLETEEFFECT          0x80040206L

/*
 *  Attempted to read buffered device data from a device that is
 *  not buffered.
 */
#define DIERR_NOTBUFFERED               0x80040207L

/*
 *  An attempt was made to modify parameters of an effect while it is
 *  playing.  Not all hardware devices support altering the parameters
 *  of an effect while it is playing.
 */
#define DIERR_EFFECTPLAYING             0x80040208L

/*
 *  The operation could not be completed because the device is not
 *  plugged in.
 */
#define DIERR_UNPLUGGED                 0x80040209L

/*
 *  SendDeviceData failed because more information was requested
 *  to be sent than can be sent to the device.  Some devices have
 *  restrictions on how much data can be sent to them.  (For example,
 *  there might be a limit on the number of buttons that can be
 *  pressed at once.)
 */
#define DIERR_REPORTFULL                0x8004020AL


/*
 *  A mapper file function failed because reading or writing the user or IHV 
 *  settings file failed.
 */
#define DIERR_MAPFILEFAIL               0x8004020BL


/*--- DINPUT Mapper Definitions: New for Dx8         ---*/


/*--- Keyboard
      Physical Keyboard Device       ---*/

#define DIKEYBOARD_ESCAPE                       0x81000401
#define DIKEYBOARD_1                            0x81000402
#define DIKEYBOARD_2                            0x81000403
#define DIKEYBOARD_3                            0x81000404
#define DIKEYBOARD_4                            0x81000405
#define DIKEYBOARD_5                            0x81000406
#define DIKEYBOARD_6                            0x81000407
#define DIKEYBOARD_7                            0x81000408
#define DIKEYBOARD_8                            0x81000409
#define DIKEYBOARD_9                            0x8100040A
#define DIKEYBOARD_0                            0x8100040B
#define DIKEYBOARD_MINUS                        0x8100040C    /* - on main keyboard */
#define DIKEYBOARD_EQUALS                       0x8100040D
#define DIKEYBOARD_BACK                         0x8100040E    /* backspace */
#define DIKEYBOARD_TAB                          0x8100040F
#define DIKEYBOARD_Q                            0x81000410
#define DIKEYBOARD_W                            0x81000411
#define DIKEYBOARD_E                            0x81000412
#define DIKEYBOARD_R                            0x81000413
#define DIKEYBOARD_T                            0x81000414
#define DIKEYBOARD_Y                            0x81000415
#define DIKEYBOARD_U                            0x81000416
#define DIKEYBOARD_I                            0x81000417
#define DIKEYBOARD_O                            0x81000418
#define DIKEYBOARD_P                            0x81000419
#define DIKEYBOARD_LBRACKET                     0x8100041A
#define DIKEYBOARD_RBRACKET                     0x8100041B
#define DIKEYBOARD_RETURN                       0x8100041C    /* Enter on main keyboard */
#define DIKEYBOARD_LCONTROL                     0x8100041D
#define DIKEYBOARD_A                            0x8100041E
#define DIKEYBOARD_S                            0x8100041F
#define DIKEYBOARD_D                            0x81000420
#define DIKEYBOARD_F                            0x81000421
#define DIKEYBOARD_G                            0x81000422
#define DIKEYBOARD_H                            0x81000423
#define DIKEYBOARD_J                            0x81000424
#define DIKEYBOARD_K                            0x81000425
#define DIKEYBOARD_L                            0x81000426
#define DIKEYBOARD_SEMICOLON                    0x81000427
#define DIKEYBOARD_APOSTROPHE                   0x81000428
#define DIKEYBOARD_GRAVE                        0x81000429    /* accent grave */
#define DIKEYBOARD_LSHIFT                       0x8100042A
#define DIKEYBOARD_BACKSLASH                    0x8100042B
#define DIKEYBOARD_Z                            0x8100042C
#define DIKEYBOARD_X                            0x8100042D
#define DIKEYBOARD_C                            0x8100042E
#define DIKEYBOARD_V                            0x8100042F
#define DIKEYBOARD_B                            0x81000430
#define DIKEYBOARD_N                            0x81000431
#define DIKEYBOARD_M                            0x81000432
#define DIKEYBOARD_COMMA                        0x81000433
#define DIKEYBOARD_PERIOD                       0x81000434    /* . on main keyboard */
#define DIKEYBOARD_SLASH                        0x81000435    /* / on main keyboard */
#define DIKEYBOARD_RSHIFT                       0x81000436
#define DIKEYBOARD_MULTIPLY                     0x81000437    /* * on numeric keypad */
#define DIKEYBOARD_LMENU                        0x81000438    /* left Alt */
#define DIKEYBOARD_SPACE                        0x81000439
#define DIKEYBOARD_CAPITAL                      0x8100043A
#define DIKEYBOARD_F1                           0x8100043B
#define DIKEYBOARD_F2                           0x8100043C
#define DIKEYBOARD_F3                           0x8100043D
#define DIKEYBOARD_F4                           0x8100043E
#define DIKEYBOARD_F5                           0x8100043F
#define DIKEYBOARD_F6                           0x81000440
#define DIKEYBOARD_F7                           0x81000441
#define DIKEYBOARD_F8                           0x81000442
#define DIKEYBOARD_F9                           0x81000443
#define DIKEYBOARD_F10                          0x81000444
#define DIKEYBOARD_NUMLOCK                      0x81000445
#define DIKEYBOARD_SCROLL                       0x81000446    /* Scroll Lock */
#define DIKEYBOARD_NUMPAD7                      0x81000447
#define DIKEYBOARD_NUMPAD8                      0x81000448
#define DIKEYBOARD_NUMPAD9                      0x81000449
#define DIKEYBOARD_SUBTRACT                     0x8100044A    /* - on numeric keypad */
#define DIKEYBOARD_NUMPAD4                      0x8100044B
#define DIKEYBOARD_NUMPAD5                      0x8100044C
#define DIKEYBOARD_NUMPAD6                      0x8100044D
#define DIKEYBOARD_ADD                          0x8100044E    /* + on numeric keypad */
#define DIKEYBOARD_NUMPAD1                      0x8100044F
#define DIKEYBOARD_NUMPAD2                      0x81000450
#define DIKEYBOARD_NUMPAD3                      0x81000451
#define DIKEYBOARD_NUMPAD0                      0x81000452
#define DIKEYBOARD_DECIMAL                      0x81000453    /* . on numeric keypad */
#define DIKEYBOARD_OEM_102                      0x81000456    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define DIKEYBOARD_F11                          0x81000457
#define DIKEYBOARD_F12                          0x81000458
#define DIKEYBOARD_F13                          0x81000464    /*                     (NEC PC98) */
#define DIKEYBOARD_F14                          0x81000465    /*                     (NEC PC98) */
#define DIKEYBOARD_F15                          0x81000466    /*                     (NEC PC98) */
#define DIKEYBOARD_KANA                         0x81000470    /* (Japanese keyboard)            */
#define DIKEYBOARD_ABNT_C1                      0x81000473    /* /? on Brazilian keyboard */
#define DIKEYBOARD_CONVERT                      0x81000479    /* (Japanese keyboard)            */
#define DIKEYBOARD_NOCONVERT                    0x8100047B    /* (Japanese keyboard)            */
#define DIKEYBOARD_YEN                          0x8100047D    /* (Japanese keyboard)            */
#define DIKEYBOARD_ABNT_C2                      0x8100047E    /* Numpad . on Brazilian keyboard */
#define DIKEYBOARD_NUMPADEQUALS                 0x8100048D    /* = on numeric keypad (NEC PC98) */
#define DIKEYBOARD_PREVTRACK                    0x81000490    /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
#define DIKEYBOARD_AT                           0x81000491    /*                     (NEC PC98) */
#define DIKEYBOARD_COLON                        0x81000492    /*                     (NEC PC98) */
#define DIKEYBOARD_UNDERLINE                    0x81000493    /*                     (NEC PC98) */
#define DIKEYBOARD_KANJI                        0x81000494    /* (Japanese keyboard)            */
#define DIKEYBOARD_STOP                         0x81000495    /*                     (NEC PC98) */
#define DIKEYBOARD_AX                           0x81000496    /*                     (Japan AX) */
#define DIKEYBOARD_UNLABELED                    0x81000497    /*                        (J3100) */
#define DIKEYBOARD_NEXTTRACK                    0x81000499    /* Next Track */
#define DIKEYBOARD_NUMPADENTER                  0x8100049C    /* Enter on numeric keypad */
#define DIKEYBOARD_RCONTROL                     0x8100049D
#define DIKEYBOARD_MUTE                         0x810004A0    /* Mute */
#define DIKEYBOARD_CALCULATOR                   0x810004A1    /* Calculator */
#define DIKEYBOARD_PLAYPAUSE                    0x810004A2    /* Play / Pause */
#define DIKEYBOARD_MEDIASTOP                    0x810004A4    /* Media Stop */
#define DIKEYBOARD_VOLUMEDOWN                   0x810004AE    /* Volume - */
#define DIKEYBOARD_VOLUMEUP                     0x810004B0    /* Volume + */
#define DIKEYBOARD_WEBHOME                      0x810004B2    /* Web home */
#define DIKEYBOARD_NUMPADCOMMA                  0x810004B3    /* , on numeric keypad (NEC PC98) */
#define DIKEYBOARD_DIVIDE                       0x810004B5    /* / on numeric keypad */
#define DIKEYBOARD_SYSRQ                        0x810004B7
#define DIKEYBOARD_RMENU                        0x810004B8    /* right Alt */
#define DIKEYBOARD_PAUSE                        0x810004C5    /* Pause */
#define DIKEYBOARD_HOME                         0x810004C7    /* Home on arrow keypad */
#define DIKEYBOARD_UP                           0x810004C8    /* UpArrow on arrow keypad */
#define DIKEYBOARD_PRIOR                        0x810004C9    /* PgUp on arrow keypad */
#define DIKEYBOARD_LEFT                         0x810004CB    /* LeftArrow on arrow keypad */
#define DIKEYBOARD_RIGHT                        0x810004CD    /* RightArrow on arrow keypad */
#define DIKEYBOARD_END                          0x810004CF    /* End on arrow keypad */
#define DIKEYBOARD_DOWN                         0x810004D0    /* DownArrow on arrow keypad */
#define DIKEYBOARD_NEXT                         0x810004D1    /* PgDn on arrow keypad */
#define DIKEYBOARD_INSERT                       0x810004D2    /* Insert on arrow keypad */
#define DIKEYBOARD_DELETE                       0x810004D3    /* Delete on arrow keypad */
#define DIKEYBOARD_LWIN                         0x810004DB    /* Left Windows key */
#define DIKEYBOARD_RWIN                         0x810004DC    /* Right Windows key */
#define DIKEYBOARD_APPS                         0x810004DD    /* AppMenu key */
#define DIKEYBOARD_POWER                        0x810004DE    /* System Power */
#define DIKEYBOARD_SLEEP                        0x810004DF    /* System Sleep */
#define DIKEYBOARD_WAKE                         0x810004E3    /* System Wake */
#define DIKEYBOARD_WEBSEARCH                    0x810004E5    /* Web Search */
#define DIKEYBOARD_WEBFAVORITES                 0x810004E6    /* Web Favorites */
#define DIKEYBOARD_WEBREFRESH                   0x810004E7    /* Web Refresh */
#define DIKEYBOARD_WEBSTOP                      0x810004E8    /* Web Stop */
#define DIKEYBOARD_WEBFORWARD                   0x810004E9    /* Web Forward */
#define DIKEYBOARD_WEBBACK                      0x810004EA    /* Web Back */
#define DIKEYBOARD_MYCOMPUTER                   0x810004EB    /* My Computer */
#define DIKEYBOARD_MAIL                         0x810004EC    /* Mail */
#define DIKEYBOARD_MEDIASELECT                  0x810004ED    /* Media Select */
  

/*--- MOUSE
      Physical Mouse Device             ---*/

#define DIMOUSE_XAXISAB                         (0x82000200 |DIMOFS_X ) /* X Axis-absolute: Some mice natively report absolute coordinates  */ 
#define DIMOUSE_YAXISAB                         (0x82000200 |DIMOFS_Y ) /* Y Axis-absolute: Some mice natively report absolute coordinates */
#define DIMOUSE_XAXIS                           (0x82000300 |DIMOFS_X ) /* X Axis */
#define DIMOUSE_YAXIS                           (0x82000300 |DIMOFS_Y ) /* Y Axis */
#define DIMOUSE_WHEEL                           (0x82000300 |DIMOFS_Z ) /* Z Axis */
#define DIMOUSE_BUTTON0                         (0x82000400 |DIMOFS_BUTTON0) /* Button 0 */
#define DIMOUSE_BUTTON1                         (0x82000400 |DIMOFS_BUTTON1) /* Button 1 */
#define DIMOUSE_BUTTON2                         (0x82000400 |DIMOFS_BUTTON2) /* Button 2 */
#define DIMOUSE_BUTTON3                         (0x82000400 |DIMOFS_BUTTON3) /* Button 3 */
#define DIMOUSE_BUTTON4                         (0x82000400 |DIMOFS_BUTTON4) /* Button 4 */
#define DIMOUSE_BUTTON5                         (0x82000400 |DIMOFS_BUTTON5) /* Button 5 */
#define DIMOUSE_BUTTON6                         (0x82000400 |DIMOFS_BUTTON6) /* Button 6 */
#define DIMOUSE_BUTTON7                         (0x82000400 |DIMOFS_BUTTON7) /* Button 7 */


/*--- VOICE
      Physical Dplay Voice Device       ---*/

#define DIVOICE_CHANNEL1                        0x83000401
#define DIVOICE_CHANNEL2                        0x83000402
#define DIVOICE_CHANNEL3                        0x83000403
#define DIVOICE_CHANNEL4                        0x83000404
#define DIVOICE_CHANNEL5                        0x83000405
#define DIVOICE_CHANNEL6                        0x83000406
#define DIVOICE_CHANNEL7                        0x83000407
#define DIVOICE_CHANNEL8                        0x83000408
#define DIVOICE_TEAM                            0x83000409
#define DIVOICE_ALL                             0x8300040A
#define DIVOICE_RECORDMUTE                      0x8300040B
#define DIVOICE_PLAYBACKMUTE                    0x8300040C
#define DIVOICE_TRANSMIT                        0x8300040D

#define DIVOICE_VOICECOMMAND                    0x83000410


/*--- Driving Simulator - Racing
      Vehicle control is primary objective  ---*/
#define DIVIRTUAL_DRIVING_RACE                  0x01000000
#define DIAXIS_DRIVINGR_STEER                   0x01008A01 /* Steering */
#define DIAXIS_DRIVINGR_ACCELERATE              0x01039202 /* Accelerate */
#define DIAXIS_DRIVINGR_BRAKE                   0x01041203 /* Brake-Axis */
#define DIBUTTON_DRIVINGR_SHIFTUP               0x01000C01 /* Shift to next higher gear */
#define DIBUTTON_DRIVINGR_SHIFTDOWN             0x01000C02 /* Shift to next lower gear */
#define DIBUTTON_DRIVINGR_VIEW                  0x01001C03 /* Cycle through view options */
#define DIBUTTON_DRIVINGR_MENU                  0x010004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIAXIS_DRIVINGR_ACCEL_AND_BRAKE         0x01014A04 /* Some devices combine accelerate and brake in a single axis */
#define DIHATSWITCH_DRIVINGR_GLANCE             0x01004601 /* Look around */
#define DIBUTTON_DRIVINGR_BRAKE                 0x01004C04 /* Brake-button */
#define DIBUTTON_DRIVINGR_DASHBOARD             0x01004405 /* Select next dashboard option */
#define DIBUTTON_DRIVINGR_AIDS                  0x01004406 /* Driver correction aids */
#define DIBUTTON_DRIVINGR_MAP                   0x01004407 /* Display Driving Map */
#define DIBUTTON_DRIVINGR_BOOST                 0x01004408 /* Turbo Boost */
#define DIBUTTON_DRIVINGR_PIT                   0x01004409 /* Pit stop notification */
#define DIBUTTON_DRIVINGR_ACCELERATE_LINK       0x0103D4E0 /* Fallback Accelerate button */
#define DIBUTTON_DRIVINGR_STEER_LEFT_LINK       0x0100CCE4 /* Fallback Steer Left button */
#define DIBUTTON_DRIVINGR_STEER_RIGHT_LINK      0x0100CCEC /* Fallback Steer Right button */
#define DIBUTTON_DRIVINGR_GLANCE_LEFT_LINK      0x0107C4E4 /* Fallback Glance Left button */
#define DIBUTTON_DRIVINGR_GLANCE_RIGHT_LINK     0x0107C4EC /* Fallback Glance Right button */
#define DIBUTTON_DRIVINGR_DEVICE                0x010044FE /* Show input device and controls */
#define DIBUTTON_DRIVINGR_PAUSE                 0x010044FC /* Start / Pause / Restart game */

/*--- Driving Simulator - Combat
      Combat from within a vehicle is primary objective  ---*/
#define DIVIRTUAL_DRIVING_COMBAT                0x02000000
#define DIAXIS_DRIVINGC_STEER                   0x02008A01 /* Steering  */
#define DIAXIS_DRIVINGC_ACCELERATE              0x02039202 /* Accelerate */
#define DIAXIS_DRIVINGC_BRAKE                   0x02041203 /* Brake-axis */
#define DIBUTTON_DRIVINGC_FIRE                  0x02000C01 /* Fire */
#define DIBUTTON_DRIVINGC_WEAPONS               0x02000C02 /* Select next weapon */
#define DIBUTTON_DRIVINGC_TARGET                0x02000C03 /* Select next available target */
#define DIBUTTON_DRIVINGC_MENU                  0x020004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIAXIS_DRIVINGC_ACCEL_AND_BRAKE         0x02014A04 /* Some devices combine accelerate and brake in a single axis */
#define DIHATSWITCH_DRIVINGC_GLANCE             0x02004601 /* Look around */
#define DIBUTTON_DRIVINGC_SHIFTUP               0x02004C04 /* Shift to next higher gear */
#define DIBUTTON_DRIVINGC_SHIFTDOWN             0x02004C05 /* Shift to next lower gear */
#define DIBUTTON_DRIVINGC_DASHBOARD             0x02004406 /* Select next dashboard option */
#define DIBUTTON_DRIVINGC_AIDS                  0x02004407 /* Driver correction aids */
#define DIBUTTON_DRIVINGC_BRAKE                 0x02004C08 /* Brake-button */
#define DIBUTTON_DRIVINGC_FIRESECONDARY         0x02004C09 /* Alternative fire button */
#define DIBUTTON_DRIVINGC_ACCELERATE_LINK       0x0203D4E0 /* Fallback Accelerate button */
#define DIBUTTON_DRIVINGC_STEER_LEFT_LINK       0x0200CCE4 /* Fallback Steer Left button */
#define DIBUTTON_DRIVINGC_STEER_RIGHT_LINK      0x0200CCEC /* Fallback Steer Right button */
#define DIBUTTON_DRIVINGC_GLANCE_LEFT_LINK      0x0207C4E4 /* Fallback Glance Left button */
#define DIBUTTON_DRIVINGC_GLANCE_RIGHT_LINK     0x0207C4EC /* Fallback Glance Right button */
#define DIBUTTON_DRIVINGC_DEVICE                0x020044FE /* Show input device and controls */
#define DIBUTTON_DRIVINGC_PAUSE                 0x020044FC /* Start / Pause / Restart game */

/*--- Driving Simulator - Tank
      Combat from withing a tank is primary objective  ---*/
#define DIVIRTUAL_DRIVING_TANK                  0x03000000
#define DIAXIS_DRIVINGT_STEER                   0x03008A01 /* Turn tank left / right */
#define DIAXIS_DRIVINGT_BARREL                  0x03010202 /* Raise / lower barrel */
#define DIAXIS_DRIVINGT_ACCELERATE              0x03039203 /* Accelerate */
#define DIAXIS_DRIVINGT_ROTATE                  0x03020204 /* Turn barrel left / right */
#define DIBUTTON_DRIVINGT_FIRE                  0x03000C01 /* Fire */
#define DIBUTTON_DRIVINGT_WEAPONS               0x03000C02 /* Select next weapon */
#define DIBUTTON_DRIVINGT_TARGET                0x03000C03 /* Selects next available target */
#define DIBUTTON_DRIVINGT_MENU                  0x030004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_DRIVINGT_GLANCE             0x03004601 /* Look around */
#define DIAXIS_DRIVINGT_BRAKE                   0x03045205 /* Brake-axis */
#define DIAXIS_DRIVINGT_ACCEL_AND_BRAKE         0x03014A06 /* Some devices combine accelerate and brake in a single axis */
#define DIBUTTON_DRIVINGT_VIEW                  0x03005C04 /* Cycle through view options */
#define DIBUTTON_DRIVINGT_DASHBOARD             0x03005C05 /* Select next dashboard option */
#define DIBUTTON_DRIVINGT_BRAKE                 0x03004C06 /* Brake-button */
#define DIBUTTON_DRIVINGT_FIRESECONDARY         0x03004C07 /* Alternative fire button */
#define DIBUTTON_DRIVINGT_ACCELERATE_LINK       0x0303D4E0 /* Fallback Accelerate button */
#define DIBUTTON_DRIVINGT_STEER_LEFT_LINK       0x0300CCE4 /* Fallback Steer Left button */
#define DIBUTTON_DRIVINGT_STEER_RIGHT_LINK      0x0300CCEC /* Fallback Steer Right button */
#define DIBUTTON_DRIVINGT_BARREL_UP_LINK        0x030144E0 /* Fallback Barrel up button */
#define DIBUTTON_DRIVINGT_BARREL_DOWN_LINK      0x030144E8 /* Fallback Barrel down button */
#define DIBUTTON_DRIVINGT_ROTATE_LEFT_LINK      0x030244E4 /* Fallback Rotate left button */
#define DIBUTTON_DRIVINGT_ROTATE_RIGHT_LINK     0x030244EC /* Fallback Rotate right button */
#define DIBUTTON_DRIVINGT_GLANCE_LEFT_LINK      0x0307C4E4 /* Fallback Glance Left button */
#define DIBUTTON_DRIVINGT_GLANCE_RIGHT_LINK     0x0307C4EC /* Fallback Glance Right button */
#define DIBUTTON_DRIVINGT_DEVICE                0x030044FE /* Show input device and controls */
#define DIBUTTON_DRIVINGT_PAUSE                 0x030044FC /* Start / Pause / Restart game */

/*--- Flight Simulator - Civilian 
      Plane control is the primary objective  ---*/
#define DIVIRTUAL_FLYING_CIVILIAN               0x04000000
#define DIAXIS_FLYINGC_BANK                     0x04008A01 /* Roll ship left / right */
#define DIAXIS_FLYINGC_PITCH                    0x04010A02 /* Nose up / down */
#define DIAXIS_FLYINGC_THROTTLE                 0x04039203 /* Throttle */
#define DIBUTTON_FLYINGC_VIEW                   0x04002401 /* Cycle through view options */
#define DIBUTTON_FLYINGC_DISPLAY                0x04002402 /* Select next dashboard / heads up display option */
#define DIBUTTON_FLYINGC_GEAR                   0x04002C03 /* Gear up / down */
#define DIBUTTON_FLYINGC_MENU                   0x040004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_FLYINGC_GLANCE              0x04004601 /* Look around */
#define DIAXIS_FLYINGC_BRAKE                    0x04046A04 /* Apply Brake */
#define DIAXIS_FLYINGC_RUDDER                   0x04025205 /* Yaw ship left/right */
#define DIAXIS_FLYINGC_FLAPS                    0x04055A06 /* Flaps */
#define DIBUTTON_FLYINGC_FLAPSUP                0x04006404 /* Increment stepping up until fully retracted */
#define DIBUTTON_FLYINGC_FLAPSDOWN              0x04006405 /* Decrement stepping down until fully extended */
#define DIBUTTON_FLYINGC_BRAKE_LINK             0x04046CE0 /* Fallback brake button */
#define DIBUTTON_FLYINGC_FASTER_LINK            0x0403D4E0 /* Fallback throttle up button */
#define DIBUTTON_FLYINGC_SLOWER_LINK            0x0403D4E8 /* Fallback throttle down button */
#define DIBUTTON_FLYINGC_GLANCE_LEFT_LINK       0x0407C4E4 /* Fallback Glance Left button */
#define DIBUTTON_FLYINGC_GLANCE_RIGHT_LINK      0x0407C4EC /* Fallback Glance Right button */
#define DIBUTTON_FLYINGC_GLANCE_UP_LINK         0x0407C4E0 /* Fallback Glance Up button */
#define DIBUTTON_FLYINGC_GLANCE_DOWN_LINK       0x0407C4E8 /* Fallback Glance Down button */
#define DIBUTTON_FLYINGC_DEVICE                 0x040044FE /* Show input device and controls */
#define DIBUTTON_FLYINGC_PAUSE                  0x040044FC /* Start / Pause / Restart game */

/*--- Flight Simulator - Military 
      Aerial combat is the primary objective  ---*/
#define DIVIRTUAL_FLYING_MILITARY               0x05000000
#define DIAXIS_FLYINGM_BANK                     0x05008A01 /* Bank - Roll ship left / right */
#define DIAXIS_FLYINGM_PITCH                    0x05010A02 /* Pitch - Nose up / down */
#define DIAXIS_FLYINGM_THROTTLE                 0x05039203 /* Throttle - faster / slower */
#define DIBUTTON_FLYINGM_FIRE                   0x05000C01 /* Fire */
#define DIBUTTON_FLYINGM_WEAPONS                0x05000C02 /* Select next weapon */
#define DIBUTTON_FLYINGM_TARGET                 0x05000C03 /* Selects next available target */
#define DIBUTTON_FLYINGM_MENU                   0x050004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_FLYINGM_GLANCE              0x05004601 /* Look around */
#define DIBUTTON_FLYINGM_COUNTER                0x05005C04 /* Activate counter measures */
#define DIAXIS_FLYINGM_RUDDER                   0x05024A04 /* Rudder - Yaw ship left/right */
#define DIAXIS_FLYINGM_BRAKE                    0x05046205 /* Brake-axis */
#define DIBUTTON_FLYINGM_VIEW                   0x05006405 /* Cycle through view options */
#define DIBUTTON_FLYINGM_DISPLAY                0x05006406 /* Select next dashboard option */
#define DIAXIS_FLYINGM_FLAPS                    0x05055206 /* Flaps */
#define DIBUTTON_FLYINGM_FLAPSUP                0x05005407 /* Increment stepping up until fully retracted */
#define DIBUTTON_FLYINGM_FLAPSDOWN              0x05005408 /* Decrement stepping down until fully extended */
#define DIBUTTON_FLYINGM_FIRESECONDARY          0x05004C09 /* Alternative fire button */
#define DIBUTTON_FLYINGM_GEAR                   0x0500640A /* Gear up / down */
#define DIBUTTON_FLYINGM_BRAKE_LINK             0x050464E0 /* Fallback brake button */
#define DIBUTTON_FLYINGM_FASTER_LINK            0x0503D4E0 /* Fallback throttle up button */
#define DIBUTTON_FLYINGM_SLOWER_LINK            0x0503D4E8 /* Fallback throttle down button */
#define DIBUTTON_FLYINGM_GLANCE_LEFT_LINK       0x0507C4E4 /* Fallback Glance Left button */
#define DIBUTTON_FLYINGM_GLANCE_RIGHT_LINK      0x0507C4EC /* Fallback Glance Right button */
#define DIBUTTON_FLYINGM_GLANCE_UP_LINK         0x0507C4E0 /* Fallback Glance Up button */
#define DIBUTTON_FLYINGM_GLANCE_DOWN_LINK       0x0507C4E8 /* Fallback Glance Down button */
#define DIBUTTON_FLYINGM_DEVICE                 0x050044FE /* Show input device and controls */
#define DIBUTTON_FLYINGM_PAUSE                  0x050044FC /* Start / Pause / Restart game */

/*--- Flight Simulator - Combat Helicopter
      Combat from helicopter is primary objective  ---*/
#define DIVIRTUAL_FLYING_HELICOPTER             0x06000000
#define DIAXIS_FLYINGH_BANK                     0x06008A01 /* Bank - Roll ship left / right */
#define DIAXIS_FLYINGH_PITCH                    0x06010A02 /* Pitch - Nose up / down */
#define DIAXIS_FLYINGH_COLLECTIVE               0x06018A03 /* Collective - Blade pitch/power */
#define DIBUTTON_FLYINGH_FIRE                   0x06001401 /* Fire */
#define DIBUTTON_FLYINGH_WEAPONS                0x06001402 /* Select next weapon */
#define DIBUTTON_FLYINGH_TARGET                 0x06001403 /* Selects next available target */
#define DIBUTTON_FLYINGH_MENU                   0x060004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_FLYINGH_GLANCE              0x06004601 /* Look around */
#define DIAXIS_FLYINGH_TORQUE                   0x06025A04 /* Torque - Rotate ship around left / right axis */
#define DIAXIS_FLYINGH_THROTTLE                 0x0603DA05 /* Throttle */
#define DIBUTTON_FLYINGH_COUNTER                0x06005404 /* Activate counter measures */
#define DIBUTTON_FLYINGH_VIEW                   0x06006405 /* Cycle through view options */
#define DIBUTTON_FLYINGH_GEAR                   0x06006406 /* Gear up / down */
#define DIBUTTON_FLYINGH_FIRESECONDARY          0x06004C07 /* Alternative fire button */
#define DIBUTTON_FLYINGH_FASTER_LINK            0x0603DCE0 /* Fallback throttle up button */
#define DIBUTTON_FLYINGH_SLOWER_LINK            0x0603DCE8 /* Fallback throttle down button */
#define DIBUTTON_FLYINGH_GLANCE_LEFT_LINK       0x0607C4E4 /* Fallback Glance Left button */
#define DIBUTTON_FLYINGH_GLANCE_RIGHT_LINK      0x0607C4EC /* Fallback Glance Right button */
#define DIBUTTON_FLYINGH_GLANCE_UP_LINK         0x0607C4E0 /* Fallback Glance Up button */
#define DIBUTTON_FLYINGH_GLANCE_DOWN_LINK       0x0607C4E8 /* Fallback Glance Down button */
#define DIBUTTON_FLYINGH_DEVICE                 0x060044FE /* Show input device and controls */
#define DIBUTTON_FLYINGH_PAUSE                  0x060044FC /* Start / Pause / Restart game */

/*--- Space Simulator - Combat
      Space Simulator with weapons  ---*/
#define DIVIRTUAL_SPACESIM                      0x07000000
#define DIAXIS_SPACESIM_LATERAL                 0x07008201 /* Move ship left / right */
#define DIAXIS_SPACESIM_MOVE                    0x07010202 /* Move ship forward/backward */
#define DIAXIS_SPACESIM_THROTTLE                0x07038203 /* Throttle - Engine speed */
#define DIBUTTON_SPACESIM_FIRE                  0x07000401 /* Fire */
#define DIBUTTON_SPACESIM_WEAPONS               0x07000402 /* Select next weapon */
#define DIBUTTON_SPACESIM_TARGET                0x07000403 /* Selects next available target */
#define DIBUTTON_SPACESIM_MENU                  0x070004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_SPACESIM_GLANCE             0x07004601 /* Look around */
#define DIAXIS_SPACESIM_CLIMB                   0x0701C204 /* Climb - Pitch ship up/down */
#define DIAXIS_SPACESIM_ROTATE                  0x07024205 /* Rotate - Turn ship left/right */
#define DIBUTTON_SPACESIM_VIEW                  0x07004404 /* Cycle through view options */
#define DIBUTTON_SPACESIM_DISPLAY               0x07004405 /* Select next dashboard / heads up display option */
#define DIBUTTON_SPACESIM_RAISE                 0x07004406 /* Raise ship while maintaining current pitch */
#define DIBUTTON_SPACESIM_LOWER                 0x07004407 /* Lower ship while maintaining current pitch */
#define DIBUTTON_SPACESIM_GEAR                  0x07004408 /* Gear up / down */
#define DIBUTTON_SPACESIM_FIRESECONDARY         0x07004409 /* Alternative fire button */
#define DIBUTTON_SPACESIM_LEFT_LINK             0x0700C4E4 /* Fallback move left button */
#define DIBUTTON_SPACESIM_RIGHT_LINK            0x0700C4EC /* Fallback move right button */
#define DIBUTTON_SPACESIM_FORWARD_LINK          0x070144E0 /* Fallback move forward button */
#define DIBUTTON_SPACESIM_BACKWARD_LINK         0x070144E8 /* Fallback move backwards button */
#define DIBUTTON_SPACESIM_FASTER_LINK           0x0703C4E0 /* Fallback throttle up button */
#define DIBUTTON_SPACESIM_SLOWER_LINK           0x0703C4E8 /* Fallback throttle down button */
#define DIBUTTON_SPACESIM_TURN_LEFT_LINK        0x070244E4 /* Fallback turn left button */
#define DIBUTTON_SPACESIM_TURN_RIGHT_LINK       0x070244EC /* Fallback turn right button */
#define DIBUTTON_SPACESIM_GLANCE_LEFT_LINK      0x0707C4E4 /* Fallback Glance Left button */
#define DIBUTTON_SPACESIM_GLANCE_RIGHT_LINK     0x0707C4EC /* Fallback Glance Right button */
#define DIBUTTON_SPACESIM_GLANCE_UP_LINK        0x0707C4E0 /* Fallback Glance Up button */
#define DIBUTTON_SPACESIM_GLANCE_DOWN_LINK      0x0707C4E8 /* Fallback Glance Down button */
#define DIBUTTON_SPACESIM_DEVICE                0x070044FE /* Show input device and controls */
#define DIBUTTON_SPACESIM_PAUSE                 0x070044FC /* Start / Pause / Restart game */

/*--- Fighting - First Person 
      Hand to Hand combat is primary objective  ---*/
#define DIVIRTUAL_FIGHTING_HAND2HAND            0x08000000
#define DIAXIS_FIGHTINGH_LATERAL                0x08008201 /* Sidestep left/right */
#define DIAXIS_FIGHTINGH_MOVE                   0x08010202 /* Move forward/backward */
#define DIBUTTON_FIGHTINGH_PUNCH                0x08000401 /* Punch */
#define DIBUTTON_FIGHTINGH_KICK                 0x08000402 /* Kick */
#define DIBUTTON_FIGHTINGH_BLOCK                0x08000403 /* Block */
#define DIBUTTON_FIGHTINGH_CROUCH               0x08000404 /* Crouch */
#define DIBUTTON_FIGHTINGH_JUMP                 0x08000405 /* Jump */
#define DIBUTTON_FIGHTINGH_SPECIAL1             0x08000406 /* Apply first special move */
#define DIBUTTON_FIGHTINGH_SPECIAL2             0x08000407 /* Apply second special move */
#define DIBUTTON_FIGHTINGH_MENU                 0x080004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIBUTTON_FIGHTINGH_SELECT               0x08004408 /* Select special move */
#define DIHATSWITCH_FIGHTINGH_SLIDE             0x08004601 /* Look around */
#define DIBUTTON_FIGHTINGH_DISPLAY              0x08004409 /* Shows next on-screen display option */
#define DIAXIS_FIGHTINGH_ROTATE                 0x08024203 /* Rotate - Turn body left/right */
#define DIBUTTON_FIGHTINGH_DODGE                0x0800440A /* Dodge */
#define DIBUTTON_FIGHTINGH_LEFT_LINK            0x0800C4E4 /* Fallback left sidestep button */
#define DIBUTTON_FIGHTINGH_RIGHT_LINK           0x0800C4EC /* Fallback right sidestep button */
#define DIBUTTON_FIGHTINGH_FORWARD_LINK         0x080144E0 /* Fallback forward button */
#define DIBUTTON_FIGHTINGH_BACKWARD_LINK        0x080144E8 /* Fallback backward button */
#define DIBUTTON_FIGHTINGH_DEVICE               0x080044FE /* Show input device and controls */
#define DIBUTTON_FIGHTINGH_PAUSE                0x080044FC /* Start / Pause / Restart game */

/*--- Fighting - First Person Shooting
      Navigation and combat are primary objectives  ---*/
#define DIVIRTUAL_FIGHTING_FPS                  0x09000000
#define DIAXIS_FPS_ROTATE                       0x09008201 /* Rotate character left/right */
#define DIAXIS_FPS_MOVE                         0x09010202 /* Move forward/backward */
#define DIBUTTON_FPS_FIRE                       0x09000401 /* Fire */
#define DIBUTTON_FPS_WEAPONS                    0x09000402 /* Select next weapon */
#define DIBUTTON_FPS_APPLY                      0x09000403 /* Use item */
#define DIBUTTON_FPS_SELECT                     0x09000404 /* Select next inventory item */
#define DIBUTTON_FPS_CROUCH                     0x09000405 /* Crouch/ climb down/ swim down */
#define DIBUTTON_FPS_JUMP                       0x09000406 /* Jump/ climb up/ swim up */
#define DIAXIS_FPS_LOOKUPDOWN                   0x09018203 /* Look up / down  */
#define DIBUTTON_FPS_STRAFE                     0x09000407 /* Enable strafing while active */
#define DIBUTTON_FPS_MENU                       0x090004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_FPS_GLANCE                  0x09004601 /* Look around */
#define DIBUTTON_FPS_DISPLAY                    0x09004408 /* Shows next on-screen display option/ map */
#define DIAXIS_FPS_SIDESTEP                     0x09024204 /* Sidestep */
#define DIBUTTON_FPS_DODGE                      0x09004409 /* Dodge */
#define DIBUTTON_FPS_GLANCEL                    0x0900440A /* Glance Left */
#define DIBUTTON_FPS_GLANCER                    0x0900440B /* Glance Right */
#define DIBUTTON_FPS_FIRESECONDARY              0x0900440C /* Alternative fire button */
#define DIBUTTON_FPS_ROTATE_LEFT_LINK           0x0900C4E4 /* Fallback rotate left button */
#define DIBUTTON_FPS_ROTATE_RIGHT_LINK          0x0900C4EC /* Fallback rotate right button */
#define DIBUTTON_FPS_FORWARD_LINK               0x090144E0 /* Fallback forward button */
#define DIBUTTON_FPS_BACKWARD_LINK              0x090144E8 /* Fallback backward button */
#define DIBUTTON_FPS_GLANCE_UP_LINK             0x0901C4E0 /* Fallback look up button */
#define DIBUTTON_FPS_GLANCE_DOWN_LINK           0x0901C4E8 /* Fallback look down button */
#define DIBUTTON_FPS_STEP_LEFT_LINK             0x090244E4 /* Fallback step left button */
#define DIBUTTON_FPS_STEP_RIGHT_LINK            0x090244EC /* Fallback step right button */
#define DIBUTTON_FPS_DEVICE                     0x090044FE /* Show input device and controls */
#define DIBUTTON_FPS_PAUSE                      0x090044FC /* Start / Pause / Restart game */

/*--- Fighting - Third Person action
      Perspective of camera is behind the main character  ---*/
#define DIVIRTUAL_FIGHTING_THIRDPERSON          0x0A000000
#define DIAXIS_TPS_TURN                         0x0A020201 /* Turn left/right */
#define DIAXIS_TPS_MOVE                         0x0A010202 /* Move forward/backward */
#define DIBUTTON_TPS_RUN                        0x0A000401 /* Run or walk toggle switch */
#define DIBUTTON_TPS_ACTION                     0x0A000402 /* Action Button */
#define DIBUTTON_TPS_SELECT                     0x0A000403 /* Select next weapon */
#define DIBUTTON_TPS_USE                        0x0A000404 /* Use inventory item currently selected */
#define DIBUTTON_TPS_JUMP                       0x0A000405 /* Character Jumps */
#define DIBUTTON_TPS_MENU                       0x0A0004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_TPS_GLANCE                  0x0A004601 /* Look around */
#define DIBUTTON_TPS_VIEW                       0x0A004406 /* Select camera view */
#define DIBUTTON_TPS_STEPLEFT                   0x0A004407 /* Character takes a left step */
#define DIBUTTON_TPS_STEPRIGHT                  0x0A004408 /* Character takes a right step */
#define DIAXIS_TPS_STEP                         0x0A00C203 /* Character steps left/right */
#define DIBUTTON_TPS_DODGE                      0x0A004409 /* Character dodges or ducks */
#define DIBUTTON_TPS_INVENTORY                  0x0A00440A /* Cycle through inventory */
#define DIBUTTON_TPS_TURN_LEFT_LINK             0x0A0244E4 /* Fallback turn left button */
#define DIBUTTON_TPS_TURN_RIGHT_LINK            0x0A0244EC /* Fallback turn right button */
#define DIBUTTON_TPS_FORWARD_LINK               0x0A0144E0 /* Fallback forward button */
#define DIBUTTON_TPS_BACKWARD_LINK              0x0A0144E8 /* Fallback backward button */
#define DIBUTTON_TPS_GLANCE_UP_LINK             0x0A07C4E0 /* Fallback look up button */
#define DIBUTTON_TPS_GLANCE_DOWN_LINK           0x0A07C4E8 /* Fallback look down button */
#define DIBUTTON_TPS_GLANCE_LEFT_LINK           0x0A07C4E4 /* Fallback glance up button */
#define DIBUTTON_TPS_GLANCE_RIGHT_LINK          0x0A07C4EC /* Fallback glance right button */
#define DIBUTTON_TPS_DEVICE                     0x0A0044FE /* Show input device and controls */
#define DIBUTTON_TPS_PAUSE                      0x0A0044FC /* Start / Pause / Restart game */

/*--- Strategy - Role Playing
      Navigation and problem solving are primary actions  ---*/
#define DIVIRTUAL_STRATEGY_ROLEPLAYING          0x0B000000
#define DIAXIS_STRATEGYR_LATERAL                0x0B008201 /* sidestep - left/right */
#define DIAXIS_STRATEGYR_MOVE                   0x0B010202 /* move forward/backward */
#define DIBUTTON_STRATEGYR_GET                  0x0B000401 /* Acquire item */
#define DIBUTTON_STRATEGYR_APPLY                0x0B000402 /* Use selected item */
#define DIBUTTON_STRATEGYR_SELECT               0x0B000403 /* Select nextitem */
#define DIBUTTON_STRATEGYR_ATTACK               0x0B000404 /* Attack */
#define DIBUTTON_STRATEGYR_CAST                 0x0B000405 /* Cast Spell */
#define DIBUTTON_STRATEGYR_CROUCH               0x0B000406 /* Crouch */
#define DIBUTTON_STRATEGYR_JUMP                 0x0B000407 /* Jump */
#define DIBUTTON_STRATEGYR_MENU                 0x0B0004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_STRATEGYR_GLANCE            0x0B004601 /* Look around */
#define DIBUTTON_STRATEGYR_MAP                  0x0B004408 /* Cycle through map options */
#define DIBUTTON_STRATEGYR_DISPLAY              0x0B004409 /* Shows next on-screen display option */
#define DIAXIS_STRATEGYR_ROTATE                 0x0B024203 /* Turn body left/right */
#define DIBUTTON_STRATEGYR_LEFT_LINK            0x0B00C4E4 /* Fallback sidestep left button */
#define DIBUTTON_STRATEGYR_RIGHT_LINK           0x0B00C4EC /* Fallback sidestep right button */
#define DIBUTTON_STRATEGYR_FORWARD_LINK         0x0B0144E0 /* Fallback move forward button */
#define DIBUTTON_STRATEGYR_BACK_LINK            0x0B0144E8 /* Fallback move backward button */
#define DIBUTTON_STRATEGYR_ROTATE_LEFT_LINK     0x0B0244E4 /* Fallback turn body left button */
#define DIBUTTON_STRATEGYR_ROTATE_RIGHT_LINK    0x0B0244EC /* Fallback turn body right button */
#define DIBUTTON_STRATEGYR_DEVICE               0x0B0044FE /* Show input device and controls */
#define DIBUTTON_STRATEGYR_PAUSE                0x0B0044FC /* Start / Pause / Restart game */

/*--- Strategy - Turn based
      Navigation and problem solving are primary actions  ---*/
#define DIVIRTUAL_STRATEGY_TURN                 0x0C000000
#define DIAXIS_STRATEGYT_LATERAL                0x0C008201 /* Sidestep left/right */
#define DIAXIS_STRATEGYT_MOVE                   0x0C010202 /* Move forward/backwards */
#define DIBUTTON_STRATEGYT_SELECT               0x0C000401 /* Select unit or object */
#define DIBUTTON_STRATEGYT_INSTRUCT             0x0C000402 /* Cycle through instructions */
#define DIBUTTON_STRATEGYT_APPLY                0x0C000403 /* Apply selected instruction */
#define DIBUTTON_STRATEGYT_TEAM                 0x0C000404 /* Select next team / cycle through all */
#define DIBUTTON_STRATEGYT_TURN                 0x0C000405 /* Indicate turn over */
#define DIBUTTON_STRATEGYT_MENU                 0x0C0004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIBUTTON_STRATEGYT_ZOOM                 0x0C004406 /* Zoom - in / out */
#define DIBUTTON_STRATEGYT_MAP                  0x0C004407 /* cycle through map options */
#define DIBUTTON_STRATEGYT_DISPLAY              0x0C004408 /* shows next on-screen display options */
#define DIBUTTON_STRATEGYT_LEFT_LINK            0x0C00C4E4 /* Fallback sidestep left button */
#define DIBUTTON_STRATEGYT_RIGHT_LINK           0x0C00C4EC /* Fallback sidestep right button */
#define DIBUTTON_STRATEGYT_FORWARD_LINK         0x0C0144E0 /* Fallback move forward button */
#define DIBUTTON_STRATEGYT_BACK_LINK            0x0C0144E8 /* Fallback move back button */
#define DIBUTTON_STRATEGYT_DEVICE               0x0C0044FE /* Show input device and controls */
#define DIBUTTON_STRATEGYT_PAUSE                0x0C0044FC /* Start / Pause / Restart game */

/*--- Sports - Hunting
      Hunting                ---*/
#define DIVIRTUAL_SPORTS_HUNTING                0x0D000000
#define DIAXIS_HUNTING_LATERAL                  0x0D008201 /* sidestep left/right */
#define DIAXIS_HUNTING_MOVE                     0x0D010202 /* move forward/backwards */
#define DIBUTTON_HUNTING_FIRE                   0x0D000401 /* Fire selected weapon */
#define DIBUTTON_HUNTING_AIM                    0x0D000402 /* Select aim/move */
#define DIBUTTON_HUNTING_WEAPON                 0x0D000403 /* Select next weapon */
#define DIBUTTON_HUNTING_BINOCULAR              0x0D000404 /* Look through Binoculars */
#define DIBUTTON_HUNTING_CALL                   0x0D000405 /* Make animal call */
#define DIBUTTON_HUNTING_MAP                    0x0D000406 /* View Map */
#define DIBUTTON_HUNTING_SPECIAL                0x0D000407 /* Special game operation */
#define DIBUTTON_HUNTING_MENU                   0x0D0004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_HUNTING_GLANCE              0x0D004601 /* Look around */
#define DIBUTTON_HUNTING_DISPLAY                0x0D004408 /* show next on-screen display option */
#define DIAXIS_HUNTING_ROTATE                   0x0D024203 /* Turn body left/right */
#define DIBUTTON_HUNTING_CROUCH                 0x0D004409 /* Crouch/ Climb / Swim down */
#define DIBUTTON_HUNTING_JUMP                   0x0D00440A /* Jump/ Climb up / Swim up */
#define DIBUTTON_HUNTING_FIRESECONDARY          0x0D00440B /* Alternative fire button */
#define DIBUTTON_HUNTING_LEFT_LINK              0x0D00C4E4 /* Fallback sidestep left button */
#define DIBUTTON_HUNTING_RIGHT_LINK             0x0D00C4EC /* Fallback sidestep right button */
#define DIBUTTON_HUNTING_FORWARD_LINK           0x0D0144E0 /* Fallback move forward button */
#define DIBUTTON_HUNTING_BACK_LINK              0x0D0144E8 /* Fallback move back button */
#define DIBUTTON_HUNTING_ROTATE_LEFT_LINK       0x0D0244E4 /* Fallback turn body left button */
#define DIBUTTON_HUNTING_ROTATE_RIGHT_LINK      0x0D0244EC /* Fallback turn body right button */
#define DIBUTTON_HUNTING_DEVICE                 0x0D0044FE /* Show input device and controls */
#define DIBUTTON_HUNTING_PAUSE                  0x0D0044FC /* Start / Pause / Restart game */

/*--- Sports - Fishing
      Catching Fish is primary objective   ---*/
#define DIVIRTUAL_SPORTS_FISHING                0x0E000000
#define DIAXIS_FISHING_LATERAL                  0x0E008201 /* sidestep left/right */
#define DIAXIS_FISHING_MOVE                     0x0E010202 /* move forward/backwards */
#define DIBUTTON_FISHING_CAST                   0x0E000401 /* Cast line */
#define DIBUTTON_FISHING_TYPE                   0x0E000402 /* Select cast type */
#define DIBUTTON_FISHING_BINOCULAR              0x0E000403 /* Look through Binocular */
#define DIBUTTON_FISHING_BAIT                   0x0E000404 /* Select type of Bait */
#define DIBUTTON_FISHING_MAP                    0x0E000405 /* View Map */
#define DIBUTTON_FISHING_MENU                   0x0E0004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_FISHING_GLANCE              0x0E004601 /* Look around */
#define DIBUTTON_FISHING_DISPLAY                0x0E004406 /* Show next on-screen display option */
#define DIAXIS_FISHING_ROTATE                   0x0E024203 /* Turn character left / right */
#define DIBUTTON_FISHING_CROUCH                 0x0E004407 /* Crouch/ Climb / Swim down */
#define DIBUTTON_FISHING_JUMP                   0x0E004408 /* Jump/ Climb up / Swim up */
#define DIBUTTON_FISHING_LEFT_LINK              0x0E00C4E4 /* Fallback sidestep left button */
#define DIBUTTON_FISHING_RIGHT_LINK             0x0E00C4EC /* Fallback sidestep right button */
#define DIBUTTON_FISHING_FORWARD_LINK           0x0E0144E0 /* Fallback move forward button */
#define DIBUTTON_FISHING_BACK_LINK              0x0E0144E8 /* Fallback move back button */
#define DIBUTTON_FISHING_ROTATE_LEFT_LINK       0x0E0244E4 /* Fallback turn body left button */
#define DIBUTTON_FISHING_ROTATE_RIGHT_LINK      0x0E0244EC /* Fallback turn body right button */
#define DIBUTTON_FISHING_DEVICE                 0x0E0044FE /* Show input device and controls */
#define DIBUTTON_FISHING_PAUSE                  0x0E0044FC /* Start / Pause / Restart game */

/*--- Sports - Baseball - Batting
      Batter control is primary objective  ---*/
#define DIVIRTUAL_SPORTS_BASEBALL_BAT           0x0F000000
#define DIAXIS_BASEBALLB_LATERAL                0x0F008201 /* Aim left / right */
#define DIAXIS_BASEBALLB_MOVE                   0x0F010202 /* Aim up / down */
#define DIBUTTON_BASEBALLB_SELECT               0x0F000401 /* cycle through swing options */
#define DIBUTTON_BASEBALLB_NORMAL               0x0F000402 /* normal swing */
#define DIBUTTON_BASEBALLB_POWER                0x0F000403 /* swing for the fence */
#define DIBUTTON_BASEBALLB_BUNT                 0x0F000404 /* bunt */
#define DIBUTTON_BASEBALLB_STEAL                0x0F000405 /* Base runner attempts to steal a base */
#define DIBUTTON_BASEBALLB_BURST                0x0F000406 /* Base runner invokes burst of speed */
#define DIBUTTON_BASEBALLB_SLIDE                0x0F000407 /* Base runner slides into base */
#define DIBUTTON_BASEBALLB_CONTACT              0x0F000408 /* Contact swing */
#define DIBUTTON_BASEBALLB_MENU                 0x0F0004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIBUTTON_BASEBALLB_NOSTEAL              0x0F004409 /* Base runner goes back to a base */
#define DIBUTTON_BASEBALLB_BOX                  0x0F00440A /* Enter or exit batting box */
#define DIBUTTON_BASEBALLB_LEFT_LINK            0x0F00C4E4 /* Fallback sidestep left button */
#define DIBUTTON_BASEBALLB_RIGHT_LINK           0x0F00C4EC /* Fallback sidestep right button */
#define DIBUTTON_BASEBALLB_FORWARD_LINK         0x0F0144E0 /* Fallback move forward button */
#define DIBUTTON_BASEBALLB_BACK_LINK            0x0F0144E8 /* Fallback move back button */
#define DIBUTTON_BASEBALLB_DEVICE               0x0F0044FE /* Show input device and controls */
#define DIBUTTON_BASEBALLB_PAUSE                0x0F0044FC /* Start / Pause / Restart game */

/*--- Sports - Baseball - Pitching
      Pitcher control is primary objective   ---*/
#define DIVIRTUAL_SPORTS_BASEBALL_PITCH         0x10000000
#define DIAXIS_BASEBALLP_LATERAL                0x10008201 /* Aim left / right */
#define DIAXIS_BASEBALLP_MOVE                   0x10010202 /* Aim up / down */
#define DIBUTTON_BASEBALLP_SELECT               0x10000401 /* cycle through pitch selections */
#define DIBUTTON_BASEBALLP_PITCH                0x10000402 /* throw pitch */
#define DIBUTTON_BASEBALLP_BASE                 0x10000403 /* select base to throw to */
#define DIBUTTON_BASEBALLP_THROW                0x10000404 /* throw to base */
#define DIBUTTON_BASEBALLP_FAKE                 0x10000405 /* Fake a throw to a base */
#define DIBUTTON_BASEBALLP_MENU                 0x100004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIBUTTON_BASEBALLP_WALK                 0x10004406 /* Throw intentional walk / pitch out */
#define DIBUTTON_BASEBALLP_LOOK                 0x10004407 /* Look at runners on bases */
#define DIBUTTON_BASEBALLP_LEFT_LINK            0x1000C4E4 /* Fallback sidestep left button */
#define DIBUTTON_BASEBALLP_RIGHT_LINK           0x1000C4EC /* Fallback sidestep right button */
#define DIBUTTON_BASEBALLP_FORWARD_LINK         0x100144E0 /* Fallback move forward button */
#define DIBUTTON_BASEBALLP_BACK_LINK            0x100144E8 /* Fallback move back button */
#define DIBUTTON_BASEBALLP_DEVICE               0x100044FE /* Show input device and controls */
#define DIBUTTON_BASEBALLP_PAUSE                0x100044FC /* Start / Pause / Restart game */

/*--- Sports - Baseball - Fielding
      Fielder control is primary objective  ---*/
#define DIVIRTUAL_SPORTS_BASEBALL_FIELD         0x11000000
#define DIAXIS_BASEBALLF_LATERAL                0x11008201 /* Aim left / right */
#define DIAXIS_BASEBALLF_MOVE                   0x11010202 /* Aim up / down */
#define DIBUTTON_BASEBALLF_NEAREST              0x11000401 /* Switch to fielder nearest to the ball */
#define DIBUTTON_BASEBALLF_THROW1               0x11000402 /* Make conservative throw */
#define DIBUTTON_BASEBALLF_THROW2               0x11000403 /* Make aggressive throw */
#define DIBUTTON_BASEBALLF_BURST                0x11000404 /* Invoke burst of speed */
#define DIBUTTON_BASEBALLF_JUMP                 0x11000405 /* Jump to catch ball */
#define DIBUTTON_BASEBALLF_DIVE                 0x11000406 /* Dive to catch ball */
#define DIBUTTON_BASEBALLF_MENU                 0x110004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIBUTTON_BASEBALLF_SHIFTIN              0x11004407 /* Shift the infield positioning */
#define DIBUTTON_BASEBALLF_SHIFTOUT             0x11004408 /* Shift the outfield positioning */
#define DIBUTTON_BASEBALLF_AIM_LEFT_LINK        0x1100C4E4 /* Fallback aim left button */
#define DIBUTTON_BASEBALLF_AIM_RIGHT_LINK       0x1100C4EC /* Fallback aim right button */
#define DIBUTTON_BASEBALLF_FORWARD_LINK         0x110144E0 /* Fallback move forward button */
#define DIBUTTON_BASEBALLF_BACK_LINK            0x110144E8 /* Fallback move back button */
#define DIBUTTON_BASEBALLF_DEVICE               0x110044FE /* Show input device and controls */
#define DIBUTTON_BASEBALLF_PAUSE                0x110044FC /* Start / Pause / Restart game */

/*--- Sports - Basketball - Offense
      Offense  ---*/
#define DIVIRTUAL_SPORTS_BASKETBALL_OFFENSE     0x12000000
#define DIAXIS_BBALLO_LATERAL                   0x12008201 /* left / right */
#define DIAXIS_BBALLO_MOVE                      0x12010202 /* up / down */
#define DIBUTTON_BBALLO_SHOOT                   0x12000401 /* shoot basket */
#define DIBUTTON_BBALLO_DUNK                    0x12000402 /* dunk basket */
#define DIBUTTON_BBALLO_PASS                    0x12000403 /* throw pass */
#define DIBUTTON_BBALLO_FAKE                    0x12000404 /* fake shot or pass */
#define DIBUTTON_BBALLO_SPECIAL                 0x12000405 /* apply special move */
#define DIBUTTON_BBALLO_PLAYER                  0x12000406 /* select next player */
#define DIBUTTON_BBALLO_BURST                   0x12000407 /* invoke burst */
#define DIBUTTON_BBALLO_CALL                    0x12000408 /* call for ball / pass to me */
#define DIBUTTON_BBALLO_MENU                    0x120004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_BBALLO_GLANCE               0x12004601 /* scroll view */
#define DIBUTTON_BBALLO_SCREEN                  0x12004409 /* Call for screen */
#define DIBUTTON_BBALLO_PLAY                    0x1200440A /* Call for specific offensive play */
#define DIBUTTON_BBALLO_JAB                     0x1200440B /* Initiate fake drive to basket */
#define DIBUTTON_BBALLO_POST                    0x1200440C /* Perform post move */
#define DIBUTTON_BBALLO_TIMEOUT                 0x1200440D /* Time Out */
#define DIBUTTON_BBALLO_SUBSTITUTE              0x1200440E /* substitute one player for another */
#define DIBUTTON_BBALLO_LEFT_LINK               0x1200C4E4 /* Fallback sidestep left button */
#define DIBUTTON_BBALLO_RIGHT_LINK              0x1200C4EC /* Fallback sidestep right button */
#define DIBUTTON_BBALLO_FORWARD_LINK            0x120144E0 /* Fallback move forward button */
#define DIBUTTON_BBALLO_BACK_LINK               0x120144E8 /* Fallback move back button */
#define DIBUTTON_BBALLO_DEVICE                  0x120044FE /* Show input device and controls */
#define DIBUTTON_BBALLO_PAUSE                   0x120044FC /* Start / Pause / Restart game */

/*--- Sports - Basketball - Defense
      Defense  ---*/
#define DIVIRTUAL_SPORTS_BASKETBALL_DEFENSE     0x13000000
#define DIAXIS_BBALLD_LATERAL                   0x13008201 /* left / right */
#define DIAXIS_BBALLD_MOVE                      0x13010202 /* up / down */
#define DIBUTTON_BBALLD_JUMP                    0x13000401 /* jump to block shot */
#define DIBUTTON_BBALLD_STEAL                   0x13000402 /* attempt to steal ball */
#define DIBUTTON_BBALLD_FAKE                    0x13000403 /* fake block or steal */
#define DIBUTTON_BBALLD_SPECIAL                 0x13000404 /* apply special move */
#define DIBUTTON_BBALLD_PLAYER                  0x13000405 /* select next player */
#define DIBUTTON_BBALLD_BURST                   0x13000406 /* invoke burst */
#define DIBUTTON_BBALLD_PLAY                    0x13000407 /* call for specific defensive play */
#define DIBUTTON_BBALLD_MENU                    0x130004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_BBALLD_GLANCE               0x13004601 /* scroll view */
#define DIBUTTON_BBALLD_TIMEOUT                 0x13004408 /* Time Out */
#define DIBUTTON_BBALLD_SUBSTITUTE              0x13004409 /* substitute one player for another */
#define DIBUTTON_BBALLD_LEFT_LINK               0x1300C4E4 /* Fallback sidestep left button */
#define DIBUTTON_BBALLD_RIGHT_LINK              0x1300C4EC /* Fallback sidestep right button */
#define DIBUTTON_BBALLD_FORWARD_LINK            0x130144E0 /* Fallback move forward button */
#define DIBUTTON_BBALLD_BACK_LINK               0x130144E8 /* Fallback move back button */
#define DIBUTTON_BBALLD_DEVICE                  0x130044FE /* Show input device and controls */
#define DIBUTTON_BBALLD_PAUSE                   0x130044FC /* Start / Pause / Restart game */

/*--- Sports - Football - Play
      Play selection  ---*/
#define DIVIRTUAL_SPORTS_FOOTBALL_FIELD         0x14000000
#define DIBUTTON_FOOTBALLP_PLAY                 0x14000401 /* cycle through available plays */
#define DIBUTTON_FOOTBALLP_SELECT               0x14000402 /* select play */
#define DIBUTTON_FOOTBALLP_HELP                 0x14000403 /* Bring up pop-up help */
#define DIBUTTON_FOOTBALLP_MENU                 0x140004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIBUTTON_FOOTBALLP_DEVICE               0x140044FE /* Show input device and controls */
#define DIBUTTON_FOOTBALLP_PAUSE                0x140044FC /* Start / Pause / Restart game */

/*--- Sports - Football - QB
      Offense: Quarterback / Kicker  ---*/
#define DIVIRTUAL_SPORTS_FOOTBALL_QBCK          0x15000000
#define DIAXIS_FOOTBALLQ_LATERAL                0x15008201 /* Move / Aim: left / right */
#define DIAXIS_FOOTBALLQ_MOVE                   0x15010202 /* Move / Aim: up / down */
#define DIBUTTON_FOOTBALLQ_SELECT               0x15000401 /* Select */
#define DIBUTTON_FOOTBALLQ_SNAP                 0x15000402 /* snap ball - start play */
#define DIBUTTON_FOOTBALLQ_JUMP                 0x15000403 /* jump over defender */
#define DIBUTTON_FOOTBALLQ_SLIDE                0x15000404 /* Dive/Slide */
#define DIBUTTON_FOOTBALLQ_PASS                 0x15000405 /* throws pass to receiver */
#define DIBUTTON_FOOTBALLQ_FAKE                 0x15000406 /* pump fake pass or fake kick */
#define DIBUTTON_FOOTBALLQ_MENU                 0x150004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIBUTTON_FOOTBALLQ_FAKESNAP             0x15004407 /* Fake snap  */
#define DIBUTTON_FOOTBALLQ_MOTION               0x15004408 /* Send receivers in motion */
#define DIBUTTON_FOOTBALLQ_AUDIBLE              0x15004409 /* Change offensive play at line of scrimmage */
#define DIBUTTON_FOOTBALLQ_LEFT_LINK            0x1500C4E4 /* Fallback sidestep left button */
#define DIBUTTON_FOOTBALLQ_RIGHT_LINK           0x1500C4EC /* Fallback sidestep right button */
#define DIBUTTON_FOOTBALLQ_FORWARD_LINK         0x150144E0 /* Fallback move forward button */
#define DIBUTTON_FOOTBALLQ_BACK_LINK            0x150144E8 /* Fallback move back button */
#define DIBUTTON_FOOTBALLQ_DEVICE               0x150044FE /* Show input device and controls */
#define DIBUTTON_FOOTBALLQ_PAUSE                0x150044FC /* Start / Pause / Restart game */

/*--- Sports - Football - Offense
      Offense - Runner  ---*/
#define DIVIRTUAL_SPORTS_FOOTBALL_OFFENSE       0x16000000
#define DIAXIS_FOOTBALLO_LATERAL                0x16008201 /* Move / Aim: left / right */
#define DIAXIS_FOOTBALLO_MOVE                   0x16010202 /* Move / Aim: up / down */
#define DIBUTTON_FOOTBALLO_JUMP                 0x16000401 /* jump or hurdle over defender */
#define DIBUTTON_FOOTBALLO_LEFTARM              0x16000402 /* holds out left arm */
#define DIBUTTON_FOOTBALLO_RIGHTARM             0x16000403 /* holds out right arm */
#define DIBUTTON_FOOTBALLO_THROW                0x16000404 /* throw pass or lateral ball to another runner */
#define DIBUTTON_FOOTBALLO_SPIN                 0x16000405 /* Spin to avoid defenders */
#define DIBUTTON_FOOTBALLO_MENU                 0x160004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIBUTTON_FOOTBALLO_JUKE                 0x16004406 /* Use special move to avoid defenders */
#define DIBUTTON_FOOTBALLO_SHOULDER             0x16004407 /* Lower shoulder to run over defenders */
#define DIBUTTON_FOOTBALLO_TURBO                0x16004408 /* Speed burst past defenders */
#define DIBUTTON_FOOTBALLO_DIVE                 0x16004409 /* Dive over defenders */
#define DIBUTTON_FOOTBALLO_ZOOM                 0x1600440A /* Zoom view in / out */
#define DIBUTTON_FOOTBALLO_SUBSTITUTE           0x1600440B /* substitute one player for another */
#define DIBUTTON_FOOTBALLO_LEFT_LINK            0x1600C4E4 /* Fallback sidestep left button */
#define DIBUTTON_FOOTBALLO_RIGHT_LINK           0x1600C4EC /* Fallback sidestep right button */
#define DIBUTTON_FOOTBALLO_FORWARD_LINK         0x160144E0 /* Fallback move forward button */
#define DIBUTTON_FOOTBALLO_BACK_LINK            0x160144E8 /* Fallback move back button */
#define DIBUTTON_FOOTBALLO_DEVICE               0x160044FE /* Show input device and controls */
#define DIBUTTON_FOOTBALLO_PAUSE                0x160044FC /* Start / Pause / Restart game */

/*--- Sports - Football - Defense
      Defense     ---*/
#define DIVIRTUAL_SPORTS_FOOTBALL_DEFENSE       0x17000000
#define DIAXIS_FOOTBALLD_LATERAL                0x17008201 /* Move / Aim: left / right */
#define DIAXIS_FOOTBALLD_MOVE                   0x17010202 /* Move / Aim: up / down */
#define DIBUTTON_FOOTBALLD_PLAY                 0x17000401 /* cycle through available plays */
#define DIBUTTON_FOOTBALLD_SELECT               0x17000402 /* select player closest to the ball */
#define DIBUTTON_FOOTBALLD_JUMP                 0x17000403 /* jump to intercept or block */
#define DIBUTTON_FOOTBALLD_TACKLE               0x17000404 /* tackler runner */
#define DIBUTTON_FOOTBALLD_FAKE                 0x17000405 /* hold down to fake tackle or intercept */
#define DIBUTTON_FOOTBALLD_SUPERTACKLE          0x17000406 /* Initiate special tackle */
#define DIBUTTON_FOOTBALLD_MENU                 0x170004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIBUTTON_FOOTBALLD_SPIN                 0x17004407 /* Spin to beat offensive line */
#define DIBUTTON_FOOTBALLD_SWIM                 0x17004408 /* Swim to beat the offensive line */
#define DIBUTTON_FOOTBALLD_BULLRUSH             0x17004409 /* Bull rush the offensive line */
#define DIBUTTON_FOOTBALLD_RIP                  0x1700440A /* Rip the offensive line */
#define DIBUTTON_FOOTBALLD_AUDIBLE              0x1700440B /* Change defensive play at the line of scrimmage */
#define DIBUTTON_FOOTBALLD_ZOOM                 0x1700440C /* Zoom view in / out */
#define DIBUTTON_FOOTBALLD_SUBSTITUTE           0x1700440D /* substitute one player for another */
#define DIBUTTON_FOOTBALLD_LEFT_LINK            0x1700C4E4 /* Fallback sidestep left button */
#define DIBUTTON_FOOTBALLD_RIGHT_LINK           0x1700C4EC /* Fallback sidestep right button */
#define DIBUTTON_FOOTBALLD_FORWARD_LINK         0x170144E0 /* Fallback move forward button */
#define DIBUTTON_FOOTBALLD_BACK_LINK            0x170144E8 /* Fallback move back button */
#define DIBUTTON_FOOTBALLD_DEVICE               0x170044FE /* Show input device and controls */
#define DIBUTTON_FOOTBALLD_PAUSE                0x170044FC /* Start / Pause / Restart game */

/*--- Sports - Golf
                                ---*/
#define DIVIRTUAL_SPORTS_GOLF                   0x18000000
#define DIAXIS_GOLF_LATERAL                     0x18008201 /* Move / Aim: left / right */
#define DIAXIS_GOLF_MOVE                        0x18010202 /* Move / Aim: up / down */
#define DIBUTTON_GOLF_SWING                     0x18000401 /* swing club */
#define DIBUTTON_GOLF_SELECT                    0x18000402 /* cycle between: club / swing strength / ball arc / ball spin */
#define DIBUTTON_GOLF_UP                        0x18000403 /* increase selection */
#define DIBUTTON_GOLF_DOWN                      0x18000404 /* decrease selection */
#define DIBUTTON_GOLF_TERRAIN                   0x18000405 /* shows terrain detail */
#define DIBUTTON_GOLF_FLYBY                     0x18000406 /* view the hole via a flyby */
#define DIBUTTON_GOLF_MENU                      0x180004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_GOLF_SCROLL                 0x18004601 /* scroll view */
#define DIBUTTON_GOLF_ZOOM                      0x18004407 /* Zoom view in / out */
#define DIBUTTON_GOLF_TIMEOUT                   0x18004408 /* Call for time out */
#define DIBUTTON_GOLF_SUBSTITUTE                0x18004409 /* substitute one player for another */
#define DIBUTTON_GOLF_LEFT_LINK                 0x1800C4E4 /* Fallback sidestep left button */
#define DIBUTTON_GOLF_RIGHT_LINK                0x1800C4EC /* Fallback sidestep right button */
#define DIBUTTON_GOLF_FORWARD_LINK              0x180144E0 /* Fallback move forward button */
#define DIBUTTON_GOLF_BACK_LINK                 0x180144E8 /* Fallback move back button */
#define DIBUTTON_GOLF_DEVICE                    0x180044FE /* Show input device and controls */
#define DIBUTTON_GOLF_PAUSE                     0x180044FC /* Start / Pause / Restart game */

/*--- Sports - Hockey - Offense
      Offense       ---*/
#define DIVIRTUAL_SPORTS_HOCKEY_OFFENSE         0x19000000
#define DIAXIS_HOCKEYO_LATERAL                  0x19008201 /* Move / Aim: left / right */
#define DIAXIS_HOCKEYO_MOVE                     0x19010202 /* Move / Aim: up / down */
#define DIBUTTON_HOCKEYO_SHOOT                  0x19000401 /* Shoot */
#define DIBUTTON_HOCKEYO_PASS                   0x19000402 /* pass the puck */
#define DIBUTTON_HOCKEYO_BURST                  0x19000403 /* invoke speed burst */
#define DIBUTTON_HOCKEYO_SPECIAL                0x19000404 /* invoke special move */
#define DIBUTTON_HOCKEYO_FAKE                   0x19000405 /* hold down to fake pass or kick */
#define DIBUTTON_HOCKEYO_MENU                   0x190004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_HOCKEYO_SCROLL              0x19004601 /* scroll view */
#define DIBUTTON_HOCKEYO_ZOOM                   0x19004406 /* Zoom view in / out */
#define DIBUTTON_HOCKEYO_STRATEGY               0x19004407 /* Invoke coaching menu for strategy help */
#define DIBUTTON_HOCKEYO_TIMEOUT                0x19004408 /* Call for time out */
#define DIBUTTON_HOCKEYO_SUBSTITUTE             0x19004409 /* substitute one player for another */
#define DIBUTTON_HOCKEYO_LEFT_LINK              0x1900C4E4 /* Fallback sidestep left button */
#define DIBUTTON_HOCKEYO_RIGHT_LINK             0x1900C4EC /* Fallback sidestep right button */
#define DIBUTTON_HOCKEYO_FORWARD_LINK           0x190144E0 /* Fallback move forward button */
#define DIBUTTON_HOCKEYO_BACK_LINK              0x190144E8 /* Fallback move back button */
#define DIBUTTON_HOCKEYO_DEVICE                 0x190044FE /* Show input device and controls */
#define DIBUTTON_HOCKEYO_PAUSE                  0x190044FC /* Start / Pause / Restart game */

/*--- Sports - Hockey - Defense
      Defense       ---*/
#define DIVIRTUAL_SPORTS_HOCKEY_DEFENSE         0x1A000000
#define DIAXIS_HOCKEYD_LATERAL                  0x1A008201 /* Move / Aim: left / right */
#define DIAXIS_HOCKEYD_MOVE                     0x1A010202 /* Move / Aim: up / down */
#define DIBUTTON_HOCKEYD_PLAYER                 0x1A000401 /* control player closest to the puck */
#define DIBUTTON_HOCKEYD_STEAL                  0x1A000402 /* attempt steal */
#define DIBUTTON_HOCKEYD_BURST                  0x1A000403 /* speed burst or body check */
#define DIBUTTON_HOCKEYD_BLOCK                  0x1A000404 /* block puck */
#define DIBUTTON_HOCKEYD_FAKE                   0x1A000405 /* hold down to fake tackle or intercept */
#define DIBUTTON_HOCKEYD_MENU                   0x1A0004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_HOCKEYD_SCROLL              0x1A004601 /* scroll view */
#define DIBUTTON_HOCKEYD_ZOOM                   0x1A004406 /* Zoom view in / out */
#define DIBUTTON_HOCKEYD_STRATEGY               0x1A004407 /* Invoke coaching menu for strategy help */
#define DIBUTTON_HOCKEYD_TIMEOUT                0x1A004408 /* Call for time out */
#define DIBUTTON_HOCKEYD_SUBSTITUTE             0x1A004409 /* substitute one player for another */
#define DIBUTTON_HOCKEYD_LEFT_LINK              0x1A00C4E4 /* Fallback sidestep left button */
#define DIBUTTON_HOCKEYD_RIGHT_LINK             0x1A00C4EC /* Fallback sidestep right button */
#define DIBUTTON_HOCKEYD_FORWARD_LINK           0x1A0144E0 /* Fallback move forward button */
#define DIBUTTON_HOCKEYD_BACK_LINK              0x1A0144E8 /* Fallback move back button */
#define DIBUTTON_HOCKEYD_DEVICE                 0x1A0044FE /* Show input device and controls */
#define DIBUTTON_HOCKEYD_PAUSE                  0x1A0044FC /* Start / Pause / Restart game */

/*--- Sports - Hockey - Goalie
      Goal tending  ---*/
#define DIVIRTUAL_SPORTS_HOCKEY_GOALIE          0x1B000000
#define DIAXIS_HOCKEYG_LATERAL                  0x1B008201 /* Move / Aim: left / right */
#define DIAXIS_HOCKEYG_MOVE                     0x1B010202 /* Move / Aim: up / down */
#define DIBUTTON_HOCKEYG_PASS                   0x1B000401 /* pass puck */
#define DIBUTTON_HOCKEYG_POKE                   0x1B000402 /* poke / check / hack */
#define DIBUTTON_HOCKEYG_STEAL                  0x1B000403 /* attempt steal */
#define DIBUTTON_HOCKEYG_BLOCK                  0x1B000404 /* block puck */
#define DIBUTTON_HOCKEYG_MENU                   0x1B0004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_HOCKEYG_SCROLL              0x1B004601 /* scroll view */
#define DIBUTTON_HOCKEYG_ZOOM                   0x1B004405 /* Zoom view in / out */
#define DIBUTTON_HOCKEYG_STRATEGY               0x1B004406 /* Invoke coaching menu for strategy help */
#define DIBUTTON_HOCKEYG_TIMEOUT                0x1B004407 /* Call for time out */
#define DIBUTTON_HOCKEYG_SUBSTITUTE             0x1B004408 /* substitute one player for another */
#define DIBUTTON_HOCKEYG_LEFT_LINK              0x1B00C4E4 /* Fallback sidestep left button */
#define DIBUTTON_HOCKEYG_RIGHT_LINK             0x1B00C4EC /* Fallback sidestep right button */
#define DIBUTTON_HOCKEYG_FORWARD_LINK           0x1B0144E0 /* Fallback move forward button */
#define DIBUTTON_HOCKEYG_BACK_LINK              0x1B0144E8 /* Fallback move back button */
#define DIBUTTON_HOCKEYG_DEVICE                 0x1B0044FE /* Show input device and controls */
#define DIBUTTON_HOCKEYG_PAUSE                  0x1B0044FC /* Start / Pause / Restart game */

/*--- Sports - Mountain Biking
                     ---*/
#define DIVIRTUAL_SPORTS_BIKING_MOUNTAIN        0x1C000000
#define DIAXIS_BIKINGM_TURN                     0x1C008201 /* left / right */
#define DIAXIS_BIKINGM_PEDAL                    0x1C010202 /* Pedal faster / slower / brake */
#define DIBUTTON_BIKINGM_JUMP                   0x1C000401 /* jump over obstacle */
#define DIBUTTON_BIKINGM_CAMERA                 0x1C000402 /* switch camera view */
#define DIBUTTON_BIKINGM_SPECIAL1               0x1C000403 /* perform first special move */
#define DIBUTTON_BIKINGM_SELECT                 0x1C000404 /* Select */
#define DIBUTTON_BIKINGM_SPECIAL2               0x1C000405 /* perform second special move */
#define DIBUTTON_BIKINGM_MENU                   0x1C0004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_BIKINGM_SCROLL              0x1C004601 /* scroll view */
#define DIBUTTON_BIKINGM_ZOOM                   0x1C004406 /* Zoom view in / out */
#define DIAXIS_BIKINGM_BRAKE                    0x1C044203 /* Brake axis  */
#define DIBUTTON_BIKINGM_LEFT_LINK              0x1C00C4E4 /* Fallback turn left button */
#define DIBUTTON_BIKINGM_RIGHT_LINK             0x1C00C4EC /* Fallback turn right button */
#define DIBUTTON_BIKINGM_FASTER_LINK            0x1C0144E0 /* Fallback pedal faster button */
#define DIBUTTON_BIKINGM_SLOWER_LINK            0x1C0144E8 /* Fallback pedal slower button */
#define DIBUTTON_BIKINGM_BRAKE_BUTTON_LINK      0x1C0444E8 /* Fallback brake button */
#define DIBUTTON_BIKINGM_DEVICE                 0x1C0044FE /* Show input device and controls */
#define DIBUTTON_BIKINGM_PAUSE                  0x1C0044FC /* Start / Pause / Restart game */

/*--- Sports: Skiing / Snowboarding / Skateboarding
        ---*/
#define DIVIRTUAL_SPORTS_SKIING                 0x1D000000
#define DIAXIS_SKIING_TURN                      0x1D008201 /* left / right */
#define DIAXIS_SKIING_SPEED                     0x1D010202 /* faster / slower */
#define DIBUTTON_SKIING_JUMP                    0x1D000401 /* Jump */
#define DIBUTTON_SKIING_CROUCH                  0x1D000402 /* crouch down */
#define DIBUTTON_SKIING_CAMERA                  0x1D000403 /* switch camera view */
#define DIBUTTON_SKIING_SPECIAL1                0x1D000404 /* perform first special move */
#define DIBUTTON_SKIING_SELECT                  0x1D000405 /* Select */
#define DIBUTTON_SKIING_SPECIAL2                0x1D000406 /* perform second special move */
#define DIBUTTON_SKIING_MENU                    0x1D0004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_SKIING_GLANCE               0x1D004601 /* scroll view */
#define DIBUTTON_SKIING_ZOOM                    0x1D004407 /* Zoom view in / out */
#define DIBUTTON_SKIING_LEFT_LINK               0x1D00C4E4 /* Fallback turn left button */
#define DIBUTTON_SKIING_RIGHT_LINK              0x1D00C4EC /* Fallback turn right button */
#define DIBUTTON_SKIING_FASTER_LINK             0x1D0144E0 /* Fallback increase speed button */
#define DIBUTTON_SKIING_SLOWER_LINK             0x1D0144E8 /* Fallback decrease speed button */
#define DIBUTTON_SKIING_DEVICE                  0x1D0044FE /* Show input device and controls */
#define DIBUTTON_SKIING_PAUSE                   0x1D0044FC /* Start / Pause / Restart game */

/*--- Sports - Soccer - Offense
      Offense       ---*/
#define DIVIRTUAL_SPORTS_SOCCER_OFFENSE         0x1E000000
#define DIAXIS_SOCCERO_LATERAL                  0x1E008201 /* Move / Aim: left / right */
#define DIAXIS_SOCCERO_MOVE                     0x1E010202 /* Move / Aim: up / down */
#define DIAXIS_SOCCERO_BEND                     0x1E018203 /* Bend to soccer shot/pass */
#define DIBUTTON_SOCCERO_SHOOT                  0x1E000401 /* Shoot the ball */
#define DIBUTTON_SOCCERO_PASS                   0x1E000402 /* Pass  */
#define DIBUTTON_SOCCERO_FAKE                   0x1E000403 /* Fake */
#define DIBUTTON_SOCCERO_PLAYER                 0x1E000404 /* Select next player */
#define DIBUTTON_SOCCERO_SPECIAL1               0x1E000405 /* Apply special move */
#define DIBUTTON_SOCCERO_SELECT                 0x1E000406 /* Select special move */
#define DIBUTTON_SOCCERO_MENU                   0x1E0004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_SOCCERO_GLANCE              0x1E004601 /* scroll view */
#define DIBUTTON_SOCCERO_SUBSTITUTE             0x1E004407 /* Substitute one player for another */
#define DIBUTTON_SOCCERO_SHOOTLOW               0x1E004408 /* Shoot the ball low */
#define DIBUTTON_SOCCERO_SHOOTHIGH              0x1E004409 /* Shoot the ball high */
#define DIBUTTON_SOCCERO_PASSTHRU               0x1E00440A /* Make a thru pass */
#define DIBUTTON_SOCCERO_SPRINT                 0x1E00440B /* Sprint / turbo boost */
#define DIBUTTON_SOCCERO_CONTROL                0x1E00440C /* Obtain control of the ball */
#define DIBUTTON_SOCCERO_HEAD                   0x1E00440D /* Attempt to head the ball */
#define DIBUTTON_SOCCERO_LEFT_LINK              0x1E00C4E4 /* Fallback sidestep left button */
#define DIBUTTON_SOCCERO_RIGHT_LINK             0x1E00C4EC /* Fallback sidestep right button */
#define DIBUTTON_SOCCERO_FORWARD_LINK           0x1E0144E0 /* Fallback move forward button */
#define DIBUTTON_SOCCERO_BACK_LINK              0x1E0144E8 /* Fallback move back button */
#define DIBUTTON_SOCCERO_DEVICE                 0x1E0044FE /* Show input device and controls */
#define DIBUTTON_SOCCERO_PAUSE                  0x1E0044FC /* Start / Pause / Restart game */

/*--- Sports - Soccer - Defense
      Defense       ---*/
#define DIVIRTUAL_SPORTS_SOCCER_DEFENSE         0x1F000000
#define DIAXIS_SOCCERD_LATERAL                  0x1F008201 /* Move / Aim: left / right */
#define DIAXIS_SOCCERD_MOVE                     0x1F010202 /* Move / Aim: up / down */
#define DIBUTTON_SOCCERD_BLOCK                  0x1F000401 /* Attempt to block shot */
#define DIBUTTON_SOCCERD_STEAL                  0x1F000402 /* Attempt to steal ball */
#define DIBUTTON_SOCCERD_FAKE                   0x1F000403 /* Fake a block or a steal */
#define DIBUTTON_SOCCERD_PLAYER                 0x1F000404 /* Select next player */
#define DIBUTTON_SOCCERD_SPECIAL                0x1F000405 /* Apply special move */
#define DIBUTTON_SOCCERD_SELECT                 0x1F000406 /* Select special move */
#define DIBUTTON_SOCCERD_SLIDE                  0x1F000407 /* Attempt a slide tackle */
#define DIBUTTON_SOCCERD_MENU                   0x1F0004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_SOCCERD_GLANCE              0x1F004601 /* scroll view */
#define DIBUTTON_SOCCERD_FOUL                   0x1F004408 /* Initiate a foul / hard-foul */
#define DIBUTTON_SOCCERD_HEAD                   0x1F004409 /* Attempt a Header */
#define DIBUTTON_SOCCERD_CLEAR                  0x1F00440A /* Attempt to clear the ball down the field */
#define DIBUTTON_SOCCERD_GOALIECHARGE           0x1F00440B /* Make the goalie charge out of the box */
#define DIBUTTON_SOCCERD_SUBSTITUTE             0x1F00440C /* Substitute one player for another */
#define DIBUTTON_SOCCERD_LEFT_LINK              0x1F00C4E4 /* Fallback sidestep left button */
#define DIBUTTON_SOCCERD_RIGHT_LINK             0x1F00C4EC /* Fallback sidestep right button */
#define DIBUTTON_SOCCERD_FORWARD_LINK           0x1F0144E0 /* Fallback move forward button */
#define DIBUTTON_SOCCERD_BACK_LINK              0x1F0144E8 /* Fallback move back button */
#define DIBUTTON_SOCCERD_DEVICE                 0x1F0044FE /* Show input device and controls */
#define DIBUTTON_SOCCERD_PAUSE                  0x1F0044FC /* Start / Pause / Restart game */

/*--- Sports - Racquet
      Tennis - Table-Tennis - Squash   ---*/
#define DIVIRTUAL_SPORTS_RACQUET                0x20000000
#define DIAXIS_RACQUET_LATERAL                  0x20008201 /* Move / Aim: left / right */
#define DIAXIS_RACQUET_MOVE                     0x20010202 /* Move / Aim: up / down */
#define DIBUTTON_RACQUET_SWING                  0x20000401 /* Swing racquet */
#define DIBUTTON_RACQUET_BACKSWING              0x20000402 /* Swing backhand */
#define DIBUTTON_RACQUET_SMASH                  0x20000403 /* Smash shot */
#define DIBUTTON_RACQUET_SPECIAL                0x20000404 /* Special shot */
#define DIBUTTON_RACQUET_SELECT                 0x20000405 /* Select special shot */
#define DIBUTTON_RACQUET_MENU                   0x200004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_RACQUET_GLANCE              0x20004601 /* scroll view */
#define DIBUTTON_RACQUET_TIMEOUT                0x20004406 /* Call for time out */
#define DIBUTTON_RACQUET_SUBSTITUTE             0x20004407 /* Substitute one player for another */
#define DIBUTTON_RACQUET_LEFT_LINK              0x2000C4E4 /* Fallback sidestep left button */
#define DIBUTTON_RACQUET_RIGHT_LINK             0x2000C4EC /* Fallback sidestep right button */
#define DIBUTTON_RACQUET_FORWARD_LINK           0x200144E0 /* Fallback move forward button */
#define DIBUTTON_RACQUET_BACK_LINK              0x200144E8 /* Fallback move back button */
#define DIBUTTON_RACQUET_DEVICE                 0x200044FE /* Show input device and controls */
#define DIBUTTON_RACQUET_PAUSE                  0x200044FC /* Start / Pause / Restart game */

/*--- Arcade- 2D
      Side to Side movement        ---*/
#define DIVIRTUAL_ARCADE_SIDE2SIDE              0x21000000
#define DIAXIS_ARCADES_LATERAL                  0x21008201 /* left / right */
#define DIAXIS_ARCADES_MOVE                     0x21010202 /* up / down */
#define DIBUTTON_ARCADES_THROW                  0x21000401 /* throw object */
#define DIBUTTON_ARCADES_CARRY                  0x21000402 /* carry object */
#define DIBUTTON_ARCADES_ATTACK                 0x21000403 /* attack */
#define DIBUTTON_ARCADES_SPECIAL                0x21000404 /* apply special move */
#define DIBUTTON_ARCADES_SELECT                 0x21000405 /* select special move */
#define DIBUTTON_ARCADES_MENU                   0x210004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_ARCADES_VIEW                0x21004601 /* scroll view left / right / up / down */
#define DIBUTTON_ARCADES_LEFT_LINK              0x2100C4E4 /* Fallback sidestep left button */
#define DIBUTTON_ARCADES_RIGHT_LINK             0x2100C4EC /* Fallback sidestep right button */
#define DIBUTTON_ARCADES_FORWARD_LINK           0x210144E0 /* Fallback move forward button */
#define DIBUTTON_ARCADES_BACK_LINK              0x210144E8 /* Fallback move back button */
#define DIBUTTON_ARCADES_VIEW_UP_LINK           0x2107C4E0 /* Fallback scroll view up button */
#define DIBUTTON_ARCADES_VIEW_DOWN_LINK         0x2107C4E8 /* Fallback scroll view down button */
#define DIBUTTON_ARCADES_VIEW_LEFT_LINK         0x2107C4E4 /* Fallback scroll view left button */
#define DIBUTTON_ARCADES_VIEW_RIGHT_LINK        0x2107C4EC /* Fallback scroll view right button */
#define DIBUTTON_ARCADES_DEVICE                 0x210044FE /* Show input device and controls */
#define DIBUTTON_ARCADES_PAUSE                  0x210044FC /* Start / Pause / Restart game */

/*--- Arcade - Platform Game
      Character moves around on screen  ---*/
#define DIVIRTUAL_ARCADE_PLATFORM               0x22000000
#define DIAXIS_ARCADEP_LATERAL                  0x22008201 /* Left / right */
#define DIAXIS_ARCADEP_MOVE                     0x22010202 /* Up / down */
#define DIBUTTON_ARCADEP_JUMP                   0x22000401 /* Jump */
#define DIBUTTON_ARCADEP_FIRE                   0x22000402 /* Fire */
#define DIBUTTON_ARCADEP_CROUCH                 0x22000403 /* Crouch */
#define DIBUTTON_ARCADEP_SPECIAL                0x22000404 /* Apply special move */
#define DIBUTTON_ARCADEP_SELECT                 0x22000405 /* Select special move */
#define DIBUTTON_ARCADEP_MENU                   0x220004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_ARCADEP_VIEW                0x22004601 /* Scroll view */
#define DIBUTTON_ARCADEP_FIRESECONDARY          0x22004406 /* Alternative fire button */
#define DIBUTTON_ARCADEP_LEFT_LINK              0x2200C4E4 /* Fallback sidestep left button */
#define DIBUTTON_ARCADEP_RIGHT_LINK             0x2200C4EC /* Fallback sidestep right button */
#define DIBUTTON_ARCADEP_FORWARD_LINK           0x220144E0 /* Fallback move forward button */
#define DIBUTTON_ARCADEP_BACK_LINK              0x220144E8 /* Fallback move back button */
#define DIBUTTON_ARCADEP_VIEW_UP_LINK           0x2207C4E0 /* Fallback scroll view up button */
#define DIBUTTON_ARCADEP_VIEW_DOWN_LINK         0x2207C4E8 /* Fallback scroll view down button */
#define DIBUTTON_ARCADEP_VIEW_LEFT_LINK         0x2207C4E4 /* Fallback scroll view left button */
#define DIBUTTON_ARCADEP_VIEW_RIGHT_LINK        0x2207C4EC /* Fallback scroll view right button */
#define DIBUTTON_ARCADEP_DEVICE                 0x220044FE /* Show input device and controls */
#define DIBUTTON_ARCADEP_PAUSE                  0x220044FC /* Start / Pause / Restart game */

/*--- CAD - 2D Object Control
      Controls to select and move objects in 2D  ---*/
#define DIVIRTUAL_CAD_2DCONTROL                 0x23000000
#define DIAXIS_2DCONTROL_LATERAL                0x23008201 /* Move view left / right */
#define DIAXIS_2DCONTROL_MOVE                   0x23010202 /* Move view up / down */
#define DIAXIS_2DCONTROL_INOUT                  0x23018203 /* Zoom - in / out */
#define DIBUTTON_2DCONTROL_SELECT               0x23000401 /* Select Object */
#define DIBUTTON_2DCONTROL_SPECIAL1             0x23000402 /* Do first special operation */
#define DIBUTTON_2DCONTROL_SPECIAL              0x23000403 /* Select special operation */
#define DIBUTTON_2DCONTROL_SPECIAL2             0x23000404 /* Do second special operation */
#define DIBUTTON_2DCONTROL_MENU                 0x230004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_2DCONTROL_HATSWITCH         0x23004601 /* Hat switch */
#define DIAXIS_2DCONTROL_ROTATEZ                0x23024204 /* Rotate view clockwise / counterclockwise */
#define DIBUTTON_2DCONTROL_DISPLAY              0x23004405 /* Shows next on-screen display options */
#define DIBUTTON_2DCONTROL_DEVICE               0x230044FE /* Show input device and controls */
#define DIBUTTON_2DCONTROL_PAUSE                0x230044FC /* Start / Pause / Restart game */

/*--- CAD - 3D object control
      Controls to select and move objects within a 3D environment  ---*/
#define DIVIRTUAL_CAD_3DCONTROL                 0x24000000
#define DIAXIS_3DCONTROL_LATERAL                0x24008201 /* Move view left / right */
#define DIAXIS_3DCONTROL_MOVE                   0x24010202 /* Move view up / down */
#define DIAXIS_3DCONTROL_INOUT                  0x24018203 /* Zoom - in / out */
#define DIBUTTON_3DCONTROL_SELECT               0x24000401 /* Select Object */
#define DIBUTTON_3DCONTROL_SPECIAL1             0x24000402 /* Do first special operation */
#define DIBUTTON_3DCONTROL_SPECIAL              0x24000403 /* Select special operation */
#define DIBUTTON_3DCONTROL_SPECIAL2             0x24000404 /* Do second special operation */
#define DIBUTTON_3DCONTROL_MENU                 0x240004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_3DCONTROL_HATSWITCH         0x24004601 /* Hat switch */
#define DIAXIS_3DCONTROL_ROTATEX                0x24034204 /* Rotate view forward or up / backward or down */
#define DIAXIS_3DCONTROL_ROTATEY                0x2402C205 /* Rotate view clockwise / counterclockwise */
#define DIAXIS_3DCONTROL_ROTATEZ                0x24024206 /* Rotate view left / right */
#define DIBUTTON_3DCONTROL_DISPLAY              0x24004405 /* Show next on-screen display options */
#define DIBUTTON_3DCONTROL_DEVICE               0x240044FE /* Show input device and controls */
#define DIBUTTON_3DCONTROL_PAUSE                0x240044FC /* Start / Pause / Restart game */

/*--- CAD - 3D Navigation - Fly through
      Controls for 3D modeling  ---*/
#define DIVIRTUAL_CAD_FLYBY                     0x25000000
#define DIAXIS_CADF_LATERAL                     0x25008201 /* move view left / right */
#define DIAXIS_CADF_MOVE                        0x25010202 /* move view up / down */
#define DIAXIS_CADF_INOUT                       0x25018203 /* in / out */
#define DIBUTTON_CADF_SELECT                    0x25000401 /* Select Object */
#define DIBUTTON_CADF_SPECIAL1                  0x25000402 /* do first special operation */
#define DIBUTTON_CADF_SPECIAL                   0x25000403 /* Select special operation */
#define DIBUTTON_CADF_SPECIAL2                  0x25000404 /* do second special operation */
#define DIBUTTON_CADF_MENU                      0x250004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_CADF_HATSWITCH              0x25004601 /* Hat switch */
#define DIAXIS_CADF_ROTATEX                     0x25034204 /* Rotate view forward or up / backward or down */
#define DIAXIS_CADF_ROTATEY                     0x2502C205 /* Rotate view clockwise / counterclockwise */
#define DIAXIS_CADF_ROTATEZ                     0x25024206 /* Rotate view left / right */
#define DIBUTTON_CADF_DISPLAY                   0x25004405 /* shows next on-screen display options */
#define DIBUTTON_CADF_DEVICE                    0x250044FE /* Show input device and controls */
#define DIBUTTON_CADF_PAUSE                     0x250044FC /* Start / Pause / Restart game */

/*--- CAD - 3D Model Control
      Controls for 3D modeling  ---*/
#define DIVIRTUAL_CAD_MODEL                     0x26000000
#define DIAXIS_CADM_LATERAL                     0x26008201 /* move view left / right */
#define DIAXIS_CADM_MOVE                        0x26010202 /* move view up / down */
#define DIAXIS_CADM_INOUT                       0x26018203 /* in / out */
#define DIBUTTON_CADM_SELECT                    0x26000401 /* Select Object */
#define DIBUTTON_CADM_SPECIAL1                  0x26000402 /* do first special operation */
#define DIBUTTON_CADM_SPECIAL                   0x26000403 /* Select special operation */
#define DIBUTTON_CADM_SPECIAL2                  0x26000404 /* do second special operation */
#define DIBUTTON_CADM_MENU                      0x260004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIHATSWITCH_CADM_HATSWITCH              0x26004601 /* Hat switch */
#define DIAXIS_CADM_ROTATEX                     0x26034204 /* Rotate view forward or up / backward or down */
#define DIAXIS_CADM_ROTATEY                     0x2602C205 /* Rotate view clockwise / counterclockwise */
#define DIAXIS_CADM_ROTATEZ                     0x26024206 /* Rotate view left / right */
#define DIBUTTON_CADM_DISPLAY                   0x26004405 /* shows next on-screen display options */
#define DIBUTTON_CADM_DEVICE                    0x260044FE /* Show input device and controls */
#define DIBUTTON_CADM_PAUSE                     0x260044FC /* Start / Pause / Restart game */

/*--- Control - Media Equipment
      Remote        ---*/
#define DIVIRTUAL_REMOTE_CONTROL                0x27000000
#define DIAXIS_REMOTE_SLIDER                    0x27050201 /* Slider for adjustment: volume / color / bass / etc */
#define DIBUTTON_REMOTE_MUTE                    0x27000401 /* Set volume on current device to zero */
#define DIBUTTON_REMOTE_SELECT                  0x27000402 /* Next/previous: channel/ track / chapter / picture / station */
#define DIBUTTON_REMOTE_PLAY                    0x27002403 /* Start or pause entertainment on current device */
#define DIBUTTON_REMOTE_CUE                     0x27002404 /* Move through current media */
#define DIBUTTON_REMOTE_REVIEW                  0x27002405 /* Move through current media */
#define DIBUTTON_REMOTE_CHANGE                  0x27002406 /* Select next device */
#define DIBUTTON_REMOTE_RECORD                  0x27002407 /* Start recording the current media */
#define DIBUTTON_REMOTE_MENU                    0x270004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIAXIS_REMOTE_SLIDER2                   0x27054202 /* Slider for adjustment: volume */
#define DIBUTTON_REMOTE_TV                      0x27005C08 /* Select TV */
#define DIBUTTON_REMOTE_CABLE                   0x27005C09 /* Select cable box */
#define DIBUTTON_REMOTE_CD                      0x27005C0A /* Select CD player */
#define DIBUTTON_REMOTE_VCR                     0x27005C0B /* Select VCR */
#define DIBUTTON_REMOTE_TUNER                   0x27005C0C /* Select tuner */
#define DIBUTTON_REMOTE_DVD                     0x27005C0D /* Select DVD player */
#define DIBUTTON_REMOTE_ADJUST                  0x27005C0E /* Enter device adjustment menu */
#define DIBUTTON_REMOTE_DIGIT0                  0x2700540F /* Digit 0 */
#define DIBUTTON_REMOTE_DIGIT1                  0x27005410 /* Digit 1 */
#define DIBUTTON_REMOTE_DIGIT2                  0x27005411 /* Digit 2 */
#define DIBUTTON_REMOTE_DIGIT3                  0x27005412 /* Digit 3 */
#define DIBUTTON_REMOTE_DIGIT4                  0x27005413 /* Digit 4 */
#define DIBUTTON_REMOTE_DIGIT5                  0x27005414 /* Digit 5 */
#define DIBUTTON_REMOTE_DIGIT6                  0x27005415 /* Digit 6 */
#define DIBUTTON_REMOTE_DIGIT7                  0x27005416 /* Digit 7 */
#define DIBUTTON_REMOTE_DIGIT8                  0x27005417 /* Digit 8 */
#define DIBUTTON_REMOTE_DIGIT9                  0x27005418 /* Digit 9 */
#define DIBUTTON_REMOTE_DEVICE                  0x270044FE /* Show input device and controls */
#define DIBUTTON_REMOTE_PAUSE                   0x270044FC /* Start / Pause / Restart game */

/*--- Control- Web
      Help or Browser            ---*/
#define DIVIRTUAL_BROWSER_CONTROL               0x28000000
#define DIAXIS_BROWSER_LATERAL                  0x28008201 /* Move on screen pointer */
#define DIAXIS_BROWSER_MOVE                     0x28010202 /* Move on screen pointer */
#define DIBUTTON_BROWSER_SELECT                 0x28000401 /* Select current item */
#define DIAXIS_BROWSER_VIEW                     0x28018203 /* Move view up/down */
#define DIBUTTON_BROWSER_REFRESH                0x28000402 /* Refresh */
#define DIBUTTON_BROWSER_MENU                   0x280004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIBUTTON_BROWSER_SEARCH                 0x28004403 /* Use search tool */
#define DIBUTTON_BROWSER_STOP                   0x28004404 /* Cease current update */
#define DIBUTTON_BROWSER_HOME                   0x28004405 /* Go directly to "home" location */
#define DIBUTTON_BROWSER_FAVORITES              0x28004406 /* Mark current site as favorite */
#define DIBUTTON_BROWSER_NEXT                   0x28004407 /* Select Next page */
#define DIBUTTON_BROWSER_PREVIOUS               0x28004408 /* Select Previous page */
#define DIBUTTON_BROWSER_HISTORY                0x28004409 /* Show/Hide History */
#define DIBUTTON_BROWSER_PRINT                  0x2800440A /* Print current page */
#define DIBUTTON_BROWSER_DEVICE                 0x280044FE /* Show input device and controls */
#define DIBUTTON_BROWSER_PAUSE                  0x280044FC /* Start / Pause / Restart game */

/*--- Driving Simulator - Giant Walking Robot
      Walking tank with weapons  ---*/
#define DIVIRTUAL_DRIVING_MECHA                 0x29000000
#define DIAXIS_MECHA_STEER                      0x29008201 /* Turns mecha left/right */
#define DIAXIS_MECHA_TORSO                      0x29010202 /* Tilts torso forward/backward */
#define DIAXIS_MECHA_ROTATE                     0x29020203 /* Turns torso left/right */
#define DIAXIS_MECHA_THROTTLE                   0x29038204 /* Engine Speed */
#define DIBUTTON_MECHA_FIRE                     0x29000401 /* Fire */
#define DIBUTTON_MECHA_WEAPONS                  0x29000402 /* Select next weapon group */
#define DIBUTTON_MECHA_TARGET                   0x29000403 /* Select closest enemy available target */
#define DIBUTTON_MECHA_REVERSE                  0x29000404 /* Toggles throttle in/out of reverse */
#define DIBUTTON_MECHA_ZOOM                     0x29000405 /* Zoom in/out targeting reticule */
#define DIBUTTON_MECHA_JUMP                     0x29000406 /* Fires jump jets */
#define DIBUTTON_MECHA_MENU                     0x290004FD /* Show menu options */
/*--- Priority 2 controls                            ---*/

#define DIBUTTON_MECHA_CENTER                   0x29004407 /* Center torso to legs */
#define DIHATSWITCH_MECHA_GLANCE                0x29004601 /* Look around */
#define DIBUTTON_MECHA_VIEW                     0x29004408 /* Cycle through view options */
#define DIBUTTON_MECHA_FIRESECONDARY            0x29004409 /* Alternative fire button */
#define DIBUTTON_MECHA_LEFT_LINK                0x2900C4E4 /* Fallback steer left button */
#define DIBUTTON_MECHA_RIGHT_LINK               0x2900C4EC /* Fallback steer right button */
#define DIBUTTON_MECHA_FORWARD_LINK             0x290144E0 /* Fallback tilt torso forward button */
#define DIBUTTON_MECHA_BACK_LINK                0x290144E8 /* Fallback tilt toroso backward button */
#define DIBUTTON_MECHA_ROTATE_LEFT_LINK         0x290244E4 /* Fallback rotate toroso right button */
#define DIBUTTON_MECHA_ROTATE_RIGHT_LINK        0x290244EC /* Fallback rotate torso left button */
#define DIBUTTON_MECHA_FASTER_LINK              0x2903C4E0 /* Fallback increase engine speed */
#define DIBUTTON_MECHA_SLOWER_LINK              0x2903C4E8 /* Fallback decrease engine speed */
#define DIBUTTON_MECHA_DEVICE                   0x290044FE /* Show input device and controls */
#define DIBUTTON_MECHA_PAUSE                    0x290044FC /* Start / Pause / Restart game */

/*
 *  "ANY" semantics can be used as a last resort to get mappings for actions 
 *  that match nothing in the chosen virtual genre.  These semantics will be 
 *  mapped at a lower priority that virtual genre semantics.  Also, hardware 
 *  vendors will not be able to provide sensible mappings for these unless 
 *  they provide application specific mappings.
 */
#define DIAXIS_ANY_X_1                          0xFF00C201 
#define DIAXIS_ANY_X_2                          0xFF00C202 
#define DIAXIS_ANY_Y_1                          0xFF014201 
#define DIAXIS_ANY_Y_2                          0xFF014202 
#define DIAXIS_ANY_Z_1                          0xFF01C201 
#define DIAXIS_ANY_Z_2                          0xFF01C202 
#define DIAXIS_ANY_R_1                          0xFF024201 
#define DIAXIS_ANY_R_2                          0xFF024202 
#define DIAXIS_ANY_U_1                          0xFF02C201 
#define DIAXIS_ANY_U_2                          0xFF02C202 
#define DIAXIS_ANY_V_1                          0xFF034201 
#define DIAXIS_ANY_V_2                          0xFF034202 
#define DIAXIS_ANY_A_1                          0xFF03C201 
#define DIAXIS_ANY_A_2                          0xFF03C202 
#define DIAXIS_ANY_B_1                          0xFF044201 
#define DIAXIS_ANY_B_2                          0xFF044202 
#define DIAXIS_ANY_C_1                          0xFF04C201 
#define DIAXIS_ANY_C_2                          0xFF04C202 
#define DIAXIS_ANY_S_1                          0xFF054201 
#define DIAXIS_ANY_S_2                          0xFF054202 

#define DIAXIS_ANY_1                            0xFF004201 
#define DIAXIS_ANY_2                            0xFF004202 
#define DIAXIS_ANY_3                            0xFF004203 
#define DIAXIS_ANY_4                            0xFF004204 

#define DIPOV_ANY_1                             0xFF004601 
#define DIPOV_ANY_2                             0xFF004602 
#define DIPOV_ANY_3                             0xFF004603 
#define DIPOV_ANY_4                             0xFF004604 

#define DIBUTTON_ANY(instance)                  ( 0xFF004400 | instance )


#ifdef __cplusplus
};
#endif

#endif  /* __DINPUT_INCLUDED__ */

/****************************************************************************
 *
 *  Definitions for non-IDirectInput (VJoyD) features defined more recently
 *  than the current sdk files
 *
 ****************************************************************************/

#ifdef _INC_MMSYSTEM
#ifndef MMNOJOY

#ifndef __VJOYDX_INCLUDED__
#define __VJOYDX_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Flag to indicate that the dwReserved2 field of the JOYINFOEX structure
 * contains mini-driver specific data to be passed by VJoyD to the mini-
 * driver instead of doing a poll.
 */
#define JOY_PASSDRIVERDATA          0x10000000l

/*
 * Informs the joystick driver that the configuration has been changed
 * and should be reloaded from the registery.
 * dwFlags is reserved and should be set to zero
 */
WINMMAPI MMRESULT WINAPI joyConfigChanged( DWORD dwFlags );

#ifndef DIJ_RINGZERO
/*
 * Invoke the joystick control panel directly, using the passed window handle 
 * as the parent of the dialog.  This API is only supported for compatibility 
 * purposes; new applications should use the RunControlPanel method of a 
 * device interface for a game controller.
 * The API is called by using the function pointer returned by
 * GetProcAddress( hCPL, TEXT("ShowJoyCPL") ) where hCPL is a HMODULE returned 
 * by LoadLibrary( TEXT("joy.cpl") ).  The typedef is provided to allow 
 * declaration and casting of an appropriately typed variable.
 */
void WINAPI ShowJoyCPL( HWND hWnd );
typedef void (WINAPI* LPFNSHOWJOYCPL)( HWND hWnd );
#endif /* DIJ_RINGZERO */


/*
 * Hardware Setting indicating that the device is a headtracker
 */
#define JOY_HWS_ISHEADTRACKER       0x02000000l

/*
 * Hardware Setting indicating that the VxD is used to replace
 * the standard analog polling
 */
#define JOY_HWS_ISGAMEPORTDRIVER    0x04000000l

/*
 * Hardware Setting indicating that the driver needs a standard
 * gameport in order to communicate with the device.
 */
#define JOY_HWS_ISANALOGPORTDRIVER  0x08000000l

/*
 * Hardware Setting indicating that VJoyD should not load this
 * driver, it will be loaded externally and will register with
 * VJoyD of it's own accord.
 */
#define JOY_HWS_AUTOLOAD            0x10000000l

/*
 * Hardware Setting indicating that the driver acquires any
 * resources needed without needing a devnode through VJoyD.
 */
#define JOY_HWS_NODEVNODE           0x20000000l


/*
 * Hardware Setting indicating that the device is a gameport bus
 */
#define JOY_HWS_ISGAMEPORTBUS       0x80000000l
#define JOY_HWS_GAMEPORTBUSBUSY     0x00000001l

/*
 * Usage Setting indicating that the settings are volatile and
 * should be removed if still present on a reboot.
 */
#define JOY_US_VOLATILE             0x00000008L

#ifdef __cplusplus
};
#endif

#endif  /* __VJOYDX_INCLUDED__ */

#endif  /* not MMNOJOY */
#endif  /* _INC_MMSYSTEM */

/****************************************************************************
 *
 *  Definitions for non-IDirectInput (VJoyD) features defined more recently
 *  than the current ddk files
 *
 ****************************************************************************/

#ifndef DIJ_RINGZERO

#ifdef _INC_MMDDK
#ifndef MMNOJOYDEV

#ifndef __VJOYDXD_INCLUDED__
#define __VJOYDXD_INCLUDED__
/*
 * Poll type in which the do_other field of the JOYOEMPOLLDATA
 * structure contains mini-driver specific data passed from an app.
 */
#define JOY_OEMPOLL_PASSDRIVERDATA  7

#endif  /* __VJOYDXD_INCLUDED__ */

#endif  /* not MMNOJOYDEV */
#endif  /* _INC_MMDDK */

#endif /* DIJ_RINGZERO */
#pragma once

#include "common.h"
#include "game.h"
#include "resource_key.h"
#include "resource_manager.h"
#include "fixedstring.h"
#include "resource_partition.h"
#include "igofrontend.h"
#include "string_hash.h"
#include "actor.h"
#include "entity.h"
#include "femanager.h"
#include "debug_menu.h"
#include "mstring.h"
#include "fe_health_widget.h"
#include "wds.h"
#include "ai_player_controller.h"

#include <cassert>

constexpr auto NUM_HEROES = 19u;

const char* hero_list[NUM_HEROES] = {
        "ultimate_spiderman",
        "arachno_man_costume",
        "usm_wrestling_costume",
        "usm_blacksuit_costume",
        "peter_parker",
        "peter_parker_costume",
        "peter_hooded",
        "peter_hooded_costume",
        "venom",
        "venom_spider",
        "carnage",
        "rhino",
        "electro_suit",
        "electro_nosuit",
        "green_goblin",
        "silver_sable",
        "mary_jane",
        "wolverine",
        "beetle"
};

enum class hero_status_e {
    UNDEFINED = 0,
    REMOVE_PLAYER = 1,
    ADD_PLAYER = 2,
    CHANGE_HEALTH_TYPE = 3,
} hero_status;

int hero_selected;
int frames_to_skip = 2;

struct level_descriptor_t
{
    fixedstring<32> field_0;
    fixedstring<64> field_20;
    fixedstring<16> field_60;
    int field_70;
    int field_74;
    int field_78;
    int field_7C;
    int field_80;
    int field_84;
    int field_88;
    int field_8C;
};

VALIDATE_SIZE(level_descriptor_t, 0x90);

level_descriptor_t *get_level_descriptors(int *arg0)
{
    auto *game_partition = resource_manager::get_partition_pointer(0);
    assert(game_partition != nullptr);

    assert(game_partition->get_pack_slots().size() == 1);

    auto &v2 = game_partition->get_pack_slots();
    auto *game_slot = v2.front();
    assert(game_slot != nullptr);

    auto v6 = 9;
    string_hash v5 {"level"};
    resource_key a1 {v5, v6};
    int a2 = 0;
    auto *v11 = (level_descriptor_t *) game_slot->get_resource(a1, &a2, nullptr);

    if ( arg0 != nullptr )
    {
        *arg0 = a2 / sizeof(level_descriptor_t);
    }

    return v11;
}

void level_select_handler(debug_menu_entry *entry)
{
    auto *v1 = entry->text;
    mString v15 {v1};

    int arg0;
    auto *v13 = get_level_descriptors(&arg0);
    for ( auto i = 0; i < arg0; ++i )
    {
        auto *v2 = v15.c_str();
        fixedstring<16> v6 {v2};
        if ( v13[i].field_60 == v6 )
        {
            auto *v3 = v13[i].field_0.to_string();
            v15 = {v3};
            break;
        }
    }

    g_game_ptr()->load_new_level(v15, -1);
}


struct reboot_t {
    std::string field_0;
    const char* field_C;
    int field_10;
    int field_14;
};

std::vector<reboot_t> menu_reboot; 

void reboot_handler(debug_menu_entry* a1)
{
    auto v1 = (int)a1->m_id;
    auto v7 = &menu_reboot.at(v1);
    auto v6 = v7->field_C;
    auto v5 = v7->field_14;
    auto* v4 = v7->field_0.c_str();
    auto v3 = v7->field_10;
    auto* v2 = mission_manager::s_inst();
    v2->force_mission(v3, v4, v5, v6);
    close_debug();
}

void handle_hero_select_menu(debug_menu_entry *entry, custom_key_type)
{
    entry->m_game_flags_handler(entry);
}

void hero_entry_callback(debug_menu_entry *);

void hero_toggle_handler(debug_menu_entry *entry);



void populate_level_select_menu(debug_menu_entry* entry)
{
    // assert(debug_menu::root_menu != nullptr);

    auto* head_menu = create_menu("Level Select");
    entry->set_submenu(head_menu);

    auto* hero_select_menu = create_menu("Hero Select");
    debug_menu_entry v28 { hero_select_menu };

    head_menu->add_entry(&v28);
    for (auto i = 0u; i < 19u; ++i) {
        auto v6 = 25;
        string_hash v5 { (hero_list)[i] };
        auto v11 = resource_key { v5, v6 };
        auto v30 = resource_manager::get_pack_file_stats(v11, nullptr, nullptr, nullptr);
        if (v30) {
            mString v35 { hero_list[i] };
            debug_menu_entry v37 { v35.c_str() };
                v37.set_game_flags_handler(hero_toggle_handler);
                v37.m_id = i;
                v37.set_frame_advance_cb(hero_entry_callback);
                hero_select_menu->add_entry(&v37);
            }
            }


    mString v26 { "city" };
    debug_menu_entry v39 { v26.c_str() };

    v39.set_game_flags_handler(level_select_handler);

        head_menu->add_entry(&v39);

        const mString& v27 { "characterb" };

    debug_menu_entry v41 { v27.c_str() };

    v41.set_game_flags_handler(level_select_handler);


    head_menu->add_entry(&v41);

    mString v25 { "-- REBOOT --" };
    debug_menu_entry v40 { v25.c_str() };

    v40.set_game_flags_handler(reboot_handler);

    head_menu->add_entry(&v40);
}

void create_level_select_menu(debug_menu* parent)
{
    auto* level_select_menu = create_menu("Level Select", debug_menu::sort_mode_t::undefined);
    auto* v2 = create_menu_entry(level_select_menu);
    v2->set_game_flags_handler(populate_level_select_menu);
    parent->add_entry(v2);
}

struct debug_menu_entry;

void hero_toggle_handler(debug_menu_entry *entry)
{
    printf("hero_toggle_handler\n");
    assert(entry->get_id() < NUM_HEROES);
    hero_selected = entry->get_id();
    hero_status = hero_status_e::REMOVE_PLAYER;
}

void hero_entry_callback(debug_menu_entry *)
{
    printf("hero_entry_callback: hero_status = %d\n", hero_status);

    auto v18 = g_world_ptr()->num_players;
    switch ( hero_status )
    {
    case hero_status_e::REMOVE_PLAYER:
    {
        g_world_ptr()->remove_player2(v18 - 1);
        hero_status = hero_status_e::ADD_PLAYER;
        frames_to_skip = 2;
        g_game_ptr()->enable_marky_cam(true, true, -1000.0, 0.0);
        break;
    }
    case hero_status_e::ADD_PLAYER:
    {
        auto v1 = frames_to_skip--;
        if ( v1 <= 0 )
        {
            assert(hero_selected > -1 && hero_selected < NUM_HEROES);

            [[maybe_unused]] auto v2 = g_world_ptr()->add_player(mString {hero_list[hero_selected]});

            /*
            auto v10 = v2 <= v18;

            assert(v10 && "Cannot add another_player");
            */
               
            g_game_ptr()->enable_marky_cam(false, true, -1000.0, 0.0);
            frames_to_skip = 2;
            hero_status = hero_status_e::CHANGE_HEALTH_TYPE;
        }
        break;
    }
    case hero_status_e::CHANGE_HEALTH_TYPE: {
        auto v3 = frames_to_skip--;
        if ( v3 <= 0 )
        {
            auto v17 = 0;
            auto *v5 = (actor *) g_world_ptr()->get_hero_ptr(0);
            auto *v6 = v5->get_player_controller();
            auto v9 = v6->field_420;
            switch ( v9 )
            {
            case 1:
                v17 = 0;
                break;
            case 2:
                v17 = 4;
                break;
            case 3:
                v17 = 5;
                break;
            }

            auto *v7 = g_world_ptr()->get_hero_ptr(0);
            auto v8 = v7->my_handle;
            g_femanager().IGO->hero_health->SetType(v17, v8.field_0);
            g_femanager().IGO->hero_health->SetShown(true);
            close_debug();
            hero_status = hero_status_e::UNDEFINED;
        }
        break;
    }
    default:
        break;
    }
}
#pragma once

#include "bit.h"
#include "float.hpp"
#include "variable.h"

#include "utility.h"

struct entity;
struct mString;
struct terrain;
struct vector3d;

struct world_dynamics_system
{
    char field_0[0x1AC];
    terrain *the_terrain;
    char field_1B0[0x40 * 2];
    entity *field_230[1];
    int field_234;
    int num_players;

    auto *get_the_terrain() {
        return the_terrain;
    }

    entity *get_hero_ptr(int index);

    void remove_player2(int player_num)
    {
        void (__fastcall *func)(void *, void *, int) = bit_cast<decltype(func)>(0x00558550);

        func(this, nullptr, player_num);
    }

    void malor_point(const vector3d *a2, int a3, bool a4)
    {
        void (__fastcall *func)(void *, void *, const vector3d *, int, bool) = bit_cast<decltype(func)>(0x00530460);
        func(this, nullptr, a2, a3, a4);
    }

    int add_player(const mString &a2)
    {
 

                int(__fastcall * func)(void*, void*, const mString*) = CAST(func, 0x0055B400);
        return func(this, nullptr, &a2);
    }

    void frame_advance(Float a2);

};

inline Var<world_dynamics_system *> g_world_ptr{0x0095C770};

extern void wds_patch();

#pragma once

#include "src/chuck/vm/script_library_class.h"

#include "src/chuck/vm/vm_stack.h"

struct slf__create_debug_menu_entry__str__str__t : script_library_class::function {
    slf__create_debug_menu_entry__str__str__t(const char *a3);

    struct parms_t {
        vm_str_t str0;
        vm_str_t str1;
    };

    bool operator()(vm_stack &stack, [[maybe_unused]]script_library_class::function::entry_t entry) const;
};

extern void script_lib_debug_menu_patch();


#pragma once

#include  <cassert>

#include  "log.h"

using vm_num_t = float;
using vm_str_t = const char *;

struct vm_thread;

inline constexpr auto UNINITIALIZED_SCRIPT_PARM = 0x7BAD05CF;

class vm_stack {
    int field_0[96];

public:
    char *buffer;

private:
    char *SP;
    vm_thread *my_thread;

public:
    vm_stack(vm_thread *t);

    int capacity() const {
        return 96 * 4;
    }

    vm_thread * get_thread() {
        return my_thread;
    }

    int size() const {
        return this->SP - this->buffer;
    }

    vm_num_t& top_num() {
        return *(vm_num_t *)(SP - sizeof(vm_num_t));
    }

    vm_str_t& top_str() {
        return *(vm_str_t *)(SP - sizeof(vm_str_t));
    }



void push(const char* a2, int n)
    {
#if enable_trace
        TRACE("vm_stack::push(const char *, int)");
#endif

        sp_log("0x%X 0x%X %d", this->get_SP(), a2, n);

        assert(size() + n <= capacity());

        std::memcpy(this->SP, a2, n);
        this->move_SP(n);

        sp_log("size = %d", this->size());
    }

void move_SP(int n)
    {
#if enable_trace
        TRACE("vm_stack::move_SP");
#endif

        assert(!(n & 3));

        assert(!((unsigned)SP & 3));
        this->SP += n;

        auto my_size = this->size();

        assert(my_size >= 0 && "underflow -> VM stack corruption");
        if (my_size > this->capacity()) {
            auto v2 = this->capacity();
            printf("capacity: %d", v2);
            assert(0 && "overflow -> bad scripter");
        }
    }


    char *get_SP() const {
        return this->SP;
    }

    void set_SP(char *sp) {
        SP = sp;
    }

    void move_SP(int n)
    {
        return ;
    }

    void pop(int n) {
        this->move_SP(-n);
    }

    void * pop_addr()
    {
        this->pop(sizeof(void *));
        return *(void**)SP;
    }

    vm_str_t pop_str()
    {
        this->pop(sizeof(vm_str_t));
        return *(vm_str_t *)SP;
    }

    vm_num_t pop_num();
};
#include "script_lib_debug_menu.h"

#include "debug_menu.h"
#include "mstring.h"
#include "script_executable.h"
#include "script_object.h"
#include "trace.h"
#include "utility.h"
#include "vm_executable.h"
#include "src/chuck/vm/vm_thread.h"
#include <list>
#include "script_manager.h"

static debug_menu *script_menu = nullptr;

static debug_menu *progression_menu = nullptr;

static int vm_debug_menu_entry_garbage_collection_id = -1;



void init_script_debug_menu()
{
    if ( script_menu == nullptr )
    {

        script_menu = create_menu("Script", debug_menu::sort_mode_t::undefined);

        progression_menu = create_menu("Progression", debug_menu::sort_mode_t::undefined);

        debug_menu::root_menu->add_entry(script_menu);
        debug_menu::root_menu->add_entry(progression_menu);
    }
}

void vm_debug_menu_entry_garbage_collection_callback(script_executable *,
                                                    std::list<uint32_t> &a2,
                                                    std::list<mString> &)
{
    for ( auto &v2 : a2 )
    {
        assert(script_menu != nullptr);

        auto *entry = bit_cast<debug_menu_entry *>(v2);
        script_menu->remove_entry(entry);
    }
}

void construct_debug_menu_lib()
{
    if ( vm_debug_menu_entry_garbage_collection_id == -1 ) {
        vm_debug_menu_entry_garbage_collection_id = script_manager::register_allocated_stuff_callback(vm_debug_menu_entry_garbage_collection_callback);
    }
}

slf__create_debug_menu_entry__str__str__t::slf__create_debug_menu_entry__str__str__t(const char *a3) : function(a3)
{
    m_vtbl = CAST(m_vtbl, 0x0089C70C);
    auto address = func_address(&slf__create_debug_menu_entry__str__str__t::operator());
    m_vtbl->__cl = CAST(m_vtbl->__cl, address);
}

bool slf__create_debug_menu_entry__str__str__t::operator()(vm_stack &stack, [[maybe_unused]]script_library_class::function::entry_t entry) const
{
    TRACE("slf__create_debug_menu_entry__str__str__t::operator()");

    if constexpr (0)
    {
        SLF_PARMS;

        init_script_debug_menu();
        assert(script_menu != nullptr);

        mString v14 {parms->str0};
        auto *result = new debug_menu_entry {v14};

        mString v15 {parms->str1};
        auto *nt = stack.get_thread();
        auto *v4 = nt->get_instance();
        result->set_script_handler(v4, v15);
        int v16 {};
        uint32_t v11 = int(result);
        auto v10 = vm_debug_menu_entry_garbage_collection_id;
        auto *v6 = nt->get_executable();
        auto *so = v6->get_owner();
        auto *v8 = so->get_parent();
        v8->add_allocated_stuff(v10, v11, v16);
        script_menu->add_entry(result);

        SLF_RETURN;
        SLF_DONE;
    }
    else
    {
        bool (__fastcall *func)(const void *, void *edx, vm_stack *, entry_t) = CAST(func, 0x00678210);
        return func(this, nullptr, &stack, entry);
    }
}

void script_lib_debug_menu_patch()
{
    REDIRECT(0x0089C710, construct_debug_menu_lib);
}
#pragma once

#include "float.hpp"

#include <list>
#include <map>

#if STANDALONE_SYSTEM
#include <map>
#endif

struct mString;
struct script_var_container;
struct script_library_class;
struct resource_key;
struct script_executable_entry_key;
struct script_executable_entry;
struct script_executable;
struct script_executable_allocated_stuff_record;
struct script_object;
struct string_hash;
struct vm_executable;

enum script_manager_callback_reason
{};

namespace script_manager {
    //0x005A09B0
    void *get_game_var_address(const mString &a1, bool *a2, script_library_class **a3);

    char *get_game_var_address(int a1);

    char *get_shared_var_address(int a1);

    //0x0058F4C0
    int save_game_var_buffer(char *a1);

    //0x005AFCE0
    void init();

    void dump_threads_to_console();

    void dump_threads_to_file();

    //0x005A3620
    void link();

    //0x005A0AC0
    void run_callbacks(script_manager_callback_reason a1, script_executable *a2, const char *a3);

    //0x0058F480
    int load_game_var_buffer(char *a1);

    void un_load(const resource_key &a1, bool a2, const resource_key &a3);

    //0x005B0750
    script_executable_entry *load(const resource_key &a1, uint32_t a2, void *a3, const resource_key &a4);

    //0x0059EE90
    void init_game_var();

    //0x0059EE10
    script_executable_entry *find_entry(const script_executable *a1);

    bool using_chuck_old_fashioned();

    //0x0058F3A0
    bool is_loadable(const resource_key &a1);

    //0x005A52F0
    void destroy_game_var();

    //0x005B0640
    void clear();

    //0x005B0970
    void kill();

    //0x005AF9F0
    void run(Float a1, bool a2);

    //0x0059ED70
    vm_executable *find_function_by_address(const uint16_t *a1);

    //0x0059EDC0
    vm_executable *find_function_by_name(string_hash a1);

    int get_total_loaded();


#if !STANDALONE_SYSTEM
    std::map<script_executable_entry_key, script_executable_entry> *get_exec_list();
#else
    std::map<script_executable_entry_key, script_executable_entry> *get_exec_list();
#endif

    //0x0059ED20
    script_object *find_object(const string_hash &a1);
    
    //0x005A0870
    script_object *find_object(
        const resource_key &,
        const string_hash &,
        const resource_key &);

    //0x005AFE40
    int register_allocated_stuff_callback(
        void (*a1)(script_executable*, std::list<uint32_t>&, std::list<mString>&))
    {
        CDECL_CALL(0x005AFE40, (*a1));

        return 1;
    }

    script_object *find_global_object();

    float get_time_inc();

    int register_callback(
        void (*a2)(script_manager_callback_reason, script_executable *, const char *));
}

extern void script_manager_patch();
#pragma once

#include "script_library_class.h"
#include "fixed_pool.h"
#include "msimpletemplates_guts.h"
#include "opcodes.h"
#include "vm_stack.h"

#include "variable.h"

#include <vector>

struct mString;
struct script_instance;
struct vm_executable;

struct vm_thread {
    union argument_t {
        vm_num_t val;
        vm_str_t str;
        short word;
        char *sdr;
        script_library_class::function *lfr;
        vm_executable *sfr;
        unsigned binary;
    };

    enum flags_t
    {
        SUSPENDED   = 0x0001,
        SUSPENDABLE = 0x0002,
    };

    simple_list<vm_thread>::vars_t simple_list_vars;
    script_instance *inst;
    const vm_executable *ex;
    vm_thread *field_14;
    int field_18;

private:
    int flags;

public:
    vm_stack dstack;
    const uint16_t *PC;
    const uint16_t *field_1B0;
    float field_1B4;
    std::vector<unsigned short const *> PC_stack;
    std::vector<void *> field_1C8;
    script_library_class::function::entry_t entry;
    void *field_1DC;
    int field_1E0;
    int field_1E4;


    static Var<char[64][256]> string_registers;

    static Var<fixed_pool> pool;

    static inline Var<int> id_counter {0x00965F0C};
};

extern void vm_thread_patch();
#pragma once

#include "float.hpp"
#include "string_hash.h"
#include "entity.h"

#include <list>
#include <vector>


#include <cassert>



struct box_trigger;
struct camera;
struct convex_box;
struct entity;
struct po;
struct mString;
struct region;
struct item;
struct vector3d;

struct wds_entity_manager {
    int field_18;
    void *field_1C;
    int field_20;
    int field_24;
    entity *field_28;

    bool remove_item(item* a2)
    {
        return (bool)CDECL_CALL(0x005D5410, this, a2);
    }

    bool remove_entity(entity* a2)
    {
        return (bool)CDECL_CALL(0x005D5350, this, a2);
    }


    void destroy_entity(entity* e)
    {
        assert(e != nullptr);

        if constexpr (1) {
            bool v4;

            auto v3 = e->get_flavor() - 9;
            if (v3 && v3 == 2) {
                v4 = this->remove_item((item*)e);
            } else {
                v4 = this->remove_entity(e);
            }

            if (v4) {
                if ((e->field_8 & 0x80000000) == 0) {
                    e->release_mem();
                } else {
                    e->~entity();
                }
            }
        } else {
            CDECL_CALL(0x005D6F20, this, e);
        }
    }


};

#pragma once

#include "resource_key.h"
#include "vector3d.h"

struct fx_cache;
struct vector3d;
struct handheld_item;
struct entity_base;
struct entity;

struct generic_mash_header;
struct generic_mash_data_ptrs;

struct cached_special_effect {
    resource_key field_0;
    resource_key field_8;
    vector3d field_10;
    int field_1C;
    int field_20;
    entity *field_28;
    float field_2C;
    fx_cache *field_30;
    int16_t field_34;
    int16_t field_36;
    int field_38;
    bool field_3C;
    bool field_3D;


};

#pragma once

#include "actor.h"

struct vector3d;
struct string_hash;
struct entity;
struct mic;

struct camera : actor {
    mic *field_C0;
    float field_C4;
    float field_C8;

  

};

inline Var<camera*> g_camera_link {0x0095C720};

struct region;

#pragma once

#include "entity.h"
#include "entity_base_vhandle.h"
#include "float.hpp"
#include "mashable_vector.h"

struct generic_mash_header;
struct cached_special_effect;
struct generic_mash_data_ptrs;

struct fx_cache_ent {
    float field_0;
    int field_4;
};

struct fx_cache {
    int field_0;
    int field_4;
    mashable_vector<fx_cache_ent> field_8;
    fx_cache_ent *field_10;

 
};

#pragma once

#include "camera.h"

struct game_camera : camera {

    static inline constexpr auto CAMERA_SHAKES_TOTAL = 4u;

    struct _camera_shake_t {
        int field_0;
        int field_4;
        int field_8;
        int field_C;
        int empty[2];
        short field_18;

        char field_1A;

    };

    float field_D0[16];
    int empty[2];

    vector3d field_11C;
    float field_128;
    bool field_12C;
    char pad[3];

    _camera_shake_t field_130[CAMERA_SHAKES_TOTAL];

    int empty1[1];

    game_camera() = default;

};

#pragma once

#include "float.hpp"

#include <vector>

#include <cstdint>

struct marky_camera;
struct motion_control_system;
struct camera;
struct controller;

struct wds_camera_manager {
    std::vector<motion_control_system *> field_0;
    std::vector<controller *> field_10;
    camera *field_20;
    motion_control_system *field_24;
    motion_control_system *field_28;
    controller *field_2C;
    camera *field_30;
    motion_control_system *field_34;
    motion_control_system *field_38;
    controller *field_3C;
    camera *field_40;
    marky_camera *field_44;
    bool field_48;


    bool is_marky_cam_enabled() const {
        return this->field_48;
    }

};

#pragma once

#include <cstdint>

struct fixed_pool {
    int field_0;
    void *field_4;
    int m_size;
    uint32_t m_alignment;
    int m_number_of_entries_per_block;
    int field_14;
    int field_18;
    void *m_base;
    bool m_initialized;

    fixed_pool(int a2, int a3, int a4, int a5, int a6, void *a7);

    void init(int a2, int a3, int a4, int a5, int a6, void *a7);

    void sub_4368C0();

    bool is_empty();

    void remove(void *a2);
    
    int get_entry_size() const
    {
        return this->m_size;
    }

    void *allocate_new_block();
};

template<typename T>
void *allocate_new_block(fixed_pool &pool);
#pragma once

#include "trace.h"

#include <cstdint>

template<typename T>
struct simple_list {
    T *_first_element;
    T *_last_element;
    uint32_t m_size;

    simple_list() : _first_element(nullptr),
                    _last_element(nullptr),
                    m_size(0) {}

    struct vars_t {
        T *_sl_next_element;
        T *_sl_prev_element;
        simple_list<T> *_sl_list_owner;
        
        vars_t() : _sl_next_element(nullptr),
                    _sl_prev_element(nullptr),
                    _sl_list_owner(nullptr) {}
    };

    struct iterator {
        T *_Ptr {nullptr};

        bool operator==(const iterator &it) const {
            return this->_Ptr == it._Ptr;
        }

        bool operator!=(const iterator &it) const {
            return this->_Ptr != it._Ptr;
        }

        void operator++() {
            if ( this->_Ptr != nullptr ) {
                this->_Ptr = this->_Ptr->simple_list_vars._sl_next_element;
            }
        }

        T &operator*() {
            return (*this->_Ptr);
        }

    };

    uint32_t size() const {
        return this->m_size;
    }

    bool empty() const {
        return (this->m_size == 0);
    }

    iterator begin() {
        return iterator {this->_first_element};
    }
    
    iterator end() {
        return iterator {nullptr};
    }

    iterator push_back(T *tmp)
    {
        TRACE("simple_list::push_back");

        assert(tmp != nullptr);

        assert(tmp->simple_list_vars._sl_next_element == nullptr);

        assert(tmp->simple_list_vars._sl_prev_element == nullptr);

        assert(tmp->simple_list_vars._sl_list_owner == nullptr);

        tmp->simple_list_vars._sl_next_element = this->_first_element;
        tmp->simple_list_vars._sl_prev_element = nullptr;

        if ( this->_first_element != nullptr ) {
            this->_first_element->simple_list_vars._sl_prev_element = tmp;
        }

        this->_first_element = tmp;
        if ( tmp->simple_list_vars._sl_next_element == nullptr ) {
            this->_last_element = tmp;
        }

        tmp->simple_list_vars._sl_list_owner = this;

        ++this->m_size;

        iterator a2{tmp};
        return a2;
    }

    iterator emplace_back(T *tmp)
    {
        TRACE("simple_list::emplace_back");

        assert(tmp != nullptr);

        assert(tmp->simple_list_vars._sl_next_element == nullptr);
        
        assert(tmp->simple_list_vars._sl_prev_element == nullptr);

        assert(tmp->simple_list_vars._sl_list_owner == nullptr);

        if ( this->_last_element != nullptr )
        {
            assert(_last_element->simple_list_vars._sl_next_element == nullptr);

            this->_last_element->simple_list_vars._sl_next_element = tmp;
            tmp->simple_list_vars._sl_prev_element = this->_last_element;
            tmp->simple_list_vars._sl_next_element = nullptr;
            this->_last_element = tmp;
            tmp->simple_list_vars._sl_list_owner = this;
            ++this->m_size;
            return iterator {tmp};
        }
        else
        {
            return this->push_back(tmp);
        }
    }

    bool contains(T *iter) const {
        return (iter != nullptr)
                && (iter->simple_list_vars._sl_list_owner == this);
    }

    T * erase(T *iter, bool a3)
    {
        T *result = nullptr;
        if ( iter != nullptr )
        {
            assert(this->contains(iter));

            result = (a3
                        ? iter->simple_list_vars._sl_prev_element
                        : iter->simple_list_vars._sl_next_element
                        );

            auto *sl_prev_element = iter->simple_list_vars._sl_prev_element;
            if ( sl_prev_element != nullptr ) {
                sl_prev_element->simple_list_vars._sl_next_element = iter->simple_list_vars._sl_next_element;
            } else {
                assert(iter->simple_list_vars._sl_list_owner->_first_element == iter);

                iter->simple_list_vars._sl_list_owner->_first_element = iter->simple_list_vars._sl_next_element;
            }

            if ( iter->simple_list_vars._sl_next_element != nullptr ) {
                iter->simple_list_vars._sl_next_element->simple_list_vars._sl_prev_element = iter->simple_list_vars._sl_prev_element;
            } else {
                assert(iter->simple_list_vars._sl_list_owner->_last_element == iter);

                iter->simple_list_vars._sl_list_owner->_last_element = iter->simple_list_vars._sl_prev_element;
            }

            assert(iter->simple_list_vars._sl_list_owner->m_size >= 0);

            --iter->simple_list_vars._sl_list_owner->m_size;
            iter->simple_list_vars._sl_next_element = nullptr;
            iter->simple_list_vars._sl_prev_element = nullptr;
            iter->simple_list_vars._sl_list_owner = nullptr;
        }

        return result;
    }

    bool common_erase(T *a2)
    {
        if ( !this->contains(a2) ) {
            return false;
        }

        this->erase(a2, false);
        return true;
    }

    iterator erase(T *a3)
    {
        auto *v3 = this->erase(a3, false);
        return {v3};
    }

};
#pragma once

inline constexpr auto OP_DSIZE_FLAG   = 0x0080u;
inline constexpr auto OP_ARGTYPE_MASK = 0x007Fu;

                    // FUNCTION                 ARGUMENT_TYPES_ALLOWED
enum opcode_t {
    OP_ADD = 0,
    OP_AND = 1,
    OP_BF = 2,
    OP_BRA = 3,
    OP_BSL = 4,     // Call library function    LFR
    OP_BSR = 5,     // Branch to subroutine     SFR
    OP_BST = 6,
    OP_BTH = 7,
    OP_DEC = 8,
    OP_DIV = 9,
    OP_DUP = 10,
    OP_EQ  = 11,
    OP_GE  = 12,
    OP_GT  = 13,
    OP_INC = 14,
    OP_KIL = 15,
    OP_LE  = 16,
    OP_LNT = 18,
    OP_LT  = 20,
    OP_MOD = 21,
    OP_MUL = 22,
    OP_NE  = 23,
    OP_NEG = 24,
    OP_NOP = 25,
    OP_NOT = 26,
    OP_OR  = 27,
    OP_POP = 28,
    OP_PSH = 29,
    OP_RET = 30,
    OP_SHL = 31,
    OP_SHR = 32,
    OP_SPA = 33,
    OP_SUB = 34,
    OP_XOR = 35,
    OP_STR_EQ = 37,
    OP_STR_NE = 38,
    OP_ECB = 43,
    OP_ESB = 44,
    OP_ECO = 45,
    OP_SCO = 46,

    OP_MS2 = 55,
};

inline const char * opcode_t_str[] = {
    "OP_ADD",
    "OP_AND",
    "OP_BF",
    "OP_BRA",
    "OP_BSL",
    "OP_BSR",
    "OP_BST",
    "OP_BTH",
    "OP_DEC",
    "OP_DIV",
    "OP_DUP",
    "OP_EQ",
    "OP_GE",
    "OP_GT",
    "OP_INC",
    "OP_KIL",
    "OP_LE",
    "",
    "OP_LNT",
    "",
    "OP_LT",
    "OP_MOD",
    "OP_MUL",
    "OP_NE",
    "OP_NEG",
    "OP_NOP",
    "OP_NOT",
    "OP_OR",
    "OP_POP",
    "OP_PSH",
    "OP_RET",
    "OP_SHL",
    "OP_SHR",
    "OP_SPA",
    "OP_SUB",
    "OP_XOR",
    "OP_STR_EQ",
    "OP_STR_NE",
    "",
    "",
    "",
    "",
    "OP_ECB",
    "OP_ESB",
    "OP_ECO",
    "OP_SCO",
};

enum opcode_arg_t {
    OP_ARG_NULL = 0,
    OP_ARG_NUM = 1,
    OP_ARG_NUMR = 2,
    OP_ARG_STR = 3,
    OP_ARG_WORD = 4,
    OP_ARG_PCR = 5,     // PC-relative address (2 bytes)
    OP_ARG_SPR = 6,     // SP-relative address (2 bytes)
    OP_ARG_POPO = 7,    // stack contents (pop) plus offset (2 bytes)
    OP_ARG_SDR = 8,     // static data member reference (4 bytes)
    OP_ARG_SFR = 9,
    OP_ARG_LFR = 10,    // library function member reference (4 bytes)
    OP_ARG_CLV = 11,    // class value reference (4 bytes)
    OP_ARG_SIG = 15,
    OP_ARG_PSIG = 16,
};

inline const char *opcode_arg_t_str[] = {
    "OP_ARG_NULL",
    "OP_ARG_NUM",
    "OP_ARG_NUMR",
    "OP_ARG_STR",
    "OP_ARG_WORD",
    "OP_ARG_PCR",
    "OP_ARG_SPR",
    "OP_ARG_POPO",
    "OP_ARG_SDR",
    "OP_ARG_SFR",
    "OP_ARG_LFR",
    "OP_ARG_CLV",
    "OP_ARG_SIG",
    "OP_ARG_PSIG",
    "OP_ARG_UNDEFINED",
    "OP_ARG_UNDEFINED",
    "OP_ARG_UNDEFINED",
    "OP_ARG_UNDEFINED",
    "OP_ARG_UNDEFINED",
    "OP_ARG_UNDEFINED",
    "OP_ARG_UNDEFINED",
    "OP_ARG_UNDEFINED",
    "OP_ARG_UNDEFINED",
};

inline constexpr uint32_t opcode_arg_t_shift[] = {
    0, //OP_ARG_NULL
    2, //OP_ARG_NUM
    2, //OP_ARG_NUMR
    2, //OP_ARG_STR
    1, //OP_ARG_WORD
    1, //OP_ARG_PCR
    1, //OP_ARG_SPR
    1, //OP_ARG_POPO
    2, //OP_ARG_SDR
    2, //OP_ARG_SFR
    2, //OP_ARG_LFR
    2, //OP_ARG_CLV
    2, //OP_ARG_SIG
    2, //OP_ARG_PSIG
    2, //17
};
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













#pragma once

#include "color32.h"
#include "fixedstring.h"
#include "float.hpp"
#include "game_button.h"
#include "limited_timer.h"
#include "mstring.h"
#include "rumble_struct.h"
#include "vector3d.h"




enum class game_state {
    LEGAL = 1,
    WAIT_LINK = 4,
    LOAD_LEVEL = 5,
    RUNNING = 6,
    PAUSED = 7,
};

struct game;

// 0x006063C0
extern void game__setup_input_registrations(game* a1);

// 0x00605950
extern void game__setup_inputs(game* a1);

struct game_process {
    const char* field_0;
    int* field_4;
    int index;
    int num_states;
    int field_10;
    bool field_14;
};

inline Var<game_process> lores_game_process { 0x00922074 };

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

        void reset_level_load_data();

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
    entity_base* current_game_camera;
    mic* field_64;
    message_board* mb;
    std::vector<game_process> process_stack;
    localized_string_table* field_7C;
    game_button field_80;
    nglMesh* field_B4;
    nglMesh* field_B8;
    char field_BC;
    char empty3[3];
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
    float field_284;
    int field_288;
    float field_28C;
    rumble_struct field_290;
    bool field_2B4;
    bool field_2B5;
    int field_2B8;
    int field_2BC;
    limited_timer_base field_2C0;




















    game_settings *get_game_settings() {
        assert(gamefile != nullptr);

        return this->gamefile;
    }

    void enable_user_camera(bool a2) {
        this->field_170 = a2;
    }

    bool is_user_camera_enabled() const {
        return this->field_170;
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




        void load_this_level()
    {

        void(__fastcall * func)(void*, void*) = CAST(func, 0x0055C6E0);
        return func(this, nullptr);
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

    void handle_frame_locking(float* a1);

    void soft_reset_process();


        void load_new_level(const mString& a1, int a2);

        void load_new_level_internal(const mString& a2);
    };

inline Var<game *> g_game_ptr{0x009682E0};

extern void game_patch();

#pragma once

#include "float.hpp"
#include "input_mgr.h"
#include "commands.h"

#include <cstdint>

inline constexpr auto GBFLAG_PRESSED = 1;
inline constexpr auto GBFLAG_TRIGGERED = 2;
inline constexpr auto GBFLAG_RELEASED = 4;

class game_button {

    int m_trigger_type;
    device_id_t field_4;
    game_control_t field_8;
    game_button *field_C;
    game_button *field_10;
    float field_14;
    float field_18;

public:
    float field_1C;

private:
    float field_20;
    float field_24;
    float field_28;

public:
    float field_2C;

private:
    int16_t field_30;
    int16_t m_flags;

public:



    bool is_flagged(uint32_t a2) const {
        return (a2 & this->m_flags) != 0;
    }

    void clear_flags() {
        this->m_flags &= 0x20u;
    }

    void set_flag(uint16_t a2, bool a3)
    {
        if ( a3 ) {
            this->m_flags |= a2;
        } else {
            this->m_flags &= ~a2;
        }
    }



};

#pragma once

struct rumble_struct {
    float field_0;
    float field_4;
    int field_8;
    int field_C;
    int field_10;
    float field_14;
    int field_18;
    bool field_1C;
    bool field_1D;
    int field_20;

    //0x005BA400
    rumble_struct();
};
#pragma once

#if 1 
enum game_control_t {};
#else
enum game_control_t
{
  /////////////////////////////////////////////////////////////////////////////
  // These values are used (under the same names) by the script library (d2lib.sh)
  // don't touch them unless you know what you're doing!
  BUTTON_A,
  BUTTON_B,
  BUTTON_X,
  BUTTON_Y,
  DIR_UPDOWN,
  DIR_LEFTRIGHT,
  /////////////////////////////////////////////////////////////////////////////

  // Player control
  PLAYER_ATTACK,
  PLAYER_SHOOT,
  PLAYER_FORWARD_AXIS,
  PLAYER_TURN_AXIS,
  PLAYER_ALT_FORWARD_AXIS,
  PLAYER_ALT_TURN_AXIS,
  PLAYER_FORWARD,
  PLAYER_BACKWARD,
  PLAYER_TURN_LEFT,
  PLAYER_TURN_RIGHT,
  PLAYER_STRAFE_LEFT,
  PLAYER_STRAFE_RIGHT,
  PLAYER_BURST_LEFT,
  PLAYER_BURST_RIGHT,
  PLAYER_BURST_FORWARD,
  PLAYER_BURST_BACKWARD,
  PLAYER_BLOCK_LOW,
  PLAYER_BLOCK_HIGH,
  PLAYER_JUMP,
  PLAYER_CROUCH,
  PLAYER_FRONT_CROUCH,
  PLAYER_REAR_CROUCH,
  PLAYER_RESET_POSITION,
  PLAYER_FULL_HEAL,
  PLAYER_ALL_HEAL,
  PLAYER_MUKOR,
  PLAYER_NEXT_ITEM,
  PLAYER_USE_ITEM,
  PLAYER_ACTION,
  PLAYER_MOUSELOOK_MOD,
  PLAYER_STRAFE_MOD,
  PLAYER_VSIM_HIGH,
  PLAYER_VSIM_LOW,
  PLAYER_VSIM_LEFT,
  PLAYER_VSIM_RIGHT,
  PLAYER_VSIM_HIGH_RIGHT,
  PLAYER_VSIM_LOW_RIGHT,
  PLAYER_VSIM_HIGH_LEFT,
  PLAYER_VSIM_LOW_LEFT,
  PLAYER_VSIM_EXTEND,
  PLAYER_VSIM_THRUST_MODE,
  PLAYER_VSIM_FREE_MODE,
  PLAYER_VSIM_SWORD_FLIP,
  PLAYER_WALK_MODE,
  PLAYER_HEAD_CONTROL_MODE,
  PLAYER_HEAD_THETA,
  PLAYER_HEAD_PSI,
  PLAYER_HEAD_LEVEL,
  PLAYER_HEAD_EXTEND,
  PLAYER_JAW_OPEN,
  PLAYER_ARM_CYCLE,
  PLAYER_TAIL_MOD,
  PLAYER_FLIP,
  PLAYER_FLY_TOGGLE,
  PLAYER_SPIN_SWORD,

  // Physics control
  GRAVITY_TOGGLE,
  KILL_VELOCITIES,
  STOP_PHYSICS,
  SINGLE_STEP,

  // Debug info control
  SHOW_DEBUG_INFO,
  SHOW_PROFILE_INFO,
  OPEN_PROFILE_ENTRY,
  SCROLL_PROFILE_INFO,
  SHOW_MEMORY_BUDGET,

  // Debug
  TOGGLE_FOG_COLOR_ADJUSTMENTS,
  SELECT_RED_ADJUSTMENTS,
  SELECT_GREEN_ADJUSTMENTS,
  SELECT_BLUE_ADJUSTMENTS,
  SELECT_DISTANCE_ADJUSTMENTS,
  INC_FOG_COLOR,
  DEC_FOG_COLOR,

  // User camera control
  TOGGLE_CAMERA_LOCK,
  USERCAM_FORWARD,
  USERCAM_BACKWARD,
  USERCAM_STRAFE_LEFT,
  USERCAM_STRAFE_RIGHT,
  USERCAM_UP,
  USERCAM_DOWN,
  USERCAM_PITCH,
  USERCAM_YAW,
  USERCAM_FAST,
  USERCAM_SLOW,
  USERCAM_SCREEN_SHOT,
  USERCAM_GENERATE_CUBE_MAP,
  USERCAM_EQUALS_CHASECAM,
  PAD2_USERCAM_FORWARD,
  PAD2_USERCAM_BACKWARD,
  PAD2_USERCAM_STRAFE_LEFT,
  PAD2_USERCAM_STRAFE_RIGHT,
  PAD2_USERCAM_UP,
  PAD2_USERCAM_DOWN,
  PAD2_USERCAM_PITCH,
  PAD2_USERCAM_YAW,
  PAD2_USERCAM_FAST,
  PAD2_USERCAM_SLOW,
  PAD2_USERCAM_EQUALS_CHASECAM,

  // Replay camera control
  REPLAYCAM_ZOOM,
  REPLAYCAM_PITCH,
  REPLAYCAM_YAW,
  REPLAYCAM_FAST,
  REPLAYCAM_SLOW,


  // camera editing
  USERCAM_SAVE1,
  USERCAM_SAVE2,
  USERCAM_SAVE3,
  USERCAM_SAVE4,
  USERCAM_SAVE5,
  USERCAM_SAVE6,
  USERCAM_SAVE7,
  USERCAM_SAVE8,
  USERCAM_SAVE9,
  USERCAM_SAVE0,
  USERCAM_SAVE_GO,
  USERCAM_SAVE_CLEAR,

  // edit camera
  EDITCAM_FORWARD,
  EDITCAM_BACKWARD,
  EDITCAM_STRAFE_LEFT,
  EDITCAM_STRAFE_RIGHT,
  EDITCAM_UP,
  EDITCAM_DOWN,
  EDITCAM_PITCH_UP,
  EDITCAM_PITCH_DOWN,
  EDITCAM_YAW_LEFT,
  EDITCAM_YAW_RIGHT,
  EDITCAM_FAST,
  EDITCAM_SLOW,
  EDITCAM_USE_MOUSE,

  // Scene analyzer
  TOGGLE_SCENE_ANALYZER,

  // planes viewer
  PLANE_BOUNDS_MOD,
  FIRST_PLANE_ADJUST,
  LAST_PLANE_ADJUST,

  // Misc.
  //DO_MALOR,
  DO_MALOR_NEXT,
  DO_MALOR_PREV,
  RELOAD_PARTICLE_GENERATORS,
  RELOAD_CHARACTER_ATTRIBUTES,
  DUMP_ENTITY_PO,
  TOGGLE_BSP_SPRAY_PAINT,

  // cheats
  CHEAT_MALOR,
  CHEAT_FULL_HEAL,
  CHEAT_GOD_MODE,
  CHEAT_I_WIN,

  CHEAT_SHOW_ERRORS,

  DC_SCREEN_GRAB,

  // Temporary
  QUIT_GAME,

  // Interface stuff
  GAME_PAUSE,
  INTERFACE_BUTTON,

  // Ptreyarch Front End
  PFE_UPDOWN,
  PFE_LEFTRIGHT,
  PFE_A,
  PFE_B,
  PFE_C,
  PFE_X,
  PFE_Y,
  PFE_Z,
  PFE_START,

  // Last Minute Additions
  PLAYER_AUTOMAP_TRIGGER,
  CONFIG_INPUT_PRESS,
  CONFIG_INPUT_UP,
  CONFIG_INPUT_DOWN,
  CONFIG_INPUT_LEFT,
  CONFIG_INPUT_RIGHT,
  CONFIG_INPUT_ABUTTON, // ESC
  CONFIG_INPUT_BBUTTON, // BKSP

  // MAXSTEEL specific
  SHOOT_PUNCH,
  KICK_ZOOMOUT,
  ACTION_ZOOMIN,
  JUMP_CLIMB,
  NANOTECH,
  INVENTORY,
  START_PDA,
  STICK_AXIS_V,
  STICK_AXIS_H,

  ITEM_SWITCH_DEBUG_ON,
  ITEM_SWITCH_AXIS_TYPE,
  ITEM_INC_AXIS,
  ITEM_DEC_AXIS,
  ITEM_ACC_AXIS,

  MOUSE_AXIS_H,
  MOUSE_AXIS_V,

  PSX_X,
  PSX_CIRCLE,
  PSX_TRIANGLE,
  PSX_SQUARE,
  PSX_L1,
  PSX_L2,
  PSX_L3,
  PSX_R1,
  PSX_R2,
  PSX_R3,
  PSX_UD,
  PSX_LR,
  PSX_START,
  PSX_SELECT,

  // right stick
  PSX_RUD,
  PSX_RLR,

  PAD_EASY_TRICK,
  PAD_GRAB,
  PAD_SPRAY,
  PAD_LSTICK_H,
  PAD_LSTICK_V,
  PAD_OLLIE,
  PAD_SLIDE,
  PAD_GRIND,
  PAD_SNAP,
  PAD_CARVE,
  PAD_SPEED_PUMP,
  PAD_HARD_LEFT,
  PAD_HARD_RIGHT,
  PAD_LOOK_BACK,
  PAD_SWITCH_CAMERA,

  NEXT_AI_CAM,
  PREV_AI_CAM,
  TOGGLE_AI_CAM_MODE,

#ifndef BUILD_BOOTABLE
  DUMP_FRAME_INFO,
#endif

#ifdef TARGET_PC
	TOGGLE_WIREFRAME,
#endif

  NUM_GAME_CONTROLS
};
#endif

//static_assert(TOGGLE_CAMERA_LOCK == 30 );
#include "debug_menu_extra.h"

#include "debug_menu.h"
#include "entity_tracker_manager.h"
#include "game_settings.h"
#include "geometry_manager.h"
#include "input_mgr.h"
#include "resource_manager.h"
#include "spider_monkey.h"

#include "dvar.h"

#include "app.h"



#include <cassert>

void district_variants_handler(debug_menu_entry *entry)
{
    auto *the_terrain = g_world_ptr()->the_terrain;
    if ( the_terrain != nullptr )
    {
        auto idx = entry->get_id();
        auto *reg = the_terrain->get_region(idx);
        auto ival = entry->get_ival();
        auto district_id = reg->get_district_id();
        the_terrain->set_district_variant(district_id, ival, true);
    }
}

std::string district_variants_render_callback(debug_menu_entry *a2)
{
    if ( a2 != nullptr && g_world_ptr()->the_terrain != nullptr )
    {
        auto *the_terrain = g_world_ptr()->the_terrain;
        auto idx = a2->get_id();
        auto *reg = the_terrain->get_region(idx);
        auto v4 = reg->get_district_variant_count() - 1;
        auto district_variant = reg->get_district_variant();

        char str[100]{};
        snprintf(str, 100, "%d [0, %d]", district_variant, v4);
        return {str};
    }
    else
    {
        auto a1 = entry_render_callback_default(a2);
        return a1;
    }
}

void populate_district_variants_menu(debug_menu_entry *entry)
{
    printf("populate_district_variants_menu\n");
    auto *v5 = create_menu(entry->text);
    entry->set_submenu(v5);
    auto *the_terrain = g_world_ptr()->the_terrain;
    if ( the_terrain != nullptr )
    {
        auto num_regions = the_terrain->get_num_regions();
        for ( auto idx = 0; idx < num_regions; ++idx )
        {
            auto *reg = the_terrain->get_region(idx);
            auto district_variant = reg->get_district_variant();
            auto district_variant_count = reg->get_district_variant_count();
            if ( district_variant_count > 1 )
            {
                auto &name = reg->get_name();
                auto *v4 = name.to_string();
                debug_menu_entry v11 {mString {v4}};

                v11.set_id(idx);
                v11.set_render_cb(district_variants_render_callback);
                v11.set_ival(district_variant);
                v11.set_min_value(0.0);
                v11.set_max_value((float)(district_variant_count - 1));
                v11.set_game_flags_handler(district_variants_handler);
                v5->add_entry(&v11);
            }
        }
    }
}

void create_debug_district_variants_menu(debug_menu *parent)
{
    debug_menu_entry v5 {mString {"District variants"}};

    v5.set_submenu(nullptr);
    v5.set_game_flags_handler(populate_district_variants_menu);
    parent->add_entry(&v5);
}

void populate_gamefile_menu(debug_menu_entry *entry)
{
    auto &v1 = entry->text;
    auto *v2 = create_menu(v1, handle_game_entry, 200);

    auto *v494 = v2;
    entry->set_submenu(v2);
    if ( g_game_ptr() != nullptr )
    {
        auto *v493 = g_game_ptr()->get_game_settings();

        auto v3 = debug_menu_entry(mString {"HERO_POINTS"});
        auto &v492 = v3;
        v492.set_p_ival(&v493->field_340.m_hero_points);
        v492.set_max_value(1000.0);
        v494->add_entry(&v492);

        auto v4 = debug_menu_entry(mString {"UPG_MINIMAP_PTS"});
        auto &v491 = v4;
        v491.set_p_ival(&v493->field_340.field_4);
        v491.set_max_value(1000.0);
        v494->add_entry(&v491);

        auto v5 = debug_menu_entry(mString {"UPG_IMPACT_WEB_PTS"});
        v5.set_p_ival(&v493->field_340.m_upg_impact_web_pts);
        v5.set_max_value(1000.0);
        v494->add_entry(&v5);

        auto v6 = debug_menu_entry(mString {"UPG_HERO_METER_PTS"});
        v6.set_p_ival(&v493->field_340.m_upg_hero_meter_pts);
        v6.set_max_value(1000.0);
        v2->add_entry(&v6);

        auto v7 = debug_menu_entry(mString {"UPG_2ND_CHANCE_PTS"});
        v7.set_p_ival(&v493->field_340.field_10);
        v7.set_max_value(1000.0);
        v494->add_entry(&v7);

        auto v8 = debug_menu_entry(mString {"UPG_SPEED_PTS"});
        v8.set_p_ival(&v493->field_340.field_14);
        v8.set_max_value(1000.0);
        v494->add_entry(&v8);

        auto v9 = debug_menu_entry(mString {"UPG_UNDERDOG_PTS"});
        v9.set_p_ival(&v493->field_340.field_18);
        v9.set_max_value(1000.0);
        v494->add_entry(&v9);

        auto v10 = debug_menu_entry(mString {"HERO_METER_LEVEL_1"});
        v10.set_p_ival(&v493->field_340.m_hero_meter_level_1);
        v10.set_max_value(1000.0);
        v494->add_entry(&v10);

        auto v11 = debug_menu_entry(mString {"HERO_METER_LEVEL_2"});
        v11.set_p_ival(&v493->field_340.m_hero_meter_level_2);
        v11.set_max_value(1000.0);
        v494->add_entry(&v11);

        auto v12 = debug_menu_entry(mString {"HERO_METER_LEVEL_3"});
        v12.set_p_ival(&v493->field_340.m_hero_meter_level_3);
        v12.set_max_value(1000.0);
        v494->add_entry(&v12);

        auto v13 = debug_menu_entry(mString {"CUR_HERO_METER_LEVEL"});
        v13.set_p_ival(&v493->field_340.m_cur_hero_meter_lvl);
        v13.set_max_value(1000.0);
        v494->add_entry(&v13);

        auto v14 = debug_menu_entry(mString {"CUR_HERO_METER_POINTS"});
        v14.set_p_ival(&v493->field_340.m_cur_hero_meter_pts);
        v14.set_max_value(1000.0);
        v2->add_entry(&v14);

        auto v15 = debug_menu_entry(mString {"OPT_SCORE_DISPLAY"});
        v15.set_pt_bval(&v493->field_340.m_opt_score_display);
        v15.set_max_value(1000.0);
        v2->add_entry(&v15);

        auto v16 = debug_menu_entry(mString {"OPT_CONT_1_RUMBLE"});
        v16.set_pt_bval(&v493->field_340.field_31);
        v16.set_max_value(1000.0);
        v2->add_entry(&v16);

        auto v17 = debug_menu_entry(mString {"OPT_AUDIO_MODE"});
        v17.set_p_ival(&v493->field_340.field_34);
        v17.set_max_value(1000.0);
        v2->add_entry(&v17);

        auto v18 = debug_menu_entry(mString {"OPT_LEVELS_GAME"});
        v18.set_pt_fval(&v493->field_340.field_38);
        v18.set_max_value(1000.0);
        v2->add_entry(&v18);

        auto v19 = debug_menu_entry(mString {"OPT_LEVELS_MUSIC"});
        v19.set_pt_fval(&v493->field_340.field_3C);
        v19.set_max_value(1000.0);
        v2->add_entry(&v19);

        auto v20 = debug_menu_entry(mString {"OPT_CONT_CONFIG"});
        v20.set_p_ival(&v493->field_340.field_40);
        v20.set_max_value(1000.0);
        v2->add_entry(&v20);

        auto v21 = debug_menu_entry(mString {"MINI_MAP_ENABLED"});
        v21.set_pt_bval(&v493->field_340.m_mini_map_enabled);
        v21.set_max_value(1000.0);
        v2->add_entry(&v21);

        auto v22 = debug_menu_entry(mString {"ENABLE_WEB_SHOT"});
        v22.set_pt_bval(&v493->field_340.m_enable_web_shot);
        v22.set_max_value(1000.0);
        v2->add_entry(&v22);

        auto v23 = debug_menu_entry(mString {"ENABLE_WEB_DOME"});
        v23.set_pt_bval(&v493->field_340.field_46);
        v23.set_max_value(1000.0);
        v2->add_entry(&v23);

        auto v24 = debug_menu_entry(mString {"ENABLE_ADV_WEB_DOME"});
        v24.set_pt_bval(&v493->field_340.field_47);
        v24.set_max_value(1000.0);
        v2->add_entry(&v24);

        auto v25 = debug_menu_entry(mString {"ENABLE_WEB_GLOVES"});
        v25.set_pt_bval(&v493->field_340.m_web_gloves);
        v25.set_max_value(1000.0);
        v2->add_entry(&v25);

        auto v26 = debug_menu_entry(mString {"ENABLE_ADV_WEB_GLOVES"});
        v26.set_pt_bval(&v493->field_340.m_adv_web_gloves);
        v26.set_max_value(1000.0);
        v2->add_entry(&v26);

        auto v27 = debug_menu_entry(mString {"ENABLE_IMPACT_WEB"});
        v27.set_pt_bval(&v493->field_340.m_enable_impact_web);
        v27.set_max_value(1000.0);
        v2->add_entry(&v27);

        auto v28 = debug_menu_entry(mString {"ENABLE_ADV_IMPACT_WEB"});
        v28.set_pt_bval(&v493->field_340.m_adv_impact_web);
        v28.set_max_value(1000.0);
        v2->add_entry(&v28);

        auto v29 = debug_menu_entry(mString {"ENABLE_WEB_COWBOY"});
        v29.set_pt_bval(&v493->field_340.m_web_cowboy);
        v29.set_max_value(1000.0);
        v2->add_entry(&v29);

        auto v30 = debug_menu_entry(mString {"ENABLE_WEB_YANK"});
        v30.set_pt_bval(&v493->field_340.field_4D);
        v30.set_max_value(1000.0);
        v2->add_entry(&v30);

        auto v31 = debug_menu_entry(mString {"ENABLE_YANK_MANIP"});
        v31.set_pt_bval((bool *)&v493->field_340.field_4E);
        v31.set_max_value(1000.0);
        v2->add_entry(&v31);

        auto v32 = debug_menu_entry(mString {"ENABLE_ZIP_WEB_ATTACK"});
        v32.set_pt_bval((bool *)&v493->field_340.field_4F);
        v32.set_max_value(1000.0);
        v2->add_entry(&v32);

        auto v33 = debug_menu_entry(mString {"ENABLE_SWING_CANNONBALL"});
        v33.set_pt_bval(&v493->field_340.field_50);
        v33.set_max_value(1000.0);
        v2->add_entry(&v33);

        auto v34 = debug_menu_entry(mString {"ENABLE_DIRECTIONAL_ATTACK"});
        v34.set_pt_bval(&v493->field_340.field_51);
        v34.set_max_value(1000.0);
        v2->add_entry(&v34);

        auto v35 = debug_menu_entry(mString {"ENABLE_DIRECTIONAL_DODGE"});
        v35.set_pt_bval(&v493->field_340.field_52);
        v35.set_max_value(1000.0);
        v2->add_entry(&v35);

        auto v36 = debug_menu_entry(mString {"DIFFICULTY"});
        v36.set_p_ival(&v493->field_340.m_difficulty);
        v36.set_max_value(1000.0);
        v2->add_entry(&v36);

        auto v37 = debug_menu_entry(mString {"RUN_SENSITIVITY"});
        v37.set_pt_fval(&v493->field_340.m_run_sensitivity);
        v37.set_max_value(1000.0);
        v2->add_entry(&v37);

        auto v38 = debug_menu_entry(mString {"CHAR_REL_MOVEMENT"});
        v38.set_pt_bval((bool *)&v493->field_340.field_5C);
        v38.set_max_value(1000.0);
        v2->add_entry(&v38);

        auto v39 = debug_menu_entry(mString {"CRAWL_CHAR_REL_MOVEMENT"});
        v39.set_pt_bval((bool *)&v493->field_340.field_5D);
        v39.set_max_value(1000.0);
        v2->add_entry(&v39);

        auto v40 = debug_menu_entry(mString {"SHOW_STYLE_SCORE"});
        v40.set_pt_bval(&v493->field_340.m_show_style_points);
        v40.set_max_value(1000.0);
        v2->add_entry(&v40);

        auto v41 = debug_menu_entry(mString {"HERO_HEALTH"});
        v41.set_pt_fval(&v493->field_340.m_hero_health);
        v41.set_max_value(1000.0);
        v2->add_entry(&v41);

        auto v42 = debug_menu_entry(mString {"HERO_TYPE"});
        v42.set_p_ival(&v493->field_340.m_hero_type);
        v42.set_max_value(1000.0);
        v2->add_entry(&v42);

        auto v43 = debug_menu_entry(mString {"SWING_SPEED"});
        v43.set_p_ival(&v493->field_340.m_swing_speed);
        v43.set_max_value(1000.0);
        v2->add_entry(&v43);

        auto v44 = debug_menu_entry(mString {"CAM_INVERTED_X"});
        v44.set_pt_bval((bool *)&v493->field_340.field_6C);
        v44.set_max_value(1000.0);
        v2->add_entry(&v44);

        auto v45 = debug_menu_entry(mString {"CAM_INVERTED_Y"});
        v45.set_pt_bval((bool *)&v493->field_340.field_6D);
        v45.set_max_value(1000.0);
        v2->add_entry(&v45);

        auto v46 = debug_menu_entry(mString {"SPEED_DEMON"});
        v46.set_pt_bval((bool *)&v493->field_340.field_6E);
        v46.set_max_value(1000.0);
        v2->add_entry(&v46);

        auto v47 = debug_menu_entry(mString {"YOURE_AMAZING"});
        v47.set_pt_bval((bool *)&v493->field_340.field_6F);
        v47.set_max_value(1000.0);
        v2->add_entry(&v47);

        auto v48 = debug_menu_entry(mString {"ERRAND_BOY"});
        v48.set_pt_bval((bool *)&v493->field_340.field_70);
        v48.set_max_value(1000.0);
        v2->add_entry(&v48);

        auto v49 = debug_menu_entry(mString {"BETTER_TO_DO"});
        v49.set_pt_bval(&v493->field_340.field_71);
        v49.set_max_value(1000.0);
        v2->add_entry(&v49);

        auto v50 = debug_menu_entry(mString {"FANBOY"});
        v50.set_pt_bval(&v493->field_340.field_72);
        v50.set_max_value(1000.0);
        v2->add_entry(&v50);

        auto v51 = debug_menu_entry(mString {"ANGSTY"});
        v51.set_pt_bval(&v493->field_340.field_73);
        v51.set_max_value(1000.0);
        v2->add_entry(&v51);

        auto v52 = debug_menu_entry(mString {"SECRET_IDENTITY"});
        v52.set_pt_bval(&v493->field_340.field_74);
        v52.set_max_value(1000.0);
        v2->add_entry(&v52);

        auto v53 = debug_menu_entry(mString {"STYLE"});
        v53.set_pt_bval(&v493->field_340.field_75);
        v53.set_max_value(1000.0);
        v2->add_entry(&v53);

        auto v54 = debug_menu_entry(mString {"FASHION"});
        v54.set_pt_bval(&v493->field_340.field_76);
        v54.set_max_value(1000.0);
        v2->add_entry(&v54);

        auto v55 = debug_menu_entry(mString {"PICTURES"});
        v55.set_pt_bval(&v493->field_340.field_77);
        v55.set_max_value(1000.0);
        v2->add_entry(&v55);

        auto v56 = debug_menu_entry(mString {"BUNNY"});
        v56.set_pt_bval(&v493->field_340.field_78);
        v56.set_max_value(1000.0);
        v2->add_entry(&v56);

        auto v57 = debug_menu_entry(mString {"CLOBBER"});
        v57.set_pt_bval(&v493->field_340.field_79);
        v57.set_max_value(1000.0);
        v2->add_entry(&v57);

        auto v58 = debug_menu_entry(mString {"SCRAP_HEAP"});
        v58.set_pt_bval(&v493->field_340.field_7A);
        v58.set_max_value(1000.0);
        v2->add_entry(&v58);

        auto v59 = debug_menu_entry(mString {"SILVER_SPOON"});
        v59.set_pt_bval(&v493->field_340.field_7B);
        v59.set_max_value(1000.0);
        v2->add_entry(&v59);

        auto v60 = debug_menu_entry(mString {"KUNG_FU_FIGHTING"});
        v60.set_pt_bval(&v493->field_340.field_7C);
        v60.set_max_value(1000.0);
        v2->add_entry(&v60);

        auto v61 = debug_menu_entry(mString {"BIG_TIME_SUPER_HERO"});
        v61.set_pt_bval(&v493->field_340.field_7D);
        v61.set_max_value(1000.0);
        v2->add_entry(&v61);

        auto v62 = debug_menu_entry(mString {"ENJOY_THE_SUNSHINE"});
        v62.set_pt_bval(&v493->field_340.field_7E);
        v62.set_max_value(1000.0);
        v2->add_entry(&v62);

        auto v63 = debug_menu_entry(mString {"TIME_PLAYED"});
        v63.set_p_ival(&v493->field_340.field_80);
        v63.set_max_value(1000.0);
        v2->add_entry(&v63);

        auto v64 = debug_menu_entry(mString {"STORY_PERCENT_COMPLETE"});
        v64.set_p_ival(&v493->field_340.field_84);
        v64.set_max_value(1000.0);
        v2->add_entry(&v64);

        auto v65 = debug_menu_entry(mString {"STORY_MISSION_FAILURES"});
        v65.set_p_ival(&v493->field_340.field_88);
        v65.set_max_value(1000.0);
        v2->add_entry(&v65);

        auto v66 = debug_menu_entry(mString {"STORY_MISSIONS_COMPLETED"});
        v66.set_p_ival(&v493->field_340.field_8C);
        v66.set_max_value(1000.0);
        v2->add_entry(&v66);

        auto v67 = debug_menu_entry(mString {"SPIDEY_RACES_COMPLETED"});
        v67.set_p_ival(&v493->field_340.field_90);
        v67.set_max_value(1000.0);
        v2->add_entry(&v67);

        auto v68 = debug_menu_entry(mString {"MILES_RUN_SPIDEY"});
        v68.set_pt_fval(&v493->field_340.field_94);
        v68.set_max_value(1000.0);
        v2->add_entry(&v68);

        auto v69 = debug_menu_entry(mString {"MILES_CRAWLED_SPIDEY"});
        v69.set_pt_fval(&v493->field_340.field_98);
        v69.set_max_value(1000.0);
        v2->add_entry(&v69);

        auto v70 = debug_menu_entry(mString {"MILES_WEB_SWINGING"});
        v70.set_pt_fval(&v493->field_340.field_9C);
        v70.set_max_value(1000.0);
        v2->add_entry(&v70);

        auto v71 = debug_menu_entry(mString {"MILES_WEB_ZIPPING"});
        v71.set_pt_fval(&v493->field_340.m_miles_web_zipping);
        v71.set_max_value(1000.0);
        v2->add_entry(&v71);

        auto v72 = debug_menu_entry(mString {"WEB_FLUID_USED"});
        v72.set_pt_fval(&v493->field_340.m_web_fluid_used);
        v72.set_max_value(1000.0);
        v2->add_entry(&v72);

        auto v73 = debug_menu_entry(mString {"YANCY_DEFEATED"});
        v73.set_p_ival(&v493->field_340.field_A8);
        v73.set_max_value(1000.0);
        v2->add_entry(&v73);

        auto v74 = debug_menu_entry(mString {"DIE_CASTEDEFEATED"});
        v74.set_p_ival(&v493->field_340.field_AC);
        v74.set_max_value(1000.0);
        v2->add_entry(&v74);

        auto v75 = debug_menu_entry(mString {"HIGH_ROLLERSDEFEATED"});
        v75.set_p_ival(&v493->field_340.field_B0);
        v75.set_max_value(1000.0);
        v2->add_entry(&v75);

        auto v76 = debug_menu_entry(mString {"FOU_TOU_BANGDEFEATED"});
        v76.set_p_ival(&v493->field_340.field_B4);
        v76.set_max_value(1000.0);
        v2->add_entry(&v76);

        auto v77 = debug_menu_entry(mString {"VENOM_RACES_COMPLETED"});
        v77.set_p_ival(&v493->field_340.field_B8);
        v77.set_max_value(1000.0);
        v2->add_entry(&v77);

        auto v78 = debug_menu_entry(mString {"MILES_RUN_VENOM"});
        v78.set_pt_fval(&v493->field_340.field_BC);
        v78.set_max_value(1000.0);
        v2->add_entry(&v78);

        auto v79 = debug_menu_entry(mString {"MILES_CRAWLED_VENOM"});
        v79.set_pt_fval(&v493->field_340.field_C0);
        v79.set_max_value(1000.0);
        v2->add_entry(&v79);

        auto v80 = debug_menu_entry(mString {"MILES_LOCOMOTION_JUMPED"});
        v80.set_pt_fval(&v493->field_340.field_C4);
        v80.set_max_value(1000.0);
        v2->add_entry(&v80);

        auto v81 = debug_menu_entry(mString {"PEOPLE_EATEN"});
        v81.set_p_ival(&v493->field_340.field_C8);
        v81.set_max_value(1000.0);
        v2->add_entry(&v81);

        auto v82 = debug_menu_entry(mString {"CARS_THROWN"});
        v82.set_p_ival(&v493->field_340.field_CC);
        v82.set_max_value(1000.0);
        v2->add_entry(&v82);

        auto v83 = debug_menu_entry(mString {"VENOM_HOT_PERSUIT"});
        v83.set_p_ival(&v493->field_340.field_D0);
        v83.set_max_value(1000.0);
        v2->add_entry(&v83);

        auto v84 = debug_menu_entry(mString {"TOKENS_COLLECTED_COUNT"});
        v84.set_p_ival(&v493->field_340.field_D4);
        v84.set_max_value(1000.0);
        v2->add_entry(&v84);

        auto v85 = debug_menu_entry(mString {"RACE_POINTS_EARNED_COUNT"});
        v85.set_p_ival(&v493->field_340.field_D8);
        v85.set_max_value(1000.0);
        v2->add_entry(&v85);

        auto v86 = debug_menu_entry(mString {"COMBAT_TOURS_COMPLETED_COUNT"});
        v86.set_p_ival(&v493->field_340.field_DC);
        v86.set_max_value(1000.0);
        v2->add_entry(&v86);

        auto v87 = debug_menu_entry(mString {"CITY_EVENTS_COMPLETED_COUNT"});
        v87.set_p_ival(&v493->field_340.field_E0);
        v87.set_max_value(1000.0);
        v2->add_entry(&v87);

        auto v88 = debug_menu_entry(mString {"TOKENS_COLLECTED_MAX"});
        v88.set_p_ival(&v493->field_340.field_E4);
        v88.set_max_value(1000.0);
        v2->add_entry(&v88);

        auto v89 = debug_menu_entry(mString {"RACE_POINTS_EARNED_MAX"});
        v89.set_p_ival(&v493->field_340.field_E8);
        v89.set_max_value(1000.0);
        v2->add_entry(&v89);

        auto v90 = debug_menu_entry(mString {"COMBAT_TOURS_COMPLETED_MAX"});
        v90.set_p_ival(&v493->field_340.field_EC);
        v90.set_max_value(1000.0);
        v2->add_entry(&v90);

        auto v91 = debug_menu_entry(mString {"CITY_EVENTS_COMPLETED_MAX"});
        v91.set_p_ival(&v493->field_340.field_F0);
        v91.set_max_value(1000.0);
        v2->add_entry(&v91);

        auto v92 = debug_menu_entry(mString {"TOKENS_COLLECTED_TOT_COUNT"});
        v92.set_p_ival(&v493->field_340.field_F4);
        v92.set_max_value(1000.0);
        v2->add_entry(&v92);

        auto v93 = debug_menu_entry(mString {"RACE_POINTS_EARNED_TOT_COUNT"});
        v93.set_p_ival(&v493->field_340.field_F8);
        v93.set_max_value(1000.0);
        v2->add_entry(&v93);

        auto v94 = debug_menu_entry(mString {"COMBAT_TOURS_COMPLETED_TOT_COUNT"});
        auto &v401 = v94;
        v401.set_p_ival(&v493->field_340.field_FC);
        v401.set_max_value(1000.0);
        v2->add_entry(&v401);

        auto v95 = debug_menu_entry(mString {"CITY_EVENTS_COMPLETED_TOT_COUNT"});
        v95.set_p_ival(&v493->field_340.field_100);
        v95.set_max_value(1000.0);
        v494->add_entry(&v95);

        auto v96 = debug_menu_entry(mString {"TOKENS_COLLECTED_TOT_MAX"});
        v96.set_p_ival(&v493->field_340.field_104);
        v96.set_max_value(1000.0);
        v494->add_entry(&v96);

        auto v97 = debug_menu_entry(mString {"RACE_POINTS_EARNED_TOT_MAX"});
        v97.set_p_ival(&v493->field_340.field_108);
        v97.set_max_value(1000.0);
        v494->add_entry(&v97);

        auto v98 = debug_menu_entry(mString {"COMBAT_TOURS_COMPLETED_TOT_MAX"});
        v98.set_p_ival(&v493->field_340.field_10C);
        v98.set_max_value(1000.0);
        v494->add_entry(&v98);

        auto v99 = debug_menu_entry(mString {"CITY_EVENTS_COMPLETED_TOT_MAX"});
        auto &v396 = v99;
        v396.set_p_ival(&v493->field_340.field_110);
        v396.set_max_value(1000.0);
        v494->add_entry(&v396);
    }
}

void create_gamefile_menu(debug_menu *parent)
{
    assert(parent != nullptr);

    auto v5 = debug_menu_entry(mString{"Saved Game Settings"});
    parent->add_entry(&v5);

    debug_menu_entry *entry = &parent->entries[parent->used_slots - 1];
    populate_gamefile_menu(entry);
}

void warp_handler(debug_menu_entry *entry)
{
    auto idx = entry->get_id();
    auto *the_terrain = g_world_ptr()->the_terrain;
    if ( the_terrain != nullptr )
    {
        auto *reg = the_terrain->get_region(idx);
        if ( reg->is_locked() )
        {
            auto district_id = reg->get_district_id();
            auto *v1 = g_world_ptr()->get_the_terrain();
            v1->unlock_district(district_id);
        }

        auto *v2 = &reg->field_A4;
        g_world_ptr()->malor_point(v2, 0, false);
    }

    debug_menu::hide();
}

void warp_poi_handler([[maybe_unused]] debug_menu_entry *menu_entry)
{
    auto *v2 = g_femanager().IGO->field_54;
    if ( v2 != nullptr )
    {
        vector3d a1{};
        if ( v2->get_the_arrow_target_pos(&a1) )
        {
            g_world_ptr()->malor_point(&a1, 0, false);
            debug_menu::hide();
        }
    }
}

void populate_warp_menu(debug_menu_entry* entry)
{
    auto& v1 = entry->get_name();
    auto* v20 = create_menu(entry->text);

    entry->set_submenu(v20);

    auto* v19 = create_menu_entry(mString { "--WARP TO POI--" });

    v19->set_game_flags_handler(warp_poi_handler);
    v20->add_entry(v19);

    auto* v18 = g_world_ptr()->get_the_terrain();
    if (v18 != nullptr) {
        for (auto idx = 0; idx < v18->total_regions; ++idx) {
            auto* v16 = v18->get_region(idx);
            if ((v16->flags & 0x4000) != 0) {

                auto& v3 = v16->get_name();
                mString v13 { v3.to_string() };

                auto* v15 = create_menu_entry(v13);

                v15->set_id(idx);
                v15->set_game_flags_handler(warp_handler);
                v20->add_entry(v15);
            }
        }
    }
}

void create_warp_menu(debug_menu* parent)
{
    assert(parent != nullptr);

    auto* v5 = create_menu_entry(mString { "Warp" });

    v5->set_submenu(nullptr);
    v5->set_game_flags_handler(populate_warp_menu);
    parent->add_entry(v5);
}



inline std::string ngl_render_callback(debug_menu_entry *a2)
{
    auto v2 = a2->get_script_handler();
    auto *v3 = v2.c_str();
    auto v4 = nglGetDebugFlag(v3);
    a2->set_ival(v4, true);
    auto result = entry_render_callback_default(a2);
    return result;
}

inline void ngl_handler(debug_menu_entry *a1)
{
    auto v3 = a1->get_ival();
    auto v1 = a1->get_script_handler();
    auto *v2 = v1.c_str();
    nglSetDebugFlag(v2, v3);
}

void create_ngl_menu(debug_menu *parent)
{
    assert(parent != nullptr);

    auto *v22 = create_menu("NGL Debug", debug_menu::sort_mode_t::undefined);

    auto *v133 = v22;
    auto *v23 = create_menu_entry(v133);

    parent->add_entry(v23);
    if ( nglGetDebugFlagPtr("ShowPerfInfo") != nullptr ) {
        auto *v24 = create_menu_entry(mString {"ShowPerfInfo"});

        auto v1 = nglGetDebugFlag("ShowPerfInfo");
        v24->set_ival(v1);
        v24->set_min_value(0.0);
        v24->set_max_value(2.0);
        v24->set_game_flags_handler(ngl_handler);
        v24->set_render_cb(ngl_render_callback);
        v133->add_entry(v24);
    }
    if (nglGetDebugFlagPtr("ShowPerfBar") != nullptr) {
        auto* v50 = create_menu_entry(mString { "ShowPerfBar" });

        auto v1 = nglGetDebugFlag("ShowPerfBar");
        v50->set_ival(v1);
        v50->set_min_value(0.0);
        v50->set_max_value(1.0);
        v50->set_game_flags_handler(ngl_handler);
        v50->set_render_cb(ngl_render_callback);
        v133->add_entry(v50);
    }

    if ( nglGetDebugFlagPtr("ScreenShot") != nullptr )
    {
        auto *v25 = create_menu_entry(mString {"ScreenShot"});
        auto v2 = nglGetDebugFlag("ScreenShot");
        v25->set_ival(v2);
        v25->set_min_value(0.0);
        v25->set_max_value(1.0);
        v25->set_game_flags_handler(ngl_handler);
        v25->set_render_cb(ngl_render_callback);
        v133->add_entry(v25);
    }

    if ( nglGetDebugFlagPtr("DisableQuads") != nullptr )
    {
        auto *v26 = create_menu_entry(mString {"DisableQuads"});

        auto v3 = nglGetDebugFlag("DisableQuads");
        v26->set_ival(v3);
        v26->set_min_value(0.0);
        v26->set_max_value(1.0);
        v26->set_game_flags_handler(ngl_handler);
        v26->set_render_cb(ngl_render_callback);
        v133->add_entry(v26);
    }

    if ( nglGetDebugFlagPtr("DisableVSync") != nullptr )
    {
        auto *v27 = create_menu_entry(mString {"DisableVSync"});
        auto v4 = nglGetDebugFlag("DisableVSync");
        v27->set_ival(v4);
        v27->set_min_value(0.0);
        v27->set_max_value(1.0);
        v27->set_game_flags_handler(ngl_handler);
        v27->set_render_cb(ngl_render_callback);
        v133->add_entry(v27);
    }

    if ( nglGetDebugFlagPtr("DisableScratch") != nullptr )
    {
        auto *v28 = create_menu_entry(mString{"DisableScratch"});
        auto v5 = nglGetDebugFlag("DisableScratch");
        v28->set_ival(v5);
        v28->set_min_value(0.0);
        v28->set_max_value(1.0);
        v28->set_game_flags_handler(ngl_handler);
        v28->set_render_cb(ngl_render_callback);
        v133->add_entry(v28);
    }

    if ( nglGetDebugFlagPtr("DebugPrints") != nullptr )
    {
        auto *v29 = create_menu_entry("DebugPrints");

        auto v6 = nglGetDebugFlag("DebugPrints");
        v29->set_ival(v6);
        v29->set_min_value(0.0);
        v29->set_max_value(1.0);
        v29->set_game_flags_handler(ngl_handler);
        v29->set_render_cb(ngl_render_callback);
        v133->add_entry(v29);
    }

    if ( nglGetDebugFlagPtr("DumpFrameLog") != nullptr )
    {
        auto *v30 = create_menu_entry(mString{"DumpFrameLog"});
        auto v7 = nglGetDebugFlag("DumpFrameLog");
        v30->set_ival(v7);
        v30->set_min_value(0.0);
        v30->set_max_value(1.0);
        v30->set_game_flags_handler(ngl_handler);
        v30->set_render_cb(ngl_render_callback);
        v133->add_entry(v30);
    }

    if ( nglGetDebugFlagPtr("DumpSceneFile") != nullptr )
    {
        auto *v31 = create_menu_entry("DumpSceneFile");
        auto v8 = nglGetDebugFlag("DumpSceneFile");
        v31->set_ival(v8);
        v31->set_min_value(0.0);
        v31->set_max_value(1.0);
        v31->set_game_flags_handler(ngl_handler);
        v31->set_render_cb(ngl_render_callback);
        v133->add_entry(v31);
    }

    if ( nglGetDebugFlagPtr("DumpTextures") != nullptr )
    {
        auto *v32 = create_menu_entry("DumpTextures");
        auto v9 = nglGetDebugFlag("DumpTextures");
        v32->set_ival(v9);
        v32->set_min_value(0.0);
        v32->set_max_value(1.0);
        v32->set_game_flags_handler(ngl_handler);
        v32->set_render_cb(ngl_render_callback);
        v133->add_entry(v32);
    }

    if ( nglGetDebugFlagPtr("DrawLightSpheres") != nullptr )
    {
        auto *v33 = create_menu_entry("DrawLightSpheres");
        auto v10 = nglGetDebugFlag("DrawLightSpheres");
        v33->set_ival(v10);
        v33->set_min_value(0.0);
        v33->set_max_value(1.0);
        v33->set_game_flags_handler(ngl_handler);
        v33->set_render_cb(ngl_render_callback);
        v133->add_entry(v33);
    }

    if ( nglGetDebugFlagPtr("DrawMeshSpheres") != nullptr )
    {
        auto *v34 = create_menu_entry(mString{"DrawMeshSpheres"});

        auto v11 = nglGetDebugFlag("DrawMeshSpheres");
        v34->set_ival(v11);
        v34->set_min_value(0.0);
        v34->set_max_value(1.0);
        v34->set_game_flags_handler(ngl_handler);
        v34->set_render_cb(ngl_render_callback);
        v133->add_entry(v34);
    }

    if ( nglGetDebugFlagPtr("DisableDuplicateMaterialWarning") != nullptr )
    {
        auto *v35 = create_menu_entry(mString{"DisableDuplicateMaterialWarning"});
        auto v12 = nglGetDebugFlag("DisableDuplicateMaterialWarning");
        v35->set_ival(v12);
        v35->set_min_value(0.0);
        v35->set_max_value(1.0);
        v35->set_game_flags_handler(ngl_handler);
        v35->set_render_cb(ngl_render_callback);
        v133->add_entry(v35);
    }

    if ( nglGetDebugFlagPtr("DisableMissingTextureWarning") != nullptr )
    {
        auto *v36 = create_menu_entry(mString{"DisableMissingTextureWarning"});
        auto v13 = nglGetDebugFlag("DisableMissingTextureWarning");
        v36->set_ival(v13);
        v36->set_min_value(0.0);
        v36->set_max_value(1.0);
        v36->set_game_flags_handler(ngl_handler);
        v36->set_render_cb(ngl_render_callback);
        v133->add_entry(v36);
    }

    if ( nglGetDebugFlagPtr("DisableMipOpt") != nullptr )
    {
        auto *v37 = create_menu_entry(mString{"DisableMipOpt"});

        auto v14 = nglGetDebugFlag("DisableMipOpt");
        v37->set_ival(v14);
        v37->set_min_value(0.0);
        v37->set_max_value(1.0);
        v37->set_game_flags_handler(ngl_handler);
        v37->set_render_cb(ngl_render_callback);
        v133->add_entry(v37);
    }

    if ( nglGetDebugFlagPtr("DisableFSAA") != nullptr )
    {
        auto *v38 = create_menu_entry(mString{"DisableFSAA"});

        auto v15 = nglGetDebugFlag("DisableFSAA");
        v38->set_ival(v15);
        v38->set_min_value(0.0);
        v38->set_max_value(1.0);
        v38->set_game_flags_handler(ngl_handler);
        v38->set_render_cb(ngl_render_callback);
        v133->add_entry(v38);
    }

    if ( nglGetDebugFlagPtr("DrawToFrontBuffer") != nullptr )
    {
        auto *v39 = create_menu_entry(mString{"DrawToFrontBuffer"});

        auto v16 = nglGetDebugFlag("DrawToFrontBuffer");
        v39->set_ival(v16);
        v39->set_min_value(0.0);
        v39->set_max_value(1.0);
        v39->set_game_flags_handler(ngl_handler);
        v39->set_render_cb(ngl_render_callback);
        v133->add_entry(v39);
    }

    if ( nglGetDebugFlagPtr("SyncRender") != nullptr )
    {
        auto *v40 = create_menu_entry(mString{"SyncRender"});

        auto v17 = nglGetDebugFlag("SyncRender");
        v40->set_ival(v17);
        v40->set_min_value(0.0);
        v40->set_max_value(1.0);
        v40->set_game_flags_handler(ngl_handler);
        v40->set_render_cb(ngl_render_callback);
        v133->add_entry(v40);
    }

    if ( nglGetDebugFlagPtr("RenderSingleNode") != nullptr )
    {
        auto *v41 = create_menu_entry(mString{"RenderSingleNode"});
        auto v18 = nglGetDebugFlag("RenderSingleNode");
        v41->set_ival(v18);
        v41->set_min_value(0.0);
        v41->set_max_value(1.0);
        v41->set_game_flags_handler(ngl_handler);
        v41->set_render_cb(ngl_render_callback);
        v133->add_entry(v41);
    }

    if ( nglGetDebugFlagPtr("ShowPerfGraph") != nullptr )
    {
        auto *v42 = create_menu_entry(mString{"ShowPerfGraph"});
        auto v19 = nglGetDebugFlag("ShowPerfGraph");
        v42->set_ival(v19);
        v42->set_min_value(0.0);
        v42->set_max_value(1.0);
        v42->set_game_flags_handler(ngl_handler);
        v42->set_render_cb(ngl_render_callback);
        v133->add_entry(v42);
    }

    if ( nglGetDebugFlagPtr("RenderTargetShot") != nullptr )
    {
        auto *v43 = create_menu_entry(mString{"RenderTargetShot"});

        auto v20 = nglGetDebugFlag("RenderTargetShot");
        v43->set_ival(v20);
        v43->set_min_value(0.0);
        v43->set_max_value(1.0);
        v43->set_game_flags_handler(ngl_handler);
        v43->set_render_cb(ngl_render_callback);
        v133->add_entry(v43);
    }

    if ( nglGetDebugFlagPtr("WarningLevel") != nullptr )
    {
        auto *v44 = create_menu_entry(mString{"WarningLevel"});

        auto v21 = nglGetDebugFlag("WarningLevel");
        v44->set_ival(v21);
        v44->set_min_value(0.0);
        v44->set_max_value(1.0);
        v44->set_game_flags_handler(ngl_handler);
        v44->set_render_cb(ngl_render_callback);
        v133->add_entry(v44);
    }
}
inline Var<mString[51]> debug_render_items_names { 0x00961168 };

inline Var<int[51]> debug_render_items { 0x00960D30 };

enum debug_render_items_e {};

inline int debug_render_get_min(int a1)
{
    static Var<int[51]> min_values { 0x009617A0 };

    return min_values()[a1];
}

inline int debug_render_get_max(int a1)
{
    static Var<int[51]> max_values { 0x009227E8 };
    return max_values()[a1];
}

void debug_render_set_ival(debug_render_items_e a1, int a2)
{
    auto max = a2;
    if (a2 > debug_render_get_max(a1)) {
        max = debug_render_get_max(a1);
    }

    auto min = debug_render_get_min(a1);
    if (max >= min) {
        debug_render_items()[a1] = max;
    } else {
        debug_render_items()[a1] = min;
    }
}

void create_debug_render_menu(debug_menu *parent)
{
    printf("create_debug_render_menu\n");

    assert(parent != nullptr);

    auto *debug_render_menu = create_menu("Debug Render", debug_menu::sort_mode_t::ascending);

        create_ngl_menu(debug_render_menu);
    auto *v4 = create_menu_entry(debug_render_menu);
    parent->add_entry(v4);


    for ( auto i = 0u; i < 51u; ++i ) {
        auto *v5 = create_menu_entry((debug_render_items_names())[i]);
        v5->set_id(i);
        v5->set_ival(debug_render_items()[i]);
        v5->set_p_ival(&debug_render_items()[i]);

        auto v1 = debug_render_get_min((debug_render_items_e)i);
        v5->set_min_value((float)v1);

        auto v2 = debug_render_get_max((debug_render_items_e)i);
        v5->set_max_value((float)v2);

        debug_render_menu->add_entry(v5);
    }


}

void set_god_mode(int a1)
{
    CDECL_CALL(0x004BC040, a1);
}

std::string camera_render_callback(debug_menu_entry *a2)
{
    if ( a2 == nullptr ) {
        auto arg0 = entry_render_callback_default(a2);
        return arg0;
    }

    switch ( a2->get_ival() )
    {
    case 0:
        return std::string {"Chase"};
    case 1:
        return std::string {"User"};
    case 2:
        return std::string {"SceneAnalyzer"};
    default:
        auto arg0 = entry_render_callback_default(a2);
        return arg0;
    }
}

void game_flags_handler(debug_menu_entry *a1)
{
    enum {
        SLOW_MOTION = 2u,
    };

    switch ( a1->get_id() )
    {
    case 0u: //Physics Enabled
    {
        auto v1 = a1->get_bval();
        g_game_ptr()->enable_physics(v1);
        debug_menu::physics_state_on_exit = a1->get_bval();
        break;
    }
    case 1u: //Single Step
    {
        g_game_ptr()->flag.single_step = true;
        break;
    }
    case SLOW_MOTION: {
        static int old_frame_lock = 0;
        int v27;
        if (a1->get_bval()) {
            old_frame_lock = os_developer_options::instance()->get_int(mString { "FRAME_LOCK" });
            v27 = 120;
        } else {
            v27 = old_frame_lock;
        }

        os_developer_options::instance()->set_int(mString { "FRAME_LOCK" }, v27);
        debug_menu::hide();
        break;
    }
    case 3u: //Monkey Enabled
    {
        if ( a1->get_bval() )
        {
            spider_monkey::start();
            spider_monkey::on_level_load();
            auto *v2 = input_mgr::instance();
            auto *rumble_device = v2->rumble_ptr;

            assert(rumble_device != nullptr);
            rumble_device->disable_vibration();
        }
        else
        {
            spider_monkey::on_level_unload();
            spider_monkey::stop();
        }

        debug_menu::hide();
        break;
    }
    case 4u:
    {
        auto *v3 = input_mgr::instance();
        auto *rumble_device = v3->rumble_ptr;
        assert(rumble_device != nullptr);

        if ( a1->get_bval() )
            rumble_device->enable_vibration();
        else
            rumble_device->disable_vibration();

        break;
    }
    case 5u: //God Mode
    {
        auto v4 = a1->get_ival();
        set_god_mode(v4);
        debug_menu::hide();
        break;
    }
    case 6u: //Show Districts
    {
        debug_menu::hide();
        os_developer_options::instance()->set_flag(mString{"SHOW_STREAMER_INFO"}, a1->get_bval());

        if ( a1->get_bval() )
        {
            os_developer_options::instance()->set_flag(mString{"SHOW_DEBUG_TEXT"}, true);
        }

        //TODO
        //sub_66C242(&g_game_ptr->field_4C);
        break;
    }
    case 7u: //Show Hero Position
    {
        debug_menu::hide();
        os_developer_options::instance()->set_flag(mString{"SHOW_DEBUG_INFO"}, a1->get_bval());
        break;
    }
    case 8u:
    {
        debug_menu::hide();
        os_developer_options::instance()->set_flag(mString{"SHOW_FPS"}, a1->get_bval());
        break;
    }
    case 9u:
    {
        auto v24 = a1->get_bval();
        auto *v5 = input_mgr::instance();
        if ( !v5->field_30[1] )
        {
            v24 = false;
        }


        os_developer_options::instance()->set_flag(mString{"USERCAM_ON_CONTROLLER2"}, v24);
        auto *v6 = input_mgr::instance();
        auto *v23 = v6->field_30[1];

        //TODO
        /*
        if ( !(*(unsigned __int8 (__thiscall **)(int))(*(_DWORD *)v23 + 44))(v23) )
        {
            j_debug_print_va("Controller 2 is not connected!");
            ->set_bval(a1, 0, 1);
            v24 = 0;
        }
        if ( v24 )
        {
            j_debug_print_va("User cam (theoretically) enabled on controller 2");
            v7 = (*(int (__thiscall **)(int))(*(_DWORD *)v23 + 8))(v23);
            sub_676E45(g_mouselook_controller, v7);
        }
        else
        {
            sub_676E45(g_mouselook_controller, -1);
        }
        */

        auto *v8 = g_world_ptr()->get_hero_ptr(0);
        if ( v8 != nullptr && g_game_ptr()->is_user_camera_enabled() )
        {
            if ( a1->get_bval() )
            {
                auto *v14 = g_world_ptr()->get_hero_ptr(0);
                v14->unsuspend(1);
            }
            else
            {
                auto *v15 = g_world_ptr()->get_hero_ptr(0);
                v15->suspend(1);
            }
        }
        break;
    }
    case 11u: //Hires Screenshot
    {
        debug_menu::hide();
        auto a2 = os_developer_options::instance()->get_int(mString{"HIRES_SCREENSHOT_X"});
        auto a3 = os_developer_options::instance()->get_int(mString{"HIRES_SCREENSHOT_Y"});
        assert(a2 != 0 && a3 != 0 && "HIRES_SCREENSHOT_X and HIRES_SCREENSHOT_Y must be not equal 0");
        g_game_ptr()->begin_hires_screenshot(a2, a3);
        break;
    }
    case 12u: //Lores Screenshot
    {
        g_game_ptr()->push_lores();
        break;
    }
    case 13u:
    {
        static auto load_districts = TRUE;
        if ( load_districts )
        {
            auto *v11 = g_world_ptr()->the_terrain;
            v11->unload_all_districts_immediate();
            resource_manager::set_active_district(false);
        }
        else
        {
            resource_manager::set_active_district(true);
        }

        load_districts = !load_districts;
        debug_menu::hide();
        break;
    }
    case 14u:
    {
        //TODO
        //sub_66FBE0();
        debug_menu::hide();
        break;
    }
    case 15u:
    {
        //sub_697DB1();
        debug_menu::hide();
        break;
    }
    case 16u:
    {
        //TODO
        //sub_698D33();
        debug_menu::hide();
        break;
    }
    case 17u:
    {
        [[maybe_unused]]auto v12 = a1->get_bval();

        //TODO
        //sub_6A88A5(g_game_ptr, v12);
        break;
    }
    case 18u:
    {
        auto v13 = a1->get_ival();
        a1->set_ival(v13, false);
        auto v16 = a1->get_ival();
        if ( v16 )
        {
            if ( v16 == 1 )
            {
                if ( geometry_manager::is_scene_analyzer_enabled() )
                {
                    geometry_manager::enable_scene_analyzer(false);
                }

                g_game_ptr()->enable_user_camera(true);

            }
            else if ( v16 == 2 )
            {
                g_game_ptr()->enable_user_camera(false);
                geometry_manager::enable_scene_analyzer(true);
            }
        }
        else
        {
            if ( geometry_manager::is_scene_analyzer_enabled() )
            {
                geometry_manager::enable_scene_analyzer(false);
            }

            g_game_ptr()->enable_user_camera(false);
        }
        break;
    }
    default:
        return;
    }
}



void create_camera_menu_items(debug_menu *parent)
{
    assert(parent != nullptr);

    auto *new_entry = create_menu_entry(mString {"Camera"});

    new_entry->set_render_cb(camera_render_callback);
    const float v1[4] {0, 2, 1, 1};
    new_entry->set_fl_values(v1);
    new_entry->set_game_flags_handler(game_flags_handler);
    new_entry->set_id(18);
    new_entry->set_ival(0);
    parent->add_entry(new_entry);
    g_debug_camera_entry = new_entry;
}

Var<float> g_camera_min_dist { 0x00881AB4 };

Var<float> g_camera_max_dist { 0x00881AB8 };

Var<float> g_camera_supermax_dist { 0x00881ABC };


void dvar_select_handler(debug_menu_entry* a1)
{


    auto v1 = a1->get_name();
    auto v2 = a1->get_script_handler();
    auto* v3 = v2.c_str();
    auto v6 = 0;
    a1->get_fval();
    a1->set_fval(0.0, true);
    auto v7 = -1;
}

void set_dvars()
{
    debug_variable_t v0 { "camera_min_dist", g_camera_min_dist() };
    g_camera_min_dist() = v0;

    debug_variable_t v1 { "camera_max_dist", g_camera_max_dist() };
    g_camera_max_dist() = v1;

    debug_variable_t v2 { "camera_supermax_dist", g_camera_supermax_dist() };
    g_camera_supermax_dist() = v2;
}





#include "singleton.h"



#include "limited_timer.h"

#include "os_developer_options.h"

#include "game.h"
#include "mstring.h"


#include "log.h"

#include "trace.h"

#include "common.h"

#include <cassert>

struct game;

struct app : singleton {
    struct internal {
        mString field_0;
        int field_10;
        int field_14;
        int field_18;
        int field_1C;
        int field_20;
        float field_24;
        int field_28;

    };

    internal field_4;
    game *m_game;
    limited_timer_base field_34;
    int field_38;

    //0x005D6FC0




    void tick()
    {
        TRACE("tick");

        {

            float v6 = this->field_34.elapsed();
            sp_log("%f", v6);

            auto frame_lock = os_developer_options::instance()->get_int(mString { "FRAME_LOCK" });
            sp_log("frame_lock = %d", frame_lock);


            float time_inc = 0.0f;
            do {
                time_inc = this->field_34.elapsed();
                g_game_ptr()->handle_frame_locking(&time_inc);

                assert(time_inc >= 0 && time_inc < 1e9f);

                const float v4 = 0.066733405f;
                if (time_inc > v4) {
                    time_inc = v4;
                }
            } while (0 /* time_inc < 0.0f */);

            this->field_34.reset();

            void(__fastcall * func)(void*) = (decltype(func))0x005D6FC0;
            func(this);
        }
    }
    
      static inline Var<app*> instance { 0x009685D4 };
};

   inline   void app_patch()
{
    if constexpr (1) {
    }

    {
        auto address = func_address(&app::tick);
        REDIRECT(0x005AD495, address);
    }
}
#include "app.h"

#pragma once

#include <cstdint>

#include "common.h"
#include "fixedstring32.h"

namespace MemoryUnitManager
{
    struct Container {
        int empty[144];
        int field_240;
        int field_244;
        char field_248[64];
    };
}

struct game_data_essentials {
    short field_0;
    short field_2;
    short field_4;
    short field_6;
    short field_8;
    short field_A;
    int field_C;
    int field_10;
    char field_14[25];
    char field_2E[12];
};

VALIDATE_SIZE(game_data_essentials, 0x3C);

struct game_data_meat {
    int m_hero_points;
    int field_4;
    int m_upg_impact_web_pts;
    int m_upg_hero_meter_pts;
    int field_10;
    int field_14;
    int field_18;
    int m_hero_meter_level_1;
    int m_hero_meter_level_2;
    int m_hero_meter_level_3;
    int m_cur_hero_meter_lvl;
    int m_cur_hero_meter_pts;
    bool m_opt_score_display;
    bool field_31;
    int field_34;
    float field_38;
    float field_3C;
    int field_40;
    bool m_mini_map_enabled;
    bool m_enable_web_shot;
    bool field_46;
    bool field_47;
    bool m_web_gloves;
    bool m_adv_web_gloves;
    bool m_enable_impact_web;
    bool m_adv_impact_web;
    bool m_web_cowboy;
    bool field_4D;
    bool field_4E;
    bool field_4F;
    bool field_50;
    bool field_51;
    bool field_52;
    int m_difficulty;
    float m_run_sensitivity;
    bool field_5C;
    bool field_5D;
    bool m_show_style_points;
    float m_hero_health;
    int m_hero_type;
    int m_swing_speed;
    bool field_6C;
    bool field_6D;
    bool field_6E;
    bool field_6F;
    bool field_70;
    bool field_71;
    bool field_72;
    bool field_73;
    bool field_74;
    bool field_75;
    bool field_76;
    bool field_77;
    bool field_78;
    bool field_79;
    bool field_7A;
    bool field_7B;
    bool field_7C;
    bool field_7D;
    bool field_7E;
    int field_80;
    int field_84;
    int field_88;
    int field_8C;
    int field_90;
    float field_94;
    float field_98;
    float field_9C;
    float m_miles_web_zipping;
    float m_web_fluid_used;
    int field_A8;
    int field_AC;
    int field_B0;
    int field_B4;
    int field_B8;
    float field_BC;
    float field_C0;
    float field_C4;
    int field_C8;
    int field_CC;
    int field_D0;
    int field_D4;
    int field_D8;
    int field_DC;
    int field_E0;
    int field_E4;
    int field_E8;
    int field_EC;
    int field_F0;
    int field_F4;
    int field_F8;
    int field_FC;
    int field_100;
    int field_104;
    int field_108;
    int field_10C;
    int field_110;
    fixedstring32 field_114;
    fixedstring32 field_134;
};

struct game_settings {
    std::intptr_t m_vtbl;
    MemoryUnitManager::Container field_4;
    game_data_essentials field_28C[3]{};
    game_data_meat field_340;
    char *field_494;
    char *field_498;
    char *field_49C[3];
    char field_4A8[12];
    int field_4B4;
    int field_4B8;
    bool field_4BC[3];
    bool field_4BF;
    bool field_4C0;
    bool field_4C1;
    bool field_4C2;
    int m_slot_num;
    int field_4C8;
};

VALIDATE_SIZE(game_settings, 0x4CCu);
#pragma once

#include "mstring.h"
#include "utility.h"

#include "common.h"

#include <cstdint>
#include <windows.h>



struct os_file {
    enum mode_flags
    {
        FILE_READ = 1,
        FILE_WRITE = 2,
        FILE_MODIFY = 3,
        FILE_APPEND = 4
    };

    enum filepos_t {
        FP_BEGIN,
        FP_CURRENT,
        FP_END
    };

    mString m_path;
    uint32_t flags;
    bool opened;
    bool field_15;

    HANDLE io;

    uint32_t field_1C;
    uint32_t m_fileSize;
    bool field_24;
    uint32_t m_offset;
    uint32_t field_2C;
    uint32_t field_30;



        // 0x0058DEA0
    os_file()
    {
        void(__fastcall * func)(void*, void*) = CAST(func, 0x0058DEA0);
        func(this, nullptr);
    }

        // 0x0059EAA0
    explicit os_file(const mString& a2, int dwShareMode)
    {
        void(__fastcall * func)(void*, void*, const mString*, int) = CAST(func, 0x0059EAA0);
        func(this, nullptr, &a2, dwShareMode);
    }

    int write(const void* lpBuffer, int nNumberOfBytesToWrite)
    {
        int(__fastcall * func)(void*, void*, const void*, int) = CAST(func, 0x00598C30);
        return func(this, nullptr, lpBuffer, nNumberOfBytesToWrite);
    }

    bool is_open() const
    {
        return opened;
    }

    // 0x0059B6F0
    ~os_file()
    {
        void(__fastcall * func)(void*, void*) = CAST(func, 0x0059B6F0);
        func(this, nullptr);
    }


    void sub_58DEF0()
    {
        void(__fastcall * func)(void*, void*) = CAST(func, 0x0058DEF0);
        func(this, nullptr);
    }

    size_t sub_58DF50()
    {
        void(__fastcall * func)(void*, void*) = CAST(func, 0x0058DF50);
        func(this, nullptr);
    }

    // 0x0058DF90
    int get_size()
    {
        void(__fastcall * func)(void*, void*) = CAST(func, 0x0058DF90);
        func(this, nullptr);

        return 1;
    }

    // 0x0059B740
    void open(const mString& path, int shareMode)
    {
        void(__fastcall * func)(void*, void*, const mString&, int) = CAST(func, 0x0059B740);
        func(this, nullptr, path, shareMode);
    }


    // 0x00598AD0
    int read(LPVOID data, int bytes)
    {
        void(__fastcall * func)(void*, void*, LPVOID, int) = CAST(func, 0x00598AD0);
        func(this, nullptr, data, bytes);

        return 0;
    }

    // 0x0058E020
    void set_fp(uint32_t lDistanceToMove, filepos_t a3)
    {
        void(__fastcall * func)(void*, void*, uint32_t, filepos_t) = CAST(func, 0x0058E020);
        func(this, nullptr, lDistanceToMove, a3);
    }

    // 0x00598A60
    void close()
    {
        void(__fastcall * func)(void*, void*) = CAST(func, 0x00598A60);
        func(this, nullptr);
    }

    static Var<bool> system_locked;




};



    void os_file_patch();
   

#include "mstring.h"
#include "utility.h"

#include "common.h"

#include "os_file.h"

#include <cstdint>
#include <windows.h>

    VALIDATE_OFFSET(os_file, field_1C, 0x1C);

    void os_file_patch()
    {
        {
            auto address = func_address(&os_file::write);
            REDIRECT(0x0052A72B, address);
            REDIRECT(0x0052A74C, address);
            REDIRECT(0x0052A79A, address);
        }

        {
            auto address = func_address(&os_file::read);
            REDIRECT(0x00550E8E, address);
            REDIRECT(0x00598B28, address);
        }

        {
            auto address = func_address(&os_file::open);
            SET_JUMP(0x0059B740, address);
        }
    }
    

   

#include "ini_parser.h"

#include "func_wrapper.h"
#include "log.h"
#include "os_developer_options.h"

#include "variables.h"
#include "os_file.h"

#include <cassert>

int& ini_parser::scan_pos = var<int>  (0x009684F8);
char *& ini_parser::line = var<char *>(0x009684FC);
char & ini_parser::token = var<char>(0x009683F8);
char & ini_parser::stored_token = var<char>(0x00968500);
char & ini_parser::stored_type = var<char>(0x00968501);
char & ini_parser::stored_num = var<char>(0x00968502);


void ini_parser::new_line(char *lpBuffer)
{
    ini_parser::line = lpBuffer;
    ini_parser::scan_pos = 0;
}

signed int ini_parser::build_token(char *a1, char *a2)
{
    char *v2 = a1;
    char v3 = *a1;
    signed int result = 0;
    if (*a1 == '=') {
        *a2 = '=';
        a2[1] = 0;
        result = 1;
    } else {
        for (; v3 != '\n'; ++v2) {
            if (v3 == '\r') {
                break;
            }

            if (v3 == '\0') {
                break;
            }

            if (v3 == '=') {
                break;
            }

            if (v3 == ';') {
                break;
            }

            v2[a2 - a1] = v3;
            v3 = v2[1];
            ++result;
        }
        a2[result] = '\0';
    }
    return result;
}

int sub_5B8AB0(char *a1) {
    int result; // eax
    int i;      // esi

    result = strlen(a1);
    for (i = result - 1; i >= 0; a1[--i + 1] = 0) {
        result = isspace(a1[i]);
        if (!result)
            break;
    }
    return result;
}

int ini_parser::get_token(char **a1, int *a2, int *a3)
{
    int result;
    char *v4;
    int v5;
    int v6;
    char v7;
    int *v8;
    unsigned int v9;

    if (stored_token) {
        *a1 = &token;
        *a2 = stored_type;
        *a3 = stored_num;
        stored_token = 0;
        result = *a2;
    } else {
        v4 = line;
        v5 = scan_pos;
        token = 0;

        while (1)
        {
            v6 = isspace(v4[v5]);
            v4 = line;
            v5 = scan_pos;
            if (!v6 || (v7 = line[scan_pos]) == 0) {
                v7 = line[scan_pos];
                if (v7 != '\n' && v7 != '\r' && v7 != ';') {
                    break;
                }
            }

            if (v7 == ';') {
                do {
                    if (v7 == 13) {
                        break;
                    }
                    if (!v7) {
                        break;
                    }

                    scan_pos = ++v5;
                    v7 = line[v5];
                } while (v7 != 10);
            } else {
                v5 = scan_pos++ + 1;
            }
        }
        result = ini_parser::build_token(&line[scan_pos],
                                         &token);
        scan_pos = result + v5;
        if (result) {
            *a1 = &token;
            sub_5B8AB0(&token);
            if (token == '[') {
                _strlwr(&token);
                v8 = a2;
                *a2 = 1;
            } else if (token == '=') {
                v8 = a2;
                *a2 = 3;
            } else {
                v9 = strlen(&token);
                v8 = a2;
                *a2 = v9 > 0 ? 2 : 0;
            }

            stored_num = *bit_cast<char *>(a3);
            stored_type = *bit_cast<char *>(v8);
            result = *v8;
        } else {
            *a1 = nullptr;
        }
    }
    return result;
}

static constexpr int TOKEN_STRING = 2;

        void sub_59B6F0(os_file* a1)
{

    void(__fastcall * func)(void*, void*, os_file*) = CAST(func, 0x0059B6F0);
    return func(0, nullptr, a1);
}

void ini_parser::parse(const char *ini_filename, os_developer_options *a2)
{
    if constexpr (1)
    {
        assert(ini_filename != nullptr);

        int v11;

        os_file file{};

        int v9 = 0;
        strncpy(filename, ini_filename, 256u);

        static char & byte_9683F7 = var<char>(0x009683F7);
        byte_9683F7 = 0;
        scan_pos = 0;
        line = nullptr;
        token = 0;
        stored_token = 0;
        stored_type = 0;
        stored_num = 0;
        mString v13 {filename};

        file.open(v13, 1u);

        //sub_4209C0(&v13, 0);
        //nullsub_2(0);
        if (file.opened)
        {
            auto file_size = file.get_size();
            char *buf = static_cast<char *>(malloc(file_size + 1));
            assert(buf != nullptr);

            char *v12 = buf;
            file.read(buf, file_size);
            buf[file_size] = '\0';
            file.close();
            char *Str = nullptr;
            ini_parser::new_line(buf);

            int token_type;
            if (ini_parser::get_token(&Str, &token_type, &v11)) {
                signed int num_names = 0;
                while (1) {
                    if (token_type == 1) {
                        char *v5 = Str;
                        _strlwr(Str);
                        if (memcmp("[flags]", v5, 8u) == 0) {
                            v9 = 1;
                        } else {
                            v9 = (memcmp("[ints]", v5, 7u) == 0)    ? 2
                                : memcmp("[strings]", v5, 10u) != 0 ? 0
                                                                    : 3;
                        }
                    } else if (token_type == TOKEN_STRING) {
                        _strupr(Str);
                        const char **names = nullptr;
                        char* scene_names = nullptr;
                        switch (v9) {
                        case 1:
                            names = flag_names();
                            num_names = 150;
                            break;
                        case 2:
                            names = int_names();
                            num_names = 76;
                            break;
                        case 3:
                            names = string_names();
                            num_names = 14;
                            break;
                        case 4:
                            scene_names = g_scene_name();
                            num_names = 9;
                            break;
                        default:
                            names = nullptr;
                            break;
                        }

                        signed int i;
                        for (i = 0; i < num_names; ++i) {
                            if (!strcmp(names[i], Str)) {
                                break;
                            }
                        }

                        if (ini_parser::get_token(&Str, &token_type, &v11) == 3) {
                            ini_parser::get_token(&Str, &token_type, &v11);

                            assert(token_type == TOKEN_STRING);

                            if (i != num_names) { // useless checking
                                switch (v9) {
                                case 1:
                                    a2->m_flags[i] = atoi(Str) != 0;
                                    break;
                                case 2:
                                    a2->m_ints[i] = atoi(Str);
                                    break;
                                case 3:
                                    a2->m_strings[i] = Str;
                                    break;
                                }
                            }
                        } else {
                            sp_log(
                                "Mangled INI file, expected '=' but found '%s', trying to salvage "
                                "things...",
                                ini_filename);
                            stored_token = 1;
                        }
                    } else if (token_type == 3) {
                        sp_log(
                            "Mangled INI file, expected a key value, but found '=', trying to "
                            "salvage "
                            "things...");
                    }

                    if (!ini_parser::get_token(&Str, &token_type, &v11)) {
                        break;
                    }

                    num_names = 0;
                }
                buf = v12;
            }
            free(buf);
        }
        else
        {
            sp_log("Error.  Could not open ini file %s\n", filename);
          //  assert(0);
        }

     //   sub_59B6F0(&file);
    }
    else
    {
        CDECL_CALL(0x005CA120, ini_filename, a2);

    }

}
#pragma once

#include "variable.h"

#include <d3d9.h>

#include <windows.h>


struct string_hash;


inline Var<int> nglListWorkPos {0x00971F0C};



inline Var<float> flt_87EBD4 = { 0x87EBD4 };

inline Var<float> flt_86F860 = { 0x86F860 };

inline Var<int> dword_975308 = { 0x00975308 };

inline Var<bool> byte_959561 = { 0x00959561 };

inline Var<int> dword_975314 = { 0x00975314 };

inline Var<int> dword_97530C = { 0x0097530C };

inline Var<bool> byte_971F9C = { 0x00971F9C };

inline Var<bool> s_freeze_game_time = { 0x009680AC };

inline Var<bool> g_generating_vtables = { 0x0095A6F1 };

inline Var<float> g_tan_half_fov_ratio { 0x00921D7C };

inline Var<bool> byte_965C20 { 0x00965C20 };

inline Var<bool> g_distance_clipping_enabled { 0x0095C718 };

inline Var<int> g_distance_clipping { 0x0095C2F8 };

inline Var<int> g_disable_occlusion_culling { 0x0095C7AC };

inline Var<int> globalTextLanguage { 0x0096B430 };

inline Var<HWND> g_appHwnd { 0x009874C4 };

inline Var<bool> g_is_the_packer { 0x009682E4 };

inline Var<bool> g_master_clock_is_up { 0x00965EB0 };

inline Var<int> g_TOD { 0x0091E000 };

inline Var<char* [14]> dword_965C24 { 0x00965C24 };

inline Var<char[1024]> g_scene_name = (0x0095C308);

inline Var<bool> bExit { 0x0095C178 };

inline Var<bool> byte_965BF9 = { 0x00965BF9 };

inline Var<HCURSOR> hCursor = { 0x0096596C };

inline Var<HANDLE> hEvent { 0x00965C5C };

inline Var<HANDLE> hObject { 0x00965C60 };

inline Var<bool> byte_965BF5 = { 0x00965BF5 };
inline Var<bool> byte_965BF6 = { 0x00965BF6 };

inline Var<int> dword_922908 = { 0x00922908 };

inline Var<bool> byte_922994 { 0x00922994 };

inline Var<IDirect3DDevice9*> g_Direct3DDevice = (0x00971F94);

inline Var<char> byte_965C21 = { 0x00965C21 };

inline Var<char> byte_965BF8 = { 0x00965BF8 };

inline Var<int> g_cx = { 0x00965978 };
inline Var<int> g_cy = { 0x00965968 };

inline Var<char*> dword_95C730 = { 0x0095C730 };
inline Var<char*> dword_95C72C = { 0x0095C72C };

inline Var<int> dword_91E1D8 { 0x0091E1D8 };

inline Var<bool> g_indoors { 0x0095C87B };

inline Var<bool> g_player_shadows_enabled = { 0x00922C5C };
inline Var<char> g_enable_stencil_shadows = { 0x00921C98 };

inline Var<int> g_cur_shadow_target = { 0x00965F44 };

inline Var<bool> byte_975468 { 0x00975468 };

inline Var<string_hash> bip01_l_calf = { 0x0095BA1C };
inline Var<string_hash> bip01_r_calf = { 0x0095AB1C };
inline Var<string_hash> bip01_pelvis = { 0x0095AAFC };
inline Var<string_hash> bip01_head = { 0x0095B9DC };
inline Var<string_hash> bip01_spine = { 0x0095BA18 };

inline Var<char[260]> byte_9659B8 { 0x009659B8 };

inline Var<int> nWidth = { 0x0093AE84 };
inline Var<int> nHeight = { 0x0093AE88 };

inline Var<HWND> g_hWnd = { 0x00971F98 };

inline Var<float> flt_965BDC { 0x00965BDC };

inline Var<char> byte_95C718 = { 0x0095C718 };
inline Var<int> dword_95C2F8 = { 0x0095C2F8 };

inline Var<bool> ChromeEffect = { 0x0091E1D4 };

inline Var<int> g_Windowed = { 0x00946530 };

inline Var<float> flt_88E518 { 0x0088E518 };

inline Var<float> flt_88E51C { 0x0088E51C };


inline Var<bool> cam_target_locked { 0x0095C754 };

inline Var<bool> EnableShader { 0x00972AB0 };


inline constexpr uint32_t RESOURCE_VERSION_INVALID = 0xFFFFFFFF;

inline constexpr uint32_t RESOURCE_PACK_VERSION = 0xE;
inline constexpr uint32_t RESOURCE_ENTITY_MASH_VERSION = 0x24D;
inline constexpr uint32_t RESOURCE_NONENTITY_MASH_VERSION = 0x12D;
inline constexpr uint32_t RESOURCE_AUTO_MASH_VERSION = 0x249;
inline constexpr uint32_t RESOURCE_RAW_MASH_VERSION = 0x115;#pragma once

#include "variable.h"


struct os_developer_options;

namespace ini_parser {

    //0x005CA120
    extern void parse(const char *ini_filename, os_developer_options *a2);


    //0x005C3000
    extern int get_token(char **a1, int *a2, int *a3);

    //0x005B8B00
    extern signed int build_token(char *a1, char *a2);


    extern void new_line(char *lpBuffer);




    inline auto & filename = var<char[255]>(0x009682F8);

    extern int & scan_pos;
    extern char *& line;
    extern char & token;
    extern char & stored_token;
    extern char & stored_type;
    extern char & stored_num;


}
#pragma once

#include "mstring.h"
#include "variables.h"

#include <cstdint>



struct resource_versions {
    uint32_t field_0 = RESOURCE_VERSION_INVALID;
    uint32_t field_4 = RESOURCE_VERSION_INVALID;
    uint32_t field_8 = RESOURCE_VERSION_INVALID;
    uint32_t field_C = RESOURCE_VERSION_INVALID;
    uint32_t field_10 = RESOURCE_VERSION_INVALID;

    resource_versions() = default;

    constexpr resource_versions(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
        : field_0(a0), field_4(a1), field_8(a2), field_C(a3), field_10(a4) {}

    //0x0050E530
    [[nodiscard]] mString to_string() const;
};
#pragma once


#include "variable.h"
#include "variables.h"

#include <windows.h>
#include "float.hpp"

#include <cstdint>





struct limited_timer_base {
    uint32_t field_0; //ms

    limited_timer_base()
    {

        }

    

void reset()
    {

    }



double elapsed() const
    {
        return 0.001;
    }
};

struct limited_timer : limited_timer_base {
    float field_4;

    limited_timer() = default;

      limited_timer(Float a1)
        : limited_timer_base()
    {
        field_4 = a1;
    }

      ~limited_timer()
    {
        this->reset();
    }
};







#pragma once

#include "common.h"
#include "func_wrapper.h"
#include "mstring.h"
#include "variable.h"

#include <algorithm>
#include <iterator>
#include <optional>

#include <windows.h>

inline Var<int[76]> int_defaults{0x00936A70};

inline Var<const char *[76]> int_names { 0x00936940 };

inline Var<const char* [14]> string_names { 0x009368D0 };

inline Var<const char* [8]> string_defauts { 0x00936908 };


inline Var<BOOL[150]> flag_defaults{0x00936678};

inline Var<const char *[150]> flag_names { 0x00936420 };

struct os_developer_options
{
    enum strings_t {
        SOUND_LIST = 0,
        SCENE_NAME = 1,
        HERO_NAME = 2,
        GAME_TITLE = 3,
        GAME_LONG_TITLE = 4,
        SAVE_GAME_DESC = 5,
        VIDEO_MODE = 6,
        GFX_DEVICE = 7,
        FORCE_DEBUG_MISSION = 8,
        FORCE_LANGUAGE = 9,
        SKU = 10,
        CONSOLE_EXEC = 11,
        HERO_START_DISTRICT = 12,
        DEBUG_ENTITY_NAME = 13,
    };



    int m_vtbl;
    bool m_flags[150];
    mString m_strings[14];
    int m_ints[76];
    mString field_2AC;

    std::optional<mString> get_string(strings_t a2)
    {
        if (a2 < strings_t::SOUND_LIST || a2 > strings_t::DEBUG_ENTITY_NAME) {
            return {};
        } else {
            auto a3 = this->m_strings[a2];
            return a3;
        }
    }

    int get_int(const mString &a2)
    {
        return this->m_ints[this->get_int_from_name(a2)];
    }

    void set_int(const mString &a2, int a3)
    {
        this->m_ints[this->get_int_from_name(a2)] = a3;
    }

    void set_flag(const mString &a2, bool a3)
    {
        this->m_flags[this->get_flag_from_name(a2)] = a3;
    }

    int get_int_from_name(const mString &a1) {

        auto func = [&a1](auto &v2) {
            return (_strcmpi(v2, a1.c_str()) == 0);
        };

        auto it = std::find_if(std::begin(int_names()),
                                std::end(int_names()),
                                func);

        if (it == std::end(int_names())) {
            printf("Nonexistent int %s", a1.c_str());
        }

        return std::distance(std::begin(int_names()), it);
    }

    int get_flag_from_name(const mString &a1) const
    {
        auto func = [&a1](const char *v2) {
            return (_strcmpi(v2, a1.c_str()) == 0);
        };

        auto it = std::find_if(std::begin(flag_names()), std::end(flag_names()), func);

        size_t v3 = std::distance(std::begin(flag_names()), it);
        if (v3 == std::size(flag_names())) {
            mString out = "Nonexistent option flag " + a1;
            printf("%s\n", out.c_str());
        }

        return v3;
    }

    bool get_flag(int a2)
    {
        return this->m_flags[a2];
    }

    bool get_flag(const mString &a2) const
    {
        if constexpr (1)
        {
            return this->m_flags[this->get_flag_from_name(a2)];
        }
        else
        {
            bool (__fastcall *func)(const void *, void *, const mString *)
                = bit_cast<decltype(func)>(0x005C2F20);
            return func(this, nullptr, &a2);
        }
    }

    static inline Var<os_developer_options *> instance{0x0096858C};

    
};



VALIDATE_SIZE(os_developer_options, 0x2BCu);#pragma once

#include "bit.h"

template<typename T>
struct Var {
    using value_type = T;

    Var() = delete;

    Var(const Var &) = delete;
    Var &operator=(const Var &) = delete;

    Var(Var &&) = delete;
    Var &operator=(Var &&) = delete;

    Var(ptrdiff_t &&address) : pointer(bit_cast<T *>(address)) {
    }

    T *pointer;

    inline T &operator()() {
        return (*pointer);
    }

    inline T &operator*() {
        return (*pointer);
    }

    inline const T &operator()() const {
        return (*pointer);
    }
};

template <typename T>
inline auto& var(ptrdiff_t&& address)
{
    return *bit_cast<T*>(address);
}
#include "resource_amalgapak_header.h"

#include "common.h"
#include "log.h"
#include "mstring.h"
#include "variables.h"


#include <cassert>
#include <cstdlib>

VALIDATE_SIZE(resource_amalgapak_header, 0x38);

resource_amalgapak_header::resource_amalgapak_header() {
    this->clear();
}

void resource_amalgapak_header::clear() {
    this->field_0 = {};

    this->field_14 = rand();

    this->field_18 = 0;
    this->field_1C = 0;
    this->location_table_size = 0;
    this->field_24 = 0;
    this->memory_map_table_size = 0;
    this->field_2C = 0;
    this->prerequisite_table_size = 0;
    this->field_34 = 0;
}

bool resource_amalgapak_header::verify([[maybe_unused]] const mString& a2)
{
    static constexpr resource_versions v20 = { RESOURCE_PACK_VERSION,
        RESOURCE_ENTITY_MASH_VERSION,
        RESOURCE_NONENTITY_MASH_VERSION,
        RESOURCE_AUTO_MASH_VERSION,
        RESOURCE_RAW_MASH_VERSION };

    bool v29 = false, v28 = false;

    if (this->field_0.field_0 < v20.field_0 || this->field_0.field_4 < v20.field_4 || this->field_0.field_8 < v20.field_8 || this->field_0.field_C < v20.field_C || this->field_0.field_10 < v20.field_10) {
        v28 = true;
    } else if (this->field_0.field_0 > v20.field_0 || this->field_0.field_4 > v20.field_4 || this->field_0.field_8 > v20.field_8 || this->field_0.field_C > v20.field_C || this->field_0.field_10 > v20.field_10) {
        v29 = true;
    }

    if (v29) {
        auto v18 = v20.to_string();
        auto v19 = this->field_0.to_string();

        auto v2 = a2.c_str();
        sp_log("Error. The amalgapak file %s (v%s) is newer than this executable code (v%s).",
            v2,
            v19.c_str(),
            v18.c_str());
        // assert(0);

        return false;
    } else if (v28) {
        auto v18 = v20.to_string();
        auto v19 = this->field_0.to_string();

        auto v2 = a2.c_str();

        sp_log("Error. The amalgapak file %s (v%s) is older than this executable code (v%s).",
            v2,
            v19.c_str(),
            v18.c_str());
        // assert(0);

        return false;
    } else {
        return true;
    }

            void(__fastcall * func)(void*, void*, const mString&) = CAST(func, 0x0050E6B0);
    func(this, nullptr, a2);

    return true;


}




  void   resource_amalgapak_header_patch()
{
    {

        auto  address = func_address(&resource_amalgapak_header::verify);
        REDIRECT(0x0053E1D2, address);
    }
}

#pragma once

#include "resource_versions.h"
#include "variable.h"

struct mString;

struct resource_amalgapak_header {
    resource_versions field_0;
    int field_14;
    int field_18;
    int field_1C;
    int location_table_size;
    int field_24;
    int memory_map_table_size;
    int field_2C;
    int prerequisite_table_size;
    int field_34;




    resource_amalgapak_header();

    //0x0050E650
    void clear();

    //0x0050E6B0
    bool verify(const mString &a2);



};

void resource_amalgapak_header_patch();#include "movie_manager.h"

#include "func_wrapper.h"

movie_manager::movie_manager() {}

bool movie_manager::load_and_play_movie(const char *a2, const char *str, bool a3) {
    return (bool) CDECL_CALL(0x006299E0, a2, str, a3);
}
#pragma once

struct movie_manager {
    movie_manager();

    static bool load_and_play_movie(const char *a2, const char *str, bool a3);
};
#pragma once

#include "func_wrapper.h"
#include "resource_key.h"

#include "resource_amalgapak_header.h"

#include "nlPlatformEnum.h"

#include "mstring.h"
#include "resource_pack_location.h"

#include <vector>



#include "nfl_system.h"

#include "memory.h"
#include "common.h"
#include "func_wrapper.h"
#include "log.h"


#include "utility.h"
#include "variable.h"

#include <cassert>
#include <numeric>

#include "trace.h"
#include "os_file.h"
#include "nfl_system.h"
using namespace nflSystem;

struct resource_partition;
struct resource_pack_location;
struct resource_pack_slot;
struct mString;
struct nflFileID;

     struct resource_memory_map {
    char field_0;
    int field_4;
    int field_8;
    int field_C;
    struct {
        int field_0;
        int field_4;
        int field_8;
        int field_C;
    } field_10[8];

    resource_memory_map()
    {
        this->field_0 = 0;
        std::memset(this->field_10, 0, sizeof(this->field_10));
    }
};

     using modified_callback_t = void (*)(std::vector<resource_key> &a1);

     static inline Var<std::vector<modified_callback_t>> resource_pack_modified_callbacks { 0x0095CAC4 };

     inline     Var<mString> amalgapak_name { 0x0095CAD4 };

inline Var<_nlPlatformEnum> g_platform { 0 };

inline Var<std::vector<resource_partition*>*> partitions { 0x0095C7F0 };

inline Var<std::vector<resource_pack_slot*>> resource_context_stack { 0x0096015C };







inline Var<int> amalgapak_base_offset { 0x00921CB4 };

inline Var<nflFileID> amalgapak_id { 0x00921CB8 };

inline Var<int> resource_buffer_used { 0x0095C180 };

inline Var<int> memory_maps_count { 0x0095C7F4 };

inline Var<size_t> resource_buffer_size { 0x0095C1C8 };

inline Var<int> in_use_memory_map { 0x00921CB0 };

inline Var<uint8_t*> resource_buffer { 0x0095C738 };

inline Var<bool> using_amalga { 0x0095C800 };

inline Var<int> amalgapak_signature { 0x0095C804 };

inline Var<resource_memory_map*> memory_maps { 0x0095C2F0 };

inline Var<int> amalgapak_pack_location_count { 0x0095C7FC };

inline Var<resource_pack_location*> amalgapak_pack_location_table { 0x0095C7F8 };

inline Var<int> amalgapak_prerequisite_count { 0x0095C174 };

inline Var<resource_key*> amalgapak_prerequisite_table { 0x0095C300 };

namespace resource_manager {



    inline bool can_reload_amalgapak()
    {
        return (bool) CDECL_CALL(0x0053DE90);
    }

      inline mString get_amalgapak_filename(_nlPlatformEnum arg4)
    {
        const char* a2[] = { ".PAK", "_XB.PAK", "_GC.PAK", "_PC.PAK" };

#ifdef TARGET_XBOX
        mString v1 { a2[1] };
#else
        mString v1 { a2[arg4] };
#endif

        mString v2 { "packs\\amalga" };

        mString res = v2 + v1;

        return res;

        void(__fastcall * func)(void*, _nlPlatformEnum) = (decltype(func))0x005BA9A0;
        func(0, arg4);
    }

       inline void load_amalgapak()
    {
        CDECL_CALL(0x00537650);

    }

     inline bool using_amalgapak()
    {
        return using_amalga();
    }





  inline  void load_amalgapak_from_game()
    {
        TRACE("resource_manager::load_amalgapak");

        if constexpr (1) {
            os_file file;

            {
                amalgapak_name() = get_amalgapak_filename(g_platform());
                sp_log("Loading amalgapak...");

                mString a1 { amalgapak_name().c_str() };

                file.open(a1, os_file::FILE_READ);
            }

            if (!file.is_open()) {
                auto* v1 = amalgapak_name().c_str();
                sp_log("Could not open amalgapak file %s!", v1);
                assert(0);
            }

            resource_amalgapak_header pack_file_header {};
            file.read(&pack_file_header, sizeof(resource_amalgapak_header));

            {
                mString a1 { amalgapak_name().c_str() };

                pack_file_header.verify(a1);
            }

            if constexpr (0) {
                pack_file_header.field_18 = 0;


                       amalgapak_base_offset() = pack_file_header.field_18;
                using_amalga() = (pack_file_header.field_18 != 0);
                amalgapak_signature() = pack_file_header.field_14;
                amalgapak_pack_location_count() = pack_file_header.location_table_size / sizeof(resource_pack_location);

                amalgapak_pack_location_table() = static_cast<resource_pack_location*>(
                    arch_memalign(16u, pack_file_header.location_table_size));
                assert(amalgapak_pack_location_table() != nullptr);

                file.set_fp(pack_file_header.field_1C, os_file::FP_BEGIN);
                auto how_many_did_we_get = file.read(amalgapak_pack_location_table(),
                    pack_file_header.location_table_size);
                assert(how_many_did_we_get == pack_file_header.location_table_size);

                amalgapak_prerequisite_count() = static_cast<uint32_t>(
                                                     pack_file_header.prerequisite_table_size)
                    >> 3;

                amalgapak_prerequisite_table() = static_cast<resource_key*>(
                    arch_memalign(8u, pack_file_header.prerequisite_table_size));
                assert(amalgapak_prerequisite_table() != nullptr);

                file.set_fp(pack_file_header.field_2C, os_file::FP_BEGIN);
                how_many_did_we_get = file.read(amalgapak_prerequisite_table(),
                    pack_file_header.prerequisite_table_size);
                assert(how_many_did_we_get == pack_file_header.prerequisite_table_size);

                resource_buffer_size() = pack_file_header.field_34;
                assert(pack_file_header.memory_map_table_size % sizeof(resource_memory_map) == 0);

                memory_maps_count() = pack_file_header.memory_map_table_size / sizeof(resource_memory_map);

                memory_maps() = new resource_memory_map[memory_maps_count()];
                file.set_fp(pack_file_header.field_24, os_file::FP_BEGIN);
                how_many_did_we_get = file.read(memory_maps(), pack_file_header.memory_map_table_size);
                assert(how_many_did_we_get == pack_file_header.memory_map_table_size);

                file.close();

                if (using_amalgapak()) {
                    amalgapak_id() = nflOpenFile({ 1 }, amalgapak_name().c_str());

                    if (amalgapak_id() == NFL_FILE_ID_INVALID) {
                        amalgapak_id() = nflOpenFile({ 2 }, amalgapak_name().c_str());

                        if (amalgapak_id() == NFL_FILE_ID_INVALID) {
                            mString v12 { amalgapak_name().c_str() };
                            mString v13 { "data\\" };

                            mString a1 = v13 + v12;

                            amalgapak_id() = nflOpenFile({ 2 }, a1.c_str());
                        }
                    }

                    sp_log("Using amalgapak found on the HOST");
                } else {
                    sp_log("Using amalgapak found on the CD");
                }

                if constexpr (0) {
                    sp_log("amalgapak_base_offset = 0x%08X\n", amalgapak_base_offset());

                    std::for_each(amalgapak_pack_location_table(),
                        amalgapak_pack_location_table() + amalgapak_prerequisite_count(),
                        [](auto& pack_loc) {
                            auto& key = pack_loc.loc.field_0;
                            {
                                printf("%s %s 0x%08X %d\n",
                                    key.get_platform_name(g_platform()).c_str(),
                                    pack_loc.m_name,
                                    pack_loc.loc.m_offset,
                                    pack_loc.loc.m_size);
                                assert(to_hash(pack_loc.m_name) == key.m_hash.source_hash_code);
                                // pack_loc.loc.m_offset = 0u;
                            }
                        });

                    assert(0);
                }

            } else {

                        void(__fastcall * func)(void*) = (decltype(func))0x00537650;
                func(0);
            }
        }
            }


                inline void reload_amalgapak_from_game()
            {
                TRACE("resource_manager::reload_amalgapak");

                if constexpr (1) {
                    assert(!using_amalgapak());

                    assert(amalgapak_pack_location_table() != nullptr);

                    assert(amalgapak_prerequisite_table() != nullptr);

                    assert(memory_maps() != nullptr);

                    mem_freealign(amalgapak_prerequisite_table());
                    mem_freealign(amalgapak_pack_location_table());
                    delete[] (memory_maps());
                    amalgapak_prerequisite_table() = nullptr;
                    amalgapak_pack_location_table() = nullptr;
                    memory_maps() = nullptr;
                    load_amalgapak_from_game();

                    std::vector<resource_key> v3;
                    for (auto i = 0; i < amalgapak_pack_location_count(); ++i) {
                        if (amalgapak_pack_location_table()[i].field_2C != 0) {
                            v3.push_back(amalgapak_pack_location_table()[i].loc.field_0);
                        }
                    }

                    for (auto& cb : resource_pack_modified_callbacks()) {
                        (*cb)(v3);
                    }
                } else {
                    void(__fastcall * func)(void*) = (decltype(func))0x0054C2E0;
                    func(0);
                }
            }
         inline   void reload_amalgapak()

                {
                CDECL_CALL(0x0054C2E0);
            }

    inline bool get_pack_file_stats(const resource_key &a1,
                                    resource_pack_location *a2,
                                    mString *a3,
                                    int *a4)
    {
        return (bool) CDECL_CALL(0x0052A820, &a1, a2, a3, a4);
    }

    inline resource_pack_slot *push_resource_context(resource_pack_slot *pack_slot) {
        return (resource_pack_slot *) CDECL_CALL(0x00542740, pack_slot);
    }

    inline resource_pack_slot *pop_resource_context() {
        return (resource_pack_slot *) CDECL_CALL(0x00537530);
    }

    inline void set_active_district(bool a1)
    {
        void (__fastcall *func)(void *, int, bool) = (decltype(func)) 0x00573620;
        func(nullptr, 0, a1);
    }

 

    inline resource_partition *get_partition_pointer(int which_type) {
        return (resource_partition *) CDECL_CALL(0x00537AA0, which_type);
    }


}


#pragma once

#include "common.h"
#include "memory.h"
#include <cstdint>

struct singleton {
    int m_vtbl;

    singleton() = default;

    //virtual
    ~singleton() = default;

    void* operator new(size_t size);


    void operator delete(void* ptr, size_t sz);

};
#pragma once


#pragma once

#include <source_location>
#include <string_view>

#include <cstdarg>
#include <cstdio>
#include <iostream>

void __log(const char* file, int line, const char* format, ...);

#define sp_log(fmt, ...)                                                                     \
                                                                            
                                                               


inline void __log(const char* file, int line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    printf("[%s:%d] ", file, line);
    vprintf(format, args);
    printf("\n");
    va_end(args);
}#pragma once

#include "func_wrapper.h"
#include "string_hash.h"


struct resource_key
{
    string_hash field_0;
    int field_4;
};

inline resource_key create_resource_key_from_path(const char *in_string, int a3)
{
    resource_key key;
    CDECL_CALL(0x004217B0, &key, in_string, a3);

    return key;
}

#include "nfl_system.h"

#include "func_wrapper.h"

#include "trace.h"

nflFileID nflSystem::openFile(nflMediaID a1, const char* a2, nfdFileFlags Flags, uint32_t* p_fileSize)
{

    return (nflFileID)CDECL_CALL(0x0079E180, a1, a2, Flags, p_fileSize);
}

nflFileID nflSystem::nflOpenFile(nflMediaID a1, const char* a2)
{
    return nflFileID();
}

nflFileID nflOpenFile(nflMediaID a1, const char *a2) {
    TRACE("nflOpenFile");

    auto result = nflSystem::openFile(a1, a2, NFD_FILE_FLAGS_READ, nullptr);
    return result;
}
#pragma once
#include <cstdint>

struct nflFileID {
    int field_0;

    nflFileID() = default;

    constexpr nflFileID(int a1) : field_0(a1) {}

    bool operator==(const nflFileID &id) const
    {
        return field_0 == id.field_0;
    }

    bool operator!=(const nflFileID &id) const
    {
        return !(field_0 == id.field_0);
    }
};

enum nfdFileFlags
{
    NFD_FILE_FLAGS_READ = 0x1,
    NFD_FILE_FLAGS_WRITE = 0x2,
    NFD_FILE_FLAGS_CREATE = 0x4,
    NFD_FILE_FLAGS_UNDEFINED
};

inline constexpr nflFileID NFL_FILE_ID_INVALID {-1};


struct nflMediaID {
    int field_0;

    nflMediaID()
    {
        field_0 = 0;
    }

    nflMediaID(int a1)
    {
        field_0 = a1;
    }
};

namespace nflSystem {
//0x0079E180
nflFileID openFile(nflMediaID a1, const char *a2, nfdFileFlags a3, uint32_t *a4);


nflFileID nflOpenFile(nflMediaID a1, const char* a2);

};#pragma once


#include "debug_menu.h"
#include <iostream>
#include <string>
#include <map>
#include <cassert>

struct debug_variable_t {
    std::string field_0;


    std::map<std::string, std::string> g_dvars;

    void add_value(const std::string& a1)
    {
        auto it = g_dvars.find(this->field_0);
        if (it == g_dvars.end()) {
            std::pair<std::string, std::string> v3 { this->field_0, a1 };
            g_dvars.insert(v3);
        }
    }

    const std::string& get_value() const
    {
        auto a1 = g_dvars.find(this->field_0);

        auto end = g_dvars.end();
        if (a1 == end) {
            assert("Uninitialized dvar" && 0);
        }

        return a1->second;
    }

    operator float() const
    {
        const std::string& value = get_value();
        return std::stof(value);
    }

    debug_variable_t(const char* a1, float a2)
        : field_0(a1)
    {
        std::string val { "a2" };
        this->add_value(val);
    }



};

#include "nlPlatformEnum.h"

#pragma once

#include <cstdint>

typedef enum _nlPlatformEnum {
    NL_PLATFORM_PS2,
    NL_PLATFORM_XBOX,
    NL_PLATFORM_GAMECUBE,
    NL_PLATFORM_PC,
    NL_PLATFORM_Z 
} nlPlatformEnum;

#include "dvar.h"

#include <cassert>

std::map<mString, mString> g_dvars {};


void debug_variable_t::add_value(const mString &a1)
{
    auto it = g_dvars.find(this->field_0);
    if ( it == g_dvars.end() )
    {
        std::pair<mString, mString> v3 {this->field_0, a1};
        g_dvars.insert(v3);
    }
}

mString & debug_variable_t::get_value() const
{
    auto a1 = g_dvars.find(this->field_0);

    auto end = g_dvars.end();
    if ( a1 == end ) {
        assert("Uninitialized dvar" && 0);
    }

    return a1->second;
}

debug_variable_t::operator float() const
{
    auto &v1 = this->get_value();
    return v1.to_float();
}
#pragma once

#include <cstddef>

#include <cstring>

#ifndef __has_feature
#define __has_feature(x) 0 // Compatibility with non-clang compilers.
#endif

// Any compiler claiming C++11 supports, Visual C++ 2015 and Clang version supporting constexpr
#if ((__cplusplus >= 201103L) || (_MSC_VER >= 1900) || \
     (__has_feature(cxx_constexpr))) // C++ 11 implementation
#define _STDEX_NATIVE_CPP11_SUPPORT
#define _STDEX_NATIVE_CPP11_TYPES_SUPPORT
#endif

#if !defined(_STDEX_NATIVE_CPP11_TYPES_SUPPORT)
#if ((__cplusplus > 199711L) || defined(__CODEGEARC__))
#define _STDEX_NATIVE_CPP11_TYPES_SUPPORT
#endif
#endif

#if ((!defined(_MSC_VER) || _MSC_VER < 1600) && !defined(_STDEX_NATIVE_CPP11_SUPPORT))
#define _STDEX_IMPLEMENTS_nullptr_SUPPORT
#else
#define _STDEX_NATIVE_nullptr_SUPPORT
#endif

#if (_MSC_VER >= 1600)
#ifndef _STDEX_NATIVE_CPP11_TYPES_SUPPORT
#define _STDEX_NATIVE_CPP11_TYPES_SUPPORT
#endif
#endif

#if _MSC_VER // Visual C++ fallback
#define _STDEX_NATIVE_MICROSOFT_COMPILER_EXTENSIONS_SUPPORT
#define _STDEX_CDECL __cdecl

#if (__cplusplus >= 199711L)
#define _STDEX_NATIVE_CPP_98_SUPPORT
#endif
#endif

// C++ 98 check:
#if ((__cplusplus >= 199711L) &&                          \
     ((defined(__INTEL_COMPILER) || defined(__clang__) || \
       (defined(__GNUC__) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))))))
#ifndef _STDEX_NATIVE_CPP_98_SUPPORT
#define _STDEX_NATIVE_CPP_98_SUPPORT
#endif
#endif

#ifdef _STDEX_NATIVE_CPP11_SUPPORT
#include <cstdint>
#else
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int size_t;
typedef unsigned uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef int ptrdiff_t;
#endif

using ushort = uint16_t;
using uint = uint32_t;

#ifndef _STDEX_NATIVE_CPP11_SUPPORT
#define nullptr 0
#endif

#ifdef _STDEX_NATIVE_CPP11_SUPPORT
#include <type_traits>
#endif

#ifdef _STDEX_NATIVE_CPP11_SUPPORT
#include <functional>
#endif

using rational_t = float;

template<typename T>
bool equal(T a1, T a2) {
    std::equal_to<T> q{};
    return q(a1, a2);
}

template<typename T>
bool not_equal(T a1, T a2) {
    std::not_equal_to<T> q{};
    return q(a1, a2);
}

template<class To, class From>
constexpr
    typename std::enable_if_t<sizeof(To) == sizeof(From) && std::is_trivially_copyable_v<From> &&
                                  std::is_trivially_copyable_v<To>,
                              To>
    // constexpr support needs compiler magic
    bit_cast(const From &src) noexcept {
    static_assert(
        std::is_trivially_constructible_v<To>,
        "This implementation additionally requires destination type to be trivially constructible");

    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

template<typename T0, typename T1>
decltype(auto) CAST([[maybe_unused]] const T0 &var, T1 address) {
    return bit_cast<T0>(address);
}
#include "resource_versions.h"

mString resource_versions::to_string() const {
    mString result = mString{mString::fmtd{0},
                             "%d.%d.%d.%d.%d",
                             this->field_0,
                             this->field_4,
                             this->field_8,
                             this->field_C,
                             this->field_10};
    return result;
}
#include "mstring.h"

#include "func_wrapper.h"
#include "log.h"
#include "mash_info_struct.h"
#include "memory.h"
#include "common.h"
#include "mash.h"
#include "trace.h"

#include <cassert>
#include <stdio.h>
#include <windows.h>

VALIDATE_SIZE(mString, 0x10);

Var<int> mString_count {0x00957CEC};

Var<const char *[4]> packfile_ext {0x00936BF0};
Var<const char *[4]> packfile_dir {0x00936BD0};

int mString::npos = -1;

mString::mString(float a1) : mString() {
    char Dest[128];
    sprintf(Dest, "%0.3f", float{a1});

    this->update_guts(Dest, -1);
}

mString::mString([[maybe_unused]] int a2, const char *Format, ...)
    : mString()
{
    char Dest[1024];
    va_list Args;

    va_start(Args, Format);
    vsprintf(Dest, Format, Args);

    this->update_guts(Dest, -1);
}

mString::mString(int a2)
    : mString()

{
    char Dest[32];

    sprintf(Dest, "%d", a2);

    this->guts = (char *) mString::null();

    this->field_C = nullptr;
    ++mString_count();
    this->update_guts(Dest, -1);
}

mString &mString::operator=(const char *a2) {
    this->update_guts(a2, -1);
    return (*this);
}

mString &mString::operator=(const mString &a2) {
    if (this != (&a2)) {
        this->update_guts(a2.guts, a2.size());
    }

    return (*this);
}

mString::~mString() {
    finalize(0);
}

mString mString::to_lower() {
    strlwr(this->guts);
    return *this;
}

mString mString::to_upper() {
    strupr(this->guts);
    return *this;
}

char mString::at(int i) const
{
    return (*this)[i];
}

char mString::operator[](int i) const {
    assert(guts != nullptr);
    assert(i <= (int) m_size);

    assert(i >= 0);

    return this->guts[i];
}

void mString::initialize() {
    this->set_size(0);
    this->field_0 = 0;
    this->field_C = nullptr;

    this->guts = (char *) mString::null();
    ++mString_count();
}

mString::mString(const char *a2)
    : mString()
{
    if (a2 != nullptr) {
        this->update_guts(a2, -1);
    }
}

mString mString::from_float(float a2) {
    mString a1{a2};

    return a1;
}

int mString::find(const char *str, int a3) const {
    char *v1 = strstr(&this->guts[a3], str);
    if (v1 != nullptr) {
        return v1 - this->guts;
    }

    return mString::npos;
}

mString *mString::operator+=(const char *Source) {
    this->append(Source, -1);
    return this;
}

mString mString::substr(int a3, int Count) const {
    mString v7;
    v7.append(&this->guts[a3], Count);

    mString result;
    result.update_guts(v7.guts, -1);

    return result;
}

int mString::find(mString::pos_t a2, char a3) const {
    char *v3 = this->guts;
    char v4 = v3[a2];
    char *v5 = &v3[a2];
    if (!v4) {
        return -1;
    }

    while (v4 != a3) {
        v4 = (v5++)[1];
        if (!v4) {
            return -1;
        }
    }

    return v5 - v3;
}

mString mString::slice(int start, int end) {
    if (start < 0) {
        start += this->size();
    }

    if (end < 0) {
        end += this->size();
    }

    assert(start <= end);

    mString result = substr(start, end - start);
    return result;
}

int mString::rfind(const char *str) const {
    assert(guts && str);
    int v5 = std::strlen(str);
    for (auto i = this->size() - v5; i >= 0; --i) {
        int j;
        for (j = 0; j < v5 && this->guts[j + i] == str[j]; ++j) {
            ;
        }

        if (j == v5) {
            return i;
        }
    }

    return -1;
}

int mString::rfind(char a2, int a3) const {
    if (!this->size()) {
        return -1;
    }

    int v5 = a3;
    if (a3 == -1) {
        v5 = this->size() - 1;
    }

    char *v6 = this->guts;
    char *v7 = &v6[v5];
    if (*v7 == a2) {
        return v7 - v6;
    }

    while (--v7 >= v6) {
        if (*v7 == a2) {
            return v7 - v6;
        }
    }

    return -1;
}

void mString::finalize(int) {
    this->destroy_guts();
    --mString_count();
}

mString::mString()
    : mContainer(), guts(""),

      field_C(nullptr) {

    ++(mString_count());
}

mString::mString([[maybe_unused]] mString::fmtd fmt, const char *Format, ...) {
    char Dest[1024];
    va_list Args;

    va_start(Args, Format);
    this->set_size(0);
    this->field_0 = 0;
    vsprintf(Dest, Format, Args);

    this->guts = (char *) mString::null();
    ++mString_count();

    this->field_C = nullptr;
    this->update_guts(Dest, -1);
}

mString::mString(const mString &a2)
    : mString()

{
    if constexpr (1) {
        this->update_guts(a2.guts, -1);
    } else {
        THISCALL(0x00421220, this, &a2);
    }
}

mString mString::from_int(int a2) {
    mString str = mString{a2};

    return str;
}

int mString::to_int() const {
    return std::atoi(this->guts);
}

double mString::to_float() const {
    return std::atof(this->guts);
}

static const char *sub_1067BA0(const char *a1, int a2) {
    if (!a1) {
        return nullptr;
    }

    const char *i;
    for (i = a1; *i && *i != a2; ++i) {
        ;
    }

    const char *result = nullptr;

    if (*i) {
        result = i;
    }

    return result;
}

mString mString::remove_leading(const char *a1) {
    int start;
    for (start = 0; start < this->size() && sub_1067BA0(a1, this->guts[start]) != nullptr; ++start) {
        ;
    }

    auto end = this->size();
    auto v5 = this->slice(start, end);
    *this = v5;

    return *this;
}

mString mString::remove_trailing(const char *a2) {
    int end;
    for (end = this->size(); end > 0 && sub_1067BA0(a2, this->guts[end - 1]) != nullptr; --end) {
        ;
    }

    auto a1 = this->slice(0, end);
    *this = a1;

    return *this;
}

mString mString::remove_surrounding_whitespace() {
    this->remove_leading(" \n\t\r");

    this->remove_trailing(" \n\t\r");
    return *this;
}

bool operator==(const mString &a1, const mString &a2) {
    auto *v2 = a2.c_str();
    return (a1 == v2);
}

bool operator!=(const mString &a1, const mString &a2) {
    return !(a1 == a2);
}

bool operator<(const mString &a1, const mString &a2) {
    auto *v2 = a2.c_str();
    return a1.compare(v2) == 1;
}

bool operator>(const mString &a1, const mString &a2) {
    auto *v2 = a2.c_str();

    return a1.compare(v2) == -1;
}

const char *mString::c_str() const {
    return guts;
}

char *mString::data() {
    return guts;
}

void *dialog_box_formatting(mString *out_string, mString a2, int a3, int a4) {
    //sp_log("dialog_box_formatting: %s", a2.guts);

    return (void *) CDECL_CALL(0x0064DF30, out_string, a2, a3, a4);
}

void mString::append(char a3) {
    char a1[2];
    a1[0] = a3;
    a1[1] = 0;
    this->append(a1, -1);
}

void mString::append(const char *from_string, int from_string_length) {
    if (from_string_length == -1) {
        from_string_length = strlen(from_string);
    }

    if (from_string_length) {
        assert(from_string_length > 0);
        assert(((uint32_t) from_string_length) < MAX_MSTRING_LENGTH);

        size_t v6 = from_string_length + this->size();

        {
            auto v5 = this->field_C;
            if (v5 != nullptr && v6 < v5->m_size) {
                this->set_size(v6);
                strncat(this->guts, from_string, from_string_length);
                this->guts[v6] = 0;
            } else {
                slab_allocator::slab_t *v12 = nullptr;

                auto *v8 = static_cast<char *>(mem_alloc(v6 + 1));

                if (this->size() <= 0) {
                    v8[0] = 0;
                } else {
                    strncpy(v8, this->guts, this->size());
                    v8[this->size()] = 0;
                }

                strncat(v8, from_string, from_string_length);

                this->guts = v8;
                this->set_size(v6);
                this->field_C = v12;
                this->guts[v6] = 0;
            }
        }
    }
}

bool mString::operator==(const char *a2) const {
    assert(guts != nullptr);

    return strncmp(this->guts, a2, 65535u) == 0;
}

void mString::destroy_guts() {
    if constexpr (1) {
        auto *v2 = this->guts;
        if (v2 != mString::null()) {
            if ((int) v2 < (int) this || (int) v2 > (int) this + this->field_0) {
                if (this->field_C == nullptr) {
                    delete[](v2);
                } else {
                    slab_allocator::deallocate(v2, this->field_C);
                }
            }

            this->guts = (char *) mString::null();

            this->field_C = nullptr;
        }

    } else {
        THISCALL(0x0041F970, this);
    }
}

mString operator+(const char *a2, const mString &a3) {
    mString v5;

    if (a2 != nullptr) {
        v5.update_guts(a2, -1);
    }

    v5.append(a3.guts, 0xFFFFFFFF);

    mString a1{};

    a1.update_guts(v5.guts, -1);

    return a1;
}

mString operator+(const mString &arg4, const char *Source) {
    char *v3 = arg4.guts;

    mString v6;
    v6.update_guts(v3, -1);

    v6.append(Source, 0xFFFFFFFF);
    char *v4 = v6.guts;

    mString a1 {};

    a1.update_guts(v4, -1);

    return a1;
}

mString mString::get_standalone_filename(const mString &arg4, _nlPlatformEnum a3) {
    const char *vec[] = {".PAK", "_XB.PAK", "_GC.PAK", "_PC.PAK"};

    mString v9 = mString{vec[a3]};

    mString v8 = mString{"packs\\"};

    mString v2 = v8 + arg4;
    mString res = v2 + v9;

    return res;
}

mString *mString::operator+=(const mString &a2) {
    this->append(a2.guts, 0xFFFFFFFF);
    return this;
}

mString operator+(const mString &a2, const mString &a3) {
    const char *v3 = a2.guts;
    mString v7;
    v7.update_guts(v3, -1);

    const char *v4 = a3.guts;
    v7.append(v4, 0xFFFFFFFF);
    const char *v5 = v7.guts;

    mString a1{};
    a1.update_guts(v5, -1);

    return a1;
}

mString mString::truncate(int a2) {
    int v4; // edx

    auto *result = this;
    auto v3 = this->size();
    if (v3) {
        v4 = a2;
        if (a2 < 0) {
            v4 = 0;
        }

        if (v4 > v3) {
            v4 = v3;
        }

        result->guts[v4] = 0;
        result->set_size(v4);
    }

    return *result;
}

int mString::compare(const char *str) const {
    assert(str != nullptr);
    assert(guts != nullptr);

    auto *v3 = this->guts;

    uint32_t i;
    for (i = 0; i < this->m_size; ++i) {
        if (str[i] == '\0') {
            return -1;
        }

        if (v3[i] != str[i]) {
            if (str[i] > v3[i]) {
                return 1;
            }

            if (str[i] < v3[i]) {
                return -1;
            }
        }
    }

    return str[i] != '\0';
}

bool mString::is_equal(const char *a2) const {
    assert(guts != nullptr);
    return strncmp(this->guts, a2, 65535) == 0;
}

void mString::update_guts(const char *from_string, int n) {
    if constexpr (1) {
        assert(from_string != nullptr);

        if (n == -1) {
            n = strlen(from_string);
        }

        assert(n >= 0 && ((uint32_t) n) <= MAX_MSTRING_LENGTH - 1);

        if (n > static_cast<int>(m_size)) {
            this->destroy_guts();

            this->guts = (n < 176)
                ? (static_cast<char *>(slab_allocator::allocate(n + 1, &this->field_C)))
                : (new char[n + 1]);
        }

        if (n > 0) {
            std::memcpy(this->guts, from_string, n + 1);
            this->m_size = n;
        } else {
            this->destroy_guts();

            this->m_size = n;

            this->guts = (char *) mString::null();
        }
    } else {
        THISCALL(0x0041F9D0, this, from_string, n);
    }
}

void mString::unmash(mash_info_struct *a1, void *a2)
{
    this->custom_unmash(a1, a2);
}

void mString::custom_unmash(mash_info_struct *a1, void *a2)
{
    TRACE("mString::custom_unmash");

#ifdef TARGET_XBOX
    assert(guts == (char *)mash::CUSTOM_MASH_SENTRY);
#endif

    if constexpr (1)
    {
        auto size = this->size();
        if (size <= 0) {
            this->guts = (char *) mString::null();
        } else {
            a1->align_buffer(
#ifdef TARGET_XBOX
                mash::NORMAL_BUFFER,
#endif 
                    1);

            this->guts = (char *) a1->read_from_buffer(
#ifdef TARGET_XBOX
                mash::NORMAL_BUFFER,
#endif 
                this->m_size + 1, 1);
;
            a1->align_buffer(
#ifdef TARGET_XBOX
                mash::NORMAL_BUFFER,
#endif 
                    4);
        }

#ifndef TARGET_XBOX
        this->field_0 = (int) &a1->mash_image_ptr[a1->buffer_size_used[0] - (uint32_t)this];
#endif
    }
    else
    {
        THISCALL(0x004209F0, this, a1, a2);
    }

    sp_log("%d %s", this->m_size, this->c_str());
}
#pragma once

struct debug_menu;
struct debug_menu_entry;

extern void create_gamefile_menu(debug_menu *parent);

extern void create_warp_menu(debug_menu *parent);

extern void create_debug_render_menu(debug_menu *parent);

extern void create_debug_district_variants_menu(debug_menu *parent);

extern void create_camera_menu_items(debug_menu *parent);

extern void game_flags_handler(debug_menu_entry *a1);
#pragma once

#include "wds.h"
#include "mission_manager.h"
#include "resource_key.h"
#include "func_wrapper.h"
#include "debug_menu_extra.h"
#include "region.h"
#include "string_hash.h"

#include <cassert>
#include <cstdio>

struct region;

struct terrain
{
    region **regions;
    int field_4;
    int field_8;
    int total_regions;
    int strips;
    int total_strips;
    float field_18[3];

    region *get_region(int idx)
    {
        assert(idx >= 0);
        assert(idx < total_regions);

        return this->regions[idx];
    }

    int get_num_regions() const
    {
        return this->total_regions;
    }

    void update_region_pack_info()
    {
        void (__fastcall *func)(void *) = bit_cast<decltype(func)>(0x0054F380);
        func(this);
    }

    region *find_region(string_hash a2)
    {
        region *(__fastcall *func)(void *, int, string_hash) = bit_cast<decltype(func)>(0x00534920);
        return func(this, 0, a2);
    }

    region *find_region(const vector3d &a2, const region *a3)
    {
        region *(__fastcall *func)(void *, int, const vector3d *, const region *) = bit_cast<decltype(func)>(0x0052DFF0);
        return func(this, 0, &a2, a3);
    }

    void unload_district_immediate(int a2)
    {
        auto func = bit_cast<fastcall_call>(0x005571B0);
        func(this, 0, a2);
    }

    void unload_all_districts_immediate()
    {
        void (__fastcall *func)(void *) = (decltype(func)) 0x00557350;
        func(this);
    }

    void set_district_variant(int a2, int variant, bool a4)
    {
        void (__fastcall *func)(void *, void *, int, int, bool) =
            bit_cast<decltype(func)>(0x00557480);

        func(this, nullptr, a2, variant, a4);
    }

    void unlock_district(int a2) {
        void (__fastcall *func)(void *, void *, int) =
            bit_cast<decltype(func)>(0x005148A0);

        func(this, nullptr, a2);
    }


};


#include "wds_render_manager.h"


#include "line_info.h"
#include "city_lod.h"

#include "vector2d.h"
#include "debug_render.h"
#include "func_wrapper.h"
#include "game.h"
#include "geometry_manager.h"
#include "ngl_mesh.h"

#include "region.h"

#include "trace.h"
#include "wds.h"
#include "terrain.h"
#include "vector2di.h"
#include "utility.h"
#include "physical_interface.h"
#include "os_developer_options.h"

#include "common.h"

#include <cassert>
#include <cmath>

VALIDATE_SIZE(wds_render_manager, 156u);

struct traversed_entity {
    entity_base_vhandle m_handle;
    int field_4;
};
static Var<fixed_vector<traversed_entity, 750> *> traversed_entities_last_frame {0x0095C7B4};

wds_render_manager::wds_render_manager() {
    this->field_30.sub_56FCB0();

    
    this->field_94 = nullptr;
    this->field_98 = 0;

    this->field_10[0] = 0.30000001f;
    this->field_10[1] = -1.0f;
    this->field_10[2] = 0.2f;
    this->field_10[3] = 0;

    this->field_20[0] = 0.80000001f;
    this->field_20[1] = 0.80000001f;
    this->field_20[2] = 0.85000002f;
    this->field_20[3] = 1.0f;

    this->field_60 = {0, 0, 0, 1};

    this->field_90 = 1.0f;
    this->field_5C = nullptr;
    this->field_8C = 0;
    this->field_84 = 175.0f;
    this->field_88 = 250.0f;
}


void show_terrain_info()
{
    if ( g_world_ptr() != nullptr )
    {
        auto *v0 = g_world_ptr()->get_hero_ptr(0);
        if ( v0 != nullptr )
        {
            auto *v8 = g_world_ptr()->get_hero_ptr(0);
            if ( v8->has_physical_ifc() )
            {
                auto *v3 = g_world_ptr()->get_hero_ptr(0);
                auto *v4 = v3->physical_ifc();

                string_hash v17;
                v4->get_parent_terrain_type(&v17);

                vector2d v5{512.0, 32.0};
                vector2di v14 {v5};

                auto *v6 = v17.to_string();
                mString v16 {v6};
                color32 v7{255, 255, 255, 255};
                render_text(v16, v14, v7, 1.0, 1.0);
            }
        }
    }
}



void wds_render_manager::debug_render()
{
    TRACE("wds_render_manager::debug_render");

    if constexpr (1) {

        if (os_developer_options::instance()->get_flag(mString{"SHOW_TERRAIN_INFO"}))
        {
            show_terrain_info();
        }

        if ( debug_render_get_ival((debug_render_items_e)20) || os_developer_options::instance()->get_flag(mString {"SHOW_GLASS_HOUSE"}))
        {
            //glass_house_manager::show_glass_houses();
        }

        //if ( debug_render_get_ival((debug_render_items_e)21) || SHOW_OBBS || SHOW_DISTRICTS )
        {
            auto *ter= g_world_ptr()->get_the_terrain();
            ter->show_obbs();
        }

        render_debug_spheres();

    }
}





static constexpr auto g_projected_fov_multiplier = 0.80000001f;


#include "debug_menu.h"

void wds_render_manager::render(camera &a2, int a3)
{
    TRACE("wds_render_manager::render");

    assert(this->field_94 != nullptr);

    CDECL_CALL(0x0054B250, this, &a2, a3);

    //_populate_missions();

    if ( debug_render_get_bval(OCCLUSION) )
    {
    }

    this->debug_render();
}

void render_data::sub_56FCB0() {
    CDECL_CALL(0x0056FCB0, this);
}

void wds_render_manager::frame_advance(Float a2) {
    TRACE("wds_render_manager::frame_advance");

    CDECL_CALL(0x0054ADE0, this, a2);
}

void wds_render_manager::render_stencil_shadows(const camera &a2)
{
    TRACE("wds_render_manager::render_stencil_shadows");
    
    CDECL_CALL(0x0053D5E0, this, &a2);
}

void wds_render_manager::build_render_data_regions(render_data *arg0, camera *arg4)
{
    TRACE("wds_render_manager::build_render_data_regions");

    CDECL_CALL(0x00547000, this, arg0, arg4);
}

void wds_render_manager::build_render_data_ents(render_data *a2, camera *a3, int a4)
{
    TRACE("wds_render_manager::build_render_data_ents");

    CDECL_CALL(0x00547250, this, a2, a3, a4);
}

void wds_render_manager::sub_53D560(const camera *a2)
{
    TRACE("wds_render_manager::sub_53D560");

    CDECL_CALL(0x0053D560, this, a2);
}

#pragma once

#include "float.hpp"

#include <vector>

#include "entity.h"
#include "entity_base_vhandle.h"


#include "color.h"
#include "ngl_mesh.h"
#include "trace.h"


struct RenderOptimizations;
struct city_lod;
struct nglMesh;
struct camera;

struct render_data {
    struct region_info {
        region *field_0;
    };

    struct entity_info {};

    std::vector<render_data::region_info> field_0;
    std::vector<render_data::entity_info> field_10;
    vector3d field_20;

    void sub_56FCB0();
};

struct wds_render_manager {
    RenderOptimizations *field_0;
    int empty0[3];
    float field_10[4];
    float field_20[4];
    render_data field_30;
    nglMesh *field_5C;
    color field_60;
    int empty2[5];
    float field_84;
    float field_88;
    float field_8C;
    float field_90;
    city_lod *field_94;
    int field_98;

    //0x00542270
    wds_render_manager();

    void debug_render();

    //0x0052A470

    void update_occluders(camera *a2);

    //0x0054B250
    void render(camera &a2, int a3);

    //0x00550930
    void init_level(const char* a2);

    //0x0054ADE0
    void frame_advance(Float a2);

    void render_stencil_shadows(const camera &a2);

    //0x00547000
    void build_render_data_regions(render_data *arg0, camera *arg4);

    //0x00547250
    void build_render_data_ents(render_data *a2, camera *a3, int a4);

    void sub_53D560(const camera *a2);
};


extern void wds_render_manager_patch();
#pragma once

#include "entity.h"
#include "entity_base_vhandle.h"
#include "variable.h"
#include "vector3d.h"

struct line_info_local_query {};

struct line_info {
    vector3d field_0;
    vector3d field_C;
    vector3d hit_pos;
    vector3d hit_norm;
    vector3d field_30;
    vector3d field_3C;
    bool collision;
    bool field_59;
    bool queued_for_collision_check;
    bool field_5C;



};

#ifndef TEST_CASE
int num_debug_line_info[2] {};

extern std::array<line_info[64], 2> debug_line_info;





#endif

//0x0068A9D0
extern bool is_noncrawlable_surface(line_info &a1);

extern void line_info_patch();
#pragma once

#include "hashstring.h"
#include "ngl_math.h"
#include "log.h"
#include "variable.h"

#include "mstring.h"


struct nglMeshSection;
struct nglMeshFile;

struct nglMeshParams {
    uint32_t Flags;
    int NBones;
    math::MatClass<4, 3> *field_8;
    int field_C;

    math::VecClass<3, 1> Scale;

    nglMeshParams() = default;

    nglMeshParams(uint32_t f) : Flags(f) {}
};

constexpr auto NGLMESH_PROCESSED = 0x10000;
constexpr auto NGLMESH_SCRATCH_MESH = 0x20000;

struct tlInstanceBankResourceDirectory {
    int m_vtbl;
};


        Var<tlInstanceBankResourceDirectory*> nglMeshDirectory = (0x00972810);


struct nglMesh {
    tlHashString Name;
    uint32_t Flags;
    uint32_t NSections;


    struct {
        int field_0;
        nglMeshSection *Section;
    } * Sections;
    int NBones;
    math::MatClass<4, 3> *Bones;
    int NLODs;
    struct Lod {
        nglMesh *field_0;
        int field_4;
    };

    Lod *LODs;
    math::VecClass<3, 1> field_20;
    float SphereRadius;
    nglMeshFile *File;
    nglMesh *NextMesh;
    int field_3C;

    nglMesh* nglGetMesh(const tlHashString& a1, bool a2)
    {
        nglMesh*(__fastcall * Find)(void*, int, const tlHashString*) = CAST(Find, get_vfunc(nglMeshDirectory()->m_vtbl, 0xC));
        auto* v4 = Find(nglMeshDirectory(), 0, &a1);
        if (v4 == nullptr && a2) {
            auto* v2 = a1.c_str();
        }

        return v4;
    }


    static tlHashString *get_string(nglMesh *Mesh);
};#include "pc_joypad_device.h"

#include "game.h"
#include "input.h"

#include "gamepadinput.h"
#include "trace.h"
#include "utility.h"

#include <optional>


pc_joypad_device::pc_joypad_device()
{
    using vtbl_t = std::decay_t<decltype(*m_vtbl)>;
    Var<vtbl_t> tmp { 0x0088EA80 };
    static vtbl_t vtbl = tmp();

    this->m_vtbl = &vtbl;
    this->field_4 = INVALID_DEVICE_ID;

    {
        auto address = func_address(&pc_joypad_device::clear_state);
        this->m_vtbl->clear_state = CAST(this->m_vtbl->clear_state, address);
    }

    {
        auto address = func_address(&pc_joypad_device::get_axis_state);
        this->m_vtbl->get_axis_state = CAST(this->m_vtbl->get_axis_state, address);
    }

    {
        auto address = func_address(&pc_joypad_device::get_axis_old_state);
        this->m_vtbl->get_axis_old_state = CAST(this->m_vtbl->get_axis_old_state, address);
    }

    {
        auto address = func_address(&pc_joypad_device::get_axis_delta);
        this->m_vtbl->get_axis_delta = CAST(this->m_vtbl->get_axis_delta, address);
    }

    {
        auto address = func_address(&pc_joypad_device::poll);
        this->m_vtbl->poll = CAST(this->m_vtbl->poll, address);
    }

    {
        auto address = func_address(&pc_joypad_device::is_connected);
        this->m_vtbl->is_connected = CAST(this->m_vtbl->is_connected, address);
    }
}

static pc_joypad_device g_gamepad {};

pc_joypad_device* pc_joypad_device::instance = &g_gamepad;



void pc_joypad_device::clear()
{
    this->field_4 = INVALID_DEVICE_ID;
}

void pc_joypad_device::initialize(int a2)
{
    this->field_4 = 0xF4247;
    this->field_8 = a2;
}

mString pc_joypad_device::get_name() const
{
    mString a1 { "USB GamePad" };
    return a1;
}

void pc_joypad_device::clear_state()
{
    ;
}

float pc_joypad_device::get_axis_state(int axis, int a3)
{
    TRACE("pc_joypad_device::get_axis_state");

    printf("axis = %d, %d\n", axis, a3);

    if (Input::instance() != nullptr) {
        static uint16_t gamepad_codes[Gamepad_Num_Axes] {};

#define register_buttons_code(buttons) gamepad_codes[buttons] = buttons

        register_buttons_code(L2);
        register_buttons_code(R2);

#undef register_buttons_code

        float result = (Input::instance()->m_state_gamepad[gamepad_codes[axis]] != 1);
        return result;
    }

    return 1.0f;
}

float pc_joypad_device::get_axis_old_state(int axis, int)
{
    return 0.0f;
}

float pc_joypad_device::get_axis_delta(int axis, int a2)
{
    return this->get_axis_state(axis, a2) - 1.0f;
}

void pc_joypad_device::poll()
{
    TRACE("pc_joypad_device::poll");
    ;
}

bool pc_joypad_device::is_connected() const
{
    return this->field_4 != INVALID_DEVICE_ID;
}


bool pc_joypad_device::is_vibrator_present() const
{
    return true;
}#include "input.h" 
#include "func_wrapper.h"
#include "utility.h"
#include "common.h"

VALIDATE_SIZE(Input, 0x129F8);

VALIDATE_OFFSET(Input, m_din, 0x27FC);

DIJOYSTATE2* Input::sub_820570(int a2)
{
    return &this->field_4F8[a2];
}

bool Input::sub_820590(int a2)
{
    bool result = false;

    if (this->field_14[a2]) {
        result = (this->field_64[a2] > 0);
    }

    return result;
}#pragma once

#include "variable.h"

#include <cstdint>
#include <windows.h>

#include <dinput.h>

struct IDirectInput8A;
struct InputSettings;
struct IDirectInputDevice8A;

struct Input {
    bool field_0;
    char empty0[3];
    int field_4;
    bool field_8;
    bool field_9;
    char empty1[2];
    IDirectInputDevice8A *m_di_keyboard;
    IDirectInputDevice8A *m_di_mouse;
    int field_14[10];
    int field_3C[10];
    int field_64[10];
    int field_8C[20];
    int empty2[250];
    DIMOUSESTATE2 m_mouse_state;
    DIMOUSESTATE2 m_old_mouse_state;
    char field_4EC[4];
    int field_4F0;
    int field_4F4;

    DIJOYSTATE2 field_4F8[10];
    DIJOYSTATE2 field_F98[10];
    DIMOUSESTATE2 field_1A38;
    int field_1A4C[680];
    char field_24EC[256];
    char m_state_keys[256];
    char m_state_gamepad[14];
    char m_old_state_keys[256];
    IDirectInput8A *m_din;
    int field_27F0[10][4];
    bool m_initialized;
    char empty6[3];
    HWND m_hwnd;
    float m_sensitivity;
    int empty26[2];
    char m_keys[222][256];
    int field_106A4;

    int empty27[2175];
    char m_mouseLeft[30];
    char m_mouseMiddle[30];
    char m_mouseRight[30];
    char m_mouseBtn[30];
    char m_mouseAxis[30];
    char m_mouseWheelUp[30];
    char m_mouseWheelDown[30];

    char m_gamepadAxis[30];
    char m_gamepadBtn[30];
    char m_gamepadPoV[30];
    uint32_t field_129D0;
    uint32_t m_current_connected;
    InputSettings* field_129D8[4];


    static inline Var<Input*> instance { 0x00987948 };

DIJOYSTATE2* sub_820570(int a2);


    bool sub_820590(int a2);


};

extern void Input_patch();
#pragma once

#include "device_id.h"
#include "float.hpp"

#include <cstdint>

struct input_device {

    struct    {
        int field_0;
        int get_name;
        device_id_t (__fastcall *get_id)(const input_device *);
        int (__fastcall *get_axis_count)(const input_device *);
        int (__fastcall *get_axis_id)(input_device *, void *, int a1);
        float (__fastcall *get_axis_state)(input_device *, void *edx, int , int);
        float (__fastcall *get_axis_old_state)(input_device *, void *edx, int , int);
        float (__fastcall *get_axis_delta)(input_device *, void *edx, int , int );
        void (__fastcall *poll)(input_device *);
        int normalize;
        void (__fastcall *finalize)(input_device *, void *, bool);
        bool (__fastcall *is_connected)(const input_device *);
        int (__fastcall *clear_state)(input_device *);
        void (__fastcall *vibrate)(input_device *, void *edx, int , int , int , int a4);
        void (__fastcall *vibrate_0)(input_device *, void *edx, Float );
        void (__fastcall *stop_vibration)(input_device *);
        bool (__fastcall *is_vibrator_present)(const input_device *);
        

    } *m_vtbl;

    int field_4;

    input_device();

    //0x005B0A60
    //virtual
    uint8_t normalize(int a1);

    //virtual
    bool is_connected() const;

    //virtual
    int get_axis_id(int a1);

    //virtual
    float get_axis_delta(int a2, int a3);

    //virtual
    void poll();

    //virtual
    float get_axis_old_state(int a2, int a3);

    //virtual
    float get_axis_state(int a2, int a3);

    //virtual
    device_id_t get_id() const;
};
#pragma once

enum GamepadEvent {
    Release_,
    Press_,
};

enum GamepadInput {
    Cross = 0,
    Triangle = 1,
    Circle = 4,
    Square = 5,

    R2 = 6,
    R3 = 7,
    L2 = 8,
    Right = 9,
    Select = 10,
    Forward = 11,
    Left = 12,
    Start = 13,

    Gamepad_Num_Axes

};
#pragma once

#include "input_device.h"
#include "mstring.h"

struct pc_joypad_device : input_device {

    int field_8;

    pc_joypad_device();

    // virtual
    mString get_name() const;

    void clear();

    void initialize(int a2);

    // virtual
    void clear_state();

    // virtual
    float get_axis_state(int axis, int);

    float get_axis_old_state(int a2, int a3);

    float get_axis_delta(int axis, int a3);

    void poll();

    bool is_connected() const;

    bool is_vibrator_present() const;

    static pc_joypad_device* instance;
};


#include "keyboard_device.h"

#include "game.h"
#include "input.h"
#include "keyboard.h"
#include "trace.h"
#include "utility.h"

#include <optional>

keyboard_device::keyboard_device()
{
    using vtbl_t = std::decay_t<decltype(*m_vtbl)>;
    Var<vtbl_t> tmp {0x0088EA80};
    static vtbl_t vtbl = tmp();

    this->m_vtbl = &vtbl;
    this->field_4 = INVALID_DEVICE_ID;

    {
        auto address = func_address(&keyboard_device::_clear_state);
        this->m_vtbl->clear_state = CAST(this->m_vtbl->clear_state, address);
    }

    {
        auto address = func_address(&keyboard_device::_get_axis_state);
        this->m_vtbl->get_axis_state = CAST(this->m_vtbl->get_axis_state, address);
    }

    {
        auto address = func_address(&keyboard_device::_get_axis_old_state);
        this->m_vtbl->get_axis_old_state = CAST(this->m_vtbl->get_axis_old_state, address);
    }

    {
        auto address = func_address(&keyboard_device::_get_axis_delta);
        this->m_vtbl->get_axis_delta = CAST(this->m_vtbl->get_axis_delta, address);
    }

    {
        auto address = func_address(&keyboard_device::_poll);
        this->m_vtbl->poll = CAST(this->m_vtbl->poll, address);
    }

    {
        auto address = func_address(&keyboard_device::_is_connected);
        this->m_vtbl->is_connected = CAST(this->m_vtbl->is_connected, address);
    }

}

static keyboard_device g_device {};

keyboard_device * keyboard_device::instance = &g_device;

mString keyboard_device::get_name() const
{
    mString a1 {"USB Keyboard"};
    return a1;
}

void keyboard_device::clear()
{
    this->field_4 = INVALID_DEVICE_ID;
}

void keyboard_device::initialize(int a2)
{
    this->field_4 = 0x1E8480;
    this->field_8 = a2;
}

void keyboard_device::_clear_state()
{
    ;
}

float keyboard_device::_get_axis_state(int axis, int a3)
{
    TRACE("keyboard_device::get_axis_state");

    printf("axis = %d, %d\n", axis, a3);

    if (Input::instance() != nullptr)
    {
        static uint16_t key_codes[KB_NUM_AXES] {};

#define register_key_code(key)  key_codes[KB_##key] = DIK_##key

        register_key_code(I);
        register_key_code(K);
        register_key_code(T);
        register_key_code(J);
        register_key_code(L);
        register_key_code(O);
        register_key_code(U);
        register_key_code(RSHIFT);
        register_key_code(RALT);

#undef register_key_code

        float result = (Input::instance()->m_state_keys[key_codes[axis]] != 0);
        return result;
    }

    return 0.0f;
}

float keyboard_device::_get_axis_old_state(int axis, int )
{
    return 0.0f;
}

float keyboard_device::_get_axis_delta(int axis, int a2)
{
    return this->_get_axis_state(axis, a2) - 0.0f;
}

void keyboard_device::_poll()
{
    TRACE("keyboard_device::poll");
    ;
}

bool keyboard_device::_is_connected() const {
    return this->field_4 != INVALID_DEVICE_ID;
}

bool keyboard_device::_is_vibrator_present() const {
    return false;
}
#include "actor.h"

#include <cstdio>
#include <vector>

struct nalAnyPose;
struct nalBaseSkeleton;

template<typename T>
struct nalAnimClass;

struct ai_player_controller;
struct damage_interface;
struct physical_interface;

VALIDATE_OFFSET(actor, m_player_controller, 0x8C);
VALIDATE_OFFSET(actor, field_BC, 0xBC);

std::vector<nalAnimClass<nalAnyPose> *> actor::get_animations(actor *a1)
{
    std::vector<nalAnimClass<nalAnyPose> *> a2;
    a2.reserve(100u);

    auto *v11 = a1->field_BC;
    if ( v11 != nullptr )
    {
        auto &res_dir = v11->get_resource_directory();
        auto tlresource_count = res_dir.get_tlresource_count(TLRESOURCE_TYPE_ANIM_FILE);
        for (auto idx = 0; idx < tlresource_count; ++idx)
        {
            auto *tlres_loc = res_dir.get_tlresource_location(idx, TLRESOURCE_TYPE_ANIM_FILE);
            auto *animFile = (nalAnimFile *) tlres_loc->field_8;
            if ( animFile->field_0 == 0x10101 )
            {
                for ( auto *anim = bit_cast<nalAnimClass<nalAnyPose> *>(animFile->field_34);
                        anim != nullptr;
                        anim = anim->field_4 ) {
                    a2.push_back(anim);
                }
            }
        }
    }

    return a2;
}

animation_controller::anim_ctrl_handle actor::play_anim(const string_hash &a3)
{
    printf("actor::play_anim\n");
    if ( this->anim_ctrl == nullptr ) {
        this->allocate_anim_controller(0u, nullptr);
    }

    assert(anim_ctrl != nullptr);
    this->anim_ctrl->play_base_layer_anim(a3, 0.0, 32u, true);

    animation_controller::anim_ctrl_handle result{};
    result.field_0 = true;
    result.field_8 = bit_cast<decltype(result.field_8)>(this->anim_ctrl);
    return result;
}
#pragma once

#include "entity.h"

#include "string_hash.h"
#include "animation_controller.h"
#include "nal_system.h"
#include "resource_key.h"
#include "resource_pack_slot.h"
#include "vector3d.h"

#include <vector>

struct nalAnyPose;
struct nalBaseSkeleton;

template<typename T>
struct nalAnimClass;

struct ai_player_controller;
struct damage_interface;
struct physical_interface;

struct actor : entity
{
    damage_interface *m_damage_interface;
    physical_interface *m_physical_interface;
    nalBaseSkeleton *m_skeleton;
    animation_controller *anim_ctrl;
    void *adv_ptrs;
    void *field_7C;
    void *m_interactable_ifc;
    void *m_facial_expression_interface;
    int field_88;
    ai_player_controller *m_player_controller;
    char field_90[0xC];
    uint8_t *field_9C;
    void *field_A0;
    int16_t field_A4;
    int16_t field_A6;
    int16_t field_A8[1];
    int16_t field_AA;
    vector3d field_AC;
    int field_B8;
    resource_pack_slot *field_BC;

    ai_player_controller *get_player_controller() {
        return this->m_player_controller;
    }

    physical_interface *physical_ifc()
    {
        physical_interface * (__fastcall *func)(void *) = bit_cast<decltype(func)>(0x004B8B40);
        return func(this);
    }

    void allocate_anim_controller(unsigned int a2, nalBaseSkeleton *a3) {
        void (__fastcall *func)(void *, int, uint32_t, nalBaseSkeleton *) = 
            bit_cast<decltype(func)>(0x004CC630);
        func(this, 0, a2, a3);
    }

    animation_controller::anim_ctrl_handle play_anim(const string_hash &a3);

    static std::vector<nalAnimClass<nalAnyPose> *> get_animations(actor *a1);
};
#include "ai_player_controller.h"

#include "common.h"
#include "utility.h"
#include "variable.h"

VALIDATE_SIZE(ai_player_controller, 0x424u);

static Var<void *> g_spiderman_camera_ptr {0x00959A70};

void ai_player_controller::lock_controls(bool a2)
{
    this->field_3DC = false;

    void (__fastcall *func)(void *, void *, bool) = CAST(func, 0x004B3260);
    func(g_spiderman_camera_ptr(), nullptr, a2);
}
#pragma once

#include "vector3d.h"

struct ai_player_controller
{
    char field_0[0x3DC];
    bool field_3DC;
    vector3d field_3E0;
    vector3d field_3EC;
    float field_3F8;
    float field_3FC;
    vector3d field_400;
    float field_40C;
    int field_410;
    int field_414;
    int field_418;
    int field_41C;
    int field_420;

    void lock_controls(bool a2);
};
#include "animation_controller.h"

#include "bit.h"

#include <cstdio>

animation_controller::anim_ctrl_handle animation_controller::play_base_layer_anim(
        const string_hash &a3,
        Float a4,
        uint32_t a5,
        bool a6)
{
    printf("animation_controller::play_base_layer_anim\n");

    animation_controller::anim_ctrl_handle result;
    void (__fastcall *func)(void *, int, animation_controller::anim_ctrl_handle *, const string_hash *, Float, uint32_t, bool) = bit_cast<decltype(func)>(0x0049B9A0);
    func(this, 0, &result, &a3, a4, a5, a6);

    return result;
}

void animation_controller::anim_ctrl_handle::set_anim_speed(Float a2)
{
    printf("animation_controller::anim_ctrl_handle::set_anim_speed\n");

    struct {
        char field_0[0x58];
        void (__fastcall *field_58)(void *, int, Float);
        void (__fastcall *field_5C)(void *, int, Float, Float);
    } * vtbl = bit_cast<decltype(vtbl)>(this->field_8->m_vtbl);

    if ( this->field_0 ) {
        vtbl->field_58(this->field_8, 0, a2);
    } else {
        vtbl->field_5C(
            this->field_8,
            0,
            a2,
            this->field_4);
    }
};
#pragma once

#include "float.hpp"

#include <cstdint>

struct string_hash;

struct animation_controller {
    struct anim_ctrl_handle {
        int field_0;
        int field_4;
        struct {
            int m_vtbl;
        } * field_8;

        void set_anim_speed(Float a2);
    };

    anim_ctrl_handle play_base_layer_anim(
            const string_hash &a3,
            Float a4,
            uint32_t a5,
            bool a6);
};
#pragma once

#include "mvector.h"
#include "param_block.h"
#include "variable.h"

#include "list.hpp"

struct actor;

namespace ai {

struct info_node;

struct ai_core {
    char field_0[0x50];
    param_block field_50;
    int field_5C;
    mVector<ai::info_node> *field_60;
    actor *field_64;

    param_block * get_param_block()
    {
        return &this->field_50;
    }

    actor * get_actor(int) {
        return field_64;
    }

    static inline Var<_std::list<ai_core *> *> the_ai_core_list_high {0x0096BE24};

};


}
#include "base_ai_debug.h"

#include "actor.h"
#include "base_ai_core.h"
#include "debug_menu.h"
#include "info_node.h"
#include "os_file.h"

#include <cstdio>
#include <string>

static ai::ai_core *debug_menu_ai_core = nullptr;

static const char *TYPE_NAME_ARRAY[8] {
    "float",
    "int",
    "string_hash",
    "fixedstring",
    "vector3d",
    "float_variance",
    "entity",
    "pointer"
};


void handle_export_block(debug_menu_entry *arg0)
{
    auto *the_pb = static_cast<ai::param_block *>(arg0->get_data());
    assert(the_pb != nullptr);

    auto *v2 = debug_menu_ai_core->get_actor(0);
    auto id = v2->get_id();
    auto v4 = id.to_string();
    mString a1 {0, "param_dump_%s.txt", v4};
    os_file file {a1, 2};
    mString v32 {};

    auto *v31 = the_pb->param_array;
    if ( v31 != nullptr )
    {
        auto *v8 = id.to_string();
        v32 = mString {0, "// Parameter list for %s\r\n", v8};
        auto v14 = v32.length();
        auto *v9 = v32.c_str();
        file.write(v9, v14);

        for ( auto &v28 : v31->field_0 )
        {
            auto v21 = v28->get_value_in_string_form();
            auto v19 = v28->field_8;

            auto v15 = v21.c_str();
            auto *v13 = v19.to_string();
            auto data_type = v28->get_data_type();
            v32 = mString {0, "%s %s %s\r\n", TYPE_NAME_ARRAY[data_type], v13, v15};
            auto v16 = v32.length();
            auto *v11 = v32.c_str();
            file.write(v11, v16);
        }
    }
    else
    {
        v32 = mString {"// no parameters defined in this block.\r\n"};
        auto v17 = v32.length();
        auto *v12 = v32.c_str();
        file.write(v12, v17);
    }

    debug_menu::hide();
}

std::string ai_param_render_callback(debug_menu_entry *a2)
{
    using namespace ai;

    mString result {};

    auto *the_data = static_cast<ai::param_block::param_data *>(a2->get_data());
    assert(the_data != nullptr);

    switch ( the_data->get_data_type() )
    {
    case PT_STRING_HASH: {
        auto v19 = the_data->get_data_hash();
        auto v18 = the_data->field_8;
        auto *v15 = v19.to_string();
        auto *v2 = v18.to_string();
        result = mString {0, "%s %s (hash)", v2, v15};
        break;
    }
    case PT_FIXED_STRING: {
        auto v20 = the_data->field_8;
        auto *v16 = the_data->get_data_string();
        auto v4 = v20.to_string();
        result = mString {0, "%s %s (fixedstring)", v4, v16};
        break;
    }
    case PT_VECTOR_3D: {
        vector3d v32 = *the_data->get_data_vector3d();
        auto v21 = the_data->field_8;
        auto v13 = v32[2];
        auto v11 = v32[1];
        auto v10 = v32[0];
        auto *v6 = v21.to_string();
        result = mString {0, "%s (%.2f %.2f %.2f) (vector3d)", v6, v10, v11, v13};
        break;
    }
    case PT_FLOAT_VARIANCE: {
        auto v31 = *the_data->get_data_float_variance();
        auto v22 = the_data->field_8;
        auto v14 = v31.field_4;
        auto v12 = v31.field_0;
        auto *v8 = v22.to_string();
        result = mString {0, "%s (b%.2f v%.2f) (float variance)", v8, v12, v14};
        break;
    }
    default: {
        auto v23 = the_data->field_8;
        auto data_type = the_data->get_data_type();
        auto *v9 = v23.to_string();
        printf("Unsupported param data type for %s %d.  This code needs to be updated\n", v9, data_type);
        assert(0);
        return result.c_str();
    }
    }

    return result.c_str();
}

void populate_param_block(debug_menu_entry *a2)
{
    using namespace ai;

    auto *the_pb = static_cast<ai::param_block *>(a2->get_data());
    assert(the_pb != nullptr);

    auto name_menu = a2->get_script_handler();
    auto *v26 = create_menu(name_menu.c_str(), debug_menu::sort_mode_t::ascending);
    a2->set_submenu(v26);

    debug_menu_entry *v27 = nullptr;
    auto *v25 = the_pb->param_array;
    if ( v25 != nullptr )
    {
        for ( auto &v22 : v25->field_0 )
        {
            auto data_hash = v22->field_8;
            auto *v2 = data_hash.to_string();
            mString a1 {0, "%s", v2};

            debug_menu_entry v27 {a1};

            switch ( v22->get_data_type() )
            {
            case PT_FLOAT: {
                auto *v3 = bit_cast<float *>(v22);
                
                v27.set_pt_fval(v3);
                v27.set_step_size(0.30000001);
                v27.set_step_scale(30.0);
                v27.set_max_value(3.4028235e38);
                v27.set_min_value(-3.4028235e38);
                break;
            }
            case PT_INTEGER: {
                auto *v4 = bit_cast<int *>(v22);
                
                v27.set_p_ival(v4);
                v27.set_max_value(2147483600.0);
                v27.set_min_value(-2147483600.0);
                break;
            }
            case PT_STRING_HASH:
            case PT_FIXED_STRING:
            case PT_VECTOR_3D:
            case PT_FLOAT_VARIANCE:
                v27.set_render_cb(ai_param_render_callback);
                break;
            default:
                auto data_type = v22->get_data_type();
                printf("Unknown param data type %d.  This code needs to be updated\n", data_type);
                assert(0);
                return;
            }

            v27.set_data(v22);
            v26->add_entry(&v27);
        }

        v27 = create_menu_entry(mString {"--Export this block--"});
        v27->set_game_flags_handler(handle_export_block);
        v27->set_data(the_pb);
    }
    else
    {
        v27 = create_menu_entry(mString {"--None defined--"});
    }

    v26->add_entry(v27);
}

void ai_core_menu_handler(debug_menu_entry *a2)
{
    auto *the_core = static_cast<ai::ai_core *>(a2->get_data());
    assert(the_core != nullptr);

    debug_menu_ai_core = the_core;

    auto name_menu = a2->get_script_handler();
    auto *v21 = create_menu(name_menu.c_str(), debug_menu::sort_mode_t::ascending);
    a2->set_submenu(v21);

    debug_menu_entry v20 {mString {"-Core params"}};
    v20.set_submenu(nullptr);
    v20.set_game_flags_handler(populate_param_block);
    auto *v2 = the_core->get_param_block();
    v20.set_data(v2);
    v21->add_entry(&v20);

    auto *v19 = the_core->field_60;
    if ( v19 != nullptr )
    {
        for ( auto &v16 : (*v19) )
        {
            auto v5 = v16->field_4;
            auto *v3 = v5.to_string();

            debug_menu_entry v20 {mString {0, "%s inode params", v3}};
            v20.set_submenu(nullptr);
            v20.set_game_flags_handler(populate_param_block);
            auto &v4 = v16->m_param_block;
            v20.set_data(&v4);
            v21->add_entry(&v20);
        }
    }
}

void populate_ai_root(debug_menu_entry *arg0)
{
    auto name_menu = arg0->get_script_handler();
    debug_menu *v20 = create_menu(name_menu.c_str(), debug_menu::sort_mode_t::undefined);
    arg0->set_submenu(v20);

    static auto *g_the_ai_core_list = ai::ai_core::the_ai_core_list_high();

    if ( g_the_ai_core_list != nullptr )
    {
        for ( auto &v17 : (*g_the_ai_core_list) )
        {
            auto *v3 = v17->get_actor(0);
            if ( !v3->is_flagged(0x800u) )
            {
                auto id = v3->get_id();
                auto *v7 = id.to_string();
                debug_menu_entry v16 {mString {v7}};

                v16.set_data(v17);
                v16.set_submenu(nullptr);
                v16.set_game_flags_handler(ai_core_menu_handler);
                v20->add_entry(&v16);
            }
        }
    }
}

void create_ai_root_menu(debug_menu *parent)
{
    assert(parent != nullptr);

    debug_menu_entry v5 {mString {"AI"}};
    v5.set_submenu(nullptr);
    v5.set_game_flags_handler(populate_ai_root);
    parent->add_entry(&v5);
}
#pragma once

struct debug_menu;

extern void create_ai_root_menu(debug_menu *parent);
#pragma once

#include <cstring>

#include <type_traits>

template<class To, class From>
constexpr
    typename std::enable_if_t<sizeof(To) == sizeof(From) && std::is_trivially_copyable_v<From> &&
                                  std::is_trivially_copyable_v<To>,
                              To>
    // constexpr support needs compiler magic
    bit_cast(const From &src) noexcept {
    static_assert(
        std::is_trivially_constructible_v<To>,
        "This implementation additionally requires destination type to be trivially constructible");

    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

#pragma once

struct color {
    float r, g, b, a;
};
#pragma once

#define VALIDATE_SIZE(struc, size) \
    static_assert(sizeof(struc) == size, "Invalid structure size of " #struc)

#define VALIDATE_OFFSET(struc, member, offset)       \
    static_assert(offsetof(struc, member) == offset, \
                  "The offset of " #member " in " #struc " is not " #offset "...")

#pragma once

enum device_id_t {
    INVALID_DEVICE_ID = -1,
};
#pragma once

#include "os_developer_options.h"

#include <cassert>

struct game_option_t
{
    const char *m_name;
    union {
        bool *p_bval;
        int *p_ival;
        float *p_fval;
        mString *p_strval;
    } m_value;
    enum {
        UNDEFINED,
        FLAG_OPTION = 1,
        INT_OPTION = 2,
        FLOAT_OPTION = 3,
        STRING_OPTION = 4,
    } m_type;
};


inline constexpr auto NUM_OPTIONS = 150u + 76u;

game_option_t *get_option(int idx)
{
    assert(idx >= 0);
    assert(idx < NUM_OPTIONS);

    static game_option_t option{};

    if (idx < 150)
    {
        auto &name = flag_names()[idx];
        bool *flag = &os_developer_options::instance()->m_flags[idx];

        option.m_name = name;
        option.m_type = game_option_t::FLAG_OPTION;
        option.m_value.p_bval = flag;

        return &option;
    }

    idx = idx - 150;
    auto &name = int_names()[idx];
    int *i = &os_developer_options::instance()->m_ints[idx];

    option.m_name = name;
    option.m_type = game_option_t::INT_OPTION;
    option.m_value.p_ival = i;

    return &option;
}
#pragma once

#include "signaller.h"

#include "common.h"
#include "string_hash.h"
#include "vtbl.h"
#include "variable.h"
#include "vector3d.h"

#include "list.hpp"

struct collision_geometry;
struct region;
struct po;
struct time_interface;

template<typename T, uint32_t N>
struct fixed_vector;

struct entity : signaller 
{
    static constexpr inline auto FIXED_REGIONS_ARRAY_SIZE = 2;

    region *regions[FIXED_REGIONS_ARRAY_SIZE];
    fixed_vector<region *, 7> *extended_regions;
    collision_geometry *colgeom;
    time_interface *field_58;
    int field_5C;
    int field_60;
    int field_64;

    void suspend(bool a1) {
        auto &func = get_vfunc(m_vtbl, 0x1B8);

        func(this, 0, a1);
    }

    void unsuspend(bool a1) {
        auto &func = get_vfunc(m_vtbl, 0x1BC);

        func(this, 0, a1);
    }

    region * get_primary_region()
    {
        region *(__fastcall *func)(void *) = bit_cast<decltype(func)>(0x004C0760);
        return func(this);
    }

    vector3d *get_abs_position()
    {
        vector3d * (__fastcall *func)(void *) = bit_cast<decltype(func)>(0x0048AC00);
        return func(this);
    }

    po *get_abs_po()
    {
        po * (__fastcall *func)(void *) = bit_cast<decltype(func)>(0x0048AC20);
        return func(this);
    }

    bool is_an_actor() const {
        bool (__fastcall *func)(const void *) =
            bit_cast<decltype(func)>(get_vfunc(m_vtbl, 0x64));
        return func(this);
    }

    static int find_entities(int a1)
    {
        return CDECL_CALL(0x004D67D0, a1);
    }

    static inline Var<_std::list<entity *> *> found_entities {0x0095A6E0};
};

VALIDATE_SIZE(entity, 0x68u);
VALIDATE_OFFSET(entity, my_handle, 0x1C);
#pragma once

#include "actor.h"
#include "entity.h"
#include "debug_menu.h"
#include "nal_system.h"
#include "resource_manager.h"

#include <list>

void entity_animation_handler(debug_menu_entry *entry)
{
    printf("handle_animation_entry\n");

    auto *v7 = static_cast<entity *>(entry->data1);
    if ( v7 != nullptr && v7->is_an_actor() )
    {
        auto *v6 = (actor *) v7;
        auto *context = v6->field_BC;
        assert(context != nullptr);

        string_hash v4 {entry->text};
        resource_manager::push_resource_context(context);

        auto v3 = v6->play_anim(v4);
        resource_manager::pop_resource_context();
        v3.set_anim_speed(1.0);
        debug_menu::hide();
    }
}

void sub_6918AD(debug_menu_entry *entry)
{
    auto *e = static_cast<entity *>(entry->data1);
    if (e->is_an_actor())
    {
        auto *a1 = create_menu(entry->text);
        entry->set_submenu(a1);
        auto *v18 = (actor *) e;

        auto v17 = actor::get_animations(v18);
        for (auto *v15 : v17)
        {
            auto &v3 = v15->field_8;
            auto *v4 = v3.to_string();
            mString v14 {v4};

            debug_menu_entry v13 {v14};
            v13.data1 = entry->data1;
            v13.set_game_flags_handler(entity_animation_handler);
            a1->add_entry(&v13);
        }
    }
}

void populate_entity_animation_menu(debug_menu_entry *entry)
{
    auto *v26 = create_menu(entry->text);
    entry->set_submenu(v26);
    entity::find_entities(1);
    auto &found_entities = (*entity::found_entities());
    for (auto *ent : found_entities)
    {
        if ( ent->is_an_actor() )
        {
            auto *v23 = (actor *) ent;

            auto v22 = actor::get_animations(v23);
            if ( !v22.empty() )
            {
                auto id = v23->get_id();
                debug_menu_entry v21 {mString {id.to_string()}};

                v21.set_game_flags_handler(sub_6918AD);
                v21.data1 = ent;
                v21.set_submenu(nullptr);
                v26->add_entry(&v21);
            }
        }
    }
}

void create_entity_animation_menu(debug_menu *parent)
{
    debug_menu_entry v5 {mString{"Entity Animations"}};

    v5.set_submenu(nullptr);
    v5.set_game_flags_handler(populate_entity_animation_menu);
    parent->add_entry(&v5);
}
#pragma once

#include "common.h"
#include "entity_base_vhandle.h"
#include "string_hash.h"

#include <cstdint>

struct po;

struct conglomerate;
struct motion_effect_struct;
struct sound_and_pfx_interface;

struct entity_base {
    int m_vtbl;
    uint32_t field_4;
    uint32_t field_8;
    po *my_rel_po;
    string_hash field_10;
    po *my_abs_po;
    motion_effect_struct *field_18;
    entity_base_vhandle my_handle;
    entity_base *m_parent;
    entity_base *m_child;
    entity_base *field_28;
    int16_t proximity_map_cell_reference_count;
    uint8_t m_timer;
    void *adopted_children;
    conglomerate *my_conglom_root;
    sound_and_pfx_interface *my_sound_and_pfx_interface;
    int16_t field_3C;
    int16_t field_3E;
    int8_t field_40;
    int8_t field_41;
    int8_t rel_po_idx;
    int8_t proximity_map_reference_count;

    bool is_flagged(unsigned int a2) const
    {
        return (a2 & this->field_4) != 0;
    }

    auto get_id() const {
        return this->field_10;
    }
};

VALIDATE_SIZE(entity_base, 0x44u);
#pragma once

struct entity_base;

struct entity_base_vhandle {
    int field_0;

    entity_base *get_volatile_ptr()
    {
        entity_base *(__fastcall *func)(void *) = (decltype(func)) 0x00601580;
        return func(this);
    }
};
#pragma once

struct string_hash;
struct entity;

enum entity_flavor_t {
    ACTOR = 0,
    SIGNALLER = 1,
    CAMERA = 2,
    BASE = 3,
    ENTITY = 4,
    MARKER = 5,
    PARKING_MARKER = 6,
    WATER_EXIT_MARKER = 7,
    MIC = 8,
    LIGHT_SOURCE = 9,
    PFX = 10,
    ITEM = 11,
    CONGLOMERATE = 12,
    CONGLOMERATE_CLONE = 13,
    BEAM = 14,
    MANIP = 15,
    SWITCH = 16,
    POLYTUBE = 17,
    LENSFLARE = 18,
    NEOLIGHT = 19,
    GAME_CAMERA = 20,
    SPIDERMAN_CAMERA = 21,
    MARKY_CAMERA = 22,
    SNIPER_CAMERA = 23,
    AI_CAMERA = 24,
    ANCHOR_MARKER = 25,
    LINE_ANCHOR = 26,
    AI_COVER_MARKER = 27,
    NULL_FLAVOR = 28,
    IGNORE_FLAVOR = 29,
};

namespace entity_handle_manager
{
    inline entity *find_entity(const string_hash &a1, entity_flavor_t a2, bool a3)
    {
        return (entity *) CDECL_CALL(0x004DC300, &a1, a2, a3);
    }
}
#pragma once

#include "bit.h"

struct vector3d;

struct entity_tracker_manager {

    bool get_the_arrow_target_pos(vector3d *a2)
    {
        bool (__fastcall *func)(void *, void *, vector3d *) =
            bit_cast<decltype(func)>(0x0062EE10);
        return func(this, nullptr, a2);
    }
};

#pragma once

#include "bit.h"

struct PanelQuad;
struct PanelFile;

struct fe_health_widget {
    PanelFile *field_0[5];
    int field_14[7];
    int field_30;
    int field_34;
    int field_38;
    int field_3C;
    PanelQuad *field_40;
    PanelQuad *field_44;
    PanelQuad *field_48;
    float field_4C;
    float field_50;
    bool field_54;
    bool field_55;
    bool field_56;

    void SetShown(bool a2) {
        void (__fastcall *func)(void *, void *, bool) = bit_cast<decltype(func)>(0x0061A3F0);

        func(this, nullptr, a2);
    }

    void SetType(int the_type, int a3)
    {
        void (__fastcall *func)(void *, void *, int, int) = bit_cast<decltype(func)>(0x00641BC0);

        func(this, nullptr, the_type, a3);
    }
};
#pragma once

#include "float.hpp"
#include "func_wrapper.h"
#include "color32.h"
#include "common.h"
#include "mstring.h"
#include "os_developer_options.h"
#include "trace.h"
#include "vector2di.h"

struct font_index
{
    int field_0;
};

struct global_text_enum
{
    int field_0;
};

struct panel_layer
{
    int field_0;
};

struct FEText
{
    char field_0[0x18];
    font_index field_18;
    mString field_1C;
    int field_2C;
    int field_30;
    float field_34;
    float field_38;
    float field_3C;
    float field_40;
    float field_44;
    float field_48;
    color32 field_4C;
    mString field_50;
    int field_60;
    int8_t field_64;

    FEText(font_index a2,
           global_text_enum a3,
           Float a4,
           Float a5,
           int a6,
           panel_layer a7,
           Float a8,
           int a9,
           int a10,
           color32 a11)
    {
        void (__fastcall *func)(void *, void *, font_index, global_text_enum, Float, Float,
                int, panel_layer, Float, int, int, color32) = bit_cast<decltype(func)>(0x00617500);

        func(this, 0, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
    }

    void Draw()
    {
        TRACE("FEText::Draw");

        void (__fastcall *func)(void *) = bit_cast<decltype(func)>(0x00617640);
        func(this);
    }
};

VALIDATE_OFFSET(FEText, field_1C, 0x1C);

inline void render_text(const mString &a1, const vector2di &a2, color32 a3, float a4, float a5)
{
    TRACE("render_text");

    if ( os_developer_options::instance()->get_flag(mString{"SHOW_DEBUG_TEXT"}) )
    {
        FEText fe_text{font_index{0},
                       global_text_enum{0},
                       (float) a2.x,
                       (float) a2.y,
                       (int) a4,
                       panel_layer{0},
                       a5,
                       16,
                       0,
                       a3};

        fe_text.field_1C = a1;

        fe_text.Draw();
    }
}

#pragma once

#include <cstdint>
#include <cstdio>
#include <cassert>

template<uint32_t Num>
struct fixedstring
{
    static inline auto MAX_CHARS = Num;

    char field_0[Num];

    fixedstring(const char *str)
    {
        assert(str != nullptr);

        auto len = strlen(str);
        if ( len > 16 )
        {
            printf("String too big for fixed size, str is %s", str);
            assert(0);
        }

        assert(len <= MAX_CHARS);

        std::memcpy(this->field_0, str, len);
    }

    const char *to_string() const
    {
        return field_0;
    }

    bool operator==(const fixedstring<Num> &a2) const
    {
        for ( auto i = 0; i < 2; ++i )
        {
            if ( *(const int *)&this->field_0[8 * i] != *(const int *)&a2.field_0[8 * i]
                || *(const int *)&this->field_0[8 * i + 4] != *(const int *)&a2.field_0[8 * i + 4] )
            {
                return false;
            }
        }

        return true;
    }

};

struct tlFixedString {
    uint32_t m_hash;
    char field_4[28];

    auto GetHash() const {
        return this->m_hash;
    }

    const char *to_string() const {
        return field_4;
    }
};
#pragma once

#include "bit.h"

typedef int(__cdecl *cdecl_call)(...);
typedef int(__stdcall *stdcall_call)(...);
typedef int(__fastcall *fastcall_call)(...);


template<typename... Args>
decltype(auto) STDCALL(int address, Args... args) {

    return (reinterpret_cast<stdcall_call>(address))(args...);
}

template<typename... Args>
decltype(auto) CDECL_CALL(int address, Args... args) {

    return (reinterpret_cast<cdecl_call>(address))(args...);
}
#pragma once

#include "func_wrapper.h"

namespace geometry_manager
{
    inline void enable_scene_analyzer(bool a1)
    {
        CDECL_CALL(0x00515730, a1);
    }

    inline bool is_scene_analyzer_enabled()
    {
        return (bool) CDECL_CALL(0x00515720);
    }
}
#pragma once

struct tlHashString {
    unsigned int field_0;
};
#pragma once

struct fe_health_widget;
struct entity_tracker_manager;
struct fe_score_widger;
struct fe_hotpursuit_indicator;
struct combo_words;
struct IGOZoomOutMap;
struct fe_game_credits;
struct fe_crosshair;
struct race_announcer;
struct medal_award_ui;
struct threat_assessment_meters;
struct thug_health;
struct fe_distance_chase;
struct fe_distance_race;
struct fe_mission_text;
struct fe_track_and_field;
struct fe_mini_map_widget;
struct targeting_reticle;
struct tutorial_controller_gauge;
struct fe_score_widget;

struct IGOFrontEnd
{
    void *field_0;
    fe_mini_map_widget *field_4;
    fe_health_widget *boss_health;
    fe_health_widget *hero_health;
    fe_health_widget *third_party_health;
    fe_track_and_field *field_14;
    fe_distance_chase *field_18;
    fe_distance_race *field_1C;
    fe_mission_text *field_20;
    threat_assessment_meters *field_24;
    thug_health *field_28;
    targeting_reticle *field_2C;
    tutorial_controller_gauge *field_30;
    medal_award_ui *field_34;
    race_announcer *field_38;
    fe_crosshair *field_3C;
    fe_game_credits *field_40;
    IGOZoomOutMap *field_44;
    combo_words *field_48;
    fe_hotpursuit_indicator *field_4C;
    fe_score_widget *field_50;
    entity_tracker_manager *field_54;
};

VALIDATE_SIZE(IGOFrontEnd, 0x58);
#pragma once

#include "string_hash.h"

struct actor;
struct param_block;

namespace ai {

struct ai_core;

struct info_node {
    int m_vtbl;
    string_hash field_4;
    ai_core *field_8;
    actor *field_C;
    param_block m_param_block;
};

}
#include "input_device.h"

#include "common.h"
#include "utility.h"
#include "vtbl.h"

VALIDATE_SIZE(input_device, 0x8);

input_device::input_device()
{
    m_vtbl = CAST(m_vtbl, 0x0088E4D0);
    this->field_4 = INVALID_DEVICE_ID;
}

uint8_t input_device::normalize(int a1) {
    return a1;
}

bool input_device::is_connected() const {

    return this->m_vtbl->is_connected(this);
}

int input_device::get_axis_id(int a1) {

    return this->m_vtbl->get_axis_id(this, nullptr, a1);
}

float input_device::get_axis_delta(int a2, int a3) {

    return this->m_vtbl->get_axis_delta(this, nullptr, a2, a3);
}

void input_device::poll() {

    return this->m_vtbl->poll(this);
}

float input_device::get_axis_old_state(int a2, int a3) {

    return this->m_vtbl->get_axis_old_state(this, nullptr, a2, a3);
}

float input_device::get_axis_state(int a2, int a3) {

    return this->m_vtbl->get_axis_state(this, nullptr, a2, a3);
}

device_id_t input_device::get_id() const
{
    return this->m_vtbl->get_id(this);
}
#pragma once

enum KeyEvent {
    Release,
    Press,
};

enum Key_Axes 
{
    KB_A = 1,                   
    KB_B = 2,                   
    KB_C = 3,                   
    KB_D = 4,                   
    KB_E = 5,                   
    KB_F,                   
    KB_G,                   
    KB_H,                   
    KB_I = 9,                   
    KB_J,                   
    KB_K = 11,                   
    KB_L,                   
    KB_M,                   
    KB_N,                   
    KB_O,                   
    KB_P,                   
    KB_Q,                   
    KB_R,                   
    KB_S,                   
    KB_T,                   
    KB_U,                   
    KB_V,                   
    KB_W,                   
    KB_X,                   
    KB_Y,                   
    KB_Z,                   

    KB_1,                   
    KB_2,                   
    KB_3,                   
    KB_4,                   
    KB_5,                   
    KB_6,                   
    KB_7,                   
    KB_8,                   
    KB_9,                   
    KB_0,                   

    KB_LSHIFT,
    KB_RSHIFT,
    KB_LCONTROL,
    KB_RCONTROL,
    KB_LALT,
    KB_RALT,
    KB_LSPECIAL,
    KB_RSPECIAL,

    KB_RETURN,              
    KB_ESCAPE,              
    KB_BACKSPACE,           
    KB_TAB,                 
    KB_SPACE,               

    KB_MINUS,               
    KB_EQUALS,                
    KB_LBRACKET,            
    KB_RBRACKET,            
    KB_BACKSLASH,       
    KB_SEMICOLON,       
    KB_QUOTE,               
    KB_TILDE,               
    KB_COMMA,               
    KB_PERIOD,              
    KB_SLASH,               
    KB_CAPSLOCK,            

    KB_F1,	                 
    KB_F2,	                 
    KB_F3,	                 
    KB_F4,	                 
    KB_F5,	                 
    KB_F6,	                 
    KB_F7,	                 
    KB_F8,	                 
    KB_F9,	                 
    KB_F10,                 
    KB_F11,                 
    KB_F12,                 

    KB_PRINT,               
    KB_SCROLLLOCK,          
    KB_PAUSE,               

    KB_HOME,                
    KB_END,                 
    KB_PAGEUP,              
    KB_PAGEDOWN,            
    KB_INSERT,              
    KB_DELETE,              

    KB_RIGHT,               
    KB_LEFT,                
    KB_DOWN,                
    KB_UP,                  

    KB_NUMLOCK,             
    KB_DIVIDE,              
    KB_MULTIPLY,            
    KB_SUBTRACT,            
    KB_ADD,                 

    KB_NUMPADENTER,         

    KB_NUMPAD1, 
    KB_NUMPAD2, 
    KB_NUMPAD3, 
    KB_NUMPAD4, 
    KB_NUMPAD5,
    KB_NUMPAD6, 
    KB_NUMPAD7, 
    KB_NUMPAD8, 
    KB_NUMPAD9, 
    KB_NUMPAD0, 
    KB_DECIMAL, 

    KB_ACCESS, 

    KB_NUM_AXES
};

static_assert(KB_V == 22);
static_assert(KB_TILDE == 0x39);
//static_assert(KB_BACKSPACE == );
static_assert(KB_TAB == 0x30);

template<int I>
struct A; 

template<>
struct A<1>;

constexpr auto f10 = (int)KB_F10;
//A<KB_F10> a{};
#pragma once

#include "input_device.h"
#include "mstring.h"

struct keyboard_device : input_device {

    int field_8;

    keyboard_device();

    //virtual
    mString get_name() const;

    void clear();

    void initialize(int a2);

    //virtual
    void _clear_state();

    //virtual
    float _get_axis_state(int axis, int );

    float _get_axis_old_state(int a2, int a3);

    float _get_axis_delta(int axis, int a3);

    void _poll();

    bool _is_connected() const;

    bool _is_vibrator_present() const;

    static keyboard_device *instance;
};
#pragma once

namespace _std {
    template<typename T>
    struct list {
        int field_0;

        struct node_type {
            node_type *_Next;
            node_type *_Prev;
            T _Myval;
        };

        node_type *m_head;
        uint32_t m_size;

        struct iterator {
            node_type *_Ptr;

            auto &operator*() {
                return (_Ptr->_Myval);
            }

            bool operator==(const iterator &it) const {
                return _Ptr == it._Ptr;
            }

            bool operator!=(const iterator &it) const {
                return _Ptr != it._Ptr;
            }

            void operator++() {
                this->_Ptr = this->_Ptr->_Next;
            }
        };

        auto begin() {
            return iterator {m_head->_Next}; 
        }

        auto end() {
            return iterator {m_head};
        }

    };
}
#pragma once

#include <cstdint>
#include <cassert>

template<typename T>
struct mashable_vector
{
    using value_t = T;

    T *m_data;
    uint16_t m_size;
    bool m_shared;
    bool field_7;

    auto size() const
    {
        return m_size;
    }

    inline auto &at(int index) {
        return m_data[index];
    }

    inline auto &at(int index) const {
        return m_data[index];
    }

    struct iterator {
        T *m_ptr;

        bool operator==(const iterator &it) {
            return (this->m_ptr == it.m_ptr);
        }

        bool operator!=(const iterator &it) {
            return (this->m_ptr != it.m_ptr);
        }

        auto &operator*() {
            return (*m_ptr);
        }

        auto &operator++() {
            return (++m_ptr);
        }
    };

    iterator begin() {
        return iterator{&m_data[0]};
    }

    iterator end()
    {
        if (this->m_data != nullptr) {
            return iterator{&this->m_data[this->m_size]};
        }

        return {};
    }
};
#pragma once

#include "debug_menu.h"
#include "script_memtrack.h"
#include "slab_allocator.h"

int g_mem_checkpoint_debug_0 {-1};

void set_memtrack_checkpoint(debug_menu_entry *)
{
    g_mem_checkpoint_debug_0 = mem_set_checkpoint();
    debug_menu::hide();
}

void dump_memtrack_data(debug_menu_entry *)
{
    mem_check_leaks_since_checkpoint(g_mem_checkpoint_debug_0, 1u);
    mem_print_stats("\nMemory log\n");
    debug_menu::hide();
}

void create_memory_menu(debug_menu *parent)
{
    auto *memory_menu = create_menu("Memory", debug_menu::sort_mode_t::undefined);
    auto *v2 = create_menu_entry(memory_menu);
    parent->add_entry(v2);
    
    script_memtrack::create_debug_menu(memory_menu);

    auto *entry = create_menu_entry(mString {"Dump MemTrack Data Since Last Checkpoint"});
    entry->set_game_flags_handler(dump_memtrack_data);
    memory_menu->add_entry(entry);

    entry = create_menu_entry(mString {"Set MemTrack Checkpoint"});
    entry->set_game_flags_handler(set_memtrack_checkpoint);
    memory_menu->add_entry(entry);
    
    slab_allocator::create_slab_debug_menu(memory_menu);
}
#include "message_board.h"

#include "fetext.h"
#include "trace.h"
#include "mstring.h"
#include "utility.h"
#include "vector2di.h"

#include <cstring>

void message_board::post(string a1, Float a2, color32 a3)
{
    TRACE("message_board::post");

    auto &v10 = this->field_0;
    printf("%d\n", v10.size());

    uint32_t i;
    for (i = 0; i < v10.size() && v10.at(i).field_64 != 0.0; ++i) {
        ;
    }

    internal v1;

    strncpy(v1.field_0, a1.guts, 99);
    v1.field_64 = a2;
    v1.field_68 = a3;

    if (i == v10.size()) {
        v10.push_back(v1);
    } else {
        v10.at(i) = v1;
    }

    {
        void (__fastcall *finalize)(string *, void *, int) = CAST(finalize, 0x004209C0);
        finalize(&a1, nullptr, 0);
    }
}

void message_board::frame_advance(float a2)
{
    TRACE("message_board::frame_advance");

    for ( uint32_t i = 0; i < this->field_0.size(); ++i )
    {
        auto &v3 = this->field_0[i].field_64;
        v3 = v3 - a2;
        if ( this->field_0[i].field_64 < 0.0 )
        {
            this->field_0[i].field_64 = 0.0;
        }
    }
}

void message_board::render()
{
    TRACE("message_board::render");
    if constexpr (1)
    {
        auto &v15 = this->field_0;

        int a3 = 390;
        for (auto i = 0u; i < v15.size(); ++i)
        {
            if (v15.at(i).field_64 > 0.0)
            {
                auto v12 = v15.at(i).field_68;

                auto v1 = v15.at(i).field_64 + 0.25;
                float v7 = (v1 >= 1.0 ? 1.0 : v1);

                uint8_t v2 = (v7 * 255.0);
                v12.set_alpha(v2);
                float v10 = 0.75;

                mString v9 {v15.at(i).field_0};

                auto v6 = v10;
                auto v5 = v12;
                vector2di v4 {32, a3};
                render_text(v9, v4, v5, 1.0, v6);
            }

            a3 -= 12;
        }
    }
}

void message_board_patch()
{
    {
        auto address = func_address(&message_board::post);
        SET_JUMP(0x00515EB0, address);
    }
}
#pragma once

#include "color32.h"
#include "float.hpp"

#include <vector>

struct message_board {
    struct internal {
        char field_0[100];
        float field_64;
        color32 field_68 {};
    };

    std::vector<internal> field_0;

    message_board() = default;

    struct string {
        int field_0;
        int field_4;
        char *guts;
        void *field_C;
    };

    void post(string a1, Float a2, color32 a3);

    void frame_advance(float a2);

    void render();
};

extern void message_board_patch();
#pragma once

#include "variable.h"
#include "mission_table_container.h"
#include "mission_manager_script_data.h"

#include <cstdio>

struct mission_manager {
    int field_0;
    int field_4;
    int field_8;
    int field_C;
    int field_10;
    mission_table_container *m_global_table_container;
    mission_table_container *m_district_table_containers[1];
    int field_1C;
    int field_20;
    int field_24;
    int field_28;
    int field_2C;
    int field_30;
    int field_34;
    int m_district_table_count;
    mission_manager_script_data *m_script_to_load;
    mission_manager_script_data *m_script;
    int field_44[3];
    bool m_unload_script;
    int field_54;
    int field_58;
    int field_5C;
    float *field_60;
    float field_64;
    int field_68;
    float *field_6C;
    int field_70;
    int field_74;
    float *field_78;
    float *field_7C;
    bool field_80;
    int field_84;
    fixedstring32 field_88;
    fixedstring32 field_A8;
    int field_C8;
    bool field_CC;
    fixedstring32 field_D0;
    int hero_switch_frame;
    float field_F4;
    float field_F8;
    int field_FC;

    //0x005DA010
    mission_manager();

    int get_district_table_count() const
    {
        return this->m_district_table_count;
    }

    mission_table_container *get_district_table(int a2)
    {
        return this->m_district_table_containers[a2];
    }

    mission_table_container *get_global_table()
    {
        return this->m_global_table_container;
    }

    void prepare_unload_script()
    {
        if ( this->m_script != nullptr )
        {
            if ( !this->m_unload_script )
            {
                auto *v1 = this->m_script->field_0.c_str();
                printf("Preparing to unload script %s\n", v1);
                this->m_unload_script = true;
                this->field_80 = false;
            }
        }
    }

    void force_mission(int a2, const char *a3, int a4, const char *a5)
    {
        this->prepare_unload_script();
        this->field_80 = true;
        this->field_84 = a2;

        fixedstring32 v5 {a3};
        this->field_88 = v5;
        this->field_C8 = a4;
        if ( a5 != nullptr )
        {
            fixedstring32 v6{a5};
            this->field_A8 = v6;
        }
        else
        {
            fixedstring32 v7 {""};
            this->field_A8 = v7;
        } 
    }

    static inline Var<mission_manager *> s_inst{0x00968518};
};

#pragma once

#include "mstring.h"

struct mission_manager_script_data {
    mString field_0;
    int field_10[39];

    bool uses_script_stack;
    int filed_B0;
    int field_B4;
    mString field_B8;
    mString field_C8;
    mString field_D8;
};

#pragma once

#include "common.h"
#include "func_wrapper.h"
#include "mashable_vector.h"
#include "wds.h"
#include "actor.h"
#include "ai_player_controller.h"

struct region;

struct mission_condition_instance
{
    char field_0[0x18];
    const char *script_data_name;
    char field_1C[0x38];
    unsigned int field_54;
    mashable_vector<int> field_58;
    int field_60;
    int field_64;

    bool is_flag_set(unsigned int a2)
    {
        return (a2 & this->field_54) != 0;
    }

    const char *get_script_data_name()
    {
        if ( !this->is_flag_set(8) )
        {
            return nullptr;
        }

        //assert(script_data_name != nullptr);

        return this->script_data_name;
    }
};

VALIDATE_SIZE(mission_condition_instance, 0x68);

struct mission_condition
{
    int field_0;
    int field_4;
    mashable_vector<mission_condition_instance> instances;
    mashable_vector<int> field_10;
    const char *field_18;
    const char *field_1C;
    unsigned char field_20;
    char field_21[3];
    char field_24;
    char field_25[3];
    int field_28;
    const char *field_2C;
    const char *field_30;

    bool is_flag_set(unsigned int a2) const
    {
        return (a2 & this->field_28) != 0;
    }

    bool applies_to_current_hero()
    {
        assert(g_world_ptr()->get_hero_ptr( 0 ) != nullptr);

        assert(bit_cast<actor*>(g_world_ptr()->get_hero_ptr( 0 ))->get_player_controller() != nullptr);

        auto *v8 = bit_cast<actor*>(g_world_ptr()->get_hero_ptr( 0 ))->get_player_controller();
        auto v15 = v8->field_420;
        if ( v15 == 1 )
        {
            if ( this->is_flag_set(0x8000) )
            {
                return true;
            }
        }
        else if ( v15 == 2 )
        {
            if ( this->is_flag_set(0x4000) )
            {
                return true;
            }
        }
        else if ( v15 == 3 && this->is_flag_set(0x10000) )
        {
            return true;
        }

        return false;
    }
};

VALIDATE_SIZE(mission_condition, 0x34);

struct mission_table_container {
    char field_0[0x38];
    mashable_vector<mission_condition> field_38;
    int field_40;
    region *field_44;

    struct script_info {
        const char *field_0;
        mission_condition_instance *field_4;
        int field_8;
    };

    region *get_region()
    {
        return this->field_44;
    }

    bool append_script_info(std::vector<mission_table_container::script_info> *info)
    {
        assert(info != nullptr);

        bool v9 = false;
        for ( auto &v6 : this->field_38 )
        {
            if ( v6.applies_to_current_hero() )
            {
                auto v5 = v6.instances.size();
                for ( auto i = 0u; i < v5; ++i )
                {
                    script_info v3;
                    v3.field_0 = v6.field_18;
                    v3.field_4 = &v6.instances.at(i);
                    v3.field_8 = i;
                    info->push_back(v3);
                    v9 = true;
                }
            }
        }

        return v9;
    }
};
#include "mouselook_controller.h"

#include "game.h"
#include "trace.h"
#include "utility.h"
#include "variable.h"

void mouselook_controller::_frame_advance(Float a2)
{
    TRACE("mouselook_controller::frame_advance");

    //dirty hack
    {
        static Var<actor *> g_debug_cam_target_actor {0x0095C758};
        g_debug_cam_target_actor() = bit_cast<actor *>(g_world_ptr()->get_hero_ptr(0));
    }

    void (__fastcall *func)(void *, void *, Float) = CAST(func, 0x00528BB0);
    func(this, nullptr, a2);
}

void mouselook_controller_patch()
{
    {
        auto address = func_address(&mouselook_controller::_frame_advance);
        set_vfunc(0x00889118, address);
    }
}
#pragma once

#include "float.hpp"

struct mouselook_controller {

    void _frame_advance(Float a2);

};

extern void mouselook_controller_patch();

#pragma once

template<typename T>
struct mVector {
    int field_0;
    int m_size;
    T **m_data;
    int field_C;
    bool field_10;

    struct iterator {
        T **_Ptr;

        bool operator==(const iterator &it) const
        {
            return _Ptr == it._Ptr;
        }

        bool operator!=(const iterator &it) const
        {
            return _Ptr != it._Ptr;
        }

        void operator++()
        {
            ++this->_Ptr;
        }

        auto &operator*()
        {
            return (*this->_Ptr);
        }

        auto &operator*() const
        {
            return (*this->_Ptr);
        }
    };

    auto begin()
    {
        if (this->m_data != nullptr)
        {
            return iterator {this->m_data};
        }

        return iterator {nullptr};
    }

    auto begin() const
    {
        if (this->m_data != nullptr)
        {
            return iterator {this->m_data};
        }

        return iterator {nullptr};
    }

    auto end()
    {
        if (this->m_data != nullptr)
        {
            return iterator {&this->m_data[this->m_size]};
        }

        return iterator {nullptr};
    }

    auto end() const
    {
        if (this->m_data != nullptr)
        {
            return iterator {&this->m_data[this->m_size]};
        }

        return iterator {nullptr};
    }
};
#pragma once

#include "common.h"
#include "fixedstring.h"

struct nalAnyPose {};

template<typename T>
struct nalAnimClass {
    struct nalInstanceClass {};

    std::intptr_t m_vtbl;
    nalAnimClass<nalAnyPose> *field_4;
    tlFixedString field_8;
    int field_28;
    int field_2C;
    void *Skeleton;
    int field_34;
    int field_38;
    int field_3C;
};

struct nalAnimFile {
    uint32_t field_0;
    char field_4[0x30];
    void *field_34;
};

VALIDATE_OFFSET(nalAnimFile, field_34, 0x34);
#pragma once

#include "utility.h"
#include "vector4d.h"

#include <cstdint>

namespace math {

template<bool>
struct Rep_Std {};

template<int Int0,
         int Int1,
         typename T0 = void,
         typename T1 = void,
         typename T2 = Rep_Std<false>>
struct VecClass {
    float field_0[4];

    auto &operator[](uint32_t idx)
    {
        return this->field_0[idx];
    }

    auto &operator[](uint32_t idx) const
    {
        return this->field_0[idx];
    }

    void sub_413530(const VecClass<Int0, Int1, T0, T1, T2> &a2, const float &a3)
    {
        auto v3 = a3;
        this->field_0[0] += v3 * a2[0];
        this->field_0[1] += v3 * a2[1];
        this->field_0[2] += v3 * a2[2];
        this->field_0[3] += v3 * a2[3];
    }

    void sub_411A50(const math::VecClass<3, 0> &a2, const math::VecClass<3, 0> &a3)
    {
        this->field_0[0] += a3[1] * a2[0];
        this->field_0[1] += a2[1] * a3[1];
        this->field_0[2] += a2[2] * a3[1];
        this->field_0[3] += a2[3] * a3[1];
    }

};

template<uint32_t, uint32_t>
struct MatClass {
    vector4d arr[4];

    vector4d GetX() const {
        return this->arr[0];
    }

    vector4d GetY() const {
        return this->arr[1];
    }

    vector4d GetZ() const {
        return this->arr[2];
    }

    void sub_4134B0(
            VecClass<3, 0> &a2,
            VecClass<3, 0> &a3,
            VecClass<3, 0> &a4,
            VecClass<3, 0> &a5) const
    {
        a2 = CAST(a2, this->arr[0]);
        a3 = CAST(a3, this->arr[1]);
        a4 = CAST(a4, this->arr[2]);
        a5 = CAST(a5, this->arr[3]);
    }
};

}
#pragma once

#include "entity_base.h"
#include "entity_base_vhandle.h"
#include "mstring.h"
#include "string_hash.h"
#include "vector3d.h"

#include <cassert>

template<typename T>
struct variance_variable {
    T field_0;
    T field_4;

    variance_variable() {
        this->field_4 = 0;
        this->field_0 = 0;
    }
};

namespace ai {

enum param_types {
    PT_FLOAT = 0,
    PT_INTEGER = 1,
    PT_STRING_HASH = 2,
    PT_FIXED_STRING = 3,
    PT_VECTOR_3D = 4,
    PT_FLOAT_VARIANCE = 5,
    PT_ENTITY = 6,
    PT_POINTER = 7,
};


struct param_block {

    struct param_data {
        union U {
            int i;
            float f;
            string_hash hash;
            char *str;
            vector3d *vec3;
            variance_variable<float> *float_variance;
            entity_base_vhandle *ent;
        } m_union;
        param_types my_type;
        string_hash field_8;

        auto get_data_type() const {
            return this->my_type;
        }

        string_hash get_data_hash() const {
            return this->m_union.hash;
        }

        float & get_data_float() {
            assert(my_type == PT_FLOAT);
            return m_union.f;
        }

        int & get_data_int() {
            assert(my_type == PT_INTEGER);
            return m_union.i;
        }

        const char * get_data_string()
        {
            assert(my_type == PT_FIXED_STRING);

            return this->m_union.str;
        }

        vector3d * get_data_vector3d()
        {
            assert(my_type == PT_VECTOR_3D);
            return this->m_union.vec3;
        }

        auto &get_data_float_variance() {
            assert(my_type == PT_FLOAT_VARIANCE);
            return m_union.float_variance;
        }

        entity_base_vhandle * get_data_entity()
        {
            assert(my_type == PT_ENTITY);

            return this->m_union.ent;
        }

        mString get_value_in_string_form()
        {
            mString result {};
            switch ( this->my_type )
            {
            case PT_FLOAT: {
                auto data_float = this->get_data_float();
                result = mString {0, "%.1f", data_float};
                break;
            }
            case PT_INTEGER: {
                auto data_int = this->get_data_int();
                result = mString {0, "%d", data_int};
                break;
            }
            case PT_STRING_HASH: {
                auto v22 = this->get_data_hash();
                auto *v4 = v22.to_string();
                result = mString {v4};
                break;
            }
            case PT_FIXED_STRING: {
                auto *v5 = this->get_data_string();
                result = mString {v5};
                break;
            }
            case PT_VECTOR_3D: {
                auto v17 = this->get_data_vector3d()->z;
                auto v16 = this->get_data_vector3d()->y;
                auto v6 =  this->get_data_vector3d()->x;
                result = mString {0, "%.1f %.1f %.1f", v6, v16, v17};
                break;
            }
            case PT_FLOAT_VARIANCE: {
                auto v7 = *this->get_data_float_variance();
                auto v18 = v7.field_4;
                auto v9 = v7.field_0;
                result = mString {0, "%.1f %.1f", v9, v18};
                break;
            }
            case PT_ENTITY: {
                mString v14 {};
                auto *v10 = this->get_data_entity();
                if ( v10->get_volatile_ptr() != nullptr )
                {
                    auto *ent = v10->get_volatile_ptr();
                    auto id = ent->get_id();
                    auto *v27 = id.to_string();
                    v14 = mString {0, "%s", v27};
                }
                else
                {
                    v14 = mString {0, "%s", "<NULL>"};
                }

                result = v14;
                break;
            }
            case PT_POINTER: {
                result = mString {"<NULL>"};
                break;
            }
            default:
                assert(0 && "Unhandled AI::param_block::param_data::param_type type!");
                return result;
            }

            return result;
        }
    };

    struct param_data_array {
        mVector<param_block::param_data> field_0;
        int field_14;
    };

    int my_curr_mode;
    param_data_array *param_array;
    bool field_8;
};

}
#pragma once

#include "vector3d.h"

struct physical_interface 
{
    vector3d get_velocity()
    {
        vector3d result;
        void (__fastcall *func)(void *, void *, vector3d *) = bit_cast<decltype(func)>(0x004CA0D0);
        func(this, 0, &result);

        return result;
    }
};

#pragma once

#include "bit.h"
#include "vector3d.h"

struct po
{
    float m[4][4];

    vector3d &get_z_facing() const {
        return *bit_cast<vector3d *>(&this->m[2]);
    }
};
#pragma once

#include "common.h"
#include "region_mash_info.h"

struct region {
    char field_0[0x2C];
    region_mash_info *mash_info;
    void *region_entities;
    int field_34;
    int field_38;
    int field_3C;
    int field_40;
    int field_44;
    int field_48;
    int field_4C;
	int flags;
	uint8_t unk1[0x50];
    vector3d field_A4;
    float field_B0[3];
    float field_BC;
	DWORD district_id;
	uint8_t unk3[0x4];
	uint8_t variants;
	uint8_t unk4[0x6B];

    fixedstring32 &get_name() {
        return this->mash_info->field_0;
    }

    bool is_locked() const
    {
        return this->flags & 1;
    }

    int get_district_variant_count() const
    {
        return bit_cast<int *>(bit_cast<char *>(this) + 0xC8)[0];
    }

    int get_district_variant() {
        int (__fastcall *func)(void *) = bit_cast<decltype(func)>(0x005503D0);
        return func(this);
    }

    int get_district_id() {
        constexpr auto REGION_UNINITIALIZED_DISTRICT_ID = 0;

        assert(district_id != REGION_UNINITIALIZED_DISTRICT_ID && "dsg and sin probably out of sync");

        return this->district_id;
    }
};

VALIDATE_OFFSET(region, mash_info, 0x2C);
VALIDATE_OFFSET(region, flags, 0x50);
VALIDATE_OFFSET(region, district_id, 0xC0);
VALIDATE_SIZE(region, 0x134);
#pragma once

#include "fixedstring32.h"
#include "color.h"

struct region_mash_info {
    fixedstring32 field_0;
    color field_20;

    int field_30;
    int field_34;
    int field_38;
    int field_3C;

    //0x005C56D0
    region_mash_info();
};

#pragma once

#include <cstdint>

#include "bit.h"
#include "tlresource_location.h"

struct resource_key;
struct resource_pack_slot;

struct resource_directory
{
    uint8_t *get_resource(const resource_key &resource_id, int *a3, resource_pack_slot **a4)
    {
        uint8_t * (__fastcall *func)(void *, void *, const resource_key *, int *, resource_pack_slot **) = bit_cast<decltype(func)>(0x0052AA70);
        return func(this, 0, &resource_id, a3, a4);
    }

    int get_tlresource_count(tlresource_type a1) {
        int (__fastcall *func)(void *, int, tlresource_type) = bit_cast<decltype(func)>(0x00563070);
        return func(this, 0, a1);
    }

    
    tlresource_location *get_tlresource_location(int idx, tlresource_type a2) {
        tlresource_location * (__fastcall *func)(void *, int, int, tlresource_type) = bit_cast<decltype(func)>(0x00563090);
        return func(this, 0, idx, a2);
    }
};
#pragma once

struct resource_directory;

struct resource_pack_directory
{
    resource_directory *field_0;

    resource_directory *get_resource_directory() {
        return this->field_0;
    }
};
#pragma once

#include "common.h"
#include "resource_key.h"

struct resource_location
{
    resource_key field_0;
    int filed_8;
    int m_size;
};

struct resource_pack_location 
{
    resource_location loc;
    int field_10[7];
    int field_2C;
    char field_30[32];
};

VALIDATE_SIZE(resource_pack_location, 0x50);
VALIDATE_OFFSET(resource_pack_location, field_2C, 0x2C);
#pragma once

#include <cassert>

#include "common.h"
#include "resource_pack_directory.h"
#include "resource_directory.h"

struct resource_key;

struct resource_pack_slot
{
    int field_0;
    resource_key field_4;
    int m_slot_state;
    int slot_size;
    int pack_size;
    uint8_t *header_mem_addr;
    resource_pack_directory pack_directory;

    resource_directory &get_resource_directory() {
        //assert(is_data_ready());
        //assert(pack_directory.get_resource_directory() != nullptr);

        return (*this->pack_directory.get_resource_directory());
    }

    uint8_t *get_resource(const resource_key &resource_id,
                                          int *a3,
                                          resource_pack_slot **a4) {
        auto &res_dir = this->get_resource_directory();
        return res_dir.get_resource(resource_id, a3, a4);
    }
};

VALIDATE_OFFSET(resource_pack_slot, pack_directory, 0x1C);
#pragma once

#include "resource_pack_slot.h"

struct resource_partition
{
    int field_0;
    int field_4;

    struct {
        int field_0;
        resource_pack_slot **m_first;
        resource_pack_slot **m_last;
        resource_pack_slot **m_end;

        struct iterator
        {
            resource_pack_slot **m_ptr;

            auto &operator*() const
            {
                return (*m_ptr);
            }
        };

        iterator begin()
        {
            return iterator{m_first};
        }

        iterator end()
        {
            return iterator{m_last};
        }

        size_t size() const
        {
            return (m_first == nullptr ? 0 : m_last - m_first);
        }

        auto &front() {
            return (*begin());
        }

    } m_pack_slots;

    auto &get_pack_slots() {
        return this->m_pack_slots;
    }
};
#pragma once

#include "bit.h"

struct script_object;

struct script_executable {

    char field_0[0x28];
    script_object **field_28;
    script_object **script_objects_by_name;
    int total_script_objects;

    void add_allocated_stuff(int a2, int a3, int a4) {
        void  (__fastcall *func)(script_executable *, void *edx, int a2, int a3, int a4) = bit_cast<decltype(func)>(0x005A34B0);
        func(this, nullptr, a2, a3, a4);
    }

};

#pragma once

#include "common.h"
#include "string_hash.h"
#include "utility.h"

struct script_object;

struct vm_executable;
struct vm_thread;

struct script_instance {
	uint8_t unk[0x2C];
	script_object *m_parent;

    auto * get_parent() {
        return m_parent;
    }

    vm_thread * add_thread(const vm_executable *a2)
    {
        vm_thread * (__fastcall *func)(void *, void *, const vm_executable *) = CAST(func, 0x005AAC20);
        return func(this, nullptr, a2);
    }

    void add_thread(const vm_executable *a2, const char *a3)
    {
        void (__fastcall *func)(void *, void *, const vm_executable *, const char *) = CAST(func, 0x005AAD00);
        func(this, nullptr, a2, a3);
    }
};

#pragma once

namespace script_library_class {
    struct function {
        int m_vtbl;
    };
}
#pragma once

#include "debug_menu.h"
#include "entity_base_vhandle.h"
#include "mstring.h"

#include <map>

namespace script_memtrack {

struct entity_class_t {
    mString field_0;
    int field_C;
    std::list<std::pair<entity_base_vhandle, int>> field_10;
};

static std::string current_class_name {}; 

static int current_heap_usage {0};

static std::map<entity_base_vhandle, script_memtrack::entity_class_t *> handle_to_class_map {};

static std::map<mString, script_memtrack::entity_class_t *> name_to_class_map {};

static debug_menu *script_memtrack_debug_menu {nullptr};

void dump_info(debug_menu_entry *)
{
    printf("script_memtrack::dump_info\n");

    for ( auto it = name_to_class_map.begin(), end = name_to_class_map.end();
            it != end; ) {
        auto *cls = it->second;
        if ( !cls->field_10.empty() ) {
            auto v5 = cls->field_C;
            auto v4 = cls->field_10.size();
            auto *v1 = cls->field_0.c_str();
            printf("%s, %d instances, %d bytes\n", v1, v4, v5);

            for ( auto &p : cls->field_10 ) {
                auto v15 = p.first;
                auto *ent = (entity *) v15.get_volatile_ptr();
                if ( ent != nullptr ) {
                    auto v6 = p.second;
                    auto id = ent->get_id();
                    auto *v3 = id.to_string();
                    printf("    %s, %d bytes\n", v3, v6);
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

}
#pragma once

#include "bit.h"
#include "common.h"
#include "string_hash.h"

struct vm_executable;

struct so_data_block {
    char field_0[0xC];
};

VALIDATE_SIZE(so_data_block, 0xC);

struct script_executable;

struct script_object {
    string_hash name;
    script_executable *parent;
    script_instance *global_instance;
    void *debug_info;
    so_data_block static_data;
    int data_blocksize;
    vm_executable **funcs;
    int total_funcs;
    int field_28;
    void *instances;
    int flags;

    int find_func(string_hash a2)
    {
        int (__fastcall *func)(void *, void *, string_hash) = CAST(func, 0x0058EF80);
        return func(this, nullptr, a2); 
    }

    vm_executable * get_func(int i)
    {
        assert(funcs);
        assert(i >= 0);
        assert(i < total_funcs);
        return this->funcs[i];
    }
};

VALIDATE_SIZE(script_object, 0x34);
#pragma once

#include "entity_base.h"

#include "common.h"

struct signaller : entity_base {
    int field_44;
};

VALIDATE_SIZE(signaller, 0x48u);
#pragma once

#include "func_wrapper.h"
#include "game.h"
#include "message_board.h"
#include "os_developer_options.h"

inline Var<bool> god_mode_cheat {0x0095A6A8};
inline Var<bool> ultra_god_mode_cheat {0x0095A6A9};
inline Var<bool> mega_god_mode_cheat {0x0095A6AA};

namespace spider_monkey
{
    inline void start()
    {
        CDECL_CALL(0x004B6690);
    }

    inline bool is_running()
    {
        return (bool) CDECL_CALL(0x004B3B60);
    }

    inline void on_level_load()
    {
        CDECL_CALL(0x004B3910);
    }

    inline void stop()
    {
        CDECL_CALL(0x004B6700);
    }

    inline void on_level_unload()
    {
        CDECL_CALL(0x004B3B20);
    }

    inline void render()
    {
        CDECL_CALL(0x004B6890);

        {
            g_game_ptr()->mb->render();
        }

        {
            if ( os_developer_options::instance()->get_flag(mString{"SHOW_DEBUG_INFO"}) )
            {
                g_game_ptr()->show_debug_info();
            }

            if ( os_developer_options::instance()->get_flag(mString{"SHOW_FPS"}) )
            {
                auto v40 = (g_game_ptr()->field_278 == 0 ? 0.f : (1.f / g_game_ptr()->field_278));
                auto v39 = (
                        ((g_game_ptr()->field_278 - g_game_ptr()->field_27C) - g_game_ptr()->field_280) >= 0.000099999997
                        ? (1.0 / (g_game_ptr()->field_278 - g_game_ptr()->field_27C - g_game_ptr()->field_280))
                        : 10000.0f);

                static float dword_1584E70 = 0;
                if ( v39 > dword_1584E70 )
                {
                    dword_1584E70 = v39;
                }

                static float dword_14EEB94 = 500;
                if ( dword_14EEB94 > v39 )
                {
                    dword_14EEB94 = v39;
                }

                color32 v38 {0};
                if ( v40 >= 14.2 )
                {
                    if ( v40 >= 29.4 )
                    {
                        if ( v40 >= 59.900002 )
                        {
                            color32 v5 {80, 255, 80, 255};
                            v38 = v5;
                        }
                        else
                        {
                            color32 v4 {255, 255, 32, 255};
                            v38 = v4;
                        }
                    }
                    else
                    {
                        color32 v3 {255, 128, 40, 255};
                        v38 = v3;
                    }
                }
                else
                {
                    color32 v2 {255, 33, 44, 255};
                    v38 = v2;
                }

                char a1a[200]{};
                sprintf(a1a, "%2d fps (%2d.%1d)", (int)v40, (int)v39, (int)(float)(v39 * 10.0) % 10);
                auto a5 = ((int)v38.field_0[0] >> 4) | ((int)v38.field_0[1] >> 4 << 8) | ((int)v38.field_0[2] >> 4 << 16) | 0xFF000000;
                nglListAddString(nglSysFont(), 19.0, 424.0, 1.01, a5, 0.80000001, 0.80000001, a1a);
                nglListAddString(nglSysFont(), 21.0, 426.0, 1.01, a5, 0.80000001, 0.80000001, a1a);
                nglListAddString(
                    nglSysFont(),
                    20.0,
                    425.0,
                    1.0,
                    v38.field_0[0] | (v38.field_0[1] << 8) | (v38.field_0[2] << 16) | 0xFF000000,
                    0.80000001,
                    0.80000001,
                    a1a);

                if ( god_mode_cheat() )
                {
                    if ( ultra_god_mode_cheat() )
                    {
                        mString v16 {"Ultra-God mode"};
                        auto v11 = v38;
                        vector2di v6 {20, 110};
                        render_text(v16, v6, v11, 1.0, 0.5);
                    }
                    else if ( mega_god_mode_cheat() )
                    {
                        mString v17 {"Mega-God mode"};
                        auto v12 = v38;
                        vector2di v7 {20, 110};
                        render_text(v17, v7, v12, 1.0, 0.5);
                    }
                    else
                    {
                        mString v18 {"God mode"};
                        auto v13 = v38;
                        vector2di v8 {20, 110};
                        render_text(v18, v8, v13, 1.0, 0.5);
                    }
                }
                else if ( ultra_god_mode_cheat() )
                {
                    mString v19 {"Ultra-Mortal mode"};
                    auto v14 = v38;
                    vector2di v9 {20, 110};
                    render_text(v19, v9, v14, 1.0, 0.5);
                }
                else if ( mega_god_mode_cheat() )
                {
                    mString v20 {"Mega-Mortal mode"};
                    auto v15 = v38;
                    vector2di v10 {20, 110};
                    render_text(v20, v10, v15, 1.0, 0.5);
                }
            }
        }
    }
}

inline void spider_monkey_patch()
{
    {
        REDIRECT(0x0052B4BF, &spider_monkey::render);
    }

}
#pragma once

#include "bit.h"

struct string_hash
{
    unsigned int source_hash_code;

    string_hash() = default;

    string_hash(const char *a1)
    {
        void (__fastcall *func)(void *, int idx, const char *) = bit_cast<decltype(func)>(0x00401960);
        func(this, 0, a1);
    }

    const char *to_string() const
    {
        const char * (__fastcall *func)(const void *) = (decltype(func)) 0x005374B0;
        return func(this);
    }

    void initialize(int a2, const char *a3, int a4)
    {
        typedef void(__fastcall* string_hash_initialize_ptr)(string_hash* , void* edx, int a2, const char* Str1, int a4);
        string_hash_initialize_ptr string_hash_initialize = (string_hash_initialize_ptr) 0x00547A00;
        string_hash_initialize(this, nullptr, a2, a3, a4);
    }


};


#pragma once

#include <cstdint>

struct tlFileBuf {
    char *Buf;
    uint32_t Size;
    uint32_t UserData;
};

#pragma once

#include "string_hash.h"

enum tlresource_type {
    TLRESOURCE_TYPE_NONE = 0,
    TLRESOURCE_TYPE_TEXTURE = 1,
    TLRESOURCE_TYPE_MESH_FILE = 2,
    TLRESOURCE_TYPE_MESH = 3,
    TLRESOURCE_TYPE_MORPH_FILE = 4,
    TLRESOURCE_TYPE_MORPH = 5,
    TLRESOURCE_TYPE_MATERIAL_FILE = 6,
    TLRESOURCE_TYPE_MATERIAL = 7,
    TLRESOURCE_TYPE_ANIM_FILE = 8,
    TLRESOURCE_TYPE_ANIM = 9,
    TLRESOURCE_TYPE_SCENE_ANIM = 10,
    TLRESOURCE_TYPE_SKELETON = 11,
    TLRESOURCE_TYPE_Z = 12
};

struct tlresource_location {
    string_hash name;
    uint32_t m_type;
    char *field_8;
};
#pragma once

#include <string>
#include <iostream>
#include <tuple>

struct trace_obj                                                                   
{                                                                               
    std::string m_func_name;
    static inline int m_indent{0};

    trace_obj(std::string func_name) : m_func_name(func_name)
    {
        std::cout << std::string(m_indent, '\t') << "--> " << m_func_name;
        ++m_indent;
    }

    ~trace_obj()
    {
        --m_indent;
        std::cout << std::string(m_indent, '\t') << "<-- " << m_func_name << '\n';
    }
};                                                                              



#if 1 

#define TRACE(func_name, ...)                                                       \
    trace_obj tr{func_name};                                                        \
                                                                                    \
    [](auto&&... args)                                                              \
    {                                                                               \
       const     auto t = std::make_tuple(std::forward<decltype(args)>(args)...);   \
       constexpr auto N = std::tuple_size<decltype(t)>::value;                      \
                                                                                    \
       if constexpr ( N != 0 ) {                                                     \
           ((std::cout << ' ' << std::forward<decltype(args)>(args)), ...);           \
       }                                                                            \
                                                                                    \
        std::cout << '\n';                                                          \
                                                                                    \
    }(__VA_ARGS__)


#else
#define TRACE(...) {} 
#endif

#pragma once

#include <cstdint>

#include "bit.h"

template<typename Func>
void set_vfunc(std::size_t address, Func func) {
    (*bit_cast<std::uint32_t *>(address)) = (bit_cast<std::uint32_t>(func));
}

template<typename Func>
void SET_JUMP(std::ptrdiff_t addr, Func my_func) {
    *bit_cast<uint8_t *>(addr) = 0xE9; //JUMP

    *bit_cast<uint32_t *>(bit_cast<uint8_t *>(addr + 1)) = (bit_cast<uint32_t>(my_func)) - addr - 5;

    *bit_cast<uint8_t *>(addr + 0x5) = 0xC3; //RET
};

template<typename Func>
void REDIRECT(std::ptrdiff_t addr, Func my_func) {
    *bit_cast<uint8_t *>(addr) = 0xE8; //CALL
    *bit_cast<uint32_t *>(
        bit_cast<uint8_t *>(addr + 1)) = (bit_cast<uint32_t>(my_func)) - addr - 5;
    //sp_log("Patched address %08X with %s", addr, #my_func);
}

template<typename Func, typename = typename std::enable_if_t<std::is_member_function_pointer_v<Func>>>
void *func_address(Func func) {
    auto result = reinterpret_cast<void *&>(func);

    return result;
}

template<typename T0, typename T1>
decltype(auto) CAST([[maybe_unused]] const T0 &var, T1 address) {
    return bit_cast<T0>(address);
}
#pragma once

#include "vector3d.h"

struct sphere {
    vector3d center;
    float radius;

    sphere() = default;
};
#pragma once

#include "config.h"
#include "log.h"
#include "trace.h"

struct resource_directory;
struct mission_table_container;

inline constexpr auto _MASH_FLAG_IN_USE = 0x80000000;

struct generic_mash_header {
    uint32_t safety_key;
    uint32_t field_4;
    int field_8;
    uint16_t class_id;
    uint16_t field_E;

    inline auto get_class_id() {
        return this->class_id;
    }

    inline bool is_flagged(uint32_t f) const {
        return (f & this->field_4) != 0;
    }

    inline auto *get_mash_data() {
        return (bit_cast<uint8_t *>(this) + this->field_8);
    }

    inline auto generate_safety_key() const {
        return ((this->field_8 + 0x7BADBA5D - (this->field_4 & 0xFFFFFFF) +
                this->class_id + this->field_E) & 0xFFFFFFF) | 0x70000000;
    }
};

struct generic_mash_data_ptrs {
    uint8_t *field_0;
    uint8_t *field_4;

    template<typename T>
    T *get(uint32_t num = 1) {
        auto *res = bit_cast<T *>(this->field_0);
        this->field_0 += sizeof(T) * num;
        return res;
    }

    template<typename T>
    T *get_from_shared(uint32_t num = 1) {
        auto *res = bit_cast<T *>(this->field_4);
        this->field_4 += sizeof(T) * num;
        return res;
    }
};

inline void rebase(uint8_t *&ptr, uint32_t i)
{
    uint32_t v9 = i - ((uint32_t) ptr % i);
    if (v9 < i) {
        ptr += v9;
    }
}


//0x004C1FA0
extern void *parse_generic_mash_init(generic_mash_header *&header,
                                     void *cur_ptr,
                                     bool *allocated_mem,
                                     generic_mash_data_ptrs *a4,
                                     uint32_t struct_size,
                                     uint32_t *virtual_table_lookup,
                                     uint32_t *size_table_lookup,
                                     uint32_t num_table_entries,
                                     uint32_t base_class_size,
                                     void *a10);

template<typename T>
bool parse_generic_object_mash(
    T *&arg0, void *a1, void *a5, uint32_t *a6, uint32_t *a7, uint32_t a8, uint32_t a9, void *a10)
{
    TRACE("parse_generic_object_mash");

    bool allocated_mem = false;

    generic_mash_data_ptrs a4;
    auto *header = static_cast<generic_mash_header *>(a1);
    auto *v4 = static_cast<T *>(parse_generic_mash_init(
        header, a1, &allocated_mem, &a4, sizeof(T), a6, a7, a8, a9, a10));

    arg0 = v4;
    v4->un_mash_start(header, v4, &a4, a5);

    return allocated_mem;
}

//0x005E4020
template<>
bool parse_generic_object_mash(mission_table_container *&a1,
                            void *a2,
                            void *a3,
                            unsigned int *a4,
                            unsigned int *a5,
                            uint32_t a6,
                            uint32_t a7,
                            void *a8);

//0x00563F40
template<>
bool parse_generic_object_mash(resource_directory *&arg0,
                               void *a1,
                               void *a5,
                               uint32_t *a6,
                               uint32_t *a7,
                               uint32_t a8,
                               uint32_t a9,
                               void *a10);
#pragma once

#include "variable.h"

struct nglTexture;
struct nglMesh;
struct generic_mash_header;
struct generic_mash_data_ptrs;

struct strip_lod {
    int field_0;
    int field_4;
    int field_8;
    int field_C;
    int field_10;
    int field_14;

    //0x0052F380
    void un_mash_start(generic_mash_header *a1,
        void *a2,
        generic_mash_data_ptrs *a3,
        void *a4);

};

struct city_lod {
    strip_lod *field_0;
    bool field_4;

    //0x00550420
    city_lod(const char *a1);

    //0x00540380
    void render();

    static inline Var<nglMesh *>  map_mesh {0x0095C188};
    static inline Var<nglMesh *>  sides_mesh {0x0095C16C};
    static inline Var<nglMesh *>  top_mesh {0x0095C748};
    static inline Var<nglMesh *>  ground_mesh {0x0095C714};
    static inline Var<nglMesh *>  box_mesh {0x0095C1d8};
    static inline Var<nglTexture *>  top_texture {0x0095C710};
};

extern void city_lod_patch();
#include "city_lod.h"

#include "common.h"
#include "func_wrapper.h"
#include "ngl.h"
#include "ngl_mesh.h"
#include "os_developer_options.h"
#include "parse_generic_mash.h"
#include "resource_key.h"
#include "resource_manager.h"
#include "trace.h"
#include "utility.h"

VALIDATE_SIZE(city_lod, 8u);
VALIDATE_SIZE(strip_lod, 0x18);

city_lod::city_lod(const char *a1) {

    if constexpr (1) {
        this->field_4 = false;
        this->field_0 = nullptr;
        this->field_4 = os_developer_options::instance()->get_flag(0xE);

        if (this->field_4) {
            resource_key resource_id {string_hash {a1}, RESOURCE_KEY_TYPE_LOD};

            int v1;
            auto *resource = resource_manager::get_resource(resource_id, &v1, nullptr);
            if (resource != nullptr) {
                parse_generic_object_mash(this->field_0, resource, nullptr, nullptr, nullptr, 0, 0, nullptr);
            }

            map_mesh() = nglGetMesh(tlFixedString {"lod_map000"}, true);

            sides_mesh() = nglGetMesh(tlFixedString {"lod_sides000"}, true);

            top_mesh() = nglGetMesh(tlFixedString {"lod_top000"}, true);

            top_texture() = nglGetTexture(tlFixedString {"SM2_CityLODroofs"});

            mString local_5c {a1};
            local_5c.append("_ground000");

            ground_mesh() = nglGetMesh(tlFixedString {local_5c.c_str()}, true);

            box_mesh() = nglGetFirstMeshInFile(tlFixedString {"bldg_box"});
            assert(box_mesh() != nullptr);

            for (auto uVar7 = 0; uVar7 < box_mesh()->NSections; ++uVar7) {
                auto *Mat = box_mesh()->Sections[uVar7].Section->Material;

                if ((*Mat->Name) == tlFixedString {"BoxMatTop"})
                {
                    static Var<nglMaterialBase *> dword_00956358 {0x00956358};
                    dword_00956358() = Mat;
                }

            }

        }
    } else {
        THISCALL(0x00550420, this, a1);
    }
}

void city_lod::render()
{
    TRACE("city_lod::render");

    THISCALL(0x00540380, this);
}

void strip_lod::un_mash_start(generic_mash_header *a1,
        void *a2,
        generic_mash_data_ptrs *a3,
        void *a4)
{
    THISCALL(0x0052F380, this, a1, a2, a3, a4);
}

void city_lod_patch()
{
    {
        FUNC_ADDRESS(address, &city_lod::render);
        REDIRECT(0x0054B2FB, address);
    }
}
#include "wds.h"

#include "common.h"
#include "game.h"
#include "message_board.h"
#include "utility.h"

#include <cassert>

VALIDATE_OFFSET(world_dynamics_system, field_230, 0x230);


entity *world_dynamics_system::get_hero_ptr(int index)
{
    constexpr auto MAX_GAME_PLAYERS = 1;

    assert(index >= 0 && index <= MAX_GAME_PLAYERS);

    auto *result = this->field_230[index];
    return result;
}

void world_dynamics_system::frame_advance(Float a2)
{
    void (__fastcall *func)(void *, void *, Float) = CAST(func, 0x00558370);
    func(this, nullptr, a2);

    {
        g_game_ptr()->mb->frame_advance(a2);
    }
}

void wds_patch()
{
    {
        auto address = func_address(&world_dynamics_system::frame_advance);
        REDIRECT(0x0055A0F7, address);
    }
}
#pragma once

#include "bit.h"
#include "func_wrapper.h"

#include <type_traits>

template<typename T, typename = typename std::enable_if_t<std::is_pointer_v<T>>>
void *&get_vtbl(T p) {
    static_assert(sizeof(int) == sizeof(void *), "");

    return reinterpret_cast<void *&>((bit_cast<int *>(p))[0]);
}

inline auto *&get_vfunc(std::intptr_t vtbl, int offset) {
    auto *v = bit_cast<fastcall_call(*)[1]>(vtbl);

    return ((*v)[offset / 4]);
}

template<typename T, typename = typename std::enable_if_t<std::is_pointer_v<T>>>
void set_vtbl(T p, std::ptrdiff_t address) {
    static_assert(sizeof(int) == sizeof(void *), "");

    (bit_cast<int *>(p))[0] = address;
}
#pragma once

#include "string_hash.h"

struct script_object;

struct vm_executable {
    script_object *owner;
    string_hash fullname;
    string_hash name;
    int parms_stacksize;

    auto get_parms_stacksize() const
    {
        return this->parms_stacksize;
    }
};

#pragma once

#include "mcontainer.h"

#include "nlPlatformEnum.h"
#include "slab_allocator.h"
#include "variable.h"

#include <windef.h>

extern Var<int> mString_count;

//0x00936BD0
extern Var<const char *[4]> packfile_dir;

//0x00936BF0
extern Var<const char *[4]> packfile_ext;

inline constexpr size_t MAX_MSTRING_LENGTH = 65535u;

struct mash_info_struct;

struct mString : mContainer {
    struct fmtd {
        int value;
    };

    char *guts;

public:
    slab_allocator::slab_t *field_C;

    //0x00420F00
    mString();

    //0x00420F60
    mString(mString::fmtd fmt, const char *Format, ...);

    //0x00421100
    mString(const char *a2);

    //0x00421090
    explicit mString(float a2);

    //0x00420FD0
    explicit mString(int a2);

    //0x00421220
    mString(const mString &a2);

    //0x00420F60
    mString(int a2, const char *Format, ...);

    //0x0041FE30
    mString &operator=(const char *a2);

    //0x0041FE10
    mString &operator=(const mString &a2);

    //0x004015C0
    ~mString();

    inline void set_size(size_t size) {
        this->m_size = size;
    }

    inline bool empty() const {
        return m_size == 0;
    }

    inline int size() const {
        return m_size;
    }

    inline int length() const {
        return m_size;
    }

    void initialize();

    //0x0041F9D0
    void update_guts(const char *from_string, int n);

    int compare(const char *str) const;

    bool is_equal(const char *a2) const;

    //0x0041FDE0
    mString truncate(int a2);

    void append(char a3);

    //0x0041FAB0
    void append(const char *from_string, int from_string_length = -1);

    struct pos_t {
        int value;

        operator int() {
            return value;
        }
    };

    //0x0041FD00
    int find(pos_t a2, char a3) const;

    //0x0041FC70
    int find(const char *str, int a3 = 0) const;

    //0x0043D7D0
    [[nodiscard]] mString slice(int start, int end);

    //0x0041FC00
    mString *operator+=(const char *Source);

    //0x0041FBE0
    mString *operator+=(const mString &a2);

    //0x00421260
    [[nodiscard]] mString substr(int a3, int Count) const;

    const char *c_str() const;

    char *data();

    //0x004209C0
    void finalize(int);

    //0x0041FD90
    mString to_lower();

    mString to_upper();

    char at(int i) const;

    char operator[](int i) const;

    void unmash(mash_info_struct *a1, void *a2);

    //0x004209F0
    void custom_unmash(mash_info_struct *a1, void *a2);

    //0x0041F970
    void destroy_guts();

    //0x00503980
    bool operator==(const char *a2) const;

    int rfind(const char *str) const;

    //0x0041FD30
    int rfind(char a2, int a3 = -1) const;

    //0x0055DAD0
    [[nodiscard]] static mString from_int(int a2);

    [[nodiscard]] int to_int() const;

    [[nodiscard]] double to_float() const;

    mString remove_surrounding_whitespace();

    //0x00421B60
    mString remove_leading(const char *a1);

    //0x00421C50
    mString remove_trailing(const char *a2);

    //0x0055DAB0
    [[nodiscard]] static mString from_float(float a2);

    //0x00421410
    friend mString operator+(const char *a2, const mString &a3);

    //0x004214E0
    friend mString operator+(const mString &arg4, const char *Source);

    //0x00421330
    friend mString operator+(const mString &arg4, const mString &a3);

    friend bool operator==(const mString &a1, const mString &a2);

    friend bool operator!=(const mString &a1, const mString &a2);

    friend bool operator<(const mString &a1, const mString &a2);
    friend bool operator>(const mString &a1, const mString &a2);

    //0x005BAA80
    [[nodiscard]] static mString get_standalone_filename(const mString &str, _nlPlatformEnum a3);

    static int npos;

    static inline Var<char *> null {0x0091E7C0};
};

//0x0064DF30
extern void *dialog_box_formatting(mString *out_string, mString a2, int a3, int a4);

void sub_79F7D0(int *a1, int a2);

int sub_79F690(int *a1, int a2);

signed int sub_79F760(int *a1);

//signed int sub_79E180(int a1, char *a2, int a3, int *a4);

int sub_79E490(int a1, char *a2);
#pragma once

#include "float.hpp"

struct vector2d {
    float arr[2];

    const float &operator[](int idx) const {
        return arr[idx];
    }

    float &operator[](int idx) {
        return arr[idx];
    }

    const char *to_string() const;

    float length2() const;

    float length() const;
};
#pragma once

#include "color32.h"
#include "float.hpp"
#include "game_button.h"
#include "limited_timer.h"
#include "mstring.h"
#include "rumble_struct.h"
#include "vector3d.h"
#include "fixedstring.h"

#include <vector.hpp>

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

enum class game_state {
    LEGAL = 1,
    WAIT_LINK = 4,
    LOAD_LEVEL = 5,
    RUNNING = 6,
    PAUSED = 7,
};

struct game;

//0x006063C0
extern void game__setup_input_registrations(game *a1);

//0x00605950
extern void game__setup_inputs(game *a1);

struct game {
    struct level_load_stuff {
        level_descriptor_t *descriptor;
        mString name_mission_table;
        mString field_14;
        vector3d field_24;
        int field_30;
        limited_timer_base field_34;
        bool load_widgets_created;
        bool load_completed;
        bool field_3A;
        bool field_3B;

        //0x00561EA0
        level_load_stuff();

        void reset_level_load_data();

        //0x0050B5F0
        bool wait_for_mem_check();

        //0x0055A1C0
        void look_up_level_descriptor();

        //0x0051D260
        void construct_loading_widgets();

        //0x0050B560
        void destroy_loading_widgets();
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
    world_dynamics_system *the_world;
    int field_54;
    int field_58;
    camera *field_5C;
    entity_base *current_game_camera;
    mic *field_64;
    message_board *mb;
    _std::vector<game_process> process_stack;
    localized_string_table *field_7C;
    game_button field_80;
    nglMesh *field_B4;
    nglMesh *field_B8;
    char field_BC;
    char empty3[3];
    game_settings *gamefile;
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
    float field_284;
    int field_288;
    float field_28C;
    rumble_struct field_290;
    bool field_2B4;
    bool field_2B5;
    int field_2B8;
    int field_2BC;
    limited_timer_base field_2C0;

    //0x00557610
    game();

    //0x00559D10
    ~game();

    void enable_user_camera(bool a2)
    {
        this->m_user_camera_enabled = a2;
    }

    bool is_user_camera_enabled() const {
        return m_user_camera_enabled;
    }

    game_settings *get_game_settings();

    void go_next_state();

    game_state get_cur_state() const;

    void handle_frame_locking(float *a1);

    //0x00548C10
    void begin_hires_screenshot(int a2, int a3);

    //0x00515230
    void enable_physics(bool a2);

    //0x0054E1B0
    void render_world();

    //0x00558100
    void advance_state_legal(Float a2);

    //0x00545B00
    void pop_process();

    //0x00514FD0
    void set_current_camera(camera *a2, bool a3);

    //0x00552E50
    void one_time_init_stuff();

    bool level_is_loaded() const;

    bool is_paused() const;

    bool is_physics_enabled() const;

    bool is_marky_cam_enabled() const;

    //0x0054FBE0
    void pause();

    //0x0053A880
    void unpause();

    //0x00558220
    void advance_state_paused(Float a1);

    //0x00524100
    void one_time_deinit_stuff();

    //0x00515140
    void clear_screen();

    //0x00515010
    void frame_advance_game_overlays(Float a1);

    //0x0054F860
    void message_board_init();

    void message_board_clear();

    //0x00559FA0
    void advance_state_running(Float a2);

    //0x00536410
    void advance_state_wait_link(Float a2);

    //0x00552F50
    void handle_cameras(input_mgr *a2, const Float &a3);

    //0x0055D510
    void handle_game_states(const Float &a2);

    //0x00514AB0
    void init_motion_blur();

    //0x00514F00
    void freeze_hero(bool a2);

    //0x0055C6E0
    void load_this_level();

    //0x0055D780
    void frame_advance(Float a2);

    //0x0055D650
    void frame_advance_level(Float time_inc);

    //0x00557E10
    void unload_current_level();

    //0x00514A50
    camera *get_current_view_camera(int a2);

private:
    void _load_new_level(const mString &a2);

public:
    //0x00514C40
    void load_new_level(const mString &a2, const vector3d &a3);

    //0x00514C70
    void load_new_level(const mString &a1, int a2);

    //0x0055D3A0
    void advance_state_load_level(Float a2);

    //0x0054F8C0
    void set_camera(int a2);

    //0x0055A420
    void load_hero_packfile(const char *str, bool a3);

    //0x00514EB0
    void setup_input_registrations();

    //0x005241E0
    void enable_marky_cam(bool a2, bool a3, Float a4, Float a5);

    //0x00548C70
    void soft_reset_process();

    //0x00514AA0
    static void load_complete();

    void push_lores();

    void push_process(game_process &process);

    //0x00557B80
    void render_intros();

    void setup_inputs();

    //0x00558320
    void unload_hero_packfile();

    //0x00521610
    void render_motion_blur();

    //0x005244E0
    float get_script_game_clock_timer() const;

    //0x00524290
    void render_interface();

    //0x005242D0
    mString get_hero_info() const;

    //0x0052E0D0
    mString get_camera_info() const;

    mString get_analyzer_info() const;

    void show_debug_info() const;

    //0x00514D00
    void show_max_velocity();

    //0x005244B0
    void reset_control_mappings();

    //0x0052B250
    void render_ui();

    //0x0051D1C0
    bool is_button_pressed(int a4) const;

    void sub_5580F0();

    void sub_524170();

    void sub_559F50(Float *a1);

    void sub_5241D0(Float a1);

    //0x00510780
    static void render_empty_list();

    //0x0095C8F8
    static inline auto & setup_input_registrations_p = var<int (*)(game *)>(0x0095C8F8);

    //0x0095C8FC
    static inline void (* setup_inputs_p)(game *) = game__setup_inputs;
};

extern game *& g_game_ptr;

//0x00581B40
extern void system_idle();

//0x0054F6D0
extern void game_packs_modified_callback(_std::vector<resource_key> &a1);

extern void game_patch();
#pragma once

struct vector2d;

struct vector2di {
    int x, y;

    vector2di() = default;


};
#pragma once

struct vector3d
{
    float x, y, z;

    float operator[](int i) {
        return (&x)[i];
    }
};

const vector3d ZEROVEC{0, 0, 0};
#pragma once

struct vector4d {
    float x, y, z, w;

    auto &operator[](uint32_t idx) {
        return (&x)[idx];
    }

    auto &operator[](uint32_t idx) const {
        return (&x)[idx];
    }

    float length2() const {
        return ((x * x) + (y * y) +
                (z * z) + (w * w));
    }

};

inline float AbsSquared(const vector4d &a1) {
    return a1.length2();
}
#pragma once

#include "vector4d.h"

struct matrix4x4 {
    vector4d arr[4];

    auto &operator[](uint32_t idx) {
        return arr[idx];
    }
};
#pragma once

#include <cstring>
#include <cassert>

struct fixedstring32 {

    static inline constexpr auto MAX_CHARS = 32;

    char str[MAX_CHARS]{};

    fixedstring32() = default;

    fixedstring32(const char *str) {
        assert(str != nullptr);

        auto len = strlen(str);
        assert(len <= MAX_CHARS && "String too big for fixed size");

        memcpy(this->str, str, len);
    }

    const char *to_string() {
        assert(str[MAX_CHARS - 1] == '\0');

        return this->str;
    }

};
#pragma once

#include <cstdint>

struct color32
{
    uint8_t field_0[4];

    void set_alpha(uint8_t a2) {
        this->field_0[3] = a2;
    }
};
