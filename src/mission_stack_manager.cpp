#include "mission_stack_manager.h"

#include "func_wrapper.h"
#include "game.h"
#include "osassert.h"
#include "resource_directory.h"
#include "resource_manager.h"
#include "resource_partition.h"
#include "trace.h"
#include "terrain.h"
#include "wds.h"

#include "common.h"

VALIDATE_SIZE(mission_stack_manager, 12u);


bool mission_stack_manager::waiting_for_push_or_pop()
{
    return (bool)CDECL_CALL(0x005BB640, this);
}


void mission_stack_manager::pop_mission_pack(const mString& a2, const mString& a3)
{
    CDECL_CALL(0x005D5800, this, &a2, &a3);
}




void mission_stack_manager::pop_mission_pack_immediate(const mString &a1, const mString &a2)
{
    this->pop_mission_pack(a1, a2);

    resource_partition *partition = resource_manager::get_partition_pointer(RESOURCE_PARTITION_MISSION);
    assert(partition != nullptr);
    assert(partition->get_streamer() != nullptr);

    auto *streamer = partition->get_streamer();

}






