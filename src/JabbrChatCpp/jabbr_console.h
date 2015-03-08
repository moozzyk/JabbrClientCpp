#pragma once

#include <Windows.h>
#include <mutex>
#include "jabbrclient\jabbr_client.h"
#include "jabbr_user.h"
#include "panel.h"

class jabbr_console
{
public:
    jabbr_console(jabbr::jabbr_client);
    ~jabbr_console();

    void run(utility::string_t user_name, utility::string_t password);

private:
    void run();
    void display_welcome();
    void connect(utility::string_t user_name, utility::string_t password);
    void reset_console();
    void redraw();
    void reset_prompt();
    void flush_prompt();
    void set_cursor_position();
    std::wstring process_input();
    void fill(short top, short left, short width, short height, wchar_t filler, unsigned short attributes);
    void safe_console_write(const CHAR_INFO* buffer, COORD buffer_size, SMALL_RECT& write_area);
    void jabbr_console::safe_console_write(const panel& panel, COORD left_top);

private:
    jabbr::jabbr_client m_jabbr_client;
    const HANDLE m_input_handle;
    const HANDLE m_output_handle;

    static const short console_width = 160;
    static const short console_height = 50;
    static const short main_panel_height = 45;
    static const short main_panel_width = 130;
    static const short prompt_panel_width = console_width;
    static const short prompt_panel_height = 3;
    static const short status_panel_height = 1;
    static const short status_panel_width = console_width;
    static const short prompt_line_length = prompt_panel_height * prompt_panel_width;

    COORD m_main_panel_coordinates;
    panel m_main_panel;
    COORD m_prompt_panel_coordinates;
    panel m_prompt_panel;
    COORD m_status_panel_coordinates;
    panel m_status_panel;

    short m_cursor_location = 0;

    std::mutex m_console_write_lock;

    jabbr_user m_jabbr_user;
};

