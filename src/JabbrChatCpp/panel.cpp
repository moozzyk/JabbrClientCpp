#include "stdafx.h"
#include "panel.h"

panel::panel(short width, short height)
    : m_width(width), m_height(height)
{
    m_size = { width, height };
    m_buffer = new CHAR_INFO[get_buffer_size()];
}

panel::~panel()
{
    delete[] m_buffer;
}

short panel::get_width() const
{
    return m_width;
}

short panel::get_height() const
{
    return m_height;
}

COORD panel::get_size() const
{
    return m_size;
}

short panel::get_buffer_size() const
{
    return get_width() * get_height();
}

SMALL_RECT panel::get_area(COORD left_top) const
{
    return SMALL_RECT{ left_top.X, left_top.Y, left_top.X + get_width() - 1, left_top.Y + get_height() - 1 };
}

CHAR_INFO* panel::get_buffer() const
{
    return m_buffer;
}

void panel::clear()
{
    fill(L' ', 0);
}

void panel::fill(wchar_t filler, unsigned short attributes)
{
    for (int i = 0; i < get_buffer_size(); i++)
    {
        m_buffer[i].Attributes = attributes;
        m_buffer[i].Char.UnicodeChar = filler;
    }
}

void panel::scroll_up()
{
    std::memmove(m_buffer, m_buffer + get_width(), get_width() * (get_height() - 1) * sizeof(CHAR_INFO));
    for (int i = 0; i < get_width(); i++)
    {
        m_buffer[get_width() * (get_height() - 1) + i].Char.UnicodeChar = ' ';
        m_buffer[get_width() * (get_height() - 1) + i].Attributes = 0;
    }
}

void panel::write(const std::wstring& message, short& row, short& position, unsigned short attributes)
{
    for (size_t i = 0; i < message.length();)
    {
        for (; position < get_width() && i < message.length(); i++, position++)
        {
            m_buffer[(row * get_width()) + position].Char.UnicodeChar = message.at(i);
            m_buffer[(row * get_width()) + position].Attributes = attributes;
        }

        if (i < message.length())
        {
            position = 0;
            if (row == get_height() - 1)
            {
                scroll_up();
            }
            else
            {
                row++;
            }
        }
    }
}

void panel::write_no_overflow(const std::wstring& message, short row, short& position, unsigned short attributes)
{
    for (size_t i = 0; position < get_width() && i < message.length(); i++, position++)
    {
        m_buffer[(row * get_width()) + position].Char.UnicodeChar = message.at(i);
        m_buffer[(row * get_width()) + position].Attributes = attributes;
    }
}