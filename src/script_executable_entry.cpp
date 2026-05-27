#include "script_executable_entry.h"

#include "common.h"

VALIDATE_SIZE(script_executable_entry_key, 0x10);
VALIDATE_SIZE(script_executable_entry, 0x14);

using pair = std::pair<script_executable_entry_key, script_executable_entry>;

VALIDATE_SIZE(pair, 0x24);


