#pragma once

#include "cpprest\details\basic_types.h"
#include "user_status.h"

namespace jabbr
{
    struct user
    {
        utility::string_t name;
        utility::string_t hash;
        bool is_active;
        user_status status;
        utility::string_t note;
        utility::string_t afk_note;
        bool is_afk;
        utility::string_t flag;
        utility::string_t country;
        utility::datetime last_activity;
        bool is_admin;
    };
};