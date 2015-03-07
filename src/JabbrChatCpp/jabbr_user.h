#pragma once

#include <vector>
#include "jabbrclient\user.h"
#include "jabbr_room.h"

class jabbr_user
{
public:
    jabbr_user();

    explicit jabbr_user(const jabbr::user& user);
    ~jabbr_user();

    utility::string_t get_name() const;
    const std::vector<jabbr_room>& get_rooms() const;

    void add_room(const jabbr_room& room);

    private:
        jabbr::user m_user;
        std::vector<jabbr_room> m_rooms;
};