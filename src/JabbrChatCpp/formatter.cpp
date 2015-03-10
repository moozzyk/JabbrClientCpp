#include "stdafx.h"
#include "formatter.h"

namespace formatter
{
    static const unsigned short white_foreground = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    short write(const utility::string_t& contents, panel& panel, short row, short column, unsigned short attributes)
    {
        auto buffer = panel.get_buffer();
        auto width = panel.get_width();
        auto height = panel.get_height();

        if (row >= height || row < 0 || column >= width || column < 0)
        {
            return -1;
        }

        auto col = column;
        for (unsigned int letter_idx = 0; col < width && letter_idx < contents.length(); col++, letter_idx++)
        {
            buffer[row * width + col].Char.UnicodeChar = contents.at(letter_idx);
            buffer[row * width + col].Attributes = attributes;
        }

        return col;
    }

    short write_centered(const utility::string_t& contents, panel& panel, short row, unsigned short attributes)
    {
        short start_column = static_cast<short>(std::max((panel.get_width() >> 1) - static_cast<short>(contents.length() >> 1), 0));

        return write(contents, panel, row, start_column, attributes);
    }

    short write_flush_left(const utility::string_t& contents, panel& panel, short row, unsigned short attributes)
    {
        return write(contents, panel, row, 0, attributes);
    }

    void format_welcome_page_header(panel& panel)
    {
        write_centered(U("*** C++ Jabbr Chat ***"), panel, 1, white_foreground | FOREGROUND_INTENSITY);
        write_centered(U("Chat like in 80's"), panel, 3, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }

    void format_manual(panel& panel)
    {
        short column = static_cast<short>(std::max((panel.get_width() >> 1) - 10, 0));
        short start_row = 10;

        write(U("Commands:"), panel, start_row, column, white_foreground | FOREGROUND_INTENSITY);
        write(U("/j {room}  - join/switch room"), panel, start_row + 2, column, white_foreground);
        write(U("/l         - leave current room"), panel, start_row + 3, column, white_foreground);
//        write(U("/j {room}  - join/switch room"), panel, start_row + 4, column, white_foreground);
//        write(U("/l{string} - list groups conatining {string}"), panel, start_row + 5, column, white_foreground);
        write(U(":q         - exit"), panel, start_row + 6, column, white_foreground);
    }

    void format_user_on_welcome_page(const std::vector<jabbr::room>& rooms, panel& panel)
    {
        short start_row = 20;
        write_centered(U("Your rooms:"), panel, start_row, white_foreground | FOREGROUND_INTENSITY);

        utility::ostringstream_t ss;
        for (const auto& r : rooms)
        {
            ss << r.name << " ";
        }

        write_centered(ss.str(), panel, start_row + 2, white_foreground);
    }

    void format_error(panel& panel, const std::wstring& error_message)
    {
        write_flush_left(error_message, panel, 0, BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }

    void format_info(panel& panel, const std::wstring& error_message)
    {
        write_flush_left(error_message, panel, 0, BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }
}
