#include "stdafx.h"
#include <stdexcept>
#include <cctype>
#include "jabbr_console.h"
#include "formatter.h"

jabbr_console::jabbr_console(const std::function<bool(const std::wstring& input)>& on_user_input)
    : m_on_user_input(on_user_input),
    m_input_handle(GetStdHandle(STD_INPUT_HANDLE)), m_output_handle(GetStdHandle(STD_OUTPUT_HANDLE)),
    m_main_panel(main_panel_width, main_panel_height), m_prompt_panel(prompt_panel_width, prompt_panel_height),
    m_status_panel(status_panel_width, status_panel_height), m_user_panel(user_panel_width, user_panel_height)
{
    m_main_panel_coordinates = { 0, 0 };
    m_prompt_panel_coordinates = { 0, console_height - prompt_panel_height };
    m_status_panel_coordinates = { 0, main_panel_height + 1 };

    m_user_panel_coordinates = { main_panel_width + 1 /*div*/ + 1, 0 };

    SMALL_RECT console_size{ 0, 0, console_width - 1, console_height - 1 };
    SetConsoleWindowInfo(m_output_handle, TRUE, &console_size);

    COORD console_buffer_size{ console_width, console_height };
    SetConsoleScreenBufferSize(m_output_handle, console_buffer_size);
    SetConsoleMode(m_input_handle, ENABLE_WINDOW_INPUT);

    reset_console();
}

jabbr_console::~jabbr_console() = default;

void jabbr_console::set_title(const std::wstring& title)
{
    SetConsoleTitle(title.c_str());
}

void jabbr_console::display_welcome(const std::vector<jabbr::room>& rooms)
{
    m_main_panel.clear();
    formatter::format_welcome_page_header(m_main_panel);
    formatter::format_manual(m_main_panel);
    formatter::format_user_on_welcome_page(rooms, m_main_panel);
    safe_console_write(m_main_panel, m_main_panel_coordinates);
}

void jabbr_console::display_connecting_status(std::wstring status_message)
{
    formatter::write_centered(status_message, m_main_panel, 6,
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    safe_console_write(m_main_panel, m_main_panel_coordinates);
}

void jabbr_console::run()
{
    INPUT_RECORD event;
    reset_prompt();
    auto exit = false;
    while (!exit)
    {
        DWORD events_read;
        if (!ReadConsoleInput(m_input_handle, &event, 1, &events_read))
        {
            throw std::runtime_error("ReadConsoleInput failed");
        }

        if (event.EventType == KEY_EVENT && event.Event.KeyEvent.bKeyDown)
        {
            switch(event.Event.KeyEvent.wVirtualKeyCode)
            {
            case VK_ESCAPE:
                exit = true;
                break;

            case VK_RETURN:
                if (!m_on_user_input(get_user_input()))
                {
                    exit = true;
                }
                reset_prompt();
                break;

            case VK_LEFT:
                if (m_cursor_location > 0)
                {
                    m_cursor_location--;
                }
                break;

            case VK_RIGHT:
                if (m_cursor_location < prompt_line_length - 1)
                {
                    m_cursor_location++;
                }
                break;

            case VK_BACK:
                if (m_cursor_location > 0)
                {
                    auto prompt_buffer = m_prompt_panel.get_buffer();

                    std::memmove(&prompt_buffer[m_cursor_location - 1], &prompt_buffer[m_cursor_location],
                        (prompt_line_length - m_cursor_location) * sizeof(CHAR_INFO));

                    prompt_buffer[prompt_line_length - 1].Char.UnicodeChar = L' ';

                    m_cursor_location--;
                }

                flush_prompt();
                break;

            default:
                if (m_cursor_location < prompt_line_length && event.Event.KeyEvent.uChar.UnicodeChar >= 0x20
                    && event.Event.KeyEvent.uChar.UnicodeChar <= 0xff)
                {
                    m_prompt_panel.get_buffer()[m_cursor_location].Char.UnicodeChar = event.Event.KeyEvent.uChar.UnicodeChar;
                    flush_prompt();

                    if (m_cursor_location < prompt_line_length - 1)
                    {
                        m_cursor_location++;
                    }
                }
            }

            set_cursor_position();
        }
    }
}

void jabbr_console::reset_console()
{
    fill(0, 0, console_width, console_height, L' ', FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
    fill(0, m_main_panel.get_width() + 1, 1, m_main_panel.get_height() + 1, L'#', FOREGROUND_GREEN);

    m_main_panel.clear();
    m_user_panel.clear();
    reset_status();
    reset_prompt();
    redraw();
}

void jabbr_console::redraw()
{
    safe_console_write(m_main_panel, m_main_panel_coordinates);
    safe_console_write(m_status_panel, m_status_panel_coordinates);
    safe_console_write(m_prompt_panel, m_prompt_panel_coordinates);
}

void jabbr_console::fill(short top, short left, short width, short height, wchar_t filler, unsigned short attributes)
{
    auto buffer_size = width * height;
    auto fill_buffer = new CHAR_INFO[buffer_size];
    for (int i = 0; i < buffer_size; i++)
    {
        fill_buffer[i].Attributes = attributes;
        fill_buffer[i].Char.UnicodeChar = filler;
    }

    auto area = SMALL_RECT{ left, top, left + width, top + height };
    safe_console_write(fill_buffer, COORD{ width, height }, area);

    delete[] fill_buffer;
}

void jabbr_console::reset_prompt()
{
    m_prompt_panel.fill(L' ', FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
    flush_prompt();
    m_cursor_location = 0;
    set_cursor_position();
}

void jabbr_console::flush_prompt()
{
    safe_console_write(m_prompt_panel, m_prompt_panel_coordinates);
}

void jabbr_console::set_cursor_position()
{
    SetConsoleCursorPosition(m_output_handle, COORD { m_prompt_panel_coordinates.X + m_cursor_location % console_width,
        m_prompt_panel_coordinates.Y + (m_cursor_location / console_width) });
}

void jabbr_console::safe_console_write(const CHAR_INFO* buffer, COORD buffer_size, SMALL_RECT& write_area)
{
    std::lock_guard<std::mutex> lock(m_console_write_lock);

    WriteConsoleOutput(m_output_handle, buffer, buffer_size, COORD{ 0, 0 }, &write_area);
}

void jabbr_console::safe_console_write(const panel& panel, COORD left_top)
{
    std::lock_guard<std::mutex> lock(m_console_write_lock);

    auto area = panel.get_area(left_top);
    WriteConsoleOutput(m_output_handle, panel.get_buffer(), panel.get_size(), COORD{ 0, 0 }, &area);
}

std::wstring jabbr_console::get_user_input()
{
    auto start_pos = 0, end_pos = prompt_line_length - 1;
    auto buffer = m_prompt_panel.get_buffer();
    for (; start_pos < prompt_line_length && std::isspace(buffer[start_pos].Char.UnicodeChar); start_pos++)
    { }

    for (; end_pos >= start_pos && std::isspace(buffer[end_pos].Char.UnicodeChar); end_pos--)
    { }

    auto input = std::wstring();
    input.reserve(end_pos - start_pos + 1);

    for (; start_pos <= end_pos; start_pos++)
    {
        input += buffer[start_pos].Char.UnicodeChar;
    }

    return input;
}

void jabbr_console::reset_status()
{
    m_status_panel.fill('#', FOREGROUND_GREEN);
}

void jabbr_console::display_error(const std::wstring& error)
{
    reset_status();
    formatter::format_error(m_status_panel, error);
    safe_console_write(m_status_panel, m_status_panel_coordinates);
}

void jabbr_console::display_info(const std::wstring& error)
{
    reset_status();
    formatter::format_info(m_status_panel, error);
    safe_console_write(m_status_panel, m_status_panel_coordinates);
}

void jabbr_console::clear_status()
{
    reset_status();
    safe_console_write(m_status_panel, m_status_panel_coordinates);

}

void jabbr_console::display_room(const jabbr::room& room)
{
    m_main_panel.clear();
    m_user_panel.clear();
    set_title(room.name + L" - " + room.topic + L" - " + room.welcome);

    for (auto const& m : room.recent_messages)
    {
        add_message(m);
    }

    safe_console_write(m_main_panel, m_main_panel_coordinates);

    short row = 1;

    short position = 1;
    m_user_panel.write_no_overflow(L"Users:", row++, position, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

    for (auto const& u : room.users)
    {
        position = 2;

        unsigned short status_color = 0;
        switch (u.status)
        {
        case user_status::active:
            status_color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case user_status::inactive:
            status_color = FOREGROUND_GREEN | FOREGROUND_RED| FOREGROUND_INTENSITY;
            break;
        case user_status::offline:
            status_color = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE;
            break;
        }

        m_user_panel.write_no_overflow(L" * ", row, position, status_color);
        m_user_panel.write_no_overflow(u.name, row, position, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        if (row == m_user_panel.get_height() - 1)
        {
            m_user_panel.scroll_up();
        }
        else
        {
            row++;
        }
    }

    safe_console_write(m_user_panel, m_user_panel_coordinates);
}

void jabbr_console::display_message(const jabbr::message& message)
{
    add_message(message);
    safe_console_write(m_main_panel, m_main_panel_coordinates);
}

void jabbr_console::add_message(const jabbr::message& message)
{
    m_main_panel.scroll_up();
    short current_position = 0;
    short row = m_main_panel.get_height() - 1;
    m_main_panel.write(message.when.to_string(utility::datetime::ISO_8601).substr(11, 8), row, current_position, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    m_main_panel.write(L" <", row, current_position, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    m_main_panel.write(message.user.name, row, current_position, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    m_main_panel.write(L">: ", row, current_position, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    m_main_panel.write(message.content, row, current_position, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void jabbr_console::display_private_message(const utility::string_t& from, const utility::string_t& message)
{
    m_main_panel.scroll_up();
    short current_position = 0;
    short row = m_main_panel.get_height() - 1;
    m_main_panel.write(L"*<", row, current_position, FOREGROUND_RED | FOREGROUND_INTENSITY);
    m_main_panel.write(from, row, current_position, FOREGROUND_RED | FOREGROUND_INTENSITY);
    m_main_panel.write(L"*>: ", row, current_position, FOREGROUND_RED | FOREGROUND_INTENSITY);
    m_main_panel.write(message, row, current_position, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    safe_console_write(m_main_panel, m_main_panel_coordinates);
}