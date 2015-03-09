#include "stdafx.h"
#include "jabbr_chat.h"
#include "pplx\pplxtasks.h"
#include "jabbr_user.h"


jabbr_chat::jabbr_chat(std::wstring url)
    : m_jabbr_client(jabbr::jabbr_client{ url }),
    m_console(jabbr_console(std::bind(&jabbr_chat::on_user_input, this, std::placeholders::_1)))
{ }

jabbr_chat::~jabbr_chat() = default;

void jabbr_chat::run(std::wstring user_name, std::wstring password)
{
    m_console.display_welcome(nullptr);

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
            utility::string_t(L"Connecting")
            .append(utility::string_t(i % 6, L'.'))
            .append(utility::string_t(6 - (i % 6), L' '));

        m_console.display_connecting_status(connecting_string);

        pplx::wait(300);
    }

    try
    {
        m_user = connect_task.get();
        m_console.display_welcome(&m_user);
        m_console.display_connecting_status(L"   Connected    ");
        m_console.run();
    }
    catch (const std::exception &e)
    {
        m_console.display_connecting_status(utility::string_t(L"Could not connect to jabbr server. Error: ")
            .append(utility::conversions::to_string_t(e.what())));
    }
}

bool jabbr_chat::on_user_input(const std::wstring& user_input)
{
    auto command = parser.parse(user_input);

    switch (command.type)
    {
    case command_type::exit:
        return false;
    case command_type::join_room:
        join_room(command.argument);
        break;
    }

    return true;
}

void jabbr_chat::join_room(const std::wstring& room)
{
    // fire and forget

    // TODO: & can go away before the task finishes
    m_jabbr_client.join_room(room)
        .then([&, room](pplx::task<void> previous_task) mutable
        {
            try
            {
                previous_task.get();
                m_console.set_title(std::wstring(L"Room: ").append(room));
            }
            catch (const std::exception& e)
            {
                m_console.display_error(std::wstring(L"Error: ")
                    .append(utility::conversions::to_string_t(e.what())));
            }
        });
}