#pragma once

#include "_exports.h"
#include "pplx\pplxtasks.h"
#include "authentication_provider.h"
#include "log_on_info.h"

namespace jabbr
{
    class jabbr_client_impl;

    class jabbr_client
    {
    public:
        JABBRCLIENTCPP_API jabbr_client(const utility::string_t& url);
        JABBRCLIENTCPP_API ~jabbr_client();

        JABBRCLIENTCPP_API pplx::task<log_on_info> connect(const utility::string_t& user_name, const utility::string_t& password,
            std::unique_ptr<authentication_provider> auth_provider = nullptr);

        JABBRCLIENTCPP_API pplx::task<user> get_user_info();
        JABBRCLIENTCPP_API pplx::task<std::vector<room>> get_rooms();
        JABBRCLIENTCPP_API pplx::task<room> get_room_info(const utility::string_t& room_name);
        JABBRCLIENTCPP_API pplx::task<void> join_room(const utility::string_t& room_name);

    private:
        std::shared_ptr<jabbr_client_impl> m_pImpl;
    };
}