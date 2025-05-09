#include "consolevars.h"
#include "actor.h"
#include "base_ai_core.h"
#include "console.h"


#include "game.h"
#include "game_settings.h"
#include "geometry_manager.h"
#include "message_board.h"
#include "mstring.h"
#include "os_developer_options.h"
#include "variables.h"
#include "wds.h"

#include <cassert>

std::list<ConsoleVariable *> *g_console_vars{nullptr};

ConsoleVariable::ConsoleVariable()
{
    this->setName({});

    if (g_console_vars == nullptr) {
        g_console_vars = new std::list<ConsoleVariable *>{};
    }

    g_console_vars->push_back(this);
}

static constexpr auto MAX_VARIABLE_NAME_LEN = 32;

void ConsoleVariable::setValue(const std::string &, const std::string &) {
    ;
}

void ConsoleVariable::setValue(const std::string &) {
    ;
}

std::string ConsoleVariable::getValue() {
    return {};
}

void ConsoleVariable::setName(const std::string &pName)
{
    assert(pName.size() < MAX_VARIABLE_NAME_LEN);

    auto *v2 = pName.c_str();
    strcpy(this->field_4, v2);
    _strlwr(this->field_4);
}



std::string ConsoleVariable::getName() const
{
    std::string a2{this->field_4};
    return a2;
}

static ConsoleHeightVariable g_ConsoleHeightVariable{};

ConsoleHeightVariable::ConsoleHeightVariable() {
    setName("console_height");
}

void ConsoleHeightVariable::setValue(const std::string &a2) {
    auto *v7 = this;
    float v6 = 0.0;

    const char *v2 = nullptr;
    if (a2.length() <= 0 || (v2 = a2.c_str(), v6 = atof(v2), v6 < 25.0) || v6 > 480.0) {
        auto v4 = v7->getName();

        auto *v3 = v4.c_str();
        g_console->addToLog("'%s' must be between 25 and %d", v3, 480);

    } else {
        g_console->setHeight(v6);
    }
}

std::string ConsoleHeightVariable::getValue() {
    auto v2 = g_console->getHeight();

    char a1[32];
    sprintf(a1, "%.2f", v2);

    std::string out{a1};
    return out;
}

//static HealthVariable g_HealthVariable{};






bool get_boolean_value(const char *a1) {
    return a1 != nullptr && (!_strcmpi(a1, "on") || !_strcmpi(a1, "true") || !strcmp(a1, "1"));
}

static RenderFramerateVariable g_RenderFramerateVariable{};

RenderFramerateVariable::RenderFramerateVariable() {
    setName("render_fps");
}

void RenderFramerateVariable::setValue(const std::string &a1) {
    auto *v1 = a1.c_str();
    os_developer_options::instance()->set_flag(mString { "SHOW_FPS" }, get_boolean_value(v1));
}

std::string RenderFramerateVariable::getValue() {
    auto show_fps = os_developer_options::instance()->get_flag(mString { "SHOW_FPS" });

    return (show_fps ? "on" : "off");
}

static RenderInterfaceVariable g_RenderInterfaceVariable {};

RenderInterfaceVariable::RenderInterfaceVariable() {
    setName("render_interface");
}

void RenderInterfaceVariable::setValue(const std::string &a1)
{
    auto *v1 = a1.c_str();
    os_developer_options::instance()->set_flag(mString { "INTERFACE_DISABLE" }, !get_boolean_value(v1));
}

std::string RenderInterfaceVariable::getValue()
{
    auto show_fps = !os_developer_options::instance()->get_flag(mString { "SHOW_FPS" });

    return (show_fps ? "on" : "off");
}

//static ProjZoomVariable g_ProjZoomVariable{};

ProjZoomVariable::ProjZoomVariable() {
    setName("proj_zoom");
}







static DisableOcclusionCullingVariable g_DisableOcclusionCullingVariable {};



std::string DisableOcclusionCullingVariable::getValue() {
    char a1[32] {};
    sprintf(a1, "%d", g_disable_occlusion_culling());
    return std::string {a1};
}
