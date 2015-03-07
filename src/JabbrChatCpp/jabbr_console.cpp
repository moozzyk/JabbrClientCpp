#include "stdafx.h"
#include <stdexcept>
#include "jabbr_console.h"
#include "jabbr_user.h"
#include "formatter.h"

jabbr_console::jabbr_console(jabbr::jabbr_client client)
    : m_jabbr_client(client), m_input_handle(GetStdHandle(STD_INPUT_HANDLE)), m_output_handle(GetStdHandle(STD_OUTPUT_HANDLE))
{
    m_main_panel_size = { m_main_panel_width, m_main_panel_width };
    m_main_panel_coordinates = { 0, 0 };
    m_main_panel_area = { m_main_panel_coordinates.X, m_main_panel_coordinates.Y, m_main_panel_width - 1, m_main_panel_height - 1 };

    m_prompt_size = { m_console_width, m_prompt_height };
    m_prompt_coordinates = { 0, m_console_height - m_prompt_height - 1 };
    m_prompt_area = { m_prompt_coordinates.X, m_prompt_coordinates.Y, m_console_width - 1, m_console_height - 1 };

    SMALL_RECT console_size{ 0, 0, m_console_width - 1, m_console_height - 1 };
    SetConsoleWindowInfo(m_output_handle, TRUE, &console_size);

    COORD console_buffer_size{ m_console_width, m_console_height };
    SetConsoleScreenBufferSize(m_output_handle, console_buffer_size);
    SetConsoleMode(m_input_handle, ENABLE_WINDOW_INPUT);
}

jabbr_console::~jabbr_console() = default;

void jabbr_console::run(utility::string_t user_name, utility::string_t password)
{
    reset_console();
    display_welcome();

    try
    {
        connect(user_name, password);
        formatter::format_manual(m_main_panel_buffer, m_main_panel_width, m_main_panel_height);
        formatter::format_user_on_welcome_page(m_jabbr_user, m_main_panel_buffer, m_main_panel_width, m_main_panel_height);
        safe_console_write(m_main_panel_buffer, m_main_panel_size, m_main_panel_area);
    }
    catch (const std::exception& e)
    {
        formatter::write_centered(
            utility::string_t(U("Could not connect to jabbr server."))
                .append(utility::conversions::to_string_t(e.what())),
            m_main_panel_buffer, m_main_panel_width, m_main_panel_height, 6,
            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

        return;
    }

    run();
}

void jabbr_console::display_welcome()
{
    formatter::format_welcome_page_header(m_main_panel_buffer, m_main_panel_width, m_main_panel_height);
    safe_console_write(m_main_panel_buffer, m_main_panel_size, m_main_panel_area);
}

void jabbr_console::connect(utility::string_t user_name, utility::string_t password)
{
    auto connect_task = m_jabbr_client.connect(user_name, password)
        .then([&](jabbr::log_on_info log_on_info)
    {
        return m_jabbr_client.get_user_info()
            .then([log_on_info](jabbr::user user)
        {
            auto user_model = jabbr_user(user);
            for (const auto &room : log_on_info.rooms)
            {
                user_model.add_room(jabbr_room(room));
            }

            return pplx::task_from_result<jabbr_user>(user_model);
        });

    });

    for (auto i = 0; !connect_task.is_done(); i++)
    {
        auto connecting_string =
            utility::string_t(U("Connecting"))
            .append(utility::string_t(i % 6, U('.')))
            .append(utility::string_t(6 - (i % 6), U(' ')));

        formatter::write_centered(connecting_string, m_main_panel_buffer, m_main_panel_width, m_main_panel_height, 6,
            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

        safe_console_write(m_main_panel_buffer, m_main_panel_size, m_main_panel_area);

        pplx::wait(300);
    }

    m_jabbr_user = connect_task.get();

    formatter::write_centered(U("     Connected!     "), m_main_panel_buffer, m_main_panel_width, m_main_panel_height, 6,
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    safe_console_write(m_main_panel_buffer, m_main_panel_size, m_main_panel_area);
}

void jabbr_console::run()
{
    INPUT_RECORD event;

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
                reset_prompt();
                break;

            case VK_LEFT:
                if (m_cursor_location > 0)
                {
                    m_cursor_location--;
                }
                break;

            case VK_RIGHT:
                if (m_cursor_location < m_prompt_line_length - 1)
                {
                    m_cursor_location++;
                }
                break;

            case VK_BACK:
                if (m_cursor_location > 0)
                {
                    std::memmove(&m_prompt_buffer[m_cursor_location - 1], &m_prompt_buffer[m_cursor_location],
                        (m_prompt_line_length - m_cursor_location) * sizeof(CHAR_INFO));

                    m_prompt_buffer[m_prompt_line_length - 1].Char.UnicodeChar = L' ';

                    m_cursor_location--;
                }

                flush_prompt();
                break;

            default:
                if (m_cursor_location < m_prompt_line_length && event.Event.KeyEvent.uChar.UnicodeChar >= 0x20
                    && event.Event.KeyEvent.uChar.UnicodeChar <= 0xff)
                {
                    m_prompt_buffer[m_cursor_location].Char.UnicodeChar = event.Event.KeyEvent.uChar.UnicodeChar;
                    flush_prompt();

                    if (m_cursor_location < m_prompt_line_length - 1)
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
    fill(0, 0, m_console_width, m_console_height, L' ', FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
    fill(m_main_panel_height, 0, m_console_width, 1, L'-', FOREGROUND_GREEN);
    fill(0, m_main_panel_width, 1, m_main_panel_height, L'|', FOREGROUND_GREEN);

    reset_main_panel();
    reset_prompt();
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

void jabbr_console::reset_main_panel()
{
    for (int i = 0; i < m_main_panel_buffer_size; ++i)
    {
        m_main_panel_buffer[i].Char.UnicodeChar = L' ';
        m_main_panel_buffer[i].Attributes = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;
    }

    safe_console_write(m_main_panel_buffer, m_main_panel_size, m_main_panel_area);

    set_cursor_position();
}

void jabbr_console::reset_prompt()
{
    m_cursor_location = 0;

    for (int i = 0; i < m_prompt_line_length; ++i)
    {
        m_prompt_buffer[i].Char.UnicodeChar = L' ';
        m_prompt_buffer[i].Attributes = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;
    }

    flush_prompt();
    set_cursor_position();
}

void jabbr_console::flush_prompt()
{
    safe_console_write(m_prompt_buffer, m_prompt_size, m_prompt_area);
}

void jabbr_console::set_cursor_position()
{
    SetConsoleCursorPosition(m_output_handle, COORD { m_prompt_coordinates.X + m_cursor_location % m_console_width,
        m_prompt_coordinates.Y + (m_cursor_location / m_console_width) });
}

void jabbr_console::safe_console_write(const CHAR_INFO* buffer, COORD buffer_size, SMALL_RECT& write_area)
{
    std::lock_guard<std::mutex> lock(m_console_write_lock);

    WriteConsoleOutput(m_output_handle, buffer, buffer_size, COORD{ 0, 0 }, &write_area);
}