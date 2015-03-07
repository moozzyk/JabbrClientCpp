#include "stdafx.h"
#include "jabbr_user.h"

jabbr_user::jabbr_user()
{}

jabbr_user::jabbr_user(const jabbr::user& user)
    : m_user(user)
{}

jabbr_user::~jabbr_user() = default;

void jabbr_user::add_room(const jabbr_room& room)
{
    m_rooms.push_back(room);
}

utility::string_t jabbr_user::get_name() const
{
    return m_user.name;
}

const std::vector<jabbr_room>& jabbr_user::get_rooms() const
{
    return m_rooms;
}
