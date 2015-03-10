#pragma once

#include <memory>
#include <functional>
#include "pplx\pplxtasks.h"
#include "jabbrclient\authentication_provider.h"
#include "jabbrclient\log_on_info.h"

namespace jabbr
{
    class jabbr_client_impl
    {
    public:
        jabbr_client_impl(const utility::string_t& url);

        pplx::task<log_on_info> connect(const utility::string_t& user_name, const utility::string_t& password,
            std::unique_ptr<authentication_provider> auth_provider = nullptr);
        pplx::task<std::vector<room>> get_rooms();
        pplx::task<room> get_room_info(const utility::string_t& room_name);
        pplx::task<user> get_user_info();
        pplx::task<void> join_room(const utility::string_t& room_name);
        pplx::task<void> leave_room(const utility::string_t& room_name);
        pplx::task<void> send_message(const utility::string_t& message, const utility::string_t& room_name);
        pplx::task<void> log_out(const utility::string_t& user_name);
        pplx::task<void> disconnect();

        void set_on_message_received(const std::function<void(const message&, const utility::string_t&)>& on_message_received);

    private:
        utility::string_t m_url;
        std::shared_ptr<signalr::hub_connection> m_jabbr_connection;
        signalr::hub_proxy m_chat_proxy;
    };
}