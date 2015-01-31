#pragma once

#include "cpprest\json.h"
#include "jabbrclient\room.h"

namespace jabbr
{
    namespace json_materializer
    {
        room create_room(const web::json::value& value);
    }
}