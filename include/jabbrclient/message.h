#pragma once

#include "cpprest\details\basic_types.h"
#include "cpprest\asyncrt_utils.h"
#include "user.h"
namespace jabbr
{
    struct message
    {
        bool is_htnl_encoded;
        utility::string_t id;
        utility::string_t content;
        utility::datetime when;
        user user;
    };
}
