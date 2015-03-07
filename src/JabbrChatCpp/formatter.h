#pragma once;
#include "Windows.h"
#include "jabbr_user.h"

namespace formatter
{
    short write_centered(const utility::string_t& contents, CHAR_INFO* buffer, short width, short height, short row, unsigned short attributes);
    void format_welcome_page_header(CHAR_INFO* buffer, short width, short height);
    void format_manual(CHAR_INFO* buffer, short width, short height);
    void format_user_on_welcome_page(const jabbr_user& user, CHAR_INFO* buffer, short width, short height);
}
