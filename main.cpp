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

static constexpr DWORD MAX_ELEMENTS_PAGE = 18;

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
	read_and_update_controller_key_button(joy, 9, MENU_SELECT);
        read_and_update_controller_key_button(joy, 11, MENU_TOGGLE);

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

    auto* mission_unload_entry = create_menu_entry(mString{ "UNLOAD CURRENT MISSION" });

    mission_unload_entry->set_game_flags_handler(mission_unload_handler);
    head_menu->add_entry(mission_unload_entry);

    auto* v2 = mission_manager::s_inst();
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

    auto* missions_menu = create_menu("Missions");
    auto* v2 = create_menu_entry(missions_menu);
   debug_menu_entry v1;
    debug_menu_entry* block = v1.alloc_block(missions_menu, 4);
    block[0] = debug_menu_entry{ missions_menu };
    v2->set_game_flags_handler(populate_missions_menu);
    parent->add_entry(v2);
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
    game_unpause(g_game_ptr());
    current_menu = current_menu;
    !os_developer_options::instance()->get_flag("ENABLE_ZOOM_MAP");
    spider_monkey::is_running();
    g_game_ptr()->enable_physics(false);
}









void menu_setup(int game_state, int keyboard) {

	//debug menu stuff
	if (is_menu_key_pressed(MENU_TOGGLE, keyboard) && (game_state == 6 || game_state == 7)) {


        if (!debug_enabled && game_state == 6) {
                game_unpause(g_game_ptr());
                debug_enabled = !debug_enabled;
                current_menu = debug_menu::root_menu;
                custom();
            }

            else if (!debug_disabled && game_state == 6) {
                game_unpause(g_game_ptr());
                debug_disabled = !debug_disabled;
                current_menu = current_menu;
                disable_physics();

            }

            else if (!debug_enabled && game_state == 6) {
                game_unpause(g_game_ptr());
                debug_enabled = !debug_enabled;
                current_menu = current_menu;
                disable_physics();

            }

            else if (!debug_enabled, debug_disabled && game_state == 6) {
                game_unpause(g_game_ptr());
                debug_disabled, debug_enabled = !debug_disabled, debug_enabled;
                current_menu = current_menu;
                enable_physics();
            }
        }

        if (is_menu_key_pressed(MENU_TOGGLE, keyboard) && (game_state == 6 || game_state == 7)) {

            if (!debug_enabled && game_state == 7) {
                game_unpause(g_game_ptr());
                debug_enabled = !debug_enabled;
                current_menu = debug_menu::root_menu;
                disable_physics();

            }

            else if (!debug_disabled && game_state == 7) {
                game_unpause(g_game_ptr());
                debug_disabled = !debug_disabled;
                current_menu = current_menu;
                disable_physics();

            }

            else if (!debug_enabled && game_state == 7) {
                game_unpause(g_game_ptr());
                debug_enabled = !debug_enabled;
                current_menu = current_menu;
                disable_physics();

            }

            else if (!debug_enabled, debug_disabled && game_state == 7) {
                game_unpause(g_game_ptr());
                debug_disabled, debug_enabled = !debug_disabled, debug_enabled;
                current_menu = current_menu;
                enable_physics();
            }

            if (!debug_enabled && game_state == 7) {
                game_unpause(g_game_ptr());
                debug_enabled = !debug_enabled;
                current_menu = debug_menu::root_menu;
                disable_physics();

            }

            else if (!debug_disabled && game_state == 7) {
                game_unpause(g_game_ptr());
                debug_disabled = !debug_disabled;
                current_menu = current_menu;
                disable_physics();

            }

            else if (!debug_enabled && game_state == 7) {
                game_unpause(g_game_ptr());
                debug_enabled = !debug_enabled;
                current_menu = current_menu;
                disable_physics();

            }

            else if (!debug_enabled, debug_disabled && game_state == 7) {
                game_unpause(g_game_ptr());
                debug_disabled, debug_enabled = !debug_disabled, debug_enabled;
                current_menu = current_menu;
                enable_physics();
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

   //resource_amalgapak_header_patch();

    mouselook_controller_patch();

   spider_monkey_patch();

    message_board_patch();
    
    wds_patch();



    ngl_patch();

    game_patch();

  //FrontEndMenuSystem_patch();







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

 








void create_devopt_menu(debug_menu* parent)
{
    assert(parent != nullptr);

    auto* v22 = create_menu("Devopts", handle_game_entry, 300);
    debug_menu_entry v1;
    debug_menu_entry* block3 = v1.alloc_block(v22, 4);
    block3[0] = debug_menu_entry { v22 };


    for (auto idx = 0u; idx < NUM_OPTIONS; ++idx) {
        auto* v21 = get_option(idx);
        switch (v21->m_type) {
        case game_option_t::INT_OPTION: {
            auto v20 = debug_menu_entry(mString { v21->m_name });
            v20.set_p_ival(v21->m_value.p_ival);
            v20.set_min_value(-1000.0);
            v20.set_max_value(1000.0);
            v22->add_entry(&v20);
            break;
        }
        case game_option_t::FLAG_OPTION: {
            auto v19 = debug_menu_entry(mString { v21->m_name });
            v19.set_pt_bval((bool*)v21->m_value.p_bval);
            v22->add_entry(&v19);
            break;
        }
        case game_option_t::FLOAT_OPTION: {
            auto v18 = debug_menu_entry(mString { v21->m_name });
            v18.set_pt_fval(v21->m_value.p_fval);
            v18.set_min_value(-1000.0);
            v18.set_max_value(1000.0);
            v22->add_entry(&v18);
            break;
        }
        default:
            break;
        }
    }

    auto v5 = debug_menu_entry(v22);
    parent->add_entry(&v5);
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

    v89 = debug_menu_entry(mString { "Slow Motion Enabled" });
    v89.set_bval(false);
    v89.set_game_flags_handler(game_flags_handler);
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
    create_devopt_menu(v92);
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