#pragma once

#include "jabbrclient\room.h"

class jabbr_room
{
public:
    jabbr_room();
    explicit jabbr_room(const jabbr::room& room);
    ~jabbr_room();

    utility::string_t get_name() const;

private:
    jabbr::room m_room;
};

