#pragma once

#include "cpprest\details\basic_types.h"
#include <vector>
#include "room.h"

namespace jabbr
{
    struct log_on_info
    {
        std::vector<room> rooms;
    };
}