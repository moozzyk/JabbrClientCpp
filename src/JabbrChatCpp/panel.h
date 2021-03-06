#pragma once
class panel
{
public:
    panel(short height, short width);
    panel(const panel&) = delete;
    panel& operator=(const panel&) = delete;
    ~panel();

    short get_width() const;
    short get_height() const;
    COORD get_size() const;
    SMALL_RECT get_area(COORD left_top) const;
    short get_buffer_size() const;
    CHAR_INFO* get_buffer() const;

    void clear();
    void fill(wchar_t filler, unsigned short attributes);
    void scroll_up();
    void write(const std::wstring& message, short& row, short& position, unsigned short attributes);
    void write_no_overflow(const std::wstring& message, short row, short& position, unsigned short attributes);

private:
    short m_width;
    short m_height;
    COORD m_size;
    CHAR_INFO* m_buffer;
};

