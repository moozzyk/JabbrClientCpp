#include "stdafx.h"
#include "jabbr_room.h"

jabbr_room::jabbr_room()
{}

jabbr_room::jabbr_room(const jabbr::room& room)
    : m_room(room)
{}

jabbr_room::~jabbr_room() = default;

utility::string_t jabbr_room::get_name() const
{
    return m_room.name;
}
