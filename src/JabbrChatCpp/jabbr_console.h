#pragma once

#include <Windows.h>
#include <mutex>
#include "jabbrclient\jabbr_client.h"
#include "jabbr_user.h"

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
    void reset_prompt();
    void flush_prompt();
    void reset_main_panel();
    void set_cursor_position();
    void fill(short top, short left, short width, short height, wchar_t filler, unsigned short attributes);
    void safe_console_write(const CHAR_INFO* buffer, COORD buffer_size, SMALL_RECT& write_area);

private:
    jabbr::jabbr_client m_jabbr_client;
    const HANDLE m_input_handle;
    const HANDLE m_output_handle;


    static const short m_console_width = 160;
    static const short m_console_height = 50;

    static const short m_main_panel_height = 45;
    static const short m_main_panel_width = 130;
    static const short m_main_panel_buffer_size = m_main_panel_height * m_main_panel_width;

    COORD m_main_panel_size;
    COORD m_main_panel_coordinates;
    SMALL_RECT m_main_panel_area;
    CHAR_INFO m_main_panel_buffer[m_main_panel_buffer_size];

    static const short m_prompt_height = 3;
    static const short m_prompt_line_length = m_prompt_height * m_console_width;

    COORD m_prompt_size;
    COORD m_prompt_coordinates;
    SMALL_RECT m_prompt_area;
    short m_cursor_location;
    CHAR_INFO m_prompt_buffer[m_prompt_line_length];

    std::mutex m_console_write_lock;

    jabbr_user m_jabbr_user;
};

