#include "stdafx.h"
#include "jabbr_chat.h"
#include "pplx\pplxtasks.h"
#include "jabbr_user.h"


jabbr_chat::jabbr_chat(std::wstring url)
    : m_jabbr_client(jabbr::jabbr_client{ url }),
    m_console(std::make_shared<jabbr_console>(std::bind(&jabbr_chat::on_user_input, this, std::placeholders::_1)))
{
    // m_jabbr_client stopped synchronously so it's OK to capture by &
    m_jabbr_client.set_on_message_received([&](const jabbr::message& msg, const utility::string_t& room_name)
    {
        if (m_current_room.name == room_name)
        {
            m_console->display_message(msg);
        }
    });

    m_jabbr_client.set_on_private_message_received([&](const utility::string_t& from, const utility::string_t& to, const utility::string_t& message)
    {
        if (to == m_user.name)
        {
            m_console->display_private_message(from, message);
        }
    });

}

jabbr_chat::~jabbr_chat() = default;

void jabbr_chat::run(std::wstring user_name, std::wstring password)
{
    m_console->display_welcome(m_rooms);

    auto connect_task = m_jabbr_client.connect(user_name, password)
        .then([&](const jabbr::log_on_info& log_on_info)
        {
            m_rooms = log_on_info.rooms;
            return m_jabbr_client.get_user_info();
        });

    for (auto i = 0; !connect_task.is_done(); i++)
    {
        auto connecting_string =
            utility::string_t(L"Connecting")
            .append(utility::string_t(i % 6, L'.'))
            .append(utility::string_t(6 - (i % 6), L' '));

        m_console->display_connecting_status(connecting_string);

        pplx::wait(300);
    }

    try
    {
        m_user = connect_task.get();
        m_console->display_welcome(m_rooms);
        m_console->display_connecting_status(L"   Connected    ");
        m_console->run();
    }
    catch (const std::exception &e)
    {
        m_console->display_connecting_status(utility::string_t(L"Could not connect to jabbr server. Error: ")
            .append(utility::conversions::to_string_t(e.what())));
    }

    m_jabbr_client.log_out(m_user.name)
        .then([&]()
        {
            m_jabbr_client.disconnect();
        }).get();
}

bool jabbr_chat::on_user_input(const std::wstring& user_input)
{
    m_console->clear_status();

    auto command = parser.parse(user_input);

    switch (command.type)
    {
    case command_type::exit:
        return false;
    case command_type::join_room:
        join_room(command.argument);
        break;
    case command_type::leave_room:
        leave_current_room();
        break;
    case command_type::private_message:
        send_private_message(command.argument, command.argument2);
        break;
    default:
        _ASSERTE(command.type == command_type::message);

        auto& console = m_console;
        m_jabbr_client.send_message(user_input, m_current_room.name)
            .then([console](pplx::task<void> send_task)
            {
                try
                {
                    send_task.get();
                }
                catch (const std::exception& e)
                {
                    console->display_error(utility::string_t(L"Error sending a message: ")
                        .append(utility::conversions::to_string_t(e.what())));
                }
            });
    }
    return true;
}

void jabbr_chat::join_room(const std::wstring& room_name)
{
    if (room_name == m_current_room.name)
    {
        m_console->display_info(std::wstring(L"Already in room ").append(room_name));
        return;
    }

    m_console->display_info(std::wstring(L"Connecting to room ").append(room_name));

    m_jabbr_client.join_room(room_name)
        .then([&, room_name]()
        {
            return m_jabbr_client.get_room_info(room_name);
        })
        .then([&](pplx::task<jabbr::room> previous_task) mutable
        {
            try
            {
                m_current_room = previous_task.get();
                if (std::find_if(m_rooms.begin(), m_rooms.end(),
                    [&](const jabbr::room& r){ return r.name == m_current_room.name; }) == m_rooms.end())
                {
                    m_rooms.push_back(m_current_room);
                }

                m_console->display_info(std::wstring(L"Connected to room ").append(m_current_room.name));
                m_console->display_room(m_current_room);
            }
            catch (const std::exception& e)
            {
                m_console->display_error(std::wstring(L"Error: ")
                    .append(utility::conversions::to_string_t(e.what())));
            }
        }).get();
}

void jabbr_chat::leave_current_room()
{
    if (m_current_room.name.length() == 0)
    {
        m_console->display_info(std::wstring(L"You currently are not in any room."));
        return;
    }

    auto room_name = m_current_room.name;
    m_current_room = jabbr::room{};

    m_rooms.erase(std::remove_if(m_rooms.begin(), m_rooms.end(),
        [&](const jabbr::room& r){ return r.name == room_name; }));

    m_jabbr_client.leave_room(room_name)
        .then([&]()
        {
            m_console->display_welcome(m_rooms);
        }).get();
}

void jabbr_chat::send_private_message(const std::wstring& user, const std::wstring& message)
{
    auto &console = m_console;

    m_jabbr_client.send_private_message(user, message)
        .then([console](pplx::task<void> send_task)
        {
            try
            {
                send_task.get();
            }
            catch (const std::exception& e)
            {
                console->display_error(utility::string_t(L"Error sending a message: ")
                    .append(utility::conversions::to_string_t(e.what())));
            }
        });
}