#pragma once

#include "cpprest\details\basic_types.h"
#include "cpprest\json.h"
#include "user.h"
#include "message.h"

namespace jabbr
{
    struct room
    {
        utility::string_t name;
        int count;
        bool is_private;
        utility::string_t topic;
        bool is_closed;
        utility::string_t welcome;
        std::vector<user> users;
        std::vector<utility::string_t> owners;
        std::vector<message> recent_messages;
    };
}