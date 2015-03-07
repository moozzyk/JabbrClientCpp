#pragma once

#include "cpprest\json.h"
#include "jabbrclient\room.h"

namespace jabbr
{
    namespace json_materializer
    {
        user create_user(const web::json::value& value);
        room create_room(const web::json::value& value);
        void create_rooms(const web::json::value& value, std::vector<room>& rooms);
    }
}