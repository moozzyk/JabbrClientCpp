#include "stdafx.h"
#include "formatter.h"

namespace formatter
{
    static const unsigned short white_foreground = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    short write(const utility::string_t& contents, CHAR_INFO* buffer, short width, short height, short row, short column, unsigned short attributes)
    {
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

    short write_centered(const utility::string_t& contents, CHAR_INFO* buffer, short width, short height, short row, unsigned short attributes)
    {
        short start_column = static_cast<short>(std::max((width >> 1) - static_cast<short>(contents.length() >> 1), 0));

        return write(contents, buffer, width, height, row, start_column, attributes);
    }

    short write_flush_left(const utility::string_t& contents, CHAR_INFO* buffer, short width, short height, short row, unsigned short attributes)
    {
        return write(contents, buffer, width, height, row, 0, attributes);
    }

    static void write_line(const utility::string_t& header, const utility::string_t& contents, CHAR_INFO* buffer, short width, short height, short row, short column = 0)
    {
        column = write(header, buffer, width, height, row, column, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        if (column >= 0)
        {
            write(contents, buffer, width, height, row, column + 1, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        }
    }

    void format_welcome_page_header(CHAR_INFO* buffer, short width, short height)
    {
        write_centered(U("*** C++ Jabbr Chat ***"), buffer, width, height, 1, white_foreground | FOREGROUND_INTENSITY);
        write_centered(U("Chat like in 80's"), buffer, width, height, 3, white_foreground);
    }

    void format_manual(CHAR_INFO* buffer, short width, short height)
    {
        short column = static_cast<short>(std::max((width >> 1) - 10, 0));
        short start_row = 10;

        write(U("Commands:"), buffer, width, height, start_row, column, white_foreground | FOREGROUND_INTENSITY);
        write(U("/h         - show help screen"), buffer, width, height, start_row + 2, column, white_foreground);
        write(U("ESC        - turns off help screen"), buffer, width, height, start_row + 3, column, white_foreground);
        write(U("/j{room}   - join/switch room"), buffer, width, height, start_row + 4, column, white_foreground);
        write(U("/l{string} - list groups conatining {string}"), buffer, width, height, start_row + 4, column, white_foreground);
        write(U(":q         - exit"), buffer, width, height, start_row + 5, column, white_foreground);
    }

    void format_user_on_welcome_page(const jabbr_user& user, CHAR_INFO* buffer, short width, short height)
    {
        short start_row = 20;
        write_centered(U("Your rooms:"), buffer, width, height, start_row, white_foreground | FOREGROUND_INTENSITY);

        utility::ostringstream_t ss;
        for (const auto& r : user.get_rooms())
        {
            ss << r.get_name() << " ";
        }

        write_centered(ss.str(), buffer, width, height, start_row + 2, white_foreground);
    }
}