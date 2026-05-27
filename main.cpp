#ifdef _WIN32
#define _USE_MATH_DEFINES
#define NOMINMAX
#endif

#include <cmath>

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <stdint.h>
#pragma comment(lib, "Dinput8.lib")
#pragma comment(lib, "Dxguid.lib")

#include "src/base_ai_debug.h"



#include "src/variant_interface.h"
#include "src/custom_ints.h"

#include "src/debug_menu_extra.h"
#include "src/devopt.h"
#include "src/debug_menu.h"
#include "src/entity_animation_menu.h"
#include "src/game.h"
#include "src/input_mgr.h"
#include "src/resource_manager.h"
#include "src/forwards.h"
#include "src/func_wrapper.h"
#include "src/fixedstring32.h"
#include "src/levelmenu.h"
#include "src/pausemenusystem.h"
#include "src/memory_menu.h"
#include "src/message_board.h"
#include "src/mission_manager.h"
#include "src/mission_table_container.h"
#include "src/mouselook_controller.h"
#include "src/mstring.h"
#include "src/region.h"
#include "src/os_developer_options.h"
#include "src/script_executable.h"
#include "src/script_library_class.h"
#include "src/script_instance.h"
#include "src/script_object.h"
#include "src/spider_monkey.h"
#include "src/geometry_manager.h"
#include "src/entity.h"
#include "src/terrain.h"
#include "src/vm_executable.h"
#include "src/variable.h"
#include "src/wds.h"





#include <ctime>
#include <corecrt_startup.h>

//
#include <list>


#include <cassert>

#include <cstdio>

#define INITGUID


#include <direct.h>

#include <float.h>


#include <dsound.h>

#include <initguid.h>
#include <dxdiag.h>


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>   
#include "src/pause_menu_root.h"
#include "src/pause_menu_status.h"
#include "src/frontendmenusystem.h"
#include "src/igozoomoutmap.h"



#pragma comment(lib, "shlwapi.lib")

DWORD* ai_current_player = nullptr;
DWORD* fancy_player_ptr = nullptr;


static HANDLE g_hWorker = nullptr;

/*
#undef IsEqualGUID
BOOL WINAPI IsEqualGUID(
	REFGUID rguid1,
	REFGUID rguid2)
{
	return !memcmp(rguid1, rguid2, sizeof(GUID));
}
*/

uint8_t color_ramp_function(float ratio, int period_duration, int cur_time) {

	if (cur_time <= 0 || 4 * period_duration <= cur_time)
		return 0;

	if (cur_time < period_duration) {

		float calc = ratio * cur_time;

		return std::min(calc, 1.0f) * 255;
	}


	if (period_duration <= cur_time && cur_time <= 3 * period_duration) {
		return 255;
	}


	if (cur_time <= 4 * period_duration) {

		int adjusted_time = cur_time - 3 * period_duration;
		float calc = 1.f - ratio * adjusted_time;

		return std::min(calc, 1.0f) * 255;
	}

    return 0;

}

static void *HookVTableFunction(void *pVTable, void *fnHookFunc, int nOffset) {
    intptr_t ptrVtable = *((intptr_t *) pVTable); // Pointer to our chosen vtable
    intptr_t ptrFunction = ptrVtable +
        sizeof(intptr_t) *
            nOffset; // The offset to the function (remember it's a zero indexed array with a size of four bytes)
    intptr_t ptrOriginal = *((intptr_t *) ptrFunction); // Save original address

    // Edit the memory protection so we can modify it
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery((LPCVOID) ptrFunction, &mbi, sizeof(mbi));
    VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &mbi.Protect);

    // Overwrite the old function with our new one
    *((intptr_t *) ptrFunction) = (intptr_t) fnHookFunc;

    // Restore the protection
    VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &mbi.Protect);

    // Return the original function address incase we want to call it
    return (void *) ptrOriginal;
}

typedef struct _list{
	struct _list* next;
	struct _list* prev;
	void* data;
}list;



debug_menu* game_menu = nullptr;
debug_menu* script_menu = nullptr;
debug_menu* progression_menu = nullptr;
debug_menu* level_select_menu = nullptr;




debug_menu** all_menus[] = {
    &debug_menu::root_menu,
    &game_menu,
    &script_menu,
	&progression_menu,
    &level_select_menu
};

void unlock_region(region* cur_region) {
	cur_region->flags &= 0xFE;
}

void remove_debug_menu_entry(debug_menu_entry* entry) {
	
	DWORD to_be = (DWORD)entry;
	for (auto i = 0u; i < (sizeof(all_menus) / sizeof(void*)); ++i) {

		debug_menu *cur = *all_menus[i];

		DWORD start = (DWORD) cur->entries;
		DWORD end = start + cur->used_slots * sizeof(debug_menu_entry);

		if (start <= to_be && to_be < end) {

			int index = (to_be - start) / sizeof(debug_menu_entry);

			memcpy(&cur->entries[index], &cur->entries[index + 1], cur->used_slots - (index + 1));
			memset(&cur->entries[cur->used_slots - 1], 0, sizeof(debug_menu_entry));
			cur->used_slots--;
			return;
		}
		
	}

	printf("FAILED TO DEALLOCATE AN ENTRY :S %08X\n", entry);

}

int vm_debug_menu_entry_garbage_collection_id = -1;

typedef int (*script_manager_register_allocated_stuff_callback_ptr)(void* func);
script_manager_register_allocated_stuff_callback_ptr script_manager_register_allocated_stuff_callback = (script_manager_register_allocated_stuff_callback_ptr) 0x005AFE40;

typedef int (*construct_client_script_libs_ptr)();
construct_client_script_libs_ptr construct_client_script_libs = (construct_client_script_libs_ptr) 0x0058F9C0;

void vm_debug_menu_entry_garbage_collection_callback(void* a1, list* lst) {

	list* end = lst->prev;

	for (list* cur = end->next; cur != end; cur = cur->next) {

		debug_menu_entry* entry = ((debug_menu_entry*)cur->data);
		//printf("Will delete %s %08X\n", entry->text, entry);
		remove_debug_menu_entry(entry);
	}
}

int construct_client_script_libs_hook() {

	if (vm_debug_menu_entry_garbage_collection_id == -1) {
		int res = script_manager_register_allocated_stuff_callback((void *) vm_debug_menu_entry_garbage_collection_callback);
		vm_debug_menu_entry_garbage_collection_id = res;
	}

	return construct_client_script_libs();
}

region** all_regions = (region **) 0x0095C924;
DWORD* number_of_allocated_regions = (DWORD *) 0x0095C920;

typedef const char* (__fastcall* region_get_name_ptr)(void* );
region_get_name_ptr region_get_name = (region_get_name_ptr) 0x00519BB0;

typedef int (__fastcall *region_get_district_variant_ptr)(region* );
region_get_district_variant_ptr region_get_district_variant = (region_get_district_variant_ptr) 0x005503D0;


#define TEXT_START 0x00401000
#define TEXT_END 0x00988000

DWORD old_perms = 0;
BOOL set_text_to_writable() {
    return VirtualProtect((void*)TEXT_START, TEXT_END - TEXT_START, PAGE_READWRITE, &old_perms);
}

BOOL restore_text_perms() {
    return VirtualProtect((void*)(TEXT_START), TEXT_END - TEXT_START, old_perms, &old_perms);
}



void HookFunc(DWORD callAdd, DWORD funcAdd, BOOLEAN jump, const char* reason) {

	//Only works for E8/E9 hooks	
	DWORD jmpOff = funcAdd - callAdd - 5;

	BYTE shellcode[] = { 0, 0, 0, 0, 0 };
	shellcode[0] = jump ? 0xE9 : 0xE8;

	memcpy(&shellcode[1], &jmpOff, sizeof(jmpOff));
	memcpy((void*)callAdd, shellcode, sizeof(shellcode));

	if (reason)
		printf("Hook: %08X -  %s\n", callAdd, reason);

}


void WriteDWORD(int address, DWORD newValue, const char* reason) {
	* ((DWORD *)address) = newValue;
	if (reason)
		printf("Wrote: %08X -  %s\n", address, reason);
}

typedef int (*nflSystemOpenFile_ptr)(HANDLE* hHandle, LPCSTR lpFileName, unsigned int a3, LARGE_INTEGER liDistanceToMove);
nflSystemOpenFile_ptr nflSystemOpenFile_orig = nullptr;

nflSystemOpenFile_ptr* nflSystemOpenFile_data = (nflSystemOpenFile_ptr *) 0x0094985C;

HANDLE USM_handle = INVALID_HANDLE_VALUE;

int nflSystemOpenFile(HANDLE* hHandle, LPCSTR lpFileName, unsigned int a3, LARGE_INTEGER liDistanceToMove) {

	//printf("Opening file %s\n", lpFileName);
	int ret = nflSystemOpenFile_orig(hHandle, lpFileName, a3, liDistanceToMove);


	if (strstr(lpFileName, "ultimate_spiderman.PCPACK")) {

	}
	return ret;
}

typedef int (*ReadOrWrite_ptr)(int a1, HANDLE* a2, int a3, DWORD a4, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite);
ReadOrWrite_ptr* ReadOrWrite_data = (ReadOrWrite_ptr *)0x0094986C;
ReadOrWrite_ptr ReadOrWrite_orig = nullptr;

int ReadOrWrite(int a1, HANDLE* a2, int a3, DWORD a4, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite) {

	int ret = ReadOrWrite_orig(a1, a2, a3, a4, lpBuffer, nNumberOfBytesToWrite);

	if (USM_handle == *a2) {
		printf("USM buffer was read %08X\n", (DWORD)lpBuffer);


	}
	return ret;
}

typedef void (*aeps_RenderAll_ptr)();
aeps_RenderAll_ptr aeps_RenderAll_orig = (aeps_RenderAll_ptr)0x004D9310;


unsigned int nglColor(int r, int g, int b, int a)
{
    return ( (a << 24) |  (r << 16) | (g << 8) | (b & 255) );
}

typedef void (*nglListEndScene_ptr)();
nglListEndScene_ptr nglListEndScene = (nglListEndScene_ptr) 0x00742B50;

typedef void (*nglSetClearFlags_ptr)(int);
nglSetClearFlags_ptr nglSetClearFlags2 = (nglSetClearFlags_ptr) 0x00769DB0;

void aeps_RenderAll() {
	static int cur_time = 0;
	int period = 60;
	int duration = 6 * period;
	float ratio = 1.f / period;

	uint8_t red = color_ramp_function(ratio, period, cur_time + 2 * period) + color_ramp_function(ratio, period, cur_time - 4 * period);
	uint8_t green = color_ramp_function(ratio, period, cur_time);
	uint8_t blue = color_ramp_function(ratio, period, cur_time - 2 * period);

	nglListAddString(*nglSysFont, 0.1f, 0.2f, 0.2f, nglColor(red, green, blue, 255), 1.f, 1.f, "");

	cur_time = (cur_time + 1) % duration;


	aeps_RenderAll_orig();
}
int debug_enabled = 0;
int debug_disabled = 0;

uint32_t keys[256];

void getStringDimensions(const char* str, int* width, int* height) {
	nglGetStringDimensions(*nglSysFont, str, width, height, 1.0, 1.0);
}

int getStringHeight(const char* str) {
	int height;
	nglGetStringDimensions(nglSysFont(), str, nullptr, &height, 1.0, 1.0);
	return height;
}

std::string getRealText(debug_menu_entry *entry) {
    assert(entry->render_callback != nullptr);

    auto v1 = entry->render_callback(entry);

    char a2a[256]{};
    if (v1.size() != 0) {
        auto *v7 = v1.c_str();
        auto *v4 = entry->text;
        snprintf(a2a, 255u, "%s: %s", v4, v7);
    } else {
        auto *v5 = entry->text;
        snprintf(a2a, 255u, "%s", v5);
    }

    return {a2a};
}





static constexpr DWORD MAX_ELEMENTS_PAGE = 18;



void render()
{
    auto UP_ARROW { " ^ ^ ^ " };
    auto DOWN_ARROW { " v v v " };

    int num_elements = std::min((DWORD)MAX_ELEMENTS_PAGE, current_menu->used_slots - current_menu->window_start);
    int needs_down_arrow = ((current_menu->window_start + MAX_ELEMENTS_PAGE) < current_menu->used_slots) ? 1 : 0;

    int cur_width, cur_height;
    int debug_width = 0;
    int debug_height = 0;

    auto get_and_update = [&](auto* x) {
        getStringDimensions(x, &cur_width, &cur_height);
        debug_height += cur_height;
        debug_width = std::max(debug_width, cur_width);
    };

    // printf("new size: %s %d %d (%d %d)\n", x, debug_width, debug_height, cur_width, cur_height);

    get_and_update(current_menu->title);
    get_and_update(UP_ARROW);

    int total_elements_page = needs_down_arrow ? MAX_ELEMENTS_PAGE : current_menu->used_slots - current_menu->window_start;

    for (int i = 0; i < total_elements_page; ++i) {
        debug_menu_entry* entry = &current_menu->entries[current_menu->window_start + i];
        auto cur = getRealText(entry);
        get_and_update(cur.c_str());
    }

    if (needs_down_arrow) {
        get_and_update(DOWN_ARROW);
    }

    nglQuad quad;

    int menu_x_start = 20, menu_y_start = 40;
    int menu_x_pad = 24, menu_y_pad = 18;

    nglInitQuad(&quad);
    nglSetQuadRect(&quad, menu_x_start, menu_y_start, menu_x_start + debug_width + menu_x_pad, menu_y_start + debug_height + menu_y_pad);
    nglSetQuadColor(&quad, debug_menu::has_focus ? 0xC8141414 : 0x64141414);
    nglSetQuadZ(&quad, 0.5);
    nglListAddQuad(&quad);

    int white_color = nglColor(255, 255, 255, 255);
    int yellow_color = nglColor(255, 255, 0, 255);
    int green_color = nglColor(0, 255, 0, 255);
    int pink_color = nglColor(255, 0, 255, 255);

    int render_height = menu_y_start;
    render_height += 12;
    int render_x = menu_x_start;
    render_x += 8;
    nglListAddString(nglSysFont(), render_x, render_height, 0.2f, green_color, 1.f, 1.f, current_menu->title);
    render_height += getStringHeight(current_menu->title);

    if (current_menu->window_start) {
        nglListAddString(*nglSysFont, render_x, render_height, 0.2f, pink_color, 1.f, 1.f, UP_ARROW);
    }

    render_height += getStringHeight(UP_ARROW);

    for (int i = 0; i < total_elements_page; i++) {

        int current_color = current_menu->cur_index == i ? yellow_color : white_color;

        debug_menu_entry* entry = &current_menu->entries[current_menu->window_start + i];
        auto cur = getRealText(entry);
        nglListAddString(*nglSysFont, render_x, render_height, 0.2f, current_color, 1.f, 1.f, cur.c_str());
        render_height += getStringHeight(cur.c_str());
    }

    if (needs_down_arrow) {
        nglListAddString(*nglSysFont, render_x, render_height, 0.2f, pink_color, 1.f, 1.f, DOWN_ARROW);
        render_height += getStringHeight(DOWN_ARROW);
    }
}





void debug_menu_patch()
{
    
    if (debug_enabled) {
        render();
    }
    if (debug_disabled) {
        render();

    }

    nglListEndScene();
}





typedef int (*wndHandler_ptr)(HWND, UINT, WPARAM, LPARAM);
wndHandler_ptr orig_WindowHandler = (wndHandler_ptr) 0x005941A0;

int WindowHandler(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

	switch (Msg) {
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYUP:
	case WM_KEYDOWN:
	case WM_INPUT:
		printf("swallowed keypress\n");
		return 0;
	}

	return orig_WindowHandler(hwnd, Msg, wParam, lParam);

}

/*
	STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) PURE;
	STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD) PURE;

*/

typedef int (__stdcall* GetDeviceState_ptr)(IDirectInputDevice8*, DWORD, LPVOID);
GetDeviceState_ptr GetDeviceStateOriginal = nullptr;

typedef int (__fastcall* game_pause_unpause_ptr)(void* );
game_pause_unpause_ptr game_pause = (game_pause_unpause_ptr) 0x0054FBE0;
game_pause_unpause_ptr game_unpause = (game_pause_unpause_ptr) 0x0053A880;

typedef int (__fastcall* game_get_cur_state_ptr)(void* );
game_get_cur_state_ptr game_get_cur_state = (game_get_cur_state_ptr) 0x005363D0;

typedef int (__fastcall* world_dynamics_system_remove_player_ptr)(void* , void* edx, int number);
world_dynamics_system_remove_player_ptr world_dynamics_system_remove_player = (world_dynamics_system_remove_player_ptr) 0x00558550;

typedef int (__fastcall* world_dynamics_system_add_player_ptr)(void* , void* edx, mString* str);
world_dynamics_system_add_player_ptr world_dynamics_system_add_player = (world_dynamics_system_add_player_ptr) 0x0055B400;

typedef int (*entity_teleport_abs_po_ptr)(DWORD, float*, int one);
entity_teleport_abs_po_ptr entity_teleport_abs_po = (entity_teleport_abs_po_ptr) 0x004F3890;

typedef DWORD* (__fastcall* ai_ai_core_get_info_node_ptr)(DWORD* , void* edx, int a2, char a3);
ai_ai_core_get_info_node_ptr ai_ai_core_get_info_node = (ai_ai_core_get_info_node_ptr) 0x006A3390;


struct vm_thread {
	uint8_t unk[0xC];
	script_instance *inst;
	vm_executable *ex;
};

struct vm_stack {
	uint8_t unk[0x184];
	DWORD stack_ptr;
	vm_thread* thread;

    void push(void* data, DWORD size) {
        memcpy((void *)this->stack_ptr, data, size);
        this->stack_ptr += size;
    }

    void pop(DWORD size) {
        this->stack_ptr -= size;
    }

};


uint8_t __stdcall slf__debug_menu_entry__set_handler__str(vm_stack *stack, void* unk) {

	stack->pop(8);

	void** params = (void**)stack->stack_ptr;

	debug_menu_entry *entry = static_cast<decltype(entry)>(params[0]);
	const char *scrpttext = static_cast<char *>(params[1]);

	string_hash strhash {scrpttext};

	script_instance *instance = stack->thread->inst;
    entry->set_script_handler(instance, mString {scrpttext});
	
	return true;
}

uint8_t __stdcall slf__destroy_debug_menu_entry__debug_menu_entry(vm_stack* function, void* unk) {

	function->pop(4);

	debug_menu_entry** entry = (decltype(entry)) function->stack_ptr;

	remove_debug_menu_entry(*entry);

	return 1;
}

void sub_65BB36(script_library_class::function *func, vm_stack *stack, char *a3, int a4)
{
    for ( auto i = 0; i < a4; ++i )
    {
        if ( *bit_cast<DWORD *>(&a3[4 * i]) == 0x7BAD05CF )
        {
            printf("uninitialized parameters in call to 0x%08X", func->m_vtbl);

            //v5 = j_vm_stack::get_thread(stack);
            //vm_thread::slf_error(v5, v6);

            assert(0 && "uninitialized parameters in call to script library function");
        }
    }
}

uint8_t __fastcall slf__create_progression_menu_entry(script_library_class::function *func, void *, vm_stack *stack, void *unk) {

	stack->pop(8);

    auto *stack_ptr = bit_cast<char *>(stack->stack_ptr);
    sub_65BB36(func, stack, stack_ptr, 2);

	char** strs = (char **)stack->stack_ptr;

	printf("Entry: %s -> %s\n", strs[0], strs[1]);

	string_hash strhash {strs[1]};

	script_instance *instance = stack->thread->inst;

	debug_menu_entry entry {strs[0]};
    entry.set_script_handler(instance, {strs[1]});

	progression_menu->add_entry(&entry);
	
	int push = 0;
	stack->push(&push, sizeof(push));
	return true;
}

void script_list_entry(debug_menu* parent,vm_stack* stack)
{

	script_instance* instance
    = stack->thread->inst;
printf("Total funcs: %d\n", instance->m_parent->total_funcs);

auto* abort_mission_entry = create_menu_entry(mString { "ABORT MISSION" });
const mString v1 { "progression_mission_aborted()" };
abort_mission_entry->set_script_handler(instance, { v1 });

script_menu->add_entry(abort_mission_entry);

auto* start_ambient_music_entry = create_menu_entry(mString { "START AMBIENT MUSIC" });
const mString v2 { "ambient_music_start()" };
start_ambient_music_entry->set_script_handler(instance, { v2 });

script_menu->add_entry(start_ambient_music_entry);

}


bool __fastcall slf__create_debug_menu_entry(script_library_class::function *func, void *, vm_stack* stack, void* unk)
{
	stack->pop(4);

    auto *stack_ptr = bit_cast<char *>(stack->stack_ptr);
    sub_65BB36(func, stack, stack_ptr, 1);
	char** strs = bit_cast<char **>(stack->stack_ptr);
    script_list_entry(debug_menu::root_menu, stack);
	//printf("Entry: %s ", strs[0]);

	debug_menu_entry entry {};
	entry.entry_type = UNDEFINED;
	strcpy(entry.text, strs[0]);
    

    printf("entry.text = %s\n", entry.text);

	script_instance *instance = stack->thread->inst;
    printf("Total funcs: %d\n", instance->m_parent->total_funcs);



	void *res = add_debug_menu_entry(script_menu, &entry);

	script_executable *se = stack->thread->ex->owner->parent;
    printf("total_script_objects = %d\n", se->total_script_objects);
    for (auto i = 0; i < se->total_script_objects; ++i) {
        auto *so = se->field_28[i];
        printf("Name of script_object = %s\n", so->name.to_string());

        for (auto i = 0; i < so->total_funcs; ++i) {
            printf("Func name: %s\n", so->funcs[i]->name.to_string());
        }

        printf("\n");
    }

	se->add_allocated_stuff(vm_debug_menu_entry_garbage_collection_id, (int) res, 0);

	//printf("%08X\n", res);

	int push = (int) res;
	stack->push(&push, sizeof(push));
	return 1;
}

DWORD modulo(int num, DWORD mod) {
	if (num >= 0) {
		return num % mod;
	}

	int absolute = abs(num);
	if (absolute % mod == 0)
		return 0;

	return mod - absolute % mod;
}


void menu_go_down() {


	if ((current_menu->window_start + MAX_ELEMENTS_PAGE) < current_menu->used_slots) {

		if (current_menu->cur_index < MAX_ELEMENTS_PAGE / 2)
			++current_menu->cur_index;
		else
			++current_menu->window_start;
	}
	else {

		int num_elements = std::min((DWORD) MAX_ELEMENTS_PAGE, current_menu->used_slots - current_menu->window_start);
		current_menu->cur_index = modulo(current_menu->cur_index + 1, num_elements);
		if (current_menu->cur_index == 0)
			current_menu->window_start = 0;
	}
}

void menu_go_up() {

	int num_elements = std::min( (DWORD) MAX_ELEMENTS_PAGE, current_menu->used_slots - current_menu->window_start);
	if (current_menu->window_start) {


		if (current_menu->cur_index > MAX_ELEMENTS_PAGE / 2)
			current_menu->cur_index--;
		else
			current_menu->window_start--;

	}
	else {

		int num_elements = std::min(MAX_ELEMENTS_PAGE, current_menu->used_slots - current_menu->window_start);
		current_menu->cur_index = modulo(current_menu->cur_index - 1, num_elements);
		if (current_menu->cur_index == (num_elements - 1))
			current_menu->window_start = current_menu->used_slots - num_elements;

	}

}

typedef enum {
	MENU_SELECT,
    MENU_TOGGLE,
    MENU_START,
	MENU_ACCEPT,
	MENU_BACK,

	MENU_UP,
	MENU_DOWN,
	MENU_LEFT,
	MENU_RIGHT,


	MENU_KEY_MAX
} MenuKey;

uint32_t controllerKeys[MENU_KEY_MAX];

int get_menu_key_value(MenuKey key, int keyboard) {
	if (keyboard) {

		int i = 0;
		switch (key) {
                case MENU_SELECT:
				i = DIK_X;
				break;
                case MENU_TOGGLE:
                    i = DIK_Z;
                    break;
			case MENU_ACCEPT:
				i = DIK_RETURN;
				break;
			case MENU_BACK:
				i = DIK_ESCAPE;
				break;

			case MENU_UP:
				i = DIK_UPARROW;
				break;
			case MENU_DOWN:
				i = DIK_DOWNARROW;
				break;
			case MENU_LEFT:
				i = DIK_LEFTARROW;
				break;
			case MENU_RIGHT:
				i = DIK_RIGHTARROW;
				break;
		}
		return keys[i];
	}



	return controllerKeys[key];
}


int is_menu_key_pressed(MenuKey key, int keyboard) {
	return (get_menu_key_value(key, keyboard) == 2);
}

int is_menu_key_clicked(MenuKey key, int keyboard) {
	return get_menu_key_value(key, keyboard);
}

void GetDeviceStateHandleKeyboardInput(LPVOID lpvData) {
	BYTE* keysCurrent = (BYTE *) lpvData;

	for (int i = 0; i < 256; i++) {

		if (keysCurrent[i]) {
			keys[i]++;
		}
		else {
			keys[i] = 0;
		}
	}

	
}

void read_and_update_controller_key_button(LPDIJOYSTATE2 joy, int index, MenuKey key) {
	int res = 0;
	if (joy->rgbButtons[index]) {
		++controllerKeys[key];
	}
	else {
		controllerKeys[key] = 0;
	}
}


void read_and_update_controller_key_dpad(LPDIJOYSTATE2 joy, int angle, MenuKey key) {
	
	if (joy->rgdwPOV[0] == 0xFFFFFFFF)
		controllerKeys[key] = 0;
	else
		controllerKeys[key] = (joy->rgdwPOV[0] == angle) ? controllerKeys[key] + 1 : 0;
}


void GetDeviceStateHandleControllerInput(LPVOID lpvData) {
	LPDIJOYSTATE2 joy = (decltype(joy)) lpvData;

	read_and_update_controller_key_button(joy, 1, MENU_ACCEPT);
	read_and_update_controller_key_button(joy, 2, MENU_BACK);
        read_and_update_controller_key_button(joy, 11, MENU_TOGGLE);
        read_and_update_controller_key_button(joy, 8, MENU_START);
        read_and_update_controller_key_button(joy, 9, MENU_SELECT);

	read_and_update_controller_key_dpad(joy, 0, MENU_UP);
	read_and_update_controller_key_dpad(joy, 9000, MENU_RIGHT);
	read_and_update_controller_key_dpad(joy, 18000, MENU_DOWN);
	read_and_update_controller_key_dpad(joy, 27000, MENU_LEFT);
}

typedef int (*resource_manager_can_reload_amalgapak_ptr)(void);
resource_manager_can_reload_amalgapak_ptr resource_manager_can_reload_amalgapak = (resource_manager_can_reload_amalgapak_ptr) 0x0053DE90;

typedef void (*resource_manager_reload_amalgapak_ptr)(void);
resource_manager_reload_amalgapak_ptr resource_manager_reload_amalgapak = (resource_manager_reload_amalgapak_ptr) 0x0054C2E0;

struct mission_t
{
    std::string field_0;
    const char *field_C;
    int field_10;
    int field_14;
    int m_district_id;
};

std::vector<mission_t> menu_missions; 

void mission_unload_handler(debug_menu_entry *a1)
{
    auto *v1 = mission_manager::s_inst();
    v1->prepare_unload_script();

	close_debug();
}








void empty_handler(debug_menu_entry* a1)
{
}

void mission_select_handler(debug_menu_entry* entry)
{
    auto v1 = (int)entry->m_id;
    auto v7 = &menu_missions.at(v1);
    auto v6 = v7->field_C;
    auto v5 = v7->field_14;
    auto* v4 = v7->field_0.c_str();
    auto v3 = v7->field_10;
    auto* v2 = mission_manager::s_inst();
    v2->force_mission(v3, v4, v5, v6);
    close_debug();
}





void create_game_flags_menu(debug_menu *parent);





void create_game_flags_menu(debug_menu* parent);

void _populate_missions()
{
    auto handle_table = [](mission_table_container* table, int district_id) -> void {
        std::vector<mission_table_container::script_info> script_infos {};

        if (table != nullptr) {
            table->append_script_info(&script_infos);
        }

        for (auto& info : script_infos) {
            mString a2 { "pk_" };
            auto v19 = a2 + info.field_0;
            auto* v11 = v19.c_str();
            auto key = create_resource_key_from_path(v11, 25);
            if (resource_manager::get_pack_file_stats(key, nullptr, nullptr, nullptr)) {
                mission_t mission {};
                mission.field_0 = info.field_0;
                mission.field_10 = district_id;
                mission.field_14 = info.field_8;

                mission.field_C = info.field_4->get_script_data_name();
                menu_missions.push_back(mission);

                auto v47 = [](mission_t& mission) -> mString {
                    if (mission.field_C != nullptr) {
                        auto* v17 = mission.field_C;
                        auto* v14 = mission.field_0.c_str();
                        mString str { 0, "%s (%s)", v14, v17 };
                        return str;
                    }

                    auto v18 = mission.field_14;
                    auto* v15 = mission.field_0.c_str();
                    mString str { 0, "%s (%d)", v15, v18 };
                    return str;
                }(mission);

                printf(v47.c_str());
            }
        }
    };

    auto* v2 = mission_manager::s_inst();
    auto count = v2->get_district_table_count();
    printf("%s %d", "table_count = ", count);

    {
        auto* v3 = mission_manager::s_inst();
        auto* table = v3->get_global_table();

        handle_table(table, 0);
    }

    std::for_each(&v2->m_district_table_containers[0], &v2->m_district_table_containers[0] + count, [&handle_table](auto* table) {
        auto* reg = table->get_region();
        auto& v6 = reg->get_name();
        fixedstring32 v53 { v6.to_string() };

        auto district_id = reg->get_district_id();

        // sp_log("%d %s", i, v53.to_string());

        handle_table(table, district_id);
    });

    assert(0);
}



void populate_missions_menu(debug_menu_entry* entry)
{
    menu_missions = {};
    if (resource_manager::can_reload_amalgapak())
    {
        resource_manager::reload_amalgapak();
    }

    auto* head_menu = create_menu("", debug_menu::sort_mode_t::ascending);
    entry->set_submenu(head_menu);

    // Allocate the entry-storage block for head_menu the same way
    // create_devopt_ints_menu / populate_warp_menu do. Without this,
    // head_menu has no place to hold the entries we're about to
    // add_entry() into it, so the submenu either shows nothing or
    // crashes on enter. This was the actual "Missions menu not
    // working" bug — the populate was running, but the entries had
    // nowhere to land.
    debug_menu_entry block_owner;
    debug_menu_entry* block = block_owner.alloc_block(head_menu, 4);
    block[0] = debug_menu_entry{ head_menu };

    auto* mission_unload_entry = create_menu_entry(mString{ "UNLOAD CURRENT MISSION" });

    mission_unload_entry->set_game_flags_handler(mission_unload_handler);
    head_menu->add_entry(mission_unload_entry);

    // Guard against entering Missions before a world is loaded —
    // mission_manager::s_inst() is the singleton populated by the
    // world bootstrap, and it's null on the title screen.
    auto* v2 = mission_manager::s_inst();
    if (v2 == nullptr) {
        return;
    }
    auto v58 = v2->get_district_table_count();
    for (auto i = -1; i < v58; ++i)
    {
        fixedstring32 v53{};
        int district_id;
        mission_table_container* table = nullptr;
        if (i == -1)
        {
            table = v2->get_global_table();
            v53 = fixedstring32{ "global" };
            district_id = 0;
        }
        else
        {
            table = v2->get_district_table(i);
            auto* reg = table->get_region();
            v53 = reg->get_name();

            district_id = reg->get_district_id();

            auto* v25 = create_menu(v53.to_string(), debug_menu::sort_mode_t::ascending);

            auto* v26 = create_menu_entry(v25);

            head_menu->add_entry(v26);
        }

        std::vector<mission_table_container::script_info> script_infos;

        if (table != nullptr)
        {
            auto res = table->append_script_info(&script_infos);
            assert(res);
        }

        for (auto& info : script_infos)
        {
            auto v50 = menu_missions.size();
            const auto v19 = mString{ "pk_" } + info.field_0;
            auto* v11 = v19.c_str();
            auto key = create_resource_key_from_path(v11, 25);
            if (resource_manager::get_pack_file_stats(key, nullptr, nullptr, nullptr))
            {
                mission_t mission{};
                mission.field_0 = info.field_0;
                mission.m_district_id = district_id;
                mission.field_14 = info.field_8;

                mission.field_C = info.field_4->get_script_data_name();
                menu_missions.push_back(mission);

                mString v47{};
                if (mission.field_C != nullptr)
                {
                    auto* v17 = mission.field_C;
                    auto* v14 = mission.field_0.c_str();
                    v47 = mString{ 0, "%s (%s)", v14, v17 };
                }
                else
                {
                    auto v18 = mission.field_14;
                    auto* v15 = mission.field_0.c_str();
                    v47 = mString{ 0, "%s (%d)", v15, v18 };
                }

                auto* v46 = create_menu_entry(v47);
                v46->set_id(v50);
                v46->set_game_flags_handler(mission_select_handler);
                head_menu->add_entry(v46);
            }
        }
    }
}




void create_missions_menu(debug_menu* parent)
{
    // Mirror create_warp_menu's pattern: a leaf entry with NO pre-built
    // submenu, lazily populated on first enter via the game-flags
    // handler. Pre-building a "Missions" submenu and *then* having
    // populate_missions_menu call entry->set_submenu(head_menu) caused
    // the menu system to descend into the empty pre-built submenu
    // BEFORE the populate callback fired (depending on enter-vs-render
    // ordering in the engine's menu loop), so the user landed on an
    // empty list and clicks went nowhere.
    debug_menu_entry entry{ mString{ "Missions" } };
    entry.set_submenu(nullptr);
    entry.set_game_flags_handler(populate_missions_menu);
    parent->add_entry(&entry);
}

void copy_str(char* Dest, const char* Source, size_t Count)
{
    strncpy(Dest, Source, Count);
    Dest[Count - 1] = '\0';
}

void replace_if_find(char* begin, char* end, const char& a3, const char& a4)
{
    if constexpr (0) {
        for (auto* i = begin; i != end; ++i) {
            if (*i == a3) {
                *i = a4;
            }
        }
    } else {
        std::replace(begin, end, a3, a4);
    }
}



void disable_physics()
{
    debug_enabled = 1;
    game_unpause(g_game_ptr());
    current_menu = current_menu;
    g_game_ptr()->enable_physics(false);
}

void enable_physics()
{
    debug_disabled = 1;
    game_unpause(g_game_ptr());
    current_menu = current_menu;
    g_game_ptr()->enable_physics(true);
}

void custom()
{
    debug_disabled = 1;
    debug_enabled = 1;
    game_unpause(g_game_ptr());
    current_menu = current_menu;
    g_game_ptr()->enable_physics(false);

    
}



    








void menu_setup(int game_state, int keyboard) {

    //debug menu stuff
    if (is_menu_key_pressed(MENU_START, keyboard) && (game_state == 6 || game_state == 7)) {

        PauseMenuSystem* pause_menu = pause_menu_system_ptr;
        if (pause_menu == nullptr) return;
        if (!debug_enabled && game_state == 6) {
            game_unpause(g_game_ptr());
            debug_enabled = !debug_enabled;
            current_menu = debug_menu::root_menu;
            pause_menu->Activate(0, true);
            custom();
        }


    }

    if (is_menu_key_pressed(MENU_SELECT, keyboard) && (game_state == 6 || game_state == 7)) {

        PauseMenuSystem* pause_menu = pause_menu_system_ptr;
        if (pause_menu == nullptr) return;

        if (!debug_disabled && game_state == 7) {
            game_unpause(g_game_ptr());
            debug_disabled = !debug_disabled;
            current_menu = debug_menu::root_menu;
            pause_menu->Deactivate();
            custom();

        }
        else  if (!debug_disabled && game_state == 7) {
            game_unpause(g_game_ptr());
            debug_disabled = !debug_disabled;
            current_menu = debug_menu::root_menu;
            pause_menu->Deactivate();
            disable_physics();

        }






        if (level_select_menu->used_slots == 0)
        {
            create_level_select_menu(level_select_menu);

        }
    }
}
void menu_input_handler(int keyboard, int SCROLL_SPEED)
{
    if (is_menu_key_clicked(MENU_DOWN, keyboard)) {

        int key_val = get_menu_key_value(MENU_DOWN, keyboard);
        if (key_val == 1) {
            menu_go_down();
        } else if ((key_val >= SCROLL_SPEED) && (key_val % SCROLL_SPEED == 0)) {
            menu_go_down();
        }
    } else if (is_menu_key_clicked(MENU_UP, keyboard)) {

        int key_val = get_menu_key_value(MENU_UP, keyboard);
        if (key_val == 1) {
            menu_go_up();
        } else if ((key_val >= SCROLL_SPEED) && (key_val % SCROLL_SPEED == 0)) {
            menu_go_up();
        }
    } else if (is_menu_key_pressed(MENU_ACCEPT, keyboard)) {
        auto* entry = &current_menu->entries[current_menu->window_start + current_menu->cur_index];
        assert(entry != nullptr);
        entry->on_select(1.0);

        // current_menu->handler(entry, ENTER);
    } else if (is_menu_key_pressed(MENU_BACK, keyboard)) {
        current_menu->go_back();
    } else if (is_menu_key_clicked(MENU_LEFT, keyboard)) { // Use is_menu_key_clicked here
        debug_menu_entry* cur = &current_menu->entries[current_menu->window_start + current_menu->cur_index];
        int key_val = get_menu_key_value(MENU_LEFT, keyboard);
        if (key_val == 1) {
            cur->on_change(-1.0, false);
        } else if ((key_val >= SCROLL_SPEED) && (key_val % SCROLL_SPEED == 0)) {
            cur->on_change(-1.0, false);
        }
    } else if (is_menu_key_clicked(MENU_RIGHT, keyboard)) { // Use is_menu_key_clicked here
        debug_menu_entry* cur = &current_menu->entries[current_menu->window_start + current_menu->cur_index];
        int key_val = get_menu_key_value(MENU_RIGHT, keyboard);
        if (key_val == 1) {
            cur->on_change(1.0, true);
        } else if ((key_val >= SCROLL_SPEED) && (key_val % SCROLL_SPEED == 0)) {
            cur->on_change(1.0, true);
        }
    }

    debug_menu_entry* highlighted = &current_menu->entries[current_menu->window_start + current_menu->cur_index];
    assert(highlighted->frame_advance_callback != nullptr);
    highlighted->frame_advance_callback(highlighted);
}

HRESULT __stdcall GetDeviceStateHook(IDirectInputDevice8* self, DWORD cbData, LPVOID lpvData)
{

    HRESULT res = GetDeviceStateOriginal(self, cbData, lpvData);

    // printf("cbData %d %d %d\n", cbData, sizeof(DIJOYSTATE), sizeof(DIJOYSTATE2));

    // keyboard time babyyy
    if (cbData == 256 || cbData == sizeof(DIJOYSTATE2)) {

        if (cbData == 256)
            GetDeviceStateHandleKeyboardInput(lpvData);
        else if (cbData == sizeof(DIJOYSTATE2))
            GetDeviceStateHandleControllerInput(lpvData);

        int game_state = 0;
        if (g_game_ptr()) {
            game_state = game_get_cur_state(g_game_ptr());
        }

        // printf("INSERT %d %d %c\n", keys[DIK_INSERT], game_state, debug_enabled ? 'y' : 'n');

        int keyboard = cbData == 256;
        menu_setup(game_state, keyboard);

        if (debug_enabled) {
            menu_input_handler(keyboard, 5);
        }
    }

    if (debug_enabled) {
        memset(lpvData, 0, cbData);
    }

    // printf("Device State called %08X %d\n", this, cbData);

    return res;
}
typedef HRESULT(__stdcall* GetDeviceData_ptr)(IDirectInputDevice8*, DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD);
GetDeviceData_ptr GetDeviceDataOriginal = nullptr;

HRESULT __stdcall GetDeviceDataHook(IDirectInputDevice8* self, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags) {

	HRESULT res = GetDeviceDataOriginal(self, cbObjectData, rgdod, pdwInOut, dwFlags);

	printf("data\n");
	if (res == DI_OK) {

		printf("All gud\n");
		for (int i = 0; i < *pdwInOut; i++) {


			if (LOBYTE(rgdod[i].dwData) > 0) {

				if (rgdod[i].dwOfs == DIK_ESCAPE) {

					printf("Pressed escaped\n");
					__debugbreak();
				}
			}
		}
	}
	//printf("Device Data called %08X\n", this);

	return res;
}



typedef HRESULT(__stdcall* IDirectInput8CreateDevice_ptr)(IDirectInput8W*, const GUID*, LPDIRECTINPUTDEVICE8W*, LPUNKNOWN);
IDirectInput8CreateDevice_ptr createDeviceOriginal = nullptr;

HRESULT __stdcall IDirectInput8CreateDeviceHook(IDirectInput8W* self, const GUID* guid, LPDIRECTINPUTDEVICE8W* device, LPUNKNOWN unk)
{

    // printf("CreateDevice %d %d %d %d %d %d %d\n", *guid, GUID_SysMouse, GUID_SysKeyboard, GUID_SysKeyboardEm, GUID_SysKeyboardEm2, GUID_SysMouseEm, GUID_SysMouseEm2);
    printf("Guid = {%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}\n",
        guid->Data1, guid->Data2, guid->Data3,
        guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3],
        guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7]);

    HRESULT res = createDeviceOriginal(self, guid, device, unk);

    if (IsEqualGUID(GUID_SysMouse, *guid))
        return res; // ignore mouse

    if (IsEqualGUID(GUID_SysKeyboard, *guid)) {
        printf("Found the keyboard");
    } else {
        printf("Hooking something different...maybe a controller");
    }

    if (GetDeviceStateOriginal == nullptr) {
        GetDeviceStateOriginal = (GetDeviceState_ptr)
            HookVTableFunction((void*)*device, (void*)GetDeviceStateHook, 9);
    }

    if (GetDeviceDataOriginal == nullptr) {
        GetDeviceDataOriginal = (GetDeviceData_ptr)HookVTableFunction((void*)*device,
            (void*)GetDeviceDataHook,
            10);
    }

    return res;
}

/*
BOOL CALLBACK EnumDevices(LPCDIDEVICEINSTANCE lpddi, LPVOID buffer) {

	wchar_t wGUID[40] = { 0 };
	char cGUID[40] = { 0 };

	//printf("%d\n", lpddi->guidProduct);
}
*/

typedef HRESULT(__stdcall* DirectInput8Create_ptr)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter);
HRESULT __stdcall HookDirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
    DirectInput8Create_ptr caller = (decltype(caller)) * (void**)0x00987944;
    HRESULT res = caller(hinst, dwVersion, riidltf, ppvOut, punkOuter);

    IDirectInput8* iDir = (IDirectInput8*)(*ppvOut);

    if (createDeviceOriginal == nullptr) {
        createDeviceOriginal = (IDirectInput8CreateDevice_ptr)
            HookVTableFunction((void*)iDir, (void*)IDirectInput8CreateDeviceHook, 3);
    }

    return res;
}



/*
void update_state() {

	while (1) {
		OutputDebugStringA("PILA %d", 6);
	}
}
*/

typedef int(__fastcall* game_handle_game_states_ptr)(game* , void* edx, void* a2);
game_handle_game_states_ptr game_handle_game_states_original = (game_handle_game_states_ptr) 0x0055D510;

int __fastcall game_handle_game_states(game* self, void* edx, void* a2) {

	if (!g_game_ptr()) {
		g_game_ptr() = self;
	}

	/*
	if (game_get_cur_state(this) == 14)
		__debugbreak();
		*/


		//printf("Current state %d %08X\n", game_get_cur_state(this), g_game_ptr);

	return game_handle_game_states_original(self, edx, a2);
}




typedef DWORD(__fastcall* ai_hero_base_state_check_transition_ptr)(DWORD* , void* edx, DWORD* a2, int a3);
ai_hero_base_state_check_transition_ptr ai_hero_base_state_check_transition = (ai_hero_base_state_check_transition_ptr) 0x00478D80;

DWORD __fastcall ai_hero_base_state_check_transition_hook(DWORD* self, void* edx, DWORD* a2, int a3) {
	ai_current_player = self;
	return ai_hero_base_state_check_transition(self, edx, a2, a3);
}

typedef DWORD* (__fastcall* get_info_node_ptr)(void*, void* edx, int a2, char a3);
get_info_node_ptr get_info_node = (get_info_node_ptr) 0x006A3390;

DWORD* __fastcall get_info_node_hook(void* self, void* edx, int a2, char a3) {

	DWORD* res = get_info_node(self, edx, a2, a3);

	fancy_player_ptr = res;
	return res;
}

void init_shadow_targets()
{
    debug_menu::init();

    CDECL_CALL(0x00592E80);
}


typedef void (__fastcall* resource_pack_streamer_load_internal_ptr)(void* self, void* edx, char* str, int a3, int a4, int a5);
resource_pack_streamer_load_internal_ptr resource_pack_streamer_load_internal = (resource_pack_streamer_load_internal_ptr) 0x0054C580;

void __fastcall resource_pack_streamer_load_internal_hook(void* self, void* edx, char* str, int a3, int a4, int a5) {

	resource_pack_streamer_load_internal(self, edx, str, a3, a4, a5);
}



unsigned int hook_controlfp(unsigned int, unsigned int) {
    return {};
}




static constexpr uint32_t NOP = 0x90;

void set_nop(ptrdiff_t address, size_t num_bytes) {
    for (size_t i = 0u; i < num_bytes; ++i) {
        *bit_cast<uint8_t*>(static_cast<size_t>(address) + i) = NOP;
    }
}

#define REDIRECT_WITH_NOP(addr, my_func)                                          \
    {                                                                             \
        *(uint8_t *) addr = 0xE8;                                                 \
        *(uint32_t *) ((uint8_t *) (addr + 1)) = ((uint32_t) my_func) - addr - 5; \
        *(uint8_t *) (addr + 5) = NOP;                                            \
        sp_log("Patched function sub_%08X with %s", addr, #my_func);              \
    }

#define MOVE(addr, my_func)                                            \
    {                                                                  \
        *bit_cast<uint8_t *>(addr) = 0xB9;                             \
        *(uint32_t *) ((uint8_t *) (addr + 1)) = ((uint32_t) my_func); \
        *bit_cast<uint8_t *>(addr + 5) = NOP;                          \
    }






BOOL install_patches()
{

    //fix invalid float operation
    {
        set_nop(0x005AC34C, 6);

        HookFunc(0x005AC347, (DWORD) hook_controlfp, 0, "Patching call to controlfp");
    }


    REDIRECT(0x005E10EE, init_shadow_targets);


          {
              DWORD hookDirectInputAddress = (DWORD)HookDirectInput8Create;
              REDIRECT(0x008218B0, hookDirectInputAddress);
              set_nop(0x008218B5, 1);
              printf("Patching the DirectInput8Create call\n");

              

            //     REDIRECT(0x00820DC0, GetDeviceStateHook);
          //    set_nop(0x00820DC5, 1);

          }

          printf("Redirects have been installed2\n");

    input_mgr_patch();

    //PauseMenuSystem_patch();

   //resource_amalgapak_header_patch();

    IGOZoomOutMap_patch();

    mouselook_controller_patch();

   spider_monkey_patch();

    message_board_patch();
    
    wds_patch();

    pause_menu_root_patch();

    ngl_patch();

    game_patch();

    pause_menu_status_patch();

  FrontEndMenuSystem_patch();







    slab_allocator_patch();

    HookFunc(0x0052B4BF, (DWORD) spider_monkey::render, 0, "Patching call to spider_monkey::render");


	HookFunc(0x004EACF0, (DWORD)aeps_RenderAll, 0, "Patching call to aeps::RenderAll");

	HookFunc(0x0052B5D7, (DWORD)debug_menu_patch, 0, "Hooking nglListEndScene to inject debug menu");



    

	//save orig ptr
	nflSystemOpenFile_orig = *nflSystemOpenFile_data;
	*nflSystemOpenFile_data = &nflSystemOpenFile;
	printf("Replaced nflSystemOpenFile %08X -> %08X\n", (DWORD)nflSystemOpenFile_orig, (DWORD)&nflSystemOpenFile);


	ReadOrWrite_orig = *ReadOrWrite_data;
	*ReadOrWrite_data = &ReadOrWrite;
	printf("Replaced ReadOrWrite %08X -> %08X\n", (DWORD)ReadOrWrite_orig, (DWORD)&ReadOrWrite);




	HookFunc(0x0055D742, (DWORD)game_handle_game_states, 0, "Hooking handle_game_states");

	/*
	WriteDWORD(0x00877524, ai_hero_base_state_check_transition_hook, "Hooking check_transition for peter hooded");
	WriteDWORD(0x00877560, ai_hero_base_state_check_transition_hook, "Hooking check_transition for spider-man");
	WriteDWORD(0x0087759C, ai_hero_base_state_check_transition_hook, "Hooking check_transition for venom");
	*/

	HookFunc(0x00478DBF, (DWORD) get_info_node_hook, 0, "Hook get_info_node to get player ptr");


	WriteDWORD(0x0089C710, (DWORD) slf__create_progression_menu_entry, "Hooking first ocurrence of create_progession_menu_entry");
	WriteDWORD(0x0089C718, (DWORD) slf__create_progression_menu_entry, "Hooking second  ocurrence of create_progession_menu_entry");


	WriteDWORD(0x0089AF70, (DWORD) slf__create_debug_menu_entry, "Hooking first ocurrence of create_debug_menu_entry");
	WriteDWORD(0x0089C708, (DWORD) slf__create_debug_menu_entry, "Hooking second  ocurrence of create_debug_menu_entry");


	HookFunc(0x005AD77D, (DWORD) construct_client_script_libs_hook, 0, "Hooking construct_client_script_libs to inject my vm");

	WriteDWORD(0x0089C720, (DWORD) slf__destroy_debug_menu_entry__debug_menu_entry, "Hooking destroy_debug_menu_entry");
	WriteDWORD(0x0089C750, (DWORD) slf__debug_menu_entry__set_handler__str, "Hooking set_handler");

	//HookFunc(0x0054C89C, resource_pack_streamer_load_internal_hook, 0, "Hooking resource_pack_streamer::load_internal to inject interior loading");

	//HookFunc(0x005B87E0, os_developer_options, 1, "Hooking os_developer_options::get_flag");

	/*

	DWORD* windowHandler = 0x005AC48B;
	*windowHandler = WindowHandler;

	DWORD* otherHandler = 0x0076D6D1;
	*otherHandler = 0;

	*/

    return TRUE;
}

    bool equal(float first1, float last1)
{
    return 1.0;
}

void close_debug()
{
    debug_enabled = 0;
    debug_disabled = 0;
    game_unpause(g_game_ptr());
    g_game_ptr()->enable_physics(true);
}


void handle_debug_entry(debug_menu_entry* entry, custom_key_type) {
	current_menu = entry->m_value.p_menu;
}

typedef bool (__fastcall *entity_tracker_manager_get_the_arrow_target_pos_ptr)(void *, void *, vector3d *);
entity_tracker_manager_get_the_arrow_target_pos_ptr entity_tracker_manager_get_the_arrow_target_pos = (entity_tracker_manager_get_the_arrow_target_pos_ptr) 0x0062EE10;


void debug_menu_enabled()
{
    debug_enabled = 1;
    game_unpause(g_game_ptr());
    g_game_ptr()->enable_physics(true);
}


void handle_devopt_entry(debug_menu_entry* entry, custom_key_type key_type)
{
    printf("handle_game_entry = %s, %s, entry_type = %s\n", entry->text, to_string(key_type), to_string(entry->entry_type));

    if (key_type == ENTER) {
        switch (entry->entry_type) {
        case UNDEFINED: {
            if (entry->m_game_flags_handler != nullptr) {
                entry->m_game_flags_handler(entry);
            }
            break;
        }
        case BOOLEAN_E:
        case POINTER_BOOL: {
            auto v3 = entry->get_bval();
            entry->set_bval(!v3, true);
            break;
        }
        case POINTER_MENU: {
            if (entry->m_value.p_menu != nullptr) {
                current_menu = entry->m_value.p_menu;
            }
            return;
        }
        default:
            break;
        }
    } else if (key_type == LEFT) {
        entry->on_change(-1.0, false);
        auto v3 = entry->get_bval();
        entry->set_bval(!v3, true);
    } else if (key_type == RIGHT) {
        entry->on_change(1.0, true);
        auto v3 = entry->get_bval();
        entry->set_bval(!v3, true);
    }
}

 









void create_devopt_ints_menu(debug_menu* parent)
{
    assert(parent != nullptr);

    auto* v22 = create_menu("Devopt Ints", handle_game_entry, 300);

    for (auto idx = 0u; idx < NUM_OPTIONS; ++idx)
    {
        auto* v21 = get_option(idx);
        switch (v21->m_type)
        {
        case game_option_t::INT_OPTION:
        {
            auto v20 = debug_menu_entry(mString{ v21->m_name });
            v20.set_p_ival(v21->m_value.p_ival);
            v20.set_min_value(-1000.0);
            v20.set_max_value(1000.0);
            v22->add_entry(&v20);
            break;
        }
        default:
            break;
        }
    }



    auto v5 = debug_menu_entry(v22);
    parent->add_entry(&v5);




}




void devopt_flags_handler(debug_menu_entry* a1)
{
    switch (a1->get_id())
    {
    case 0u: //CD_ONLY
    {
        os_developer_options::instance()->set_flag(mString{ "CD_ONLY" }, a1->get_bval());


        break;
    }
    case 1u: //ENVMAP_TOOL
    {
        os_developer_options::instance()->set_flag(mString{ "ENVMAP_TOOL" }, a1->get_bval());

        break;
    }
    case 2u: //NO_CD
    {
        os_developer_options::instance()->set_flag(mString{ "NO_CD" }, a1->get_bval());
        break;
    }
    case 3u: //CHATTY_LOAD
    {
        os_developer_options::instance()->set_flag(mString{ "CHATTY_LOAD" }, a1->get_bval());
        break;
    }
    case 4u: //WINDOW_DEFAULT
    {
        os_developer_options::instance()->set_flag(mString{ "WINDOW_DEFAULT" }, a1->get_bval());
        break;
    }
    case 5u: //SHOW_FPS
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_FPS" }, a1->get_bval());
        break;
    }
    case 6u: //SHOW_STREAMER_INFO
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_STREAMER_INFO" }, a1->get_bval());
        break;
    }
    case 7u: //SHOW_STREAMER_SPAM
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_STREAMER_SPAM" }, a1->get_bval());
        break;
    }
    case 8u: //SHOW_RESOURCE_SPAM
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_RESOURCE_SPAM" }, a1->get_bval());
        break;
    }
    case 9u: //SHOW_MEMORY_INFO
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_MEMORY_INFO" }, a1->get_bval());
        break;
    }
    case 10u: //SHOW_SPIDEY_SPEED
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_SPIDEY_SPEED" }, a1->get_bval());
        break;
    }
    case 11u: //TRACE_MISSION_MANAGER
    {
        os_developer_options::instance()->set_flag(mString{ "TRACE_MISSION_MANAGER" }, a1->get_bval());
        break;
    }
    case 12u: //DUMP_MISSION_HEAP
    {
        os_developer_options::instance()->set_flag(mString{ "DUMP_MISSION_HEAP" }, a1->get_bval());
        break;
    }
    case 13u: //CAMERA_CENTRIC_STREAMER
    {
        os_developer_options::instance()->set_flag(mString{ "CAMERA_CENTRIC_STREAMER" }, a1->get_bval());
        break;
    }
    case 14u: //RENDER_LOWLODS
    {
        os_developer_options::instance()->set_flag(mString{ "RENDER_LOWLODS" }, a1->get_bval());
        break;
    }
    case 15u: //LOAD_STRING_HASH_DICTIONARY
    {
        os_developer_options::instance()->set_flag(mString{ "LOAD_STRING_HASH_DICTIONARY" }, a1->get_bval());
        break;
    }
    case 16u: //LOG_RUNTIME_STRING_HASHES
    {
        os_developer_options::instance()->set_flag(mString{ "LOG_RUNTIME_STRING_HASHES" }, a1->get_bval());
        break;
    }
    case 17u: //SHOW_WATERMARK_VELOCITY
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_WATERMARK_VELOCITY" }, a1->get_bval());
        break;
    }
    case 18u: //SHOW_STATS
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_STATS" }, a1->get_bval());
        break;
    }
    case 19u: //ENABLE_ZOOM_MAP
    {
        os_developer_options::instance()->set_flag(mString{ "ENABLE_ZOOM_MAP" }, a1->get_bval());
        break;
    }
    case 20u: //SHOW_DEBUG_INFO
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_DEBUG_INFO" }, a1->get_bval());
        break;
    }
    case 21u: //SHOW_PROFILE_INFO
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_PROFILE_INFO" }, a1->get_bval());
        break;
    }
    case 22u: //SHOW_LOCOMOTION_INFO
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_LOCOMOTION_INFO" }, a1->get_bval());
        break;
    }
    case 23u: //GRAVITY
    {
        os_developer_options::instance()->set_flag(mString{ "GRAVITY" }, a1->get_bval());
        break;
    }
    case 24u: //TEST_MEMORY_LEAKS
    {
        os_developer_options::instance()->set_flag(mString{ "TEST_MEMORY_LEAKS" }, a1->get_bval());
        break;
    }
    case 25u: //HALT_ON_ASSERTS
    {
        os_developer_options::instance()->set_flag(mString{ "HALT_ON_ASSERTS" }, a1->get_bval());
        break;
    }
    case 26u: //SCREEN_ASSERTS
    {
        os_developer_options::instance()->set_flag(mString{ "SCREEN_ASSERTS" }, a1->get_bval());
        break;
    }
    case 27u: //NO_ANIM_WARNINGS
    {
        os_developer_options::instance()->set_flag(mString{ "NO_ANIM_WARNINGS" }, a1->get_bval());
        break;
    }
    case 28u: //PROFILING_ON
    {
        os_developer_options::instance()->set_flag(mString{ "PROFILING_ON" }, a1->get_bval());
        break;
    }
    case 29u: //MONO_AUDIO
    {
        os_developer_options::instance()->set_flag(mString{ "MONO_AUDIO" }, a1->get_bval());
        break;
    }
    case 30u: //NO_MESSAGES
    {
        os_developer_options::instance()->set_flag(mString{ "NO_MESSAGES" }, a1->get_bval());
        break;
    }
    case 31u: //LOCK_STEP
    {
        os_developer_options::instance()->set_flag(mString{ "LOCK_STEP" }, a1->get_bval());
        break;
    }
    case 32u: //TEXTURE_DUMP
    {
        os_developer_options::instance()->set_flag(mString{ "TEXTURE_DUMP" }, a1->get_bval());
        break;
    }
    case 33u: //DISABLE_SOUND_WARNINGS
    {
        os_developer_options::instance()->set_flag(mString{ "DISABLE_SOUND_WARNINGS" }, a1->get_bval());
        break;
    }
    case 34u: //DISABLE_SOUND_DEBUG_OUTPUT
    {
        os_developer_options::instance()->set_flag(mString{ "DISABLE_SOUND_DEBUG_OUTPUT" }, a1->get_bval());
        break;
    }
    case 35u: //DELETE_UNUSED_SOUND_BANKS_ON_PACK
    {
        os_developer_options::instance()->set_flag(mString{ "DELETE_UNUSED_SOUND_BANKS_ON_PACK" }, a1->get_bval());
        break;
    }
    case 36u: //LOCKED_HERO
    {
        os_developer_options::instance()->set_flag(mString{ "LOCKED_HERO" }, a1->get_bval());
        break;
    }
    case 37u: //FOG_OVERR IDE
    {
        os_developer_options::instance()->set_flag(mString{ "FOG_OVERR IDE" }, a1->get_bval());
        break;
    }
    case 38u: //FOG_DISABLE
    {
        os_developer_options::instance()->set_flag(mString{ "FOG_DISABLE" }, a1->get_bval());
        break;
    }
    case 39u: //MOVE_EDITOR
    {
        os_developer_options::instance()->set_flag(mString{ "MOVE_EDITOR" }, a1->get_bval());
        break;
    }
    case 40u: //AI_PATH_DEBUG
    {
        os_developer_options::instance()->set_flag(mString{ "AI_PATH_DEBUG" }, a1->get_bval());
        break;
    }
    case 41u: //AI_PATH_COLOR
    {
        os_developer_options::instance()->set_flag(mString{ "AI_PATH_COLOR" }, a1->get_bval());
        break;
    }
    case 42u: //AI_CRITTER_ACTIVITY
    {
        os_developer_options::instance()->set_flag(mString{ "AI_CRITTER_ACTIVITY" }, a1->get_bval());
        break;
    }
    case 43u: //AI_PATH_COLOR_CRITTER
    {
        os_developer_options::instance()->set_flag(mString{ "AI_PATH_COLOR_CRITTER" }, a1->get_bval());
        break;
    }
    case 44u: //AI_PATH_COLOR_HERO
    {
        os_developer_options::instance()->set_flag(mString{ "AI_PATH_COLOR_HERO" }, a1->get_bval());
        break;
    }
    case 45u: //NO_PARTICLES
    {
        os_developer_options::instance()->set_flag(mString{ "NO_PARTICLES" }, a1->get_bval());
        break;
    }
    case 46u: //NO_PARTICLE_PUMP
    {
        os_developer_options::instance()->set_flag(mString{ "NO_PARTICLE_PUMP" }, a1->get_bval());
        break;
    }
    case 47u: //SHOW_NORMALS
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_NORMALS" }, a1->get_bval());
        break;
    }
    case 48u: //SHOW_SHADOW_BALL
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_SHADOW_BALL" }, a1->get_bval());
        break;
    }
    case 49u: //SHOW_LIGHTS
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_LIGHTS" }, a1->get_bval());
        break;
    }
    case 50u: //SHOW_PLRCTRL
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_PLRCTRL" }, a1->get_bval());
        break;
    }
    case 51u: //SHOW_PSX_INFO
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_PSX_INFO" }, a1->get_bval());
        break;
    }
    case 52u: //FLAT_SHADE
    {
        os_developer_options::instance()->set_flag(mString{ "FLAT_SHADE" }, a1->get_bval());
        break;
    }
    case 53u: //INTERFACE_DISABLE
    {
        os_developer_options::instance()->set_flag(mString{ "INTERFACE_DISABLE" }, a1->get_bval());
        break;
    }
    case 54u: //WIDGET_TOOLS
    {
        os_developer_options::instance()->set_flag(mString{ "WIDGET_TOOLS" }, a1->get_bval());
        break;
    }
    case 55u: //LIGHTING
    {
        os_developer_options::instance()->set_flag(mString{ "LIGHTING" }, a1->get_bval());
        break;
    }
    case 56u: //FAKE_POINT_LIGHTS
    {
        os_developer_options::instance()->set_flag(mString{ "FAKE_POINT_LIGHTS" }, a1->get_bval());
        break;
    }
    case 57u: //BSP_SPRAY_PAINT
    {
        os_developer_options::instance()->set_flag(mString{ "BSP_SPRAY_PAINT" }, a1->get_bval());
        break;
    }
    case 58u: //CAMERA_EDITOR
    {
        os_developer_options::instance()->set_flag(mString{ "CAMERA_EDITOR" }, a1->get_bval());
        break;
    }
    case 59u: //IGNORE_RAMPING
    {
        os_developer_options::instance()->set_flag(mString{ "IGNORE_RAMPING" }, a1->get_bval());
        break;
    }
    case 60u: //POINT_SAMPLE
    {
        os_developer_options::instance()->set_flag(mString{ "POINT_SAMPLE" }, a1->get_bval());
        break;
    }
    case 61u: //DISTANCE_FADING
    {
        os_developer_options::instance()->set_flag(mString{ "DISTANCE_FADING" }, a1->get_bval());
        break;
    }
    case 62u: //OVERRIDE_CONTROLLER_OPTIONS
    {
        os_developer_options::instance()->set_flag(mString{ "OVERRIDE_CONTROLLER_OPTIONS" }, a1->get_bval());
        break;
    }
    case 63u: //DISABLE_MOUSE_PLAYER_CONTROL
    {
        os_developer_options::instance()->set_flag(mString{ "DISABLE_MOUSE_PLAYER_CONTROL" }, a1->get_bval());
        break;
    }
    case 64u: //NO_MOVIES
    {
        os_developer_options::instance()->set_flag(mString{ "NO_MOVIES" }, a1->get_bval());
        break;
    }
    case 65u: //XBOX_USER_CAM
    {
        os_developer_options::instance()->set_flag(mString{ "XBOX_USER_CAM" }, a1->get_bval());
        break;
    }
    case 66u: //NO_LOAD_SCREEN
    {
        os_developer_options::instance()->set_flag(mString{ "NO_LOAD_SCREEN" }, a1->get_bval());
        break;
    }
    case 67u: //EXCEPTION_HANDLER
    {
        os_developer_options::instance()->set_flag(mString{ "EXCEPTION_HANDLER" }, a1->get_bval());
        break;
    }
    case 68u: //NO_EXCEPTION_HANDLER
    {
        os_developer_options::instance()->set_flag(mString{ "NO_EXCEPTION_HANDLER" }, a1->get_bval());
        break;
    }
    case 69u: //NO_CD_CHECK
    {
        os_developer_options::instance()->set_flag(mString{ "NO_CD_CHECK" }, a1->get_bval());
        break;
    }
    case 70u: //NO_LOAD_METER
    {
        os_developer_options::instance()->set_flag(mString{ "NO_LOAD_METER" }, a1->get_bval());
        break;
    }
    case 71u: //NO_MOVIE_BUFFER_WARNING
    {
        os_developer_options::instance()->set_flag(mString{ "NO_MOVIE_BUFFER_WARNING" }, a1->get_bval());
        break;
    }
    case 72u: //LIMITED_EDITION_DISC
    {
        os_developer_options::instance()->set_flag(mString{ "LIMITED_EDITION_DISC" }, a1->get_bval());
        break;
    }
    case 73u: //NEW_COMBAT_LOCO
    {
        os_developer_options::instance()->set_flag(mString{ "NEW_COMBAT_LOCO" }, a1->get_bval());
        break;
    }
    case 74u: //LEVEL_WARP
    {
        os_developer_options::instance()->set_flag(mString{ "LEVEL_WARP" }, a1->get_bval());
        break;
    }
    case 75u: //SMOKE_TEST
    {
        os_developer_options::instance()->set_flag(mString{ "SMOKE_TEST" }, a1->get_bval());
        break;
    }
    case 76u: //SMOKE_TEST_LEVEL
    {
        os_developer_options::instance()->set_flag(mString{ "SMOKE_TEST_LEVEL" }, a1->get_bval());
        break;
    }
    case 77u: //COMBO_TESTER
    {
        os_developer_options::instance()->set_flag(mString{ "COMBO_TESTER" }, a1->get_bval());
        break;
    }
    case 78u: //DROP _SHADOWS_ALWAYS_RAYCAST
    {
        os_developer_options::instance()->set_flag(mString{ "DROP _SHADOWS_ALWAYS_RAYCAST" }, a1->get_bval());
        break;
    }
    case 79u: //DISABLE_DROP_SHADOWS
    {
        os_developer_options::instance()->set_flag(mString{ "DISABLE_DROP_SHADOWS" }, a1->get_bval());
        break;
    }
    case 80u: //DISABLE_HIRES_SHADOWS
    {
        os_developer_options::instance()->set_flag(mString{ "DISABLE_HIRES_SHADOWS" }, a1->get_bval());
        break;
    }
    case 81u: //DISABLE_STENCIL_SHADOWS
    {
        os_developer_options::instance()->set_flag(mString{ "DISABLE_STENCIL_SHADOWS" }, a1->get_bval());
        break;
    }
    case 82u: //DISABLE_COLORVOLS
    {
        os_developer_options::instance()->set_flag(mString{ "DISABLE_COLORVOLS" }, a1->get_bval());
        break;
    }
    case 83u: //DISABLE_RENDER_ENTS
    {
        os_developer_options::instance()->set_flag(mString{ "DISABLE_RENDER_ENTS" }, a1->get_bval());
        break;
    }
    case 84u: //DISABLE_FULLSCREEN_BLUR
    {
        os_developer_options::instance()->set_flag(mString{ "DISABLE_FULLSCREEN_BLUR" }, a1->get_bval());
        break;
    }
    case 85u: //FORCE_TIGHTCRAWL
    {
        os_developer_options::instance()->set_flag(mString{ "FORCE_TIGHTCRAWL" }, a1->get_bval());
        break;
    }
    case 86u: //FORCE_NONCRAWL
    {
        os_developer_options::instance()->set_flag(mString{ "FORCE_NONCRAWL" }, a1->get_bval());
        break;
    }
    case 87u: //SHOW_DEBUG_TEXT
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_DEBUG_TEXT" }, a1->get_bval());
        break;
    }
    case 88u: //SHOW_STYLE_POINTS
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_STYLE_POINTS" }, a1->get_bval());
        break;
    }
    case 89u: //CAMERA_MOUSE_MODE
    {
        os_developer_options::instance()->set_flag(mString{ "CAMERA_MOUSE_MODE" }, a1->get_bval());
        break;
    }
    case 90u: //USERCAM_ON_CONTROLLER2
    {
        os_developer_options::instance()->set_flag(mString{ "USERCAM_ON_CONTROLLER2" }, a1->get_bval());
        break;
    }
    case 91u: //DISABLE_ANCHOR_RETICLE_RENDERING
    {
        os_developer_options::instance()->set_flag(mString{ "DISABLE_ANCHOR_RETICLE_RENDERING" }, a1->get_bval());
        break;
    }
    case 92u: //SHOW_ANCHOR_LINE
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_ANCHOR_LINE" }, a1->get_bval());
        break;
    }
    case 93u: //FREE_SPIDER_REFLEXES
    {
        os_developer_options::instance()->set_flag(mString{ "FREE_SPIDER_REFLEXES" }, a1->get_bval());
        break;
    }
    case 94u: //SHOW_BAR_OF_SHAME
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_BAR_OF_SHAME" }, a1->get_bval());
        break;
    }
    case 95u: //SHOW_ENEMY_HEALTH_WIDGETS
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_ENEMY_HEALTH_WIDGETS" }, a1->get_bval());
        break;
    }
    case 96u: //ALLOW_IGC_PAUSE
    {
        os_developer_options::instance()->set_flag(mString{ "ALLOW_IGC_PAUSE" }, a1->get_bval());
        break;
    }
    case 97u: //SHOW_OBBS
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_OBBS" }, a1->get_bval());
        break;
    }
    case 98u: //SHOW_DISTRICTS
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_DISTRICTS" }, a1->get_bval());
        break;
    }
    case 99u: //SHOW_CHUCK_DEBUGGER
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_CHUCK_DEBUGGER" }, a1->get_bval());
        break;
    }
    case 100u: //CHUCK_OLD_FASHIONED
    {
        os_developer_options::instance()->set_flag(mString{ "CHUCK_OLD_FASHIONED" }, a1->get_bval());
        break;
    }
    case 101u: //CHUCK_DISABLE_BREAKPOINTS
    {
        os_developer_options::instance()->set_flag(mString{ "CHUCK_DISABLE_BREAKPOINTS" }, a1->get_bval());
        break;
    }
    case 102u: //SHOW_AUDIO_BOXES
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_AUDIO_BOXES" }, a1->get_bval());
        break;
    }
    case 103u: //DISABLE_SOUNDS
    {
        os_developer_options::instance()->set_flag(mString{ "DISABLE_SOUNDS" }, a1->get_bval());
        break;
    }
    case 104u: //SHOW_TERRAIN_INFO
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_TERRAIN_INFO" }, a1->get_bval());
        break;
    }
    case 105u: //DISABLE_AUDIO_BOXES
    {
        os_developer_options::instance()->set_flag(mString{ "DISABLE_AUDIO_BOXES" }, a1->get_bval());
        break;
    }
    case 106u: //NSL_OLD_FASHIONED
    {
        os_developer_options::instance()->set_flag(mString{ "NSL_OLD_FASHIONED" }, a1->get_bval());
        break;
    }
    case 107u: //SHOW_MASTER_CLOCK
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_MASTER_CLOCK" }, a1->get_bval());
        break;
    }
    case 108u: //LOAD_GRADIENTS
    {
        os_developer_options::instance()->set_flag(mString{ "LOAD_GRADIENTS" }, a1->get_bval());
        break;
    }
    case 109u: //BONUS_LEVELS_AVAILABLE
    {
        os_developer_options::instance()->set_flag(mString{ "BONUS_LEVELS_AVAILABLE" }, a1->get_bval());
        break;
    }
    case 110u: //COMBAT_DISPLAY
    {
        os_developer_options::instance()->set_flag(mString{ "COMBAT_DISPLAY" }, a1->get_bval());
        break;
    }
    case 111u: //COMBAT_DEBUGGER
    {
        os_developer_options::instance()->set_flag(mString{ "COMBAT_DEBUGGER" }, a1->get_bval());
        break;
    }
    case 112u: //ALLOW_ERROR_POPUPS
    {
        os_developer_options::instance()->set_flag(mString{ "ALLOW_ERROR_POPUPS" }, a1->get_bval());
        break;
    }
    case 113u: //ALLOW_WARNING_POPUPS
    {
        os_developer_options::instance()->set_flag(mString{ "ALLOW_WARNING_POPUPS" }, a1->get_bval());
        break;
    }
    case 114u: //OUTPUT_WARNING_DISABLE
    {
        os_developer_options::instance()->set_flag(mString{ "OUTPUT_WARNING_DISABLE" }, a1->get_bval());
        break;
    }
    case 115u: //OUTPUT_ASSERT_DISABLE
    {
        os_developer_options::instance()->set_flag(mString{ "OUTPUT_ASSERT_DISABLE" }, a1->get_bval());
        break;
    }
    case 116u: //ASSERT_ON_WARNING
    {
        os_developer_options::instance()->set_flag(mString{ "ASSERT_ON_WARNING" }, a1->get_bval());
        break;
    }
    case 117u: //ALWAYS_ACTIVE
    {
        os_developer_options::instance()->set_flag(mString{ "ALWAYS_ACTIVE" }, a1->get_bval());
        break;
    }
    case 118u: //FORCE_PROGRESSION
    {
        os_developer_options::instance()->set_flag(mString{ "FORCE_PROGRESSION" }, a1->get_bval());
        break;
    }
    case 119u: //LINK
    {
        os_developer_options::instance()->set_flag(mString{ "LINK" }, a1->get_bval());
        break;
    }
    case 120u: //WAIT_FOR_LINK
    {
        os_developer_options::instance()->set_flag(mString{ "WAIT_FOR_LINK" }, a1->get_bval());
        break;
    }
    case 121u: //SHOW_END_OF_PACK
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_END_OF_PACK" }, a1->get_bval());
        break;
    }
    case 122u: //LIVE_IN_GLASS_HOUSE
    {
        os_developer_options::instance()->set_flag(mString{ "LIVE_IN_GLASS_HOUSE" }, a1->get_bval());
        break;
    }
    case 123u: //SHOW_GLASS_HOUSE
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_GLASS_HOUSE" }, a1->get_bval());
        break;
    }
    case 124u: //DISABLE_RACE_PREVIEW
    {
        os_developer_options::instance()->set_flag(mString{ "DISABLE_RACE_PREVIEW" }, a1->get_bval());
        break;
    }
    case 125u: //FREE_MINI_MAP
    {
        os_developer_options::instance()->set_flag(mString{ "FREE_MINI_MAP" }, a1->get_bval());
        break;
    }
    case 126u: //SHOW_LOOPING_ANIM_WARNINGS
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_LOOPING_ANIM_WARNINGS" }, a1->get_bval());
        break;
    }
    case 127u: //SHOW_PERF_INFO
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_PERF_INFO" }, a1->get_bval());
        break;
    }
    case 128u: //COPY_ERROR_TO_CLIPBOARD
    {
        os_developer_options::instance()->set_flag(mString{ "COPY_ERROR_TO_CLIPBOARD" }, a1->get_bval());
        break;
    }
    case 129u: //BOTH_HANDS_UP_REORIENT
    {
        os_developer_options::instance()->set_flag(mString{ "BOTH_HANDS_UP_REORIENT" }, a1->get_bval());
        break;
    }
    case 130u: //SHOW_CAMERA_PROJECTION
    {
        os_developer_options::instance()->set_flag(mString{ "SHOW_CAMERA_PROJECTION" }, a1->get_bval());
        break;
    }
    case 131u: //OLD_DEFAULT_CONTROL_SETTINGS
    {
        os_developer_options::instance()->set_flag(mString{ "OLD_DEFAULT_CONTROL_SETTINGS" }, a1->get_bval());
        break;
    }
    case 132u: //IGC_SPEED_CONTROL
    {
        os_developer_options::instance()->set_flag(mString{ "IGC_SPEED_CONTROL" }, a1->get_bval());
        break;
    }
    case 133u: //RTDT_ENABLED
    {
        os_developer_options::instance()->set_flag(mString{ "RTDT_ENABLED" }, a1->get_bval());
        break;
    }
    case 134u: //ENABLE_DECALS
    {
        os_developer_options::instance()->set_flag(mString{ "ENABLE_DECALS" }, a1->get_bval());
        break;
    }
    case 135u: //AUTO_STICK_TO_WALLS
    {
        os_developer_options::instance()->set_flag(mString{ "AUTO_STICK_TO_WALLS" }, a1->get_bval());
        break;
    }
    case 136u: //ENABLE_PEDESTRIANS
    {
        os_developer_options::instance()->set_flag(mString{ "ENABLE_PEDESTRIANS" }, a1->get_bval());
        break;
    }
    case 137u: //CAMERA_INVERT_LOOKAROUND
    {
        os_developer_options::instance()->set_flag(mString{ "CAMERA_INVERT_LOOKAROUND" }, a1->get_bval());
        break;
    }
    case 138u: //CAMERA_INVERT_LOOKAROUND_X
    {
        os_developer_options::instance()->set_flag(mString{ "CAMERA_INVERT_LOOKAROUND_X" }, a1->get_bval());
        break;
    }
    case 139u: //CAMERA_INVERT_LOOKAROUND_Y
    {
        os_developer_options::instance()->set_flag(mString{ "CAMERA_INVERT_LOOKAROUND_Y" }, a1->get_bval());
        break;
    }
    case 140u: //FORCE_COMBAT_CAMERA_OFF
    {
        os_developer_options::instance()->set_flag(mString{ "FORCE_COMBAT_CAMERA_OFF" }, a1->get_bval());
        break;
    }
    case 141u: //DISPLAY_THOUGHT_BUBBLES
    {
        os_developer_options::instance()->set_flag(mString{ "DISPLAY_THOUGHT_BUBBLES" }, a1->get_bval());
        break;
    }
    case 142u: //ENABLE_DEBUG_LOG
    {
        os_developer_options::instance()->set_flag(mString{ "ENABLE_DEBUG_LOG" }, a1->get_bval());
        break;
    }
    case 143u: //ENABLE_LONG_CALLSTACK
    {
        os_developer_options::instance()->set_flag(mString{ "ENABLE_LONG_CALLSTACK" }, a1->get_bval());
        break;
    }
    case 144u: //RENDER_FE_UI
    {
        os_developer_options::instance()->set_flag(mString{ "RENDER_FE_UI" }, a1->get_bval());
        break;
    }
    case 145u: //LOCK_INTERIORS
    {
        os_developer_options::instance()->set_flag(mString{ "LOCK_INTERIORS" }, a1->get_bval());
        break;
    }
    case 146u: //MEMCHECK_ON_LOAD
    {
        os_developer_options::instance()->set_flag(mString{ "MEMCHECK_ON_LOAD" }, a1->get_bval());
        break;
    }
    case 147u: //DISPLAY_ALS_USAGE_PROFILE
    {
        os_developer_options::instance()->set_flag(mString{ "DISPLAY_ALS_USAGE_PROFILE" }, a1->get_bval());
        break;
    }
    case 148u: //ENABLE_FPU_EXCEPTION_HANDLING
    {
        os_developer_options::instance()->set_flag(mString{ "ENABLE_FPU_EXCEPTION_HANDLING" }, false);
        break;
    }
    case 149u: //UNLOCK_ALL_UNLOCKABLES
    {
        os_developer_options::instance()->set_flag(mString{ "UNLOCK_ALL_UNLOCKABLES" }, a1->get_bval());
        break;
    }
    default:
        return;
    }
}


void create_devopt_flags_menu(debug_menu* parent)
{
    assert(parent != nullptr);

    auto* game_menu = create_menu("Devopt Flags");
    auto* v92 = game_menu;
    auto* v4 = create_menu_entry(game_menu);

    parent->add_entry(v4);

    debug_menu_entry v89;

    // === BEGIN GENERATED FLAG ENTRIES (150 total, indices 0..149) =========
    // Generated from os_developer_options.cpp::g_flag_names; do not hand-edit
    // individual entries — regenerate the whole block if a flag is added or
    // renamed in g_flag_names. Two flag names contain embedded spaces
    // ("FOG_OVERR IDE", "DROP _SHADOWS_ALWAYS_RAYCAST"); these are typos in
    // the original game data preserved verbatim because flag lookups are
    // exact-string matches.

    v89 = debug_menu_entry(mString{ "CD_ONLY" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "CD_ONLY" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(0);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "ENVMAP_TOOL" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "ENVMAP_TOOL" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(1);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "NO_CD" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "NO_CD" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(2);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "CHATTY_LOAD" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "CHATTY_LOAD" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(3);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "WINDOW_DEFAULT" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "WINDOW_DEFAULT" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(4);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_FPS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_FPS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(5);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_STREAMER_INFO" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_STREAMER_INFO" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(6);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_STREAMER_SPAM" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_STREAMER_SPAM" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(7);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_RESOURCE_SPAM" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_RESOURCE_SPAM" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(8);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_MEMORY_INFO" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_MEMORY_INFO" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(9);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_SPIDEY_SPEED" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_SPIDEY_SPEED" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(10);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "TRACE_MISSION_MANAGER" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "TRACE_MISSION_MANAGER" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(11);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DUMP_MISSION_HEAP" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DUMP_MISSION_HEAP" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(12);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "CAMERA_CENTRIC_STREAMER" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "CAMERA_CENTRIC_STREAMER" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(13);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "RENDER_LOWLODS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "RENDER_LOWLODS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(14);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "LOAD_STRING_HASH_DICTIONARY" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "LOAD_STRING_HASH_DICTIONARY" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(15);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "LOG_RUNTIME_STRING_HASHES" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "LOG_RUNTIME_STRING_HASHES" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(16);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_WATERMARK_VELOCITY" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_WATERMARK_VELOCITY" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(17);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_STATS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_STATS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(18);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "ENABLE_ZOOM_MAP" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "ENABLE_ZOOM_MAP" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(19);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_DEBUG_INFO" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_DEBUG_INFO" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(20);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_PROFILE_INFO" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_PROFILE_INFO" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(21);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_LOCOMOTION_INFO" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_LOCOMOTION_INFO" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(22);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "GRAVITY" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "GRAVITY" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(23);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "TEST_MEMORY_LEAKS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "TEST_MEMORY_LEAKS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(24);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "HALT_ON_ASSERTS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "HALT_ON_ASSERTS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(25);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SCREEN_ASSERTS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SCREEN_ASSERTS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(26);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "NO_ANIM_WARNINGS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "NO_ANIM_WARNINGS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(27);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "PROFILING_ON" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "PROFILING_ON" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(28);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "MONO_AUDIO" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "MONO_AUDIO" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(29);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "NO_MESSAGES" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "NO_MESSAGES" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(30);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "LOCK_STEP" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "LOCK_STEP" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(31);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "TEXTURE_DUMP" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "TEXTURE_DUMP" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(32);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISABLE_SOUND_WARNINGS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISABLE_SOUND_WARNINGS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(33);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISABLE_SOUND_DEBUG_OUTPUT" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISABLE_SOUND_DEBUG_OUTPUT" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(34);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DELETE_UNUSED_SOUND_BANKS_ON_PACK" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DELETE_UNUSED_SOUND_BANKS_ON_PACK" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(35);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "LOCKED_HERO" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "LOCKED_HERO" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(36);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "FOG_OVERR IDE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "FOG_OVERR IDE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(37);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "FOG_DISABLE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "FOG_DISABLE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(38);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "MOVE_EDITOR" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "MOVE_EDITOR" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(39);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "AI_PATH_DEBUG" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "AI_PATH_DEBUG" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(40);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "AI_PATH_COLOR" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "AI_PATH_COLOR" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(41);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "AI_CRITTER_ACTIVITY" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "AI_CRITTER_ACTIVITY" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(42);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "AI_PATH_COLOR_CRITTER" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "AI_PATH_COLOR_CRITTER" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(43);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "AI_PATH_COLOR_HERO" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "AI_PATH_COLOR_HERO" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(44);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "NO_PARTICLES" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "NO_PARTICLES" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(45);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "NO_PARTICLE_PUMP" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "NO_PARTICLE_PUMP" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(46);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_NORMALS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_NORMALS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(47);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_SHADOW_BALL" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_SHADOW_BALL" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(48);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_LIGHTS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_LIGHTS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(49);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_PLRCTRL" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_PLRCTRL" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(50);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_PSX_INFO" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_PSX_INFO" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(51);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "FLAT_SHADE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "FLAT_SHADE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(52);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "INTERFACE_DISABLE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "INTERFACE_DISABLE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(53);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "WIDGET_TOOLS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "WIDGET_TOOLS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(54);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "LIGHTING" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "LIGHTING" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(55);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "FAKE_POINT_LIGHTS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "FAKE_POINT_LIGHTS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(56);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "BSP_SPRAY_PAINT" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "BSP_SPRAY_PAINT" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(57);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "CAMERA_EDITOR" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "CAMERA_EDITOR" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(58);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "IGNORE_RAMPING" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "IGNORE_RAMPING" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(59);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "POINT_SAMPLE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "POINT_SAMPLE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(60);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISTANCE_FADING" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISTANCE_FADING" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(61);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "OVERRIDE_CONTROLLER_OPTIONS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "OVERRIDE_CONTROLLER_OPTIONS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(62);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISABLE_MOUSE_PLAYER_CONTROL" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISABLE_MOUSE_PLAYER_CONTROL" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(63);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "NO_MOVIES" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "NO_MOVIES" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(64);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "XBOX_USER_CAM" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "XBOX_USER_CAM" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(65);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "NO_LOAD_SCREEN" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "NO_LOAD_SCREEN" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(66);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "EXCEPTION_HANDLER" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "EXCEPTION_HANDLER" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(67);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "NO_EXCEPTION_HANDLER" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "NO_EXCEPTION_HANDLER" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(68);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "NO_CD_CHECK" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "NO_CD_CHECK" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(69);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "NO_LOAD_METER" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "NO_LOAD_METER" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(70);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "NO_MOVIE_BUFFER_WARNING" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "NO_MOVIE_BUFFER_WARNING" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(71);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "LIMITED_EDITION_DISC" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "LIMITED_EDITION_DISC" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(72);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "NEW_COMBAT_LOCO" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "NEW_COMBAT_LOCO" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(73);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "LEVEL_WARP" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "LEVEL_WARP" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(74);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SMOKE_TEST" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SMOKE_TEST" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(75);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SMOKE_TEST_LEVEL" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SMOKE_TEST_LEVEL" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(76);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "COMBO_TESTER" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "COMBO_TESTER" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(77);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DROP _SHADOWS_ALWAYS_RAYCAST" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DROP _SHADOWS_ALWAYS_RAYCAST" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(78);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISABLE_DROP_SHADOWS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISABLE_DROP_SHADOWS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(79);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISABLE_HIRES_SHADOWS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISABLE_HIRES_SHADOWS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(80);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISABLE_STENCIL_SHADOWS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISABLE_STENCIL_SHADOWS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(81);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISABLE_COLORVOLS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISABLE_COLORVOLS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(82);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISABLE_RENDER_ENTS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISABLE_RENDER_ENTS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(83);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISABLE_FULLSCREEN_BLUR" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISABLE_FULLSCREEN_BLUR" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(84);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "FORCE_TIGHTCRAWL" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "FORCE_TIGHTCRAWL" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(85);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "FORCE_NONCRAWL" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "FORCE_NONCRAWL" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(86);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_DEBUG_TEXT" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_DEBUG_TEXT" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(87);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_STYLE_POINTS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_STYLE_POINTS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(88);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "CAMERA_MOUSE_MODE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "CAMERA_MOUSE_MODE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(89);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "USERCAM_ON_CONTROLLER2" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "USERCAM_ON_CONTROLLER2" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(90);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISABLE_ANCHOR_RETICLE_RENDERING" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISABLE_ANCHOR_RETICLE_RENDERING" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(91);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_ANCHOR_LINE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_ANCHOR_LINE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(92);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "FREE_SPIDER_REFLEXES" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "FREE_SPIDER_REFLEXES" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(93);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_BAR_OF_SHAME" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_BAR_OF_SHAME" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(94);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_ENEMY_HEALTH_WIDGETS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_ENEMY_HEALTH_WIDGETS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(95);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "ALLOW_IGC_PAUSE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "ALLOW_IGC_PAUSE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(96);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_OBBS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_OBBS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(97);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_DISTRICTS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_DISTRICTS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(98);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_CHUCK_DEBUGGER" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_CHUCK_DEBUGGER" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(99);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "CHUCK_OLD_FASHIONED" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "CHUCK_OLD_FASHIONED" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(100);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "CHUCK_DISABLE_BREAKPOINTS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "CHUCK_DISABLE_BREAKPOINTS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(101);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_AUDIO_BOXES" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_AUDIO_BOXES" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(102);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISABLE_SOUNDS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISABLE_SOUNDS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(103);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_TERRAIN_INFO" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_TERRAIN_INFO" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(104);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISABLE_AUDIO_BOXES" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISABLE_AUDIO_BOXES" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(105);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "NSL_OLD_FASHIONED" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "NSL_OLD_FASHIONED" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(106);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_MASTER_CLOCK" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_MASTER_CLOCK" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(107);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "LOAD_GRADIENTS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "LOAD_GRADIENTS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(108);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "BONUS_LEVELS_AVAILABLE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "BONUS_LEVELS_AVAILABLE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(109);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "COMBAT_DISPLAY" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "COMBAT_DISPLAY" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(110);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "COMBAT_DEBUGGER" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "COMBAT_DEBUGGER" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(111);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "ALLOW_ERROR_POPUPS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "ALLOW_ERROR_POPUPS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(112);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "ALLOW_WARNING_POPUPS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "ALLOW_WARNING_POPUPS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(113);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "OUTPUT_WARNING_DISABLE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "OUTPUT_WARNING_DISABLE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(114);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "OUTPUT_ASSERT_DISABLE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "OUTPUT_ASSERT_DISABLE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(115);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "ASSERT_ON_WARNING" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "ASSERT_ON_WARNING" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(116);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "ALWAYS_ACTIVE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "ALWAYS_ACTIVE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(117);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "FORCE_PROGRESSION" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "FORCE_PROGRESSION" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(118);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "LINK" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "LINK" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(119);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "WAIT_FOR_LINK" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "WAIT_FOR_LINK" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(120);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_END_OF_PACK" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_END_OF_PACK" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(121);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "LIVE_IN_GLASS_HOUSE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "LIVE_IN_GLASS_HOUSE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(122);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_GLASS_HOUSE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_GLASS_HOUSE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(123);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISABLE_RACE_PREVIEW" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISABLE_RACE_PREVIEW" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(124);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "FREE_MINI_MAP" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "FREE_MINI_MAP" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(125);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_LOOPING_ANIM_WARNINGS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_LOOPING_ANIM_WARNINGS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(126);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_PERF_INFO" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_PERF_INFO" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(127);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "COPY_ERROR_TO_CLIPBOARD" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "COPY_ERROR_TO_CLIPBOARD" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(128);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "BOTH_HANDS_UP_REORIENT" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "BOTH_HANDS_UP_REORIENT" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(129);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "SHOW_CAMERA_PROJECTION" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "SHOW_CAMERA_PROJECTION" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(130);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "OLD_DEFAULT_CONTROL_SETTINGS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "OLD_DEFAULT_CONTROL_SETTINGS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(131);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "IGC_SPEED_CONTROL" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "IGC_SPEED_CONTROL" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(132);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "RTDT_ENABLED" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "RTDT_ENABLED" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(133);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "ENABLE_DECALS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "ENABLE_DECALS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(134);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "AUTO_STICK_TO_WALLS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "AUTO_STICK_TO_WALLS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(135);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "ENABLE_PEDESTRIANS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "ENABLE_PEDESTRIANS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(136);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "CAMERA_INVERT_LOOKAROUND" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "CAMERA_INVERT_LOOKAROUND" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(137);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "CAMERA_INVERT_LOOKAROUND_X" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "CAMERA_INVERT_LOOKAROUND_X" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(138);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "CAMERA_INVERT_LOOKAROUND_Y" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "CAMERA_INVERT_LOOKAROUND_Y" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(139);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "FORCE_COMBAT_CAMERA_OFF" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "FORCE_COMBAT_CAMERA_OFF" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(140);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISPLAY_THOUGHT_BUBBLES" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISPLAY_THOUGHT_BUBBLES" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(141);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "ENABLE_DEBUG_LOG" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "ENABLE_DEBUG_LOG" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(142);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "ENABLE_LONG_CALLSTACK" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "ENABLE_LONG_CALLSTACK" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(143);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "RENDER_FE_UI" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "RENDER_FE_UI" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(144);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "LOCK_INTERIORS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "LOCK_INTERIORS" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(145);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "MEMCHECK_ON_LOAD" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "MEMCHECK_ON_LOAD" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(146);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "DISPLAY_ALS_USAGE_PROFILE" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "DISPLAY_ALS_USAGE_PROFILE" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(147);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "ENABLE_FPU_EXCEPTION_HANDLING" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "ENABLE_FPU_EXCEPTION_HANDLING" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(148);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "UNLOCK_ALL_UNLOCKABLES" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString{ "UNLOCK_ALL_UNLOCKABLES" }));
    v89.set_game_flags_handler(devopt_flags_handler);
    v89.set_id(149);
    v92->add_entry(&v89);
    // === END GENERATED FLAG ENTRIES ======================================

    // Trailing INT_OPTION pass — picks up integer-typed devopts (the bool
    // ones are already covered by the 150-entry block above).

}



void create_game_flags_menu(debug_menu* parent)
{
    if (parent->used_slots != 0) {
        return;
    }

    assert(parent != nullptr);

    auto* v92 = parent;

    auto v89 = debug_menu_entry(mString { "Report SLF Recall Timeouts" });
    static bool byte_1597BC0 = false;
    v89.set_pt_bval(&byte_1597BC0);
     v92->add_entry(&v89);

    v89 = debug_menu_entry(mString { "Physics Enabled" });
    v89.set_bval(true);
    v89.set_game_flags_handler(game_flags_handler);
    v89.set_id(0);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString { "Single Step" });
    v89.set_game_flags_handler(game_flags_handler);
    v89.set_id(1);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString{ "Slow Motion Enabled" });
    v89.set_bval(false);
    v89.set_game_flags_handler(slow_motion_handler);
    v89.set_id(2);
    v92->add_entry(&v89);

    v89 = debug_menu_entry { mString { "Monkey Enabled" } };

    auto v1 = spider_monkey::is_running();
    v89.set_bval(v1);
    v89.set_game_flags_handler(game_flags_handler);
    v89.set_id(3);
    v92->add_entry(&v89);

    v89 = debug_menu_entry { mString { "Rumble Enabled" } };
    v89.set_bval(true);
    v89.set_game_flags_handler(game_flags_handler);
    v89.set_id(4);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString { "God Mode" });
    v89.set_ival(os_developer_options::instance()->get_int(mString { "GOD_MODE" }));

    const float v2[4] = { 0, 5, 1, 1 };
    v89.set_fl_values(v2);
    v89.set_game_flags_handler(game_flags_handler);
    v89.set_id(5);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString { "Show Districts" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString { "SHOW_STREAMER_INFO" }));
    v89.set_game_flags_handler(game_flags_handler);
    v89.set_id(6);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString { "Show Hero Position" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString { "SHOW_DEBUG_INFO" }));
    v89.set_game_flags_handler(game_flags_handler);
    v89.set_id(7);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString { "Show FPS" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString { "SHOW_FPS" }));
    v89.set_game_flags_handler(game_flags_handler);
    v89.set_id(8);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString { "User Camera on Controller 2" });
    v89.set_bval(os_developer_options::instance()->get_flag(mString { "USERCAM_ON_CONTROLLER2" }));
    v89.set_game_flags_handler(game_flags_handler);
    v89.set_id(9);
    v92->add_entry(&v89);

    v89 = debug_menu_entry(mString { "Toggle Unload All Districts" });
    v89.set_game_flags_handler(game_flags_handler);
    v89.set_id(13);
    v92->add_entry(&v89);

    {
        auto* v88 = create_menu("Save/Load", handle_game_entry, 10);
        auto v18 = debug_menu_entry(v88);
        v92->add_entry(&v18);

        v89 = debug_menu_entry(mString { "Save Game" });
        v89.set_game_flags_handler(game_flags_handler);
        v89.set_id(14);
        v88->add_entry(&v89);

        v89 = debug_menu_entry(mString { "Load Game" });
        v89.set_game_flags_handler(game_flags_handler);
        v89.set_id(15);
        v88->add_entry(&v89);

        v89 = debug_menu_entry(mString { "Attemp Auto Load" });
        v89.set_game_flags_handler(game_flags_handler);
        v89.set_id(16);
        v88->add_entry(&v89);
    }

    {
        auto* v87 = create_menu("Screenshot", handle_game_entry, 10);
        auto v23 = debug_menu_entry(v87);
        v92->add_entry(&v23);

        v89 = debug_menu_entry(mString { "Hires Screenshot" });
        v89.set_game_flags_handler(game_flags_handler);
        v89.set_id(11);
        v87->add_entry(&v89);

        v89 = debug_menu_entry(mString { "Lores Screenshot" });
        v89.set_game_flags_handler(game_flags_handler);
        v89.set_id(12);
        v87->add_entry(&v89);
    }

    create_gamefile_menu(v92);
    create_devopt_ints_menu(v92);
    create_devopt_flags_menu(v92);
}





//void movie_play_handler(debug_menu_entry* entry)
//{

   // if (!movie_manager::load_and_play_movie("Attract", "Attract", false)) {
  //      close_debug();
  //  }
//}






//void populate_movie_play_menu(debug_menu_entry* entry)
//{

//    entry->set_game_flags_handler(movie_play_handler);
//}


/*
void create_movie_play_menu(debug_menu* parent)
{
    auto* script_menu = create_menu("DEMO USM Trailer");
    auto* v2 = create_menu_entry(script_menu);
    v2->entry_type = UNDEFINED;
    v2->set_game_flags_handler(populate_movie_play_menu);
    v2->set_game_flags_handler(movie_play_handler);
    parent->add_entry(v2);
}
*/

constexpr auto NUM_SCRIPTS = 40u;

const char* scripts[NUM_SCRIPTS] = {
    "ch_venom_viewer",
    "ch_vwr_sable",
    "ch_vwr_johnny_storm",
    "ch_vwr_gmu_cop_fat",
    "ch_vwr_gmu_sable_merc",
    "ch_vwr_shield_agent",
    "ch_vwr_gang_mercs_base",
    "ch_vwr_gang_mercs_boss",
    "ch_vwr_gang_mercs_lt",
    "ch_vwr_gang_ftb_base",
    "ch_vwr_gang_ftb_boss",
    "ch_vwr_gang_ftb_lt",
    "ch_gang_skin_base",
    "ch_vwr_gang_skin_boss",
    "ch_vwr_gang_skin_lt",
    "ch_vwr_gang_srk_base",
    "ch_vwr_gang_srk_boss",
    "ch_vwr_gang_srk_lt",
    "ch_vwr_electro_suit",
    "ch_vwr_electro_nosuit",
    "ch_vwr_gmu_businessman",
    "ch_vwr_gang_hellions_boss",
    "ch_vwr_shocker",
    "ch_vwr_boomerang",
    "ch_vwr_beetle_viewer",
    "ch_vwr_venom_eddie",
    "ch_vwr_venom_spider",
    "ch_vwr_ultimate_spiderman",
    "ch_vwr_usm_blacksuit",
    "ch_vwr_green_goblin",
    "ch_vwr_carnage",
    "ch_vwr_mary_jane",
    "ch_vwr_rhino",
    "ch_vwr_gmu_child_male",
    "ch_vwr_gmu_cop_thin",
    "ch_vwr_gmu_man",
    "ch_vwr_gmu_woman",
    "ch_vwr_gmu_medic_fem",
    "ch_vwr_army_mary_jane",
    "ch_vwr_army_mary_jane_p2"

};

void script_toggle_handler(debug_menu_entry* entry)
{
    printf("script_toggle_handler\n");
    assert(entry->get_id() < NUM_SCRIPTS);
    hero_selected = entry->get_id();
    hero_status = hero_status_e::REMOVE_PLAYER;
}

void script_entry_callback(debug_menu_entry* entry)
{

    printf("hero_entry_callback: character_status = %d\n", hero_status);

    auto v18 = g_world_ptr()->num_players;
    switch (hero_status) {
    case hero_status_e::REMOVE_PLAYER: {
        auto v3 = entry->get_bval();
        g_world_ptr()->remove_player(v18 - 1);
        hero_status = hero_status_e::ADD_PLAYER;
        frames_to_skip = 2;
        g_game_ptr()->enable_marky_cam(true, true, -1000.0, 0.0);
        break;
    }
    case hero_status_e::ADD_PLAYER: {
        auto v1 = frames_to_skip--;
        if (v1 <= 0) {
            assert(hero_selected > -1 && hero_selected < NUM_SCRIPTS);

            [[maybe_unused]] auto v2 = g_world_ptr()->add_player(mString { scripts[hero_selected] });

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
        if (v3 <= 0) {
            auto v17 = 0;
            auto* v5 = (actor*)g_world_ptr()->get_hero_ptr(0);
            auto* v6 = v5->get_player_controller();
            auto v9 = v6->field_420;
            switch (v9) {
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

            auto* v7 = g_world_ptr()->get_hero_ptr(0);
            auto v8 = v7->my_handle;
            g_femanager().IGO->hero_health->SetType(v17, v8.field_0);
            g_femanager().IGO->hero_health->SetShown(false);
            close_debug();
            hero_status = hero_status_e::UNDEFINED;
        }
        break;
    }
    default:
        break;
    }
}

void create_script_menu(debug_menu* script_menu)
{
    for (auto i = 0u; i < NUM_SCRIPTS; ++i) {
        auto v6 = 25;
        string_hash v5 { scripts[i] };
        auto v11 = resource_key { v5, v6 };
        auto v30 = resource_manager::get_pack_file_stats(v11, nullptr, nullptr, nullptr);
        if (v30) {
            mString v35 { scripts[i] };

            debug_menu_entry v37 { v35.c_str() };

            v37.set_game_flags_handler(script_toggle_handler);
            v37.m_id = i;
            v37.set_frame_advance_cb(script_entry_callback);
            script_menu->add_entry(&v37);
        }
    }
}








void debug_menu::init() {

	root_menu = create_menu("Debug Menu", handle_debug_entry, 10);
    game_menu = create_menu("Game", handle_game_entry, 300);
	script_menu = create_menu("Script");
        progression_menu = create_menu("Progression");
    level_select_menu = create_menu("Level Select");


    debug_menu_entry v1;
   debug_menu_entry* block = v1.alloc_block(game_menu,4);
   block[0] = debug_menu_entry{game_menu };

   debug_menu_entry v2;
   debug_menu_entry* block1 = v2.alloc_block(level_select_menu, 4);
   block1[0] = debug_menu_entry{ level_select_menu };

   debug_menu_entry v3;
   debug_menu_entry* block2 = v3.alloc_block(script_menu, 4);
   block2[0] = debug_menu_entry{ script_menu };

   debug_menu_entry v4;
   debug_menu_entry* block3 = v4.alloc_block(progression_menu, 4);
   block3[0] = debug_menu_entry{ progression_menu };
    debug_menu_entry game_entry{ game_menu };
	debug_menu_entry script_entry { script_menu };
	debug_menu_entry progression_entry { progression_menu };
	debug_menu_entry level_select_entry { level_select_menu };


    create_dvars_menu(root_menu);
    create_warp_menu(root_menu);
    create_game_flags_menu(game_menu);
    add_debug_menu_entry(root_menu, &game_entry);
	create_missions_menu(root_menu);
    create_debug_render_menu(root_menu);
    create_debug_district_variants_menu(root_menu);
    create_replay_menu(root_menu);
    create_ai_root_menu(root_menu);
    create_memory_menu(root_menu);
    create_entity_variants_menu(root_menu);
    create_entity_animation_menu(root_menu);
    add_debug_menu_entry(root_menu, &level_select_entry);
    add_debug_menu_entry(root_menu, &script_entry);

    add_debug_menu_entry(root_menu, &progression_entry);
   // create_movie_play_menu(script_menu);
    create_script_menu(script_menu);
    create_camera_menu_items(root_menu);
	
    

	/*
	for (int i = 0; i < 5; i++) {

		debug_menu_entry asdf;
		sprintf(asdf.text, "entry %d", i);
		printf("AQUI %s\n", asdf.text);

		add_debug_menu_entry(debug_menu::root_menu, &asdf);
	}
	add_debug_menu_entry(debug_menu::root_menu, &teste);
	*/
}

BOOL install_hooks() {
    return set_text_to_writable() && install_patches() &&
        restore_text_perms();
}

// ---------------------------------------------------------------------------
// 1) Put this near your other globals
// ---------------------------------------------------------------------------
static const wchar_t* kDllList[] = {
    L"console.dll",
    L"1.dll", // add as many names as you like
    nullptr // <-- terminator
};

static std::vector<HMODULE> g_loadedDlls; // handles we own (for later FreeLibrary)


static DWORD WINAPI LoaderThread(LPVOID) noexcept
{
    for (const wchar_t** p = kDllList; *p; ++p) {
        // Skip if it is already loaded in this process
        if (GetModuleHandleW(*p))
            continue;

        HMODULE h = LoadLibraryW(*p);
        if (!h) {
            wchar_t buf[256];
            swprintf_s(buf, L"[loader] %s failed (err=%lu)\n", *p, GetLastError());
            OutputDebugStringW(buf);
            // OPTIONAL: bail out entirely if *any* DLL fails
            return 0;
        }
        g_loadedDlls.push_back(h);
    }

    OutputDebugStringW(L"[loader] all helper DLLs loaded\n");
    return 0;
}


static bool CmdlineHasSwitch(PCWSTR token) noexcept
{
    PCWSTR cmd = GetCommandLineW();
    return StrStrIW(cmd, token) != nullptr;
}

static HMODULE g_hConsoleDll = nullptr;
static HANDLE g_hHotkeyThread = nullptr;
static volatile bool g_bStopHotkeyThread = false;

#include <iostream>


void RestartExecutable(const std::string& args)
{
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);  // current EXE path

    std::string command = std::string("\"") + path + "\" " + args;

    // Launch new process
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    CreateProcessA(NULL, &command[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    // Close handles to the new process
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Exit current process
    ExitProcess(0);
}




bool LaunchExeWithCmdLine(const std::wstring& exePath, const std::wstring& cmdArgs)
{
    std::wstring fullCmd = L"\"" + exePath + L"\" " + cmdArgs;

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = {};

    BOOL success = CreateProcessW(
        NULL,                           // App name (use NULL with command line)
        &fullCmd[0],                    // Command line (modifiable buffer)
        NULL, NULL,                     // Process & thread security
        FALSE,                          // Inherit handles
        0,                              // Creation flags
        NULL, NULL,                     // Env block, current dir
        &si, &pi);                      // Startup info and process info

    if (!success) {
        std::wcerr << L"Failed to launch: " << exePath << L"\nError code: " << GetLastError() << std::endl;
        return false;
    }

    // Optional: wait for the process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Cleanup
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

#define GetKeyDown(k) (GetAsyncKeyState(k) & 0x8000)


static DWORD WINAPI HotkeyThread(LPVOID) noexcept
{
    while (!g_bStopHotkeyThread)
    {
        if (GetKeyDown(VK_F12))  // Load console.dll
        {
            if (!g_hConsoleDll)
            {
                RestartExecutable("--console");



            }
        }
        if (GetKeyDown(VK_F11))  // Unload console.dll
        {
            if (!g_hConsoleDll)
            {
                RestartExecutable("--no-console");

            }
        }
        if (GetKeyDown(VK_F10))  // Exit
        {
            if (!g_hConsoleDll)
            {
                ExitProcess(0);

            }
        }
        Sleep(100);
    }
    return 0;
}
char* args = GetCommandLineA();

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
    if (sizeof(region) != 0x134)
        __debugbreak();

    memset(keys, 0, sizeof(keys));
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:

#if 0
        AllocConsole();

        if (!freopen("CONOUT$", "w", stdout)) {
            MessageBoxA(NULL, "Error", "Couldn't allocate console...Closing", 0);
            return FALSE;
        }
#endif
        DisableThreadLibraryCalls(hInst);

        if (!install_hooks())
            return FALSE;

        bool enableConsole;

        if (CmdlineHasSwitch(L"--no-console"))
            enableConsole = false;
        else if (CmdlineHasSwitch(L"--console"))
            enableConsole = true;




        if (enableConsole)
        {
            g_hWorker = CreateThread(nullptr, 0, &LoaderThread,
                nullptr, 0, nullptr);
            if (!g_hWorker)
                return FALSE;
        }
        g_hHotkeyThread = CreateThread(nullptr, 0, &HotkeyThread, nullptr, 0, nullptr);
        for (HMODULE h : g_loadedDlls)
            FreeLibrary(h);
        break;
    case DLL_PROCESS_DETACH:
        if (g_hWorker)
        {
            WaitForSingleObject(g_hWorker, 5000);
            CloseHandle(g_hWorker);
        }
        if (g_hWorker) {
            
        }
        FreeConsole();
        g_loadedDlls.clear();
        FreeConsole();
        break;
    }
    return TRUE;
}






int main() 
{ 
    return 0; 
}
