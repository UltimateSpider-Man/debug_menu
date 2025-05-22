#pragma once

#include "func_wrapper.h"
#include "resource_key.h"

#include "resource_location.h"

#include "resource_partition.h"

#include "resource_pack_location.h"

#include "nlPlatformEnum.h"

#include "variable.h"

struct resource_partition;
struct resource_pack_location;
struct resource_pack_slot;
struct mString;

namespace resource_manager {

inline mString& amalgapak_name = var<mString>(0x0095CAD4);

inline std::vector<resource_partition*>*& partitions = var<std::vector<resource_partition*>*>(0x0095C7F0);

inline int& amalgapak_base_offset = var<int>(0x00921CB4);

//    nflFileID& amalgapak_id = var<nflFileID>(0x00921CB8);

inline int& resource_buffer_used = var<int>(0x0095C180);

inline int& memory_maps_count = var<int>(0x0095C7F4);

inline int& resource_buffer_size = var<int>(0x0095C1C8);

inline int& in_use_memory_map = var<int>(0x00921CB0);

inline uint8_t*& resource_buffer = var<uint8_t*>(0x0095C738);

inline bool& using_amalga = var<bool>(0x0095C800);

inline int& amalgapak_signature = var<int>(0x0095C804);

//   resource_memory_map*& memory_maps = var<resource_memory_map*>(0x0095C2F0);

inline int& amalgapak_pack_location_count = var<int>(0x0095C7FC);

inline resource_pack_location*& amalgapak_pack_location_table = var<resource_pack_location*>(0x0095C7F8);

inline int& amalgapak_prerequisite_count = var<int>(0x0095C174);

inline resource_key*& amalgapak_prerequisite_table = var<resource_key*>(0x0095C300);

inline resource_partition* get_partition_pointer(resource_partition_enum which_type)
{
    assert(partitions != nullptr);
    assert(which_type >= 0 && which_type < static_cast<int>(partitions->size()));

    return partitions->at(which_type);
}

inline void load_amalgapak()
{
    CDECL_CALL(0x00537650);
}

inline void reload_amalgapak()
{
    CDECL_CALL(0x0054C2E0);
}

inline bool can_reload_amalgapak()
{
    return (bool)CDECL_CALL(0x0053DE90);
}
// 0x0055DEA0
inline int compare_resource_key_resource_pack_location(const resource_key& a1,
    const resource_pack_location& a2)
{
    return (int)CDECL_CALL(0x0055DEA0, &a1, &a2);
}

template <typename T0, typename T1>
inline bool binary_search_array_cmp(
    T0* a1, T1* a2, int start_index, int count, int* p_index, int (*fn_compare)(T0&, T1&))
{
    // sp_log("%d", count);
    bool result = false;
    while (start_index < count) {
        int mid_index = (start_index + count) / 2;
        int res_comp = fn_compare(*a1, a2[mid_index]);
        if (res_comp > -1) {
            if (res_comp < 1) {
                result = true;

                if (p_index != nullptr) {
                    *p_index = mid_index;
                }

                break;
            }

            start_index = mid_index + 1;
        } else {
            count = (count + start_index) / 2;
        }
    }

    if (!result && p_index != nullptr) {
        *p_index = count;
    }

    return result;
}

inline bool get_pack_file_stats(const resource_key& a1, resource_pack_location* a2, mString* a3, int* a4)
{
    TRACE("resource_manager::get_pack_file_stats", a1.get_platform_string(g_platform).c_str());

    if constexpr (1) {
        assert(amalgapak_pack_location_table != nullptr);

        if (a3 != nullptr) {
            *a3 = amalgapak_name.c_str();
        }

        assert(amalgapak_base_offset != -1);

        {
            auto is_sorted = std::is_sorted(amalgapak_pack_location_table,
                amalgapak_pack_location_table + amalgapak_pack_location_count,
                [](auto& a1, auto& a2) {
                    return a1.loc.m_offset <= a2.loc.m_offset;
                });
            assert(is_sorted);
        }

        auto i = 0;
        if (!binary_search_array_cmp<const resource_key, const resource_pack_location>(
                &a1,
                amalgapak_pack_location_table,
                0,
                amalgapak_pack_location_count,
                &i,
                compare_resource_key_resource_pack_location)) {
            return false;
        }

        if (a2 != nullptr) {
            *a2 = amalgapak_pack_location_table[i];
            a2->loc.m_size += amalgapak_base_offset;
        }

        if (a4 != nullptr) {
            *a4 = i;
        }

        return true;
    } else {
        auto result = (bool)CDECL_CALL(0x0052A820, &a1, a2, a3, a4);
        printf("%s", result ? "true" : "false");
        return result;
    }
}

inline bool get_pack_file_stats2(const resource_key& a1, resource_pack_location* a2, mString* a3, int* a4)
{

    return (bool)CDECL_CALL(0x0052A820, &a1, a2, a3, a4);
}

inline resource_pack_slot* push_resource_context(resource_pack_slot* pack_slot)
{
    return (resource_pack_slot*)CDECL_CALL(0x00542740, pack_slot);
}

inline resource_pack_slot* pop_resource_context()
{
    return (resource_pack_slot*)CDECL_CALL(0x00537530);
}

inline void set_active_district(bool a1)
{
    void(__fastcall * func)(void*, int, bool) = (decltype(func))0x00573620;
    func(nullptr, 0, a1);
}

inline resource_partition* get_partition_pointer(int which_type)
{
    return (resource_partition*)CDECL_CALL(0x00537AA0, which_type);
}

}
