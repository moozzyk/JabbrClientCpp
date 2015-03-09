#pragma once;
#include "Windows.h"
#include "jabbr_user.h"
#include "panel.h"

namespace formatter
{
    short write_centered(const utility::string_t& contents, panel& panel, short row, unsigned short attributes);
    void format_welcome_page_header(panel& panel);
    void format_manual(panel& panel);
    void format_user_on_welcome_page(const jabbr_user& user, panel& panel);
    void format_error(panel& panel, const std::wstring& error_message);
    void format_info(panel& panel, const std::wstring& error_message);
}
