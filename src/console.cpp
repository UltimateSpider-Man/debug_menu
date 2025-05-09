#include "console.h"

#include "consolecmds.h"
#include "consolevars.h"
#include "debug_menu.h"
#include "fetext.h"
#include "game.h"
#include "input_mgr.h"
#include "log.h"
#include "memory.h"
#include "ngl.h"
#include "os_developer_options.h"
#include "os_file.h"
#include "panelquad.h"
#include "tokenizer.h"
#include "trace.h"
#include "variables.h"
#include "vector2d.h"
#include "vtbl.h"

#include <vector>

std::stack<tokenizer*> s_exec_tok_stack {};

float s_exec_tick { 0 };

Console* g_console { nullptr };

static void (*kbevcb)(KeyEvent, Key_Axes, void*) = nullptr;

static void* kbevudata = nullptr;

static void (*kbchcb)(char, void*) = nullptr;

static void* kbchudata = nullptr;


char KB_register_char_callback(void (*a1)(char, void*), void* a2)
{
    kbchcb = a1;
    kbchudata = a2;
    return 1;
}

char KB_register_event_callback(void (*a1)(KeyEvent, Key_Axes, void*), void* a2)
{
    kbevcb = a1;
    kbevudata = a2;
    return 1;
}

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

// Actual callbacks that forward to g_console
void console_event_callback(KeyEvent a1, Key_Axes a2, void* a3)
{
    if (g_console != nullptr) {
        g_console->handle_event(a1, a2, a3);
    }
}

void console_char_callback(char a1, void* a2)
{
    if (g_console != nullptr) {
        g_console->handle_char(a1, a2);
    }
}

// -----------------------------------------------------------------------------

Console::Console()
{
    TRACE("Console::Console");

    this->m_visible = false;
    this->oldCurrent[0] = '\0';
    this->current[0] = '\0';

    this->field_220 = false;
    this->m_height = 240.0f;
    this->field_224 = 0.0f;
    this->lineNumber = 0;
    this->field_230 = 16;
    this->field_234 = 16;
    this->cmdLogNumber = 0;
    this->field_24E = true;

    this->field_248 = new PanelQuad {};

    vector2d v8[4];
    v8[0] = { 0.0f, 0.0f };
    v8[1] = { 640.0f, 0.0f };
    v8[2] = { 0.0f, this->m_height };
    v8[3] = { 640.0f, this->m_height };

    color32 v4[4];
    v4[0] = { 0, 0, 0, 255 };
    v4[1] = { 0, 0, 0, 255 };
    v4[2] = { 0, 0, 0, 255 };
    v4[3] = { 0, 0, 0, 255 };

    // Example usage of a "Var<char[3]>"
    Var<char[3]> Source { 0x00871BD1 };

    // Initialize the panel
    this->field_248->Init(v8, v4, panel_layer { 0 }, 10.0f, Source());

    this->field_24C = false;
    this->field_24D = false;

    this->field_23C = mString("console_log.txt");

    this->setHeight(200.0f);
    this->hide();

    // Register callbacks
    KB_register_event_callback(console_event_callback, nullptr);
    KB_register_char_callback(console_char_callback, nullptr);
}

Console::~Console()
{
    TRACE("Console::~Console()");

    if (field_248 != nullptr) {
        // In your real code, presumably call vfunc(0x8).  Placeholder:
        delete field_248;
        field_248 = nullptr;
    }
}



void Console::addToCommandLog(const char* a1)
{
    m_command_log.push_front(mString { a1 });
    while (static_cast<int>(m_command_log.size()) > field_234) {
        m_command_log.pop_back();
    }
    cmdLogNumber = 0;
}

bool Console::StrnCopy(const char* src, char* dest, int* a3)
{
    if (!src) {
        return false;
    }

    // Skip leading spaces
    while (src[*a3] == ' ') {
        ++(*a3);
    }

    int start = *a3;
    // Copy until next space or end-of-string
    while (src[*a3] != ' ' && src[*a3] != '\0') {
        ++(*a3);
    }

    if (start == *a3 || !dest) {
        return false;
    }

    int length = *a3 - start;
    std::strncpy(dest, &src[start], length);
    dest[length] = '\0';

    // Skip trailing spaces
    while (src[*a3] == ' ') {
        ++(*a3);
    }
    return true;
}

ConsoleCommand* Console::getCommand(const std::string& a1)
{
    if (g_console_cmds == nullptr) {
        return nullptr;
    }
    auto& cmds = *g_console_cmds;
    auto it = std::find_if(cmds.begin(), cmds.end(), [&a1](auto& cmd) {
        return cmd && cmd->match(a1);
    });
    return (it != cmds.end() ? *it : nullptr);
}

ConsoleVariable* Console::getVariable(const std::string& a1)
{
    if (g_console_vars == nullptr) {
        return nullptr;
    }
    auto& vars = *g_console_vars;
    auto it = std::find_if(vars.begin(), vars.end(), [&a1](auto& var) {
        return var && var->match(a1);
    });
    return (it != vars.end() ? *it : nullptr);
}

void Console::addToLog(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    if (fmt) {
        char buffer[4096] {};
        std::vsnprintf(buffer, sizeof(buffer), fmt, args);

        // Now split on newlines
        char* lineStart = buffer;
        while (true) {
            char* newline = std::strchr(lineStart, '\n');
            if (!newline) {
                // No more newlines, just push the remainder
                m_log.push_front(mString { lineStart });
                break;
            }
            // Copy up to the newline
            char temp[1024] {};
            size_t length = static_cast<size_t>(newline - lineStart);
            if (length >= sizeof(temp)) {
                length = sizeof(temp) - 1; // Safety
            }
            std::strncpy(temp, lineStart, length);
            temp[length] = '\0';

            m_log.push_front(mString { temp });
            lineStart = newline + 1; // Advance past the newline
        }

        // Trim if too large
        while (m_log.size() > static_cast<size_t>(field_230)) {
            m_log.pop_back();
        }
    }

    va_end(args);
}

void Console::processCommand(const char* a2, bool is_log)
{
    if (a2 && std::strlen(a2) > 0) {
        if (is_log) {
            addToCommandLog(a2);
        }

        std::vector<std::string> args;
        char tmp[256] {};
        int idx = 0;

        // First token is the command name
        StrnCopy(a2, tmp, &idx);
        mString cmdName { tmp };
        cmdName.to_lower();

        ConsoleCommand* command = getCommand(cmdName.c_str());
        if (command) {
            // Get remaining tokens
            while (StrnCopy(a2, tmp, &idx)) {
                args.emplace_back(tmp);
            }

            if (is_log) {
                addToLog(a2);
            }

            if (!command->process_cmd(args)) {
                if (is_log) {
                    addToLog("??? %s", a2);
                }
            }
        } else if (getVariable(cmdName.c_str())) {
            args.push_back(cmdName.c_str());
            while (StrnCopy(a2, tmp, &idx)) {
                args.emplace_back(tmp);
            }

            if (is_log) {
                addToLog(a2);
            }

            // Decide "get" vs "set"
            if (args.size() <= 1) {
                command = getCommand("get");
            } else {
                command = getCommand("set");
            }
            if (command) {
                command->process_cmd(args);
            }
        } else if (is_log) {
            addToLog("??? %s", a2);
        }
    }
}

bool Console::isVisible() const
{
    return m_visible;
}

void Console::setHeight(float a2)
{
    m_height = a2;
    if (field_248) {
        field_248->SetPos(0.0f, 0.0f, 640.0f, a2);
    }
}

void Console::handle_char(char a2, void*)
{
    if (m_visible && a2 >= ' ' && a2 != 127 && a2 != '`' && a2 != '~') {
        // '[' apparently used as a custom backspace?
        if (a2 == '[') {
            size_t v3 = std::strlen(current);
            if (v3 > 0) {
                current[v3 - 1] = '\0';
            }
            std::strcpy(oldCurrent, current);
            return;
        }

        size_t len = std::strlen(current);
        if (len < sizeof(current) - 1) {
            current[len] = a2;
            current[len + 1] = '\0';
        }
        std::strcpy(oldCurrent, current);
    }
}

void Console::hide()
{
    sp_log("Console::hide");
    if (field_248) {
        field_248->TurnOn(false);
    }
    m_visible = false;
    color32 col { 100, 100, 100, 100 };
    if (field_248) {
        field_248->SetColor(col);
    }

    // #if 0 replaced with input_mgr usage
    input_mgr::instance()->field_20 &= 0xFFFFFFFE;

    g_game_ptr()->unpause();
}

float Console::getHeight()
{
    return m_height;
}

void Console::exec_frame_advance(float a2)
{
    if (!s_exec_tok_stack.empty()) {
        s_exec_tick += a2;
        float delay = static_cast<float>(
            os_developer_options::instance()->get_int(mString("EXEC_DELAY")));
        if (s_exec_tick >= delay) {
            s_exec_tick = 0.0f;
            tokenizer* topTok = s_exec_tok_stack.top();
            char* tok = topTok->get_token();
            if (tok) {
                processCommand(tok, false);
            } else {
                delete topTok;
                s_exec_tok_stack.pop();
            }
        }
    }
}

void Console::exec(const mString& a2)
{
    // Minimal placeholder file usage:
    os_file f {};
    os_file v10 {};

    v10.open(a2, 1);
        size_t file_size = f.get_size();
        char* data = new char[file_size + 1];
        f.read(data, file_size);
        data[file_size] = '\0';
        f.close();

        void* mem = mem_alloc(sizeof(tokenizer));
        auto* t = new (mem) tokenizer(true);
        t->parse(data, "\n\r");
        t->setup_current_iterator();
        s_exec_tok_stack.push(t);

        addToLog("Executing '%s'", a2.c_str());

        delete[] data;

        addToLog("File '%s' not found", a2.c_str());
    }


void Console::frame_advance(float a2)
{
    // Original code calls a function pointer (0x0629E60). Placeholder:
    // do nothing or insert your logic

    // Then handle the exec stack
    exec_frame_advance(a2);
}

void Console::getMatchingCmds(const char* a2, std::list<mString>& cmds)
{
    cmds.clear();
    size_t prefixLen = std::strlen(a2);

    if (g_console_cmds) {
        for (auto* cmd : *g_console_cmds) {
            if (cmd && std::strncmp(cmd->field_4, a2, prefixLen) == 0) {
                cmds.push_back(mString(cmd->field_4));
            }
        }
    }
    if (g_console_vars) {
        for (auto* var : *g_console_vars) {
            if (var && std::strncmp(var->field_4, a2, prefixLen) == 0) {
                cmds.push_back(mString(var->field_4));
            }
        }
    }
}

void Console::partialCompleteCmd(char* a1, std::list<mString>& list)
{
    // Finds common prefix among multiple matches
    int a3 = -1;
    // Compare each adjacent pair
    for (auto it = list.begin(); it != list.end();) {
        auto& first = *it;
        ++it;
        if (it != list.end()) {
            auto& second = *it;
            const char* s1 = first.c_str();
            const char* s2 = second.c_str();
            int i = 0;
            while (s1[i] && s2[i] && s1[i] == s2[i]) {
                ++i;
            }
            if (a3 < 0 || i < a3) {
                a3 = i;
            }
        }
    }
    if (a3 > 0) {
        // If the common prefix is longer than what's in `a1`, extend it
        int currentLen = static_cast<int>(std::strlen(a1));
        if (a3 > currentLen) {
            auto& front = list.front();
            const char* frontStr = front.c_str();
            // Copy up to `a3`
            std::strncpy(a1, frontStr, static_cast<size_t>(a3));
            a1[a3] = '\0';
        }
    }
}

void Console::handle_event(KeyEvent a2, Key_Axes a3, [[maybe_unused]] void* a4)
{
    if (a2 == KB_DOWN) {
        switch (a3) {
        case KB_RETURN:
            if (m_visible) {
                processCommand(current, false);
                current[0] = '\0';
                std::strcpy(oldCurrent, current);
            }
            break;

        case KB_BACKSPACE:
            if (m_visible) {
                size_t len = std::strlen(current);
                if (len > 0) {
                    current[len - 1] = '\0';
                }
                std::strcpy(oldCurrent, current);
            } else if (debug_menu::active_menu != nullptr) {
                debug_menu::hide();
            } else {
                debug_menu::show();
            }
            break;

        case KB_TAB:
            if (m_visible && std::strlen(current) > 0) {
                std::list<mString> matches;
                getMatchingCmds(current, matches);
                if (!matches.empty()) {
                    if (matches.size() == 1) {
                        auto& front = matches.front();
                        std::strcpy(current, front.c_str());
                        std::strcat(current, " ");
                        std::strcpy(oldCurrent, current);
                    } else {
                        partialCompleteCmd(current, matches);
                        std::strcpy(oldCurrent, current);
                        addToLog((const char*)this); // Original code is suspicious, possibly logging pointer?
                        addToLog((const char*)this);
                        for (auto& it : matches) {
                            addToLog("%s", it.c_str());
                        }
                    }
                    matches.clear();
                } else {
                    addToLog((const char*)this);
                    addToLog((const char*)this);
                }
            }
            break;

        case KB_TILDE:
            if (isVisible()) {
                hide();
            } else {
                show();
            }
            break;

        case KB_HOME:
            if (m_visible) {
                lineNumber = static_cast<unsigned int>(m_log.size() - 1);
            }
            break;

        case KB_END:
            if (m_visible) {
                lineNumber = 0;
            }
            break;

        case KB_PAGEUP:
            if (m_visible && lineNumber < m_log.size() - 1) {
                ++lineNumber;
            }
            break;

        case KB_PAGEDOWN:
            if (m_visible && lineNumber > 0) {
                --lineNumber;
            }
            break;

        case KB_DOWN:
            if (m_visible && !m_command_log.empty()) {
                if (cmdLogNumber > 0) {
                    --cmdLogNumber;
                }
                if (cmdLogNumber > 0) {
                    auto it = m_command_log.begin();
                    auto end = m_command_log.end();
                    for (unsigned i = cmdLogNumber - 1; i > 0 && it != end; --i) {
                        ++it;
                    }
                    if (it != end) {
                        std::strcpy(current, it->c_str());
                    }
                } else {
                    cmdLogNumber = 0;
                    std::strcpy(current, oldCurrent);
                }
            }
            break;

        case KB_UP:
            if (m_visible && !m_command_log.empty()) {
                ++cmdLogNumber;
                // If we exceed size, pull it back
                while (cmdLogNumber > 0 && cmdLogNumber > m_command_log.size()) {
                    --cmdLogNumber;
                }
                if (cmdLogNumber > 0 && cmdLogNumber <= m_command_log.size()) {
                    auto it = m_command_log.begin();
                    auto end = m_command_log.end();
                    for (unsigned j = cmdLogNumber - 1; j > 0 && it != end; --j) {
                        ++it;
                    }
                    if (it != end) {
                        std::strcpy(current, it->c_str());
                    }
                }
            }
            break;

        default:
            break;
        }
    }
}

void Console::show()
{
    TRACE("Console::show");
    if (field_248) {
        field_248->TurnOn(true);
    }
    m_visible = true;
    color32 col { 100, 100, 100, 100 };
    if (field_248) {
        field_248->SetColor(col);
    }
    input_mgr::instance()->field_20 |= 1u;

    g_game_ptr()->pause();
}

void Console::setRenderCursor(bool a2)
{
    field_24E = a2;
}






void Console::render()
{
    TRACE("Console::render");

    const color32 font_color{ 255, 255, 255, 255 };
    if (this->m_visible)
    {
        this->field_248->Draw();

        auto* font = g_femanager().GetFont(font_index{0});

        uint32_t v26, v25;
        nglGetStringDimensions_console(font, &v26, &v25, "M");
        auto v24 = (float)v25;
        auto v23 = this->m_height - 20.0;

        const char* v11;
        if (this->field_220) {
            v11 = "_";
        }
        else {
            v11 = "";
        }

        auto v9 = mString{ "> " } + this->current;

        auto v8 = v9 + v11;

        mString v22 = v8;

        vector2di v2{ 10, (int)v23 };
        render_console_text(v22, v2, font_color);
        v23 = v23 - v24;
        if (this->lineNumber > 0)
        {
            mString v17{ "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" };

            auto v3 = vector2di{ 10, (int)v23 };
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
                    auto v7 = vector2di{ 10, (int)v23 };

                    render_console_text(v5, v7, font_color);
                }

                v23 = v23 - v24;
            }

            ++it;
        }
    }
}

void render_console_text(const mString& a1, vector2di a2, const color32& a4)
{
    FEText v7{ font_index{0 },
              global_text_enum{0 },
              (float)a2.x,
              (float)a2.y,
              1,
              panel_layer{0 },
              1.0,
              16,
              0,
              a4 };

    auto* v3 = a1.c_str();

    mString v4{ v3 };

    v7.field_1C = v4;

    v7.Draw();
}


void* Console::operator new(size_t size)
{
    auto* mem = mem_alloc(size);
    return mem;
}

void Console::operator delete(void* ptr, size_t size)
{
    mem_dealloc(ptr, size);
}


void terrain_types_manager_create_inst()
{
    CDECL_CALL(0x005C54B0);
    g_console = new Console{};

}

void terrain_types_manager_delete_inst()
{
    CDECL_CALL(0x005BA680);

    delete g_console;
}

void __fastcall FEManager_Update(void* self, void* edx, Float a2)
{
    void(__fastcall * func)(void*, void* edx, Float) = CAST(func, 0x00642B30);
    func(self, edx, a2);

    {
        g_console->frame_advance(a2);
    }
}

void hook_nglListEndScene()
{
    g_console->render();

    CDECL_CALL(0x0076A030);
}

void console_patch()
{

    REDIRECT(0x0052B5D7, hook_nglListEndScene);

    
//   REDIRECT(0x00552E75, terrain_types_manager_create_inst);


    REDIRECT(0x00524155, terrain_types_manager_delete_inst);


    {
        REDIRECT(0x00558289, FEManager_Update);
        REDIRECT(0x0055A102, FEManager_Update);
    }
}