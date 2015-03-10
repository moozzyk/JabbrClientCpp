#include "stdafx.h"
#include "jabbr_client_impl.h"
#include "cpprest\uri_builder.h"
#include "cpprest\http_client.h"
#include "cpprest\json.h"
#include "jabbrclient\default_authentication_provider.h"
#include "jabbr_events.h"
#include "json_materializer.h"

namespace jabbr
{
    jabbr_client_impl::jabbr_client_impl(const utility::string_t& url)
        : m_url(url), m_jabbr_connection(std::make_shared<signalr::hub_connection>(url)),
          m_chat_proxy(m_jabbr_connection->create_hub_proxy(U("Chat")))
    { }

    pplx::task<log_on_info> jabbr_client_impl::connect(const utility::string_t& user_name, const utility::string_t& password,
        std::unique_ptr<authentication_provider> auth_provider)
    {
        if (!auth_provider)
        {
            auth_provider = std::move(std::make_unique<default_authentication_provider>(m_url));
        }

        auto log_on_tce = pplx::task_completion_event<log_on_info>();

        m_chat_proxy.on(events::log_on, [log_on_tce](const web::json::value &response)
            {
                log_on_info info;

                if (response.is_array() && response.as_array().size() > 0)
                {
                    json_materializer::create_rooms(response.as_array().at(0), info.rooms);
                }

                log_on_tce.set(info);
            });

        auto& jabbr_connection = m_jabbr_connection;
        auto& chat_proxy = m_chat_proxy;

        return auth_provider->configure_connection(m_jabbr_connection, user_name, password)
            .then([jabbr_connection]() mutable
            {
                return jabbr_connection->start();
            })
            .then([chat_proxy]() mutable
            {
                return chat_proxy.invoke<void>(U("Join"));
            })
            .then([log_on_tce](pplx::task<void> chat_join_task)
            {
                try
                {
                    chat_join_task.get();
                }
                catch (const std::exception&)
                {
                    log_on_tce.set_exception(std::current_exception());
                }
            })
            .then([log_on_tce]()
            {
                return pplx::task<log_on_info>(log_on_tce);
            });
    }

    pplx::task<user> jabbr_client_impl::get_user_info()
    {
        return m_chat_proxy.invoke<web::json::value>(U("GetUserInfo"))
            .then([](const web::json::value& response)
            {
                return json_materializer::create_user(response);
            });
    }

    pplx::task<std::vector<room>> jabbr_client_impl::get_rooms()
    {
        return m_chat_proxy.invoke<web::json::value>(U("GetRooms"))
            .then([](const web::json::value &response)
            {
                std::vector<room> rooms;
                json_materializer::create_rooms(response, rooms);
                return rooms;
            });
    }

    pplx::task<room> jabbr_client_impl::get_room_info(const utility::string_t& room_name)
    {
        web::json::value params;
        params[0] = web::json::value::string(room_name);

        return m_chat_proxy.invoke<web::json::value>(U("GetRoomInfo"), params)
            .then([](const web::json::value& response)
            {
                return json_materializer::create_room(response);
            });
    }

    pplx::task<void> jabbr_client_impl::join_room(const utility::string_t& room_name)
    {
        web::json::value args;
        args[0] = web::json::value::string(utility::string_t(U("/join ")).append(room_name));
        args[1] = web::json::value::string(U(""));

        return m_chat_proxy.invoke<void>(U("Send"), args);
    }

    pplx::task<void> jabbr_client_impl::leave_room(const utility::string_t& room_name)
    {
        web::json::value args;
        args[0] = web::json::value::string(utility::string_t(U("/leave ")).append(room_name));
        args[1] = web::json::value::string(U(""));

        return m_chat_proxy.invoke<void>(U("Send"), args);
    }

    pplx::task<void> jabbr_client_impl::send_message(const utility::string_t& message, const utility::string_t& room_name)
    {
        web::json::value args;
        args[0] = web::json::value::string(message);
        args[1] = web::json::value::string(room_name);

        return m_chat_proxy.invoke<void>(U("Send"), args);
    }

    pplx::task<void> jabbr_client_impl::log_out(const utility::string_t& user_name)
    {
        web::json::value args;
        args[0] = web::json::value::string(utility::string_t(U("/logout")));
        args[1] = web::json::value::string(user_name);

        return m_chat_proxy.invoke<void>(U("Send"), args);
    }

    pplx::task<void> jabbr_client_impl::disconnect()
    {
        return m_jabbr_connection->stop();
    }

    void jabbr_client_impl::set_on_message_received(const std::function<void(const message&, const utility::string_t&)>& on_message_received)
    {
        m_chat_proxy.on(U("addMessage"), [on_message_received](const web::json::value& params)
        {
            if (!params.is_array())
            {
                return;
            }

            auto param_array = params.as_array();
            if (param_array.size() < 2 || !param_array.at(0).is_object() || !param_array.at(1).is_string())
            {
                return;
            }

            on_message_received(json_materializer::create_message(params.as_array().at(0)), param_array.at(1).as_string());
        });
    }
}